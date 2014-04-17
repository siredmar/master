/** @file apex-image.cc

    written by Marc Singer
    2 July 2008

    Copyright (C) 2008 Marc Singer

    @brief Program to view, create and modify APEX format image files.

    TODO
    ----

    o Some verbose level should dump hex of header.

    o Invalid image message should be more specific.  For example, we
      could state that the image has no signature, an invalid length,
      or a broken CRC.

    o Revise help prose to reflect the auto-mode detection and the
      basic operation of the program.

    o Payload CRCs should be calculated the same as POSIX cksum
      command so that it is easy to verify what they should be.  This
      is, but we need to document this well.

    o Normalize error reporting so that all errors look the same.  The
      error messages from within the argument parser are a good
      standard.

    o It would be nice if the verbose mode of update stated which
      fields and payloads were being updated.  This isn't super
      important since we can see the outcome with the dry-run.

    o Need a definitive error when the number of updated files is
      greater than the number of input files.  The use of '.' notation
      for updates could introduce more payloads than there are source
      files.

    o Define how the output will look in normal, verbose, and quiet
      modes.

    o Fixup comments for doxygen.

    o Document the file format in this file, or refer to another
      file.  It should, at a minimum, be in this directory.

    o Header::Iterator should have some protection against running
      past the end of a header in case the data is corrupt.  Done but
      not tested.


    NOTES
    -----

    o CRC could/should be calculated using an iostream filter.

    o Creation time.  It should be possible to override the use of the
      current time for the creation time.  We could either be able to
      use the timestamp on another file, set it by a string, or
      suppress the field altogether.
      o If we do this, the modified() operator needs to do the right
        thing since we now ignore the creation time.

    o modeUpdate.  Up until now, the only thing we've been able to do
      when updating an image are changes to the metadata.  The command
      line needs to support this in a useful way.  For example, we
      could use a special marker for 'same file that is already in the
      image'.  However, it could be handy to allow replacement of one
      of the components of an image.  For example, revise the initrd
      and leave the kernel intact.  In fact, it could be inferred from
      the input.

      The syntax may look like this:

        apex-image -u zImage-new aImage    # Replace zImage in aImage
        apex-image -u . initrd-new aImage  # Replace initrd in aImage

      It would be wise to make sure that the distinction between an
      update the creation of a new image is clear.  Including a single
      '.' could be construed as requesting an update.  However, it
      isn't clear what to do if there are no metadata changes and the
      image has only one payload.

        apex-image zImage aImage	   # Update aImage or replace it

      We could assume that update has priority and that the only way
      to force creation is to use the -c/-f switches.  I don't like
      this because it seems like the user should be require to make
      his intention clear.  If there is no ambiguity, then the
      application can do the rigth thing.  There is no harm in
      creating an image if aImage doesn't exist.   Still, if the user
      has used this form in the past and then does so again, he may
      expect that we'll be creating a new image when, in fact, we fall
      back to updating.

      The only other way to manage this would be to report on the
      progress.  If we say 'creating' or 'updating' then we give the
      user sufficient feedback to make the process clear.  This would
      also mean that there is no need for an --update switch, only a
      --create switch.  Moreover, there is no need for --force since
      the the user would only specify --create when the output file
      already exists.

*/

//#define TALK 1

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

#include <sys/types.h>
#include <regex.h>
#include <argp.h>

#include <list>
#include <vector>
#include <ext/rope>

#include "exception.h"
#include "talk.h"
#include "oprintf.h"
#include "dumpw.h"

using __gnu_cxx::crope;

const char* argp_program_version = "apex-image 0.1";

uint8_t signature[] = { 0x41, 0x69, 0x30, 0xb9 };

enum {
  sizeZero      = 0x3,          // 11b
  sizeOne       = 0x2,          // 10b
  sizeFour      = 0x1,          // 01b
  sizeVariable  = 0x0,          // 00b
};

enum {
  fieldImageDescription                 = 0x10 | sizeVariable,
  fieldImageCreationDate                = 0x14 | sizeFour,
  fieldPayloadLength                    = 0x30 | sizeFour,
  fieldPayloadLoadAddress               = 0x34 | sizeFour,
  fieldPayloadEntryPoint                = 0x38 | sizeFour,
  fieldPayloadType                      = 0x3c | sizeOne,
  fieldPayloadDescription               = 0x40 | sizeVariable,
  fieldPayloadCompression               = 0x44 | sizeZero,
  fieldLinuxKernelArchitectureID        = 0x80 | sizeFour,
  fieldNOP                              = 0xfc | sizeZero,
};

enum {
  typeNULL = 0,
  typeLinuxKernel = 1,
  typeLinuxInitrd = 2,
};

inline uint32_t swabl (uint32_t v)
{
  return 0
    | ((v & (uint32_t) (0xff <<  0)) << 24)
    | ((v & (uint32_t) (0xff <<  8)) <<  8)
    | ((v & (uint32_t) (0xff << 16)) >>  8)
    | ((v & (uint32_t) (0xff << 24)) >> 24);
}

uint32_t interpret_number (const char* sz)
{
  int base = 10;
  if (strlen (sz) > 2 && sz[0] == '0' && tolower (sz[1]) == 'x') {
    base = 16;
    sz += 2;
  }
  char* pb;
  uint32_t value = (uint32_t) strtoul (sz, &pb, base);
  if (tolower (*pb) == 'k')
    value *= 1024;
  if (tolower (*pb) == 'm')
    value *= 1024*1024;
  return value;
}

int interpret_image_type (const char* sz)
{
  int value = typeNULL;

  if (strcasecmp (sz, "ramdisk") == 0)
    value = typeLinuxInitrd;
  if (strcasecmp (sz, "initrd") == 0)
    value = typeLinuxInitrd;
  if (strcasecmp (sz, "kernel") == 0)
    value = typeLinuxKernel;

  return value;
}

const char* describe_size (uint32_t cb)
{
  static char sz[128];

  float s = cb/1024.0;
  char unit = 'K';

  if (s > 1024) {
    s /= 1024.0;
    unit = 'M';
  }
  snprintf (sz, sizeof (sz), "%d bytes (%d.%02d %ciB)",
            cb, (int) s, (int (s*100.0))%100, unit);
  return sz;
}

const char* describe_tag (uint8_t tag)
{
  switch (tag) {
  case fieldImageDescription:
    return "fieldImageDescription";
  case fieldImageCreationDate:
    return "fieldImageCreationDate";
  case fieldPayloadLength:
    return "fieldPayloadLength";
  case fieldPayloadLoadAddress:
    return "fieldPayloadLoadAddress";
  case fieldPayloadEntryPoint:
    return "fieldPayloadEntryPoint";
  case fieldPayloadType:
    return "fieldPayloadType";
  case fieldPayloadDescription:
    return "fieldPayloadDescription";
  case fieldPayloadCompression:
    return "fieldPayloadCompression";
  case fieldLinuxKernelArchitectureID:
    return "fieldLinuxKernelArchitectureID";
  case fieldNOP:
    return "fieldNOP";
  default:
    return "unknown-tag";
  }
}

const char* describe_image_type (unsigned long v)
{
  switch (v) {
  default:
  case typeNULL:	return "undefined-image-type";		break;
  case typeLinuxKernel:	return "Kernel";			break;
  case typeLinuxInitrd:	return "Initrd";			break;
  }
};

uint32_t compute_crc32 (uint32_t crc, const void* pv, int cb)
{
#define IPOLY		(0xedb88320)
#define POLY		(0x04c11db7)

#if 1
  // This version works and it is identical to the one in APEX.  I
  // should make sure I know that I cannot use a more efficient
  // algorithm like the one below, but we don't really need to care.

  const unsigned char* pb = (const unsigned char *) pv;

  while (cb--) {
    int i;
    crc ^= *pb++ << 24;

    for (i = 8; i--; ) {
      if (crc & (1<<31))
        crc = (crc << 1) ^ POLY;
      else
        crc <<= 1;
    }
  }

  return crc;
#endif

#if 0
  unsigned char* pb = (unsigned char*) pv;
  uint32_t poly = POLY;	// Hack to get a register allocated

  crc = crc ^ 0xffffffff;	// Invert because we're continuing

#define DO_CRC\
  if (crc & 1) { crc >>= 1; crc ^= poly; }\
  else	       { crc >>= 1; }

  while (cb--) {
    crc ^= *pb++;

    DO_CRC;
    DO_CRC;
    DO_CRC;
    DO_CRC;
    DO_CRC;
    DO_CRC;
    DO_CRC;
    DO_CRC;
  }

  return crc ^ 0xffffffff;
#endif
}

/** Append a CRC32 of the length of a region such that the length may
    be any size.  We add bytes to the CRC starting with the LSB until
    the length is empty.  This is the method compatible with the POSIX
    cksum command. */

uint32_t compute_crc32_length (uint32_t crc, size_t cb)
{
  for (; cb; cb >>= 8) {
    uint8_t b = cb & 0xff;
    crc = compute_crc32 (crc, &b, 1);
  }
  return crc;
}

size_t verify_image_header (void* pv, size_t cb, bool ignore_crc_errors)
{
  if (cb < 16)
    throw Exception ("invalid header");

  if (memcmp (pv, signature, sizeof (signature)) != 0)
    throw Exception ("no image signature found");

  size_t cbHeader = ((uint8_t*) pv)[4]*16;
  if (cbHeader > cb)
    throw Exception ("header_size invalid");

  uint32_t crc = compute_crc32 (0, pv, cbHeader);
  if (crc != 0 && !ignore_crc_errors)
    throw Exception ("incorrect header CRC");

  return cbHeader;
}

struct File {
protected:
  File (const File&) {}
public:

  const char* szPath;
  int fh;
  size_t cb;
  void* pv;
  struct stat stat;

  bool is_void (void) {
    return fh == -1; }
  bool is_open (void) {
    return fh >= 0; }
  bool is_external (void) {
    return fh == -2; }
  bool is_update (void) {
    return strcasecmp (szPath, ".") == 0; }
  bool is_valid_image (bool ignore_crc_errors = false) {
    try {
      verify_image_header (pv, cb, ignore_crc_errors);
    }
    catch (...) {
      return false;
    }
    return true;
  }
  void release_this (void) {
//    printf ("release this %s %d\n", szPath, fh);
    if (!is_external () && pv != NULL)
      ::munmap (pv, cb);
    if (fh >= 0)
      ::close (fh);
    void_this ();
  }
  void void_this (void) {
    pv = NULL;
    cb = 0;
    szPath = 0;
    fh = -1; }

  File () : szPath (NULL), fh (-1), cb (0), pv (NULL) {}
  File (const char* _szPath) : szPath (_szPath), fh (-1), cb (0), pv (NULL) {}
  ~File () {
    release_this (); }

  void set_external (void* _pv, size_t _cb) {
    release_this ();
    pv = _pv;
    cb = _cb;
    fh = -2; }
  void set_path (const char* _szPath) {
    release_this ();
    szPath = _szPath; }
  void open (const char* _szPath = NULL) {
    if (_szPath)
      szPath = _szPath;
    if (is_update ())
      return;
    fh = ::open (szPath, O_RDONLY);
    if (fh == -1)
      return;
    ::fstat (fh, &stat);
    cb = stat.st_size;
    pv = ::mmap (0, cb, PROT_READ, MAP_FILE | MAP_SHARED, fh, 0);
    DBG (2, "file @%d -> %d b\n", fh, cb);
  }
};

std::ostream& operator<< (std::ostream& o, const File& file)
{
  o << oprintf ("'%s' (%d) [%p %d]",
                file.szPath,
                file.fh,
                file.pv,
                file.cb);
  return o;
}


/** The Payload object holds both the mmap'd source file and the
    attributes for the payload in the image. */

struct Payload : public File {

  const char* description;
  int type;
  uint32_t load_address;
  uint32_t entry_point;
  bool compressed;

  uint32_t crc_loaded;          // CRC as read from an existing image

  bool modified (void) {
    return type
      || load_address != ~0
      || entry_point != ~0
      || description
      //      || compressed
      ; }
  size_t header_size (void) {
    return 5                    // Mandatory length
      + (type ? 2 : 0)
      + (load_address != ~0 ? 5 : 0)
      + (entry_point != ~0 ? 5 : 0)
      + (description ? 1 + 1 + strlen (description) + 1 : 0)
      + (compressed ? 1 : 0); }

  uint32_t build_header (uint32_t crc, crope& rope) {
    unsigned char tag;
    if (type) {
      tag = fieldPayloadType;
      unsigned char _type = type;
      crc = compute_crc32 (crc, &tag, 1);
      crc = compute_crc32 (crc, &_type, 1);
      rope.append (tag);
      rope.append (_type);
    }
    if (load_address != ~0) {
      tag = fieldPayloadLoadAddress;
      uint32_t _load_address = swabl (load_address);
      crc = compute_crc32 (crc, &tag, 1);
      crc = compute_crc32 (crc, &_load_address, sizeof (_load_address));
      rope.append (tag);
      rope.append ((char*) &_load_address, sizeof (_load_address));
    }
    if (entry_point != ~0) {
      tag = fieldPayloadEntryPoint;
      uint32_t _entry_point = swabl (entry_point);
      crc = compute_crc32 (crc, &tag, 1);
      crc = compute_crc32 (crc, &_entry_point, sizeof (_entry_point));
      rope.append (tag);
      rope.append ((char*) &_entry_point, sizeof (_entry_point));
    }
    if (description) {
      if (strlen (description) + 1 > 127)
        throw Exception ("description field too long");
      tag = fieldPayloadDescription;
      uint8_t length = strlen (description) + 1;
      crc = compute_crc32 (crc, &tag, 1);
      crc = compute_crc32 (crc, &length, 1);
      crc = compute_crc32 (crc, description, length);
      rope.append (tag);
      rope.append (length);
      rope.append (description, length);
    }
    if (compressed) {
      tag = fieldPayloadCompression;
      crc = compute_crc32 (crc, &tag, 1);
      rope.append (tag);
    }
    // Length is always last
    {
      tag = fieldPayloadLength;
      uint32_t _cb = swabl (cb);
      crc = compute_crc32 (crc, &tag, 1);
      crc = compute_crc32 (crc, &_cb, sizeof (_cb));
      rope.append (tag);
      rope.append ((char*) &_cb, sizeof (_cb));
    }
    return crc;
  }

  Payload () : description (NULL), type (0),
               load_address (~0), entry_point (~0),
               compressed (false), crc_loaded (0) {}
  ~Payload () { }

  /** Merge payload with this.  The process of updating an image
      somtimes involves makes  */
  Payload& operator+= (Payload& payload) {
//    printf ("operator+= on payload\n");
    if (is_void ()) {
      *(File*) this = payload;
      payload.void_this ();
    }
    if (!type)
      type = payload.type;
    if (load_address == ~0)
      load_address = payload.load_address;
    if (entry_point == ~0)
      entry_point = payload.entry_point;
    if (!description)
      description = payload.description;
    crc_loaded = 0;             // Clear the old value if it was loaded
  }

};

std::ostream& operator<< (std::ostream& o, const Payload& payload)
{
  o << oprintf ("%-20.20s %08x %08x {",
                describe_image_type (payload.type),
                payload.load_address,
                payload.entry_point)
    << *(File*) &payload << "}";
  return o;
}

struct arguments {

  Payload* m_payload;

  int mode;
  int verbose;
  bool dry_run;
  bool quiet;
  bool force;
  bool ignore_crc_errors;
  uint32_t crc_calculated;

  const char* image_name;

  arguments () {
    bzero (this, sizeof (*this)); }

  Payload* payload (void) {
    if (!m_payload)
      m_payload = new Payload;
    return m_payload; }
};

struct Image : public std::list<Payload*>, public File
{
  const char* description;
  time_t timeCreation;
  int architecture_id;

  crope header;		// Rope for reading, building, and writing header

  Image () : description (NULL),
             timeCreation (time (NULL)),
             architecture_id (0) { }

  /** Return true if there is a change to the metadata for the image. */
  bool modified (void) {
    if (description || architecture_id)
      return true;
    if (!size ())
      return false;
    for (iterator it = begin (); it != end (); ++it)
      if ((*it)->modified ())
        return true;
    return false;
  }

  bool is_update (void) {
    for (iterator it = begin (); it != end (); ++it)
      if ((*it)->is_update ())
        return true;
    return false;
  }

  /** Pull metadata and payloads from the final payload object in
      preparation for updating an image. */
  void load (struct arguments&);
  void merge_payload (int, Payload*);

  size_t header_size (void) {
    return 0
      + (description ? 1 + 1 + strlen (description) + 1 : 0)
      + 5	                // creation time
      + (architecture_id ? 5 : 0);
  }
  void build_header (struct arguments& args) {
    header.clear ();

	// Append the signature word
    uint32_t crc = 0;
    crc = compute_crc32 (crc, &signature, sizeof (signature));
    header.append ((char*) signature, sizeof (signature));

	// Calculate the header size in one go
    int _header_size = sizeof (signature) + 1 + header_size ();
    for (Image::iterator it = begin (); it != end (); ++it) {
      if ((*it) == back ())
        break;
      _header_size += (*it)->header_size ();
    }
    _header_size += 4;           // CRC

	// Compute and store the header size byte
    uint8_t __header_size = (_header_size + 15)/16;
    crc = compute_crc32 (crc, &__header_size, 1);
    header.append (__header_size);

	// Append the image header tags
    unsigned char tag;
    {
      tag = fieldImageCreationDate;
      uint32_t _timeCreation = swabl (timeCreation);
      crc = compute_crc32 (crc, &tag, 1);
      crc = compute_crc32 (crc, &_timeCreation, sizeof (_timeCreation));
      header.append (tag);
      header.append ((char*) &_timeCreation, sizeof (_timeCreation));
    }
    if (description) {
      if (strlen (description) + 1 > 127)
        throw Exception ("description field too long");
      tag = fieldImageDescription;
      uint8_t length = strlen (description) + 1;
      crc = compute_crc32 (crc, &tag, 1);
      crc = compute_crc32 (crc, &length, 1);
      crc = compute_crc32 (crc, description, length);
      header.append (tag);
      header.append (length);
      header.append (description, length);
    }
    if (architecture_id) {
      tag = fieldLinuxKernelArchitectureID;
      uint32_t _id = swabl (architecture_id);
      crc = compute_crc32 (crc, &tag, 1);
      crc = compute_crc32 (crc, &_id, sizeof (_id));
      header.append (tag);
      header.append ((char*) &_id, sizeof (_id));
    }

	// Append the payload tags
    for (Image::iterator it = begin (); it != end (); ++it)
      if ((*it) != back ())
        crc = (*it)->build_header (crc, header);

	// Pad out the header_size - 4 and then append the CRC
    for (_header_size = __header_size*16 - _header_size; _header_size--; ) {
      unsigned char b = 0xff;
      crc = compute_crc32 (crc, &b, 1);
      header.append ((char) 0xff);
    }
    crc = swabl (crc);
    header.append ((char*) &crc, sizeof (crc));
  }

  void describe (struct arguments& args);
};

Image g_image;

/** APEX image header class used to support a metadata iterator.
 */

class Header {
  void* m_pv;
  size_t m_cb;

public:
  Header (void* pv, size_t cb) : m_pv (pv), m_cb (cb) {}

  class Iterator {
    void* m_pv;
    void* m_pvEnd;

    uint8_t _tag (void) {
      return *(uint8_t*) m_pv; }

    void lengths (size_t& cbTag, size_t& cbData) {
      uint8_t tag = this->_tag ();
      cbTag = 1;
      cbData = 0;

//      printf ("tag %x  size %x", tag, tag & 3);
      switch (tag & 3) {
      case sizeZero:		cbData = 0; break;
      case sizeOne:		cbData = 1; break;
      case sizeFour:		cbData = 4; break;
      case sizeVariable:
//        printf (" (variable)");
        cbData = ((uint8_t*)m_pv)[1];
        if (cbData > 127)
          throw Exception ("invalid field length");
        ++cbTag;
        break;
      }
//      printf ("  cbTag %d cbData %d\n", cbTag, cbData);
    }

  public:
    Iterator (void* pv, void* pvEnd) : m_pv (pv), m_pvEnd (pvEnd) {}

    uint8_t tag (void) {
      return this->_tag (); }
    void* data (void) {
      return (char*) m_pv + tag_length (); }

    operator uint32_t () {
      uint32_t v;
      memcpy (&v, data(), 4);
      return swabl (v); }

    operator uint8_t () {
      return *(uint8_t*)data (); }

    size_t field_length (void) {
      size_t cbTag;
      size_t cbData;
      lengths (cbTag, cbData);
      return cbTag + cbData; }

    size_t tag_length (void) {
      size_t cbTag;
      size_t cbData;
      lengths (cbTag, cbData);
      return cbTag; }

    size_t data_length (void) {
      size_t cbTag;
      size_t cbData;
      lengths (cbTag, cbData);
      return cbData; }

    Iterator& operator++ () {
      if (m_pv >= m_pvEnd)
        throw Exception ("attempt to increment metdata iterator beyond end");
      m_pv = (char*) m_pv + field_length (); };

    bool operator!= (const Iterator& it) {
      return m_pv != it.m_pv; }
  };

  Iterator begin (void) {
    return Iterator ((char*) m_pv + 5,        (char*) m_pv + m_cb - 4); }
  Iterator end (void) {
    return Iterator ((char*) m_pv + m_cb - 4, (char*) m_pv + m_cb - 4); }

};


struct argp_option options[] = {
  { "show",		'i', 0, 0,        "Force 'show image header' mode", 1 },
  { "update",		'u', 0, 0,             "Force 'update image' mode"    },
  { "create",		'c', 0, 0,             "Force 'create image' mode"    },

  { "load-address",	'l', "ADDR", 0,       "Set load address for image", 2 },
  { "entry-point",	'e', "ADDR", 0,        "Set entry point for image"    },
  { "load-and-entry",	'L', "ADDR", 0,
			        "Set payload load address and entry point"    },
  { "image-description", 'D', "LABEL", 0,          "Set image description"    },
  { "type",		't', "TYPE", 0,        "Set payload type [kernel]"    },
  { "description",      'd', "LABEL", 0,         "Set payload description"    },
  { "architecture-id",  'A', "NUMBER", 0,
                             "Set a value to override the architecture ID"    },

  { "force",		'f', 0, 0,        "Force overwrite of output file", 3 },
  { "verbose",		'v', 0, 0,        "Verbose output, when available"    },
  { "quiet",		'q', 0, 0,              "Suppress output messages"    },
  { "dry-run",		'n', 0, 0,               "Produce no output files"    },
  { "ignore-crc-errors", 'C', 0, 0,                    "Ignore CRC errors"    },

  { 0 }
};

error_t arg_parser (int key, char* arg, struct argp_state* state);

struct argp argp = {
  options, arg_parser,
  "FILE...",
  "  Create, update, and view an APEX image files\n"
  "\v"
  "  The program attempts to guess the mode of operation.  If there is\n"
  "only one file argument and no metadata switches, it will display the\n"
  "metadata for the image file.  The distinction between a create or an\n"
  "update is the completeness of the arguments and whether or not the\n"
  "final file argument is an image.  The automatic mode detection may\n"
  "be overriden by specifying one of the mode switches.\n"
  "  The order or switches and files is significant.  Metadata switches\n"
  "must preceede the source file they modify.  The last argument is output\n"
  "file name for creating images or the name of the image to update when\n"
  "updating an existing image.\n"
  "  The CRC of the header is a simple CRC of the header data, from the\n"
  "signature to the last byte before the CRC.  The CRC that protects each\n"
  "payload is computed the same way that the POSIX cksum command computes\n"
  "the CRC.  This makes it easy to verify the identity of a payload."
  "\n"
  "  ADDR is a 32 bit number in decimal or hexadecimal if prefixed with 0x\n"
  "  TYPE is one of: kernel, initrd\n"
  "  FILE is either a filename, or possibly '.' when updating an image.\n"
  "\n  e.g.\n"
  "              # Show image metadata\n"
  "   apex-image aImage\n"
  "              # Create image with kernel and initrd\n"
  "   apex-image -t kernel zImage -t initrd aImage\n"
  "              # Update the load address for the first payload\n"
  "   apex-image -l 0xc0008000 aImage\n"
  "              # Update the load address for the second payload\n"
  "   apex-image . -l 0xc2000000 zImage\n"
};

enum {
  modeNone = 0,
  modeShow,
  modeUpdate,
  modeCreate,
};

error_t arg_parser (int key, char* arg, struct argp_state* state)
{
  struct arguments& args = *(struct arguments*) state->input;

  switch (key) {
  case 'i':
    if (args.mode)
      argp_error (state, "only one mode (-i, -u, -c) may be set");
    args.mode = modeShow;
    break;
  case 'u':
    if (args.mode)
      argp_error (state, "only one mode (-i, -u, -c) may be set");
    args.mode = modeUpdate;
    break;
  case 'c':
    if (args.mode)
      argp_error (state, "only one mode (-i, -u, -c) may be set");
    args.mode = modeCreate;
    break;

  case 'l':
    args.payload ()->load_address = interpret_number (arg);
    break;

  case 'e':
    args.payload ()->entry_point = interpret_number (arg);
    break;

  case 'L':
    args.payload ()->load_address = args.payload ()->entry_point
      = interpret_number (arg);
    break;

  case 'A':
    g_image.architecture_id = interpret_number (arg);
    break;

  case 'D':
    g_image.description = arg;
    break;

  case 'd':
    args.payload ()->description = arg;
    break;

  case 't':
    args.payload ()->type = interpret_image_type (arg);
    if (args.payload ()->type == 0)
      argp_error (state, "unrecognized image type '%s'", arg);
    break;

  case 'f':
    args.force = true;
    break;

  case 'v':
    ++args.verbose;
    args.quiet = false;
    break;

  case 'n':
    args.dry_run = true;
    break;

  case 'q':
    args.quiet = true;
    args.verbose = 0;
    break;

  case 'C':
    args.ignore_crc_errors = true;
    break;

  case ARGP_KEY_ARG:
    args.payload ()->set_path (arg);
    g_image.push_back (args.payload ());
    args.m_payload = NULL;
    break;

  case ARGP_KEY_END:

    if (args.m_payload)
      argp_error (state, "a filename must follow the last metadata switch");

    if (!g_image.size ())
      argp_usage (state);

        // there are no metadata flags associated with it.
    if (g_image.back ()->modified ())
    if (state->next >= state->argc && args.payload ()->modified ()) {
      g_image.push_back (args.payload ());
      args.m_payload = NULL;
    }

    if (!args.mode) {
      if (g_image.size () == 1)
        args.mode = g_image.modified ()
          ? modeUpdate : modeShow;
      else if (g_image.is_update ())
        args.mode = modeUpdate;
      else
        args.mode = g_image.back ()->is_valid_image ()
          ? modeUpdate : modeCreate;
    }

    switch (args.mode) {
    default:
      args.mode = modeShow;
      // -- fall through
    case modeShow:
      if (g_image.modified ())
        argp_error (state, "metadata may not be modified when showing"
                    " an image header");
      if (g_image.size () != 1)
        argp_error (state, "show mode requires one and only one file");
      break;
    case modeUpdate:
		// Make sure that the metadata is assigned to update a payload
      if (g_image.back ()->modified ()) {
        *(File*) args.payload () = *(File*) g_image.back ();
        g_image.back ()->File::void_this ();
        g_image.push_back (args.payload ());
        args.m_payload = NULL;
      }

//      if (g_image.size () != 1)
//        argp_error (state, "update mode requires one and only one file");
      break;
    case modeCreate:
      if (g_image.size () < 2)
        argp_error (state,
           "create mode requires at least two files, and input and an output");
      break;
    }
    break;

  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}


/** Load an image from a file such that it can be written out again.
    Usually, this would be used to modify an existing image, but it
    could also be used when verifying an image as well.

    Probably the best way to do this is to allocate a payload at the
    start and whenever we see a payload length tag.  When we're ready
    to apply the payload, we either merge it with the payload object
    already in the image, or we insert the new payload object.  We'll
    end up with an allocated payload at the end that we don't need,
    but that's OK.

 */

void Image::load (struct arguments& args)
{
  Payload& image = *back ();

  /* *** FIXME: this isn't really the right thing to do.  If the final
     *** paylaod isn't an image, we should be able to use it as a
     *** source payload. */

  if (!image.is_valid_image (args.ignore_crc_errors))
    throw Exception ("invalid image");

#if 0
//  printf ("starting with\n");
  for (Image::iterator it = g_image.begin (); it != g_image.end (); ++it) {
    std::cout << **it << std::endl;
    if ((*it)->pv)
      dumpw ((char*) (*it)->pv, 32, 0, 0);
  }
  printf ("---\n");
#endif

  /*
    What we do is scan the header for tags, updating the image
    metadata as well as the payload metadata.  If there are more
    payloads found in the metadata than the user specified, we add
    payload objects to the image.  If some of the payload objects have
    update '.' names, we need to setup the payload object to point to
    the data in the original image.  We're going to generate a new
    image when we update instead of attempting to update in place.

   */

  size_t cbHeader = verify_image_header (image.pv, image.cb,
                                         args.ignore_crc_errors);
  header.clear ();
  if (cbHeader > image.cb)
    throw Exception ("invalid header");
  header.append ((char*) image.pv, cbHeader);

  Header h (image.pv, cbHeader);
  Payload* payload = new Payload; // *** Could probably use an auto
  void* pvPayload = (char*) image.pv + cbHeader;
  int iPayload = 0;
  for (Header::Iterator it = h.begin (); it != h.end (); ++it) {
    if (args.verbose > 1)
      printf ("tag %s (0x%x)\n", describe_tag (it.tag ()), it.tag ());
    switch (it.tag ()) {
    case fieldImageDescription:
      if (!description)
        description = (char*) it.data ();
      break;
    case fieldImageCreationDate:
      timeCreation = uint32_t (it);
      break;
    case fieldPayloadLength:
      {
        size_t cbPayload = uint32_t (it);
//        printf ("Payload Length:       %s\n", describe_size (cbPayload));

        payload->set_external (pvPayload, cbPayload);
        memcpy (&payload->crc_loaded, (char*) pvPayload + cbPayload, 4);
        payload->crc_loaded = swabl (payload->crc_loaded);
        cbPayload += 4;         // CRC
        cbPayload += 15;        // Round up
        cbPayload &= ~0xf;      // Round-off
        pvPayload = (char*) pvPayload + cbPayload;
      }
      merge_payload (iPayload++, payload);
      payload = new Payload;
      break;
    case fieldPayloadLoadAddress:
      payload->load_address = uint32_t (it);
      break;
    case fieldPayloadEntryPoint:
      payload->entry_point = uint32_t (it);
      break;
    case fieldPayloadType:
      payload->type = uint8_t (it);
      break;
    case fieldPayloadDescription:
      payload->description = (char*) it.data ();
      break;
    case fieldPayloadCompression:
      printf ("Payload Compression:  gzip\n");
      break;
    case fieldLinuxKernelArchitectureID:
      if (!architecture_id)
        architecture_id = uint32_t (it);
      break;
    default:
      break;                    // It's OK to skip unknown tags
    }
  }
  delete payload;
}


/** Inserts payload into the image at the given index, or merges the
    payload into the one already present in the list.  Responsibility
    for the payload object is assumed by this function.  It is assumed
    (and required) that the last payload object in the list is the
    source image, so that image is always guaranteed to be last.
    Thus, if there are two images in the list and we're asked to merge
    at index 1, we insert the given payload between the two already in
    the list.  Also, it is not permitted to insert a new payload
    anywhere except for just before the final entry in the list. */

void Image::merge_payload (int index, Payload* payload)
{
  if (index < 0)
    throw Exception ("index less than zero");
  if (index >= size ())
    throw Exception ("index too large");

//  printf ("index %d size %d\n", index, size ());

  if (index < size () - 1) {        // Bonafide merge
    iterator it = begin ();
    for (; index; --index)
      ++it;
    **it += *payload;
    delete payload;
  }
  else {                        // Insert new payload
    iterator it = begin ();
    for (; index; --index)
      ++it;
    insert (it, payload);
  }
}


/** Show and possibly verify an image.  This function requires that an
    image be 'loaded' in that the tags are read from the file and the
    data stored in the image structures.  This means that unrecognized
    tags will be ignored and discarded.
*/

void Image::describe (struct arguments& args)
{
  if (header.size () < 16)
    throw Exception ("unable to describe empty header");

  size_t cbHeader = header.size ();

  if ((args.verbose || args.dry_run) && cbHeader) {
    const void* pv = header.c_str ();
    printf ("Signature:               0x%02x 0x%02x 0x%02x 0x%02x\n",
            ((uint8_t*)pv)[0], ((uint8_t*)pv)[1],
            ((uint8_t*)pv)[2], ((uint8_t*)pv)[3]);
    printf ("Header Size:             %d (%d bytes)\n",
            ((uint8_t*)pv)[4], ((uint8_t*)pv)[4]*16);
  }

  if (description)
    printf ("Image Description:       '%s' (%d bytes)\n", description,
            strlen (description) + 1);
  if (timeCreation)
    printf ("Image Creation Date:     %s", asctime (localtime (&timeCreation)));
  if (architecture_id)
    printf ("Linux Kernel Arch ID:    %d (0x%x)\n",
            architecture_id, architecture_id);

  char sz[20];
  strcpy (sz, "Payload    ");
  bool multi_payload = (size () - (cbHeader ? 1 : 0) > 1);

  int i = 0;
  for (iterator it = begin (); it != end (); ++it, ++i) {
    if (cbHeader && *it == back ())
      continue;

    if (multi_payload)
      snprintf (sz, sizeof (sz), "Payload #%d ", i);

    struct Payload& payload = **it;

    if (payload.description) {
      printf ("%sDescription:  '%s'\n", sz, payload.description,
              strlen (payload.description) + 1);
      if (multi_payload)
        strcpy (sz, "           ");
    }
    if (payload.type) {
      printf ("%sType:         %s\n", sz,
              describe_image_type (payload.type));
      if (multi_payload)
        strcpy (sz, "           ");
    }
    if (payload.load_address != ~0) {
      printf ("%sLoad Address: 0x%08lx\n", sz, payload.load_address);
      if (multi_payload)
        strcpy (sz, "           ");
    }
    if (payload.entry_point != ~0) {
      printf ("%sEntry Point:  0x%08lx\n", sz, payload.entry_point);
      if (multi_payload)
        strcpy (sz, "           ");
    }
    if (payload.cb) {
      printf ("%sLength:       %s\n", sz, describe_size (payload.cb));
      if (multi_payload)
        strcpy (sz, "           ");
    }
    if (payload.pv) {
      uint32_t crc = compute_crc32 (0, payload.pv, payload.cb);
      crc = compute_crc32_length (crc, payload.cb);
      crc = ~crc;
      printf ("%sCRC:          0x%08x (%u) ", sz,
              payload.crc_loaded ? payload.crc_loaded : crc,
              payload.crc_loaded ? payload.crc_loaded : crc);
      if (payload.crc_loaded) {
        if (crc == payload.crc_loaded)
          printf ("OK\n");
        else
          printf ("!= 0x%08x ERROR\n", crc);
      }
      else
        printf ("\n");
      if (multi_payload)
        strcpy (sz, "           ");
    }
  }

  if (cbHeader && (args.verbose || args.dry_run)) {
    uint32_t crc = compute_crc32 (0, header.c_str (), cbHeader - 4);
    uint32_t header_crc;
    memcpy (&header_crc, header.c_str () + cbHeader - 4, 4);
    header_crc = swabl (header_crc);
    bool ok = crc == header_crc;

    printf ("Header CRC:              0x%08x", header_crc);
    if (ok)
      printf (" OK\n");
    else
      printf (" != 0x%08x ERROR\n", crc);
  }
}

void apeximage (struct arguments& args)
{
	// Open files and construct pads
  {
    int i = 0;
    for (Image::iterator it = g_image.begin ();
         it != g_image.end (); ++it, ++i) {
      if (!(*it)->szPath) {
        if (args.mode == modeUpdate)
          continue;
        throw Exception ("no path for payload #%d", i + 1);
      }
      (*it)->open ();
    }
  }

  Payload& payloadIn  = *g_image.front ();
  Payload& payloadOut = *g_image.back ();

#if 0
  printf ("fileIn %s (%d B header)\n", payloadIn.szPath,
          payloadIn.header_size ());
  printf ("fileOut %s (%d B header)\n", payloadOut.szPath,
          payloadOut.header_size ());
#endif

  switch (args.mode) {
  case modeShow:
    g_image.load (args);
    g_image.describe (args);
    return;
    break;

  case modeUpdate:
    g_image.load (args);
    g_image.timeCreation = time (NULL);

    if (args.verbose > 1)
      for (Image::iterator it = g_image.begin (); it != g_image.end (); ++it) {
        std::cout << **it << std::endl;
        if ((*it)->pv)
          dumpw ((char*) (*it)->pv, 32, 0, 0);
      }

    break;

  case modeCreate:
    if (payloadOut.fh != -1 && !args.force && !args.dry_run)
      throw Exception ("unable to write file '%s', file exists",
                       payloadOut.szPath);
    break;
  }

  g_image.build_header (args);

  if (args.dry_run || args.verbose)
    g_image.describe (args);

	// At this point, we're committed to constructing a new image
	// and writing it to the output file.  The preexisting output
	// file will be clobbered if there is one.

  if (!args.quiet)
    printf ("# %s image '%s'%s\n",
            args.mode == modeCreate ? "Creating" : "Updating",
            payloadOut.szPath,
            args.dry_run ? "; dry-run, no output files produced" : "");

  if (args.dry_run)
    return;

	// Save the new image to a temporary filename
  size_t cbPathSave = strlen (payloadOut.szPath) + 32;
  char* szPathSave = (char*) alloca (cbPathSave);
  snprintf (szPathSave, cbPathSave, "%s~%d~", payloadOut.szPath, getpid ());
  int fhSave = open (szPathSave, O_WRONLY | O_TRUNC | O_CREAT, 0666);
  if (fhSave == -1)
    throw Exception ("unable to create output file '%s'", szPathSave);

  ::write (fhSave, g_image.header.c_str (), g_image.header.size ());

	// Write the payloads
  int i = 0;
  for (Image::iterator it = g_image.begin (); it != g_image.end (); ++it, ++i) {
    if ((*it) == &payloadOut)
      break;
    uint32_t crc = 0;
    crc = compute_crc32 (crc, (*it)->pv, (*it)->cb);
    crc = compute_crc32_length (crc, (*it)->cb);
    crc = ~crc;
    ::write (fhSave, (*it)->pv, (*it)->cb);
//    if (args.verbose)
//      printf ("Pyld #%d CRC:          0x%08x\n", i, crc);
    crc = swabl (crc);
    ::write (fhSave, &crc, sizeof (crc));

    for (int cbPad = 16 - (((*it)->cb + 4) & 0xf); cbPad--; ) {
      unsigned char b = 0xff;
      ::write (fhSave, &b, 1);
    }
  }

  close (fhSave);

	// Release all files that could be the same as the output
  char szPathOut[strlen (payloadOut.szPath) + 1];
  strcpy (szPathOut, payloadOut.szPath);
  payloadIn.release_this ();
  payloadOut.release_this ();

  if (rename (szPathSave, szPathOut)) {
      // I'm not sure it is possible to get this error since we've
      // already been able to write a temporary file to the same directory.
    unlink (szPathSave);
    throw ResultException (errno, "unable to rename '%s' to '%s' (%s)",
                           szPathSave, szPathOut, strerror (errno));
  }
}

int main (int argc, char** argv)
{
  struct arguments args;
  argp_parse (&argp, argc, argv, ARGP_IN_ORDER, 0, &args);

  try {
    apeximage (args);
  }
  catch (Exception& ex) {
    printf ("%s\n", ex.sz);
  }

  return 0;
}
