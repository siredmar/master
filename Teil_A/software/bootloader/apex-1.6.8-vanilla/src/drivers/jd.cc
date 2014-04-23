/* jd.cc
     $Id$

   written by Marc Singer
   17 May 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Dumper for jffs2 filesystems.  Much of the internal structure of
   this code is designed to emulate the structure of the drv-jffs2
   module.  Don't take offense.

   Truncated Files
   ---------------

   We may need more logic to handle file truncation.  If the size of
   the file has been reduced, it is possible that there are file
   extent fragments that don't reflect this new length.  There will be
   a later version inode to indicate the proper truncated length, but
   intermediate inode records may still exist.  This deserves some
   testing to be certain.

   Checksums
   ---------

   We aren't checking many of the checksums.  This must change.  All
   of them should be verified before data is considered valid.

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <string.h>
#include <ctype.h>
#include <linux/stat.h>

#define ENDIAN_SWAP

#define MARKER_JFFS2_OLD	0x1984
#define MARKER_JFFS2		0x1985
#define MARKER_JFFS2_REV	0x5981	/* Wrong endian magic */
#define MARKER_EMPTY		0xffff
#define MARKER_DIRTY		0x0000

#define NAME_LENGTH_MAX		254
#define DATA_LENGTH_MIN		128	/* Smallest data node */

#define JFFS2_ROOT_INO		1

enum {
  COMPRESSION_NONE	 = 0x00,
  COMPRESSION_ZERO	 = 0x01,
  COMPRESSION_RTIME	 = 0x02,
  COMPRESSION_RUBINMIPS	 = 0x03,
  COMPRESSION_COPY	 = 0x04,
  COMPRESSION_DYNRUBIN	 = 0x05,
  COMPRESSION_ZLIB	 = 0x06,
  COMPRESSION_LZO	 = 0x07,
  COMPRESSION_LZARI	 = 0x08,
};

#define COMPATIBILITY_MASK	0xc000
#define NODE_ACCURATE		0x2000
#define FEATURE_INCOMPAT	0xc000
#define FEATURE_ROCOMPAT	0xc000
#define FEATURE_RWCOMPAT_COPY	0x4000
#define FEATURE_RWCOMPAT_DELETE	0x0000

#define NODE_DIRENT		(FEATURE_INCOMPAT        | NODE_ACCURATE | 1)
#define NODE_INODE		(FEATURE_INCOMPAT        | NODE_ACCURATE | 2)
#define NODE_CLEAN		(FEATURE_RWCOMPAT_DELETE | NODE_ACCURATE | 3)
//#define NODE_CHECKPOINT	(FEATURE_RWCOMPAT_DELETE | NODE_ACCURATE | 3)
//#define NODE_OPTIONS		(FEATURE_RWCOMPAT_COPY	 | NODE_ACCURATE | 4)
//#define NODETYPE_DIRENT_ECC	(FEATURE_INCOMPAT	 | NODE_ACCURATE | 5)
//#define NODETYPE_INODE_ECC	(FEATURE_INCOMPAT	 | NODE_ACCURATE | 6)

#define INODE_FLAG_PREREAD	1 /* Read at mount-time */
#define INODE_FLAG_USERCOMPR	2 /* User selected compression */

	// dirent types, taken from the kernel sources
#define DT_UNKNOWN	0
#define DT_FIFO		1
#define DT_CHR		2
#define DT_DIR		4
#define DT_BLK		6
#define DT_REG		8
#define DT_LNK		10
#define DT_SOCK		12
#define DT_WHT		14

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

#if defined (ENDIAN_SWAP)
static u32 u32_to_cpu(u32 v) {
  return
      ((v &       0xff) << 24)
    | ((v &     0xff00) <<  8)
    | ((v &   0xff0000) >>  8)
    | ((v & 0xff000000) >> 24);
}

static u16 u16_to_cpu(u16 v) {
  return
      ((v &       0xff) << 8)
    | ((v &     0xff00) >>  8);
}

#else
static u32 u32_to_cpu(u32 v) { return v; }
static u16 u16_to_cpu(u16 v) { return v; }
#endif

struct unknown_node
{
  u16 marker;
  u16 node_type;
  u32 length;
  u32 header_crc;
} __attribute__((packed));

struct dirent_node
{
  u16 marker;
  u16 node_type;		/* NODETYPE_DIRENT */
  u32 length;
  u32 header_crc;
  u32 pino;
  u32 version;
  u32 ino;
  u32 mctime;
  u8 nsize;
  u8 type;
  u8 unused[2];
  u32 node_crc;
  u32 name_crc;
  u8 name[0];
} __attribute__((packed));

struct inode_node
{
  u16 marker;
  u16 node_type;		/* NODETYPE_INODE */
  u32 length;
  u32 header_crc;
  u32 ino;
  u32 version;    /* Version number.  */
  u32 mode;       /* The file's type or mode.  */
  u16 uid;        /* The file's owner.  */
  u16 gid;        /* The file's group.  */
  u32 isize;      /* Total size of this inode (used for truncations)  */
  u32 atime;      /* Last access time.  */
  u32 mtime;      /* Last modification time.  */
  u32 ctime;      /* Change time.  */
  u32 offset;     /* Where to begin to write.  */
  u32 csize;      /* (Compressed) data size */
  u32 dsize;	  /* Size of the node's data. (after decompression) */
  u8 compr;       /* Compression algorithm used */
  u8 usercompr;	  /* Compression algorithm requested by the user */
  u16 flags;	  /* See JFFS2_INO_FLAG_* */
  u32 data_crc;   /* CRC for the (compressed) data.  */
  u32 node_crc;   /* CRC for the raw inode (excluding data)  */
  //  u8 data[dsize];
} __attribute__((packed));

union node {
	struct inode_node i;
	struct dirent_node d;
	struct unknown_node u;
} __attribute__((packed));

struct dirent_cache {
  u32 ino;
  u32 pino;
  u32 version;
  u32 index_dirent;
  u32 sizeof_dirent;		// *** should change to name size
  u8 type;			// Important accelerator
//  struct dirent_node* dirent;
};

struct inode_cache {
  u32 ino;
  u32 version;
  u32 offset;
  u32 dsize;
  u32 index_inode;
//  struct inode_node* inode;
};

// Globals in order to emulate structure of the APEX driver.

const int cDirentMax = 4*1024;
const int cInodeMax = 4*1024;
struct dirent_cache dirent_cache[cDirentMax];
struct inode_cache  inode_cache[cInodeMax];
int iDirent;
int iInode;

void hexdump (const unsigned char* rgb, int cb, unsigned long index)
{
  int i;

  while (cb > 0) {
    printf ("%08lx: ", index);
    for (i = 0; i < 16; ++i) {
      if (i < cb)
	printf ("%02x ", rgb[i]);
      else
	printf ("   ");
      if (i%8 == 7)
	putchar (' ');
    }
    for (i = 0; i < 16; ++i) {
      if (i == 8)
	putchar (' ');
      putchar ( (i < cb) ? (isprint (rgb[i]) ? rgb[i] : '.') : ' ');
    }
    printf ("\n");

    cb -= 16;
    index += 16;
    rgb += 16;
  }
}

unsigned long compute_crc32 (unsigned long crc, const void *pv, int cb)
{
#define POLY        (0xedb88320)
  const unsigned char* pb = (const unsigned char *) pv;

  crc ^= 0xffffffff;

  while (cb--) {
    int i;
    crc ^= *pb++;

    for (i = 8; i--; ) {
      if (crc & 1) {
	crc >>= 1;
	crc ^= POLY;
      }
      else
	crc >>= 1;
    }
  }

  return crc ^ 0xffffffff;
}

static bool verify_header_crc (const struct unknown_node& node)
{
  return u32_to_cpu (node.header_crc)
    == ~compute_crc32 (~0, &node, sizeof (struct unknown_node) - 4);
}

static bool verify_dirent_crc (const struct dirent_node& node)
{
  return u32_to_cpu (node.node_crc)
    == ~compute_crc32 (~0, &node, sizeof (struct dirent_node) - 8);
}

static bool verify_inode_crc (const struct inode_node& node)
{
  return u32_to_cpu (node.node_crc)
    == ~compute_crc32 (~0, &node, sizeof (struct inode_node) - 8);
}

static bool verify_crc (const void* pv, size_t cb, u32 crc)
{
  return crc == ~compute_crc32 (~0, pv, cb);
}

void usage (void)
{
  printf ("usage: jd FILENAME\n");
  exit (1);
}

const int find_cached_inode (u32 inode, size_t ib = 0)
{
  int min = 0;
  int max = iInode;

  while (min + 1 < max) {
    int mid = (min + max)/2;

    if (inode > inode_cache[mid].ino) {
      min = mid + 1;
      continue;
    }

    if (inode < inode_cache[mid].ino
	|| ib < inode_cache[mid].offset) {
      max = mid;
      continue;
    }

	/* == inode and ib is >= offset */
    if (  (ib == 0 && ib == inode_cache[mid].offset)
	|| ib < inode_cache[mid].offset + inode_cache[mid].dsize)
      return mid;

    min = mid + 1;
  }

  return (inode_cache[min].ino == inode) ? min : -1;
}

/* find_cached_directory_node

   searches for an inode in the dirent cache.  This array is sorted by
   parent inodes, so this search has to be an O(n) scan of the
   array...for now.

*/

const int find_cached_directory_inode (u32 inode,
				       const struct dirent_cache* dc,
				       int max)
{
  for (int i = 0; i < max; ++i)
    if (dc[i].ino == inode)
      return i;
  return -1;
}

const int find_cached_parent_inode (u32 inode,
				    const struct dirent_cache* dc,
				    int max)
{
  int min = 0;

  while (min + 1 < max) {
    int mid = (min + max)/2;
    if (dc[mid].pino == inode) {
      while (mid > 0 && dc[mid - 1].pino == inode)
	--mid;
      min = mid;
      break;
    }
//    if (dc[mid].ino == 0)
//      max = mid;
    /*    else */ if (dc[mid].pino < inode)
      min = mid;
    else
      max = mid;
  }

  return (dc[min].pino == inode) ? min : -1;
}

int compare_dirent_cache (const void* _a, const void* _b)
{
  struct dirent_cache& a = *(struct dirent_cache*) _a;
  struct dirent_cache& b = *(struct dirent_cache*) _b;

  if (a.pino == b.pino) {
    if (a.ino == b.ino)
      return b.version - a.version;
    return a.ino - b.ino;
  }
  return a.pino - b.pino;
}

int compare_inode_cache (const void* _a, const void* _b)
{
  struct inode_cache& a = *(struct inode_cache*) _a;
  struct inode_cache& b = *(struct inode_cache*) _b;

  if (a.ino == b.ino) {
    if (a.offset == b.offset)
      return b.version - a.version;
    return a.offset - b.offset;
  }
  return a.ino - b.ino;
}

void read_node (void* node, const void* pv, size_t cb,
		size_t ib, size_t cbRead = 0)
{
  fflush (stdout);
  if (!cbRead)
    cbRead = sizeof (union node);
  cb -= ib;
  if (cbRead > cb)
    cbRead = cb;
  memcpy (node, (const unsigned char*) pv + ib, cbRead);
}

void scan (const void* pv, size_t cb)
{
  struct dirent_cache* dc = dirent_cache;
  struct inode_cache* ic = inode_cache;

  size_t ib;			// Index into the filesystem data

  union node _node;
  union node* node = &_node;
  int cbNode;

  for (ib = 0; ib < cb; ib = (ib + cbNode + 3) & ~3) {

    read_node (node, pv, cb, ib);

    cbNode = 4;

    switch (u16_to_cpu (node->u.marker)) {
    case MARKER_JFFS2:
      break;
    case MARKER_JFFS2_REV:
      // this is really only valid for reading the first marker of the block
      printf ("endian mismatch @%x\n", ib);
      if (ib == 0)
	return;
      continue;
    default:
//      printf ("marker %x type %x length %x\n",
//	      u16_to_cpu (node->u.marker),
//	      u16_to_cpu (node->u.node_type),
//	      u16_to_cpu (node->u.length));
      continue;
    }

    if (!verify_header_crc (node->u)) {
#if 0
      printf ("bad header for  marker %x type %x length %x\n",
	      u16_to_cpu (node->u.marker),
	      u16_to_cpu (node->u.node_type),
	      u16_to_cpu (node->u.length));
#endif
      continue;
    }

    cbNode = u32_to_cpu (node->u.length);

//    printf ("type %x\n", u16_to_cpu (node->u.node_type));
    switch (u16_to_cpu (node->u.node_type)) {
    case NODE_DIRENT:
      if (!verify_dirent_crc (node->d)) {
	printf ("dirent_crc error\n");
	break;
      }

      {
	char sz[node->d.nsize];
	read_node (sz,  pv, cb, ib + sizeof (struct dirent_node),
		   node->d.nsize);
	if (!verify_crc (sz, node->d.nsize, u32_to_cpu (node->d.name_crc))) {
	  printf ("name crc failure\n");
	}
      }

      dc[iDirent].ino = u32_to_cpu (node->d.ino);
      dc[iDirent].pino = u32_to_cpu (node->d.pino);
      dc[iDirent].version = u32_to_cpu (node->d.version);
      dc[iDirent].index_dirent = ib;
      dc[iDirent].sizeof_dirent = sizeof (struct dirent_node) + node->d.nsize;
      dc[iDirent].type = node->d.type;
      ++iDirent;
      break;

    case NODE_INODE:
      if (!verify_inode_crc (node->i)) {
	printf ("inode_crc error\n");
	break;
      }

      ic[iInode].ino = u32_to_cpu (node->i.ino);
      ic[iInode].offset = u32_to_cpu (node->i.offset);
      ic[iInode].dsize = u32_to_cpu (node->i.dsize);
      ic[iInode].version = u32_to_cpu (node->i.version);
      ic[iInode].index_inode = ib;
      ++iInode;
      break;;

    default:
      break;
    }
  }

  qsort (dc, iDirent, sizeof (struct dirent_cache), compare_dirent_cache);
  qsort (ic, iInode,  sizeof (struct inode_cache),  compare_inode_cache);
}

void dump (const void* pv, size_t cb)
{
  struct dirent_cache* dc = dirent_cache;
  struct inode_cache*  ic = inode_cache;

  printf ("%d directory nodes  %d inode nodes\n", iDirent, iInode);

  for (int i = 0; i < iDirent; ++i) {
    char rgb[dc[i].sizeof_dirent];
    struct dirent_node& d = *(dirent_node*) rgb;
    read_node (rgb, pv, cb, dc[i].index_dirent, dc[i].sizeof_dirent);

    printf ("i %4d  p %4d  v %4d t %c '%*.*s'\n",
	    dc[i].ino, dc[i].pino, dc[i].version,
	    "ufc?d?b?r?l?s?w"[d.type], d.nsize, d.nsize, d.name);
  }

  {
    int inode = -1;
    for (int i = 0; i < iInode; ++i) {
      struct inode_node ino;
      read_node (&ino, pv, cb, ic[i].index_inode, sizeof (struct inode_node));

      int index = -1;
      if (inode != ic[i].ino) {
	printf ("%4d", inode = ic[i].ino);
	index = find_cached_directory_inode (ic[i].ino, dc, iDirent);
      }
      else
	printf ("    ");
      printf (" o %6d l %4d v %4d %c (s %5d)",
	      ic[i].offset, ic[i].dsize, ic[i].version,
	      "n0rMcDzLA"[ino.compr],
	      u32_to_cpu (ino.isize));
      if (index != -1) {
	char rgb[dc[index].sizeof_dirent];
	struct dirent_node& d = *(dirent_node*) rgb;
	read_node (rgb, pv, cb,
		   dc[index].index_dirent, dc[index].sizeof_dirent);

	printf ("  '%*.*s'", d.nsize, d.nsize, d.name);
      }
      printf ("\n");
      if (ino.compr == COMPRESSION_NONE && ic[i].dsize < 128 && ic[i].dsize) {
	int cbRead = ic[i].dsize;
	if (cbRead > 128)
	  cbRead = 128;
	unsigned char rgb[cbRead];
	read_node (rgb, pv, cb,
		   ic[i].index_inode + sizeof (struct inode_node),
		   cbRead);
	hexdump (rgb, cbRead, ic[i].offset);
      }

    }
  }
}

void old_dump (const void* pv, size_t cb)
{
  union node* node;
  int cbNode;

  for (node = (union node*) pv;
       (size_t) node < (unsigned long) pv + cb;
       node = (union node*) ((((unsigned long) node) + cbNode + 3) & ~3)) {
    if (node->u.marker != MARKER_JFFS2) {
      cbNode = 4;
      continue;
    }
    if (!verify_header_crc (node->u)) {
      printf ("bad header\n");
      cbNode = 4;
      continue;
    }

    cbNode = node->u.length;

    switch (node->u.node_type) {
    case NODE_DIRENT:
      printf ("dir: pino %4d  version %4d  ino %4d"
	      "  nsize %2d  type %d  '%*.*s'\n",
	      node->d.pino, node->d.version, node->d.ino, node->d.nsize,
	      node->d.type, node->d.nsize, node->d.nsize, node->d.name);
      break;
    case NODE_INODE:
      printf ("ino:  ino %4d  version %4d  offset %6d size %4d/%4d  "
	      "compr %d",
	      node->i.ino, node->i.version, node->i.offset,
	      node->i.csize, node->i.dsize, node->i.compr);
      if (node->i.flags)
	printf ("  flags %d", node->i.flags);
      printf ("\n");
      break;
    default:
      printf ("marker %0x  node_type %0x  length %x (%d)\n",
	      node->u.marker, node->u.node_type,
	      node->u.length, node->u.length);
    }

  }

}

int find_ino (void* pv, size_t cb, const char* szPath)
{
  int cchPath = szPath ? strlen (szPath) : 0;
  if (!cchPath) {
    printf ("empty path\n");
    return 0;
  }

  if (szPath[0] != '/') {
    printf ("relative path is invalid\n");
    return 0;
  }

  char sz[strlen (szPath + 1) + 1];
  strcpy (sz, szPath + 1);
  char* context = NULL;

  int ino = JFFS2_ROOT_INO;

  char* pch;
  for (pch = strtok_r (sz,   "/", &context); pch;
       pch = strtok_r (NULL, "/", &context)) {
    int i = find_cached_parent_inode (ino, dirent_cache, iDirent);
    if (i == -1) {
      printf ("path not found\n");
      return 0;
    }
    for (; i < iDirent; ++i) {
      if (dirent_cache[i].sizeof_dirent - sizeof (dirent_node)
	  != strlen (pch))
	continue;

      char rgb[dirent_cache[i].sizeof_dirent];
      struct dirent_node& d = *(dirent_node*) rgb;
      read_node (rgb, pv, cb,
		 dirent_cache[i].index_dirent,
		 dirent_cache[i].sizeof_dirent);
      if (strncmp (pch, (const char*) d.name, d.nsize))
	continue;

      // Matching node found
      ino = u32_to_cpu (d.ino);
      break;
    }

    printf ("'%s'\n", pch);
  }
  printf ("inode %d\n", ino);
  return ino;
}

void show_ino (void* pv, size_t cb, int ino)
{
  u32 mode;

  if (ino != 1) {
    int index = find_cached_inode (ino);
    if (index == -1) {
      printf ("no inode %d\n", ino);
      return;
    }

    struct inode_node inode;
    read_node (&inode, pv, cb, inode_cache[index].index_inode,
	       sizeof (struct inode_node));

    mode = u32_to_cpu (inode.mode);

    printf ("  m %08o  o %ld  c %ld  d %ld  s %ld\n",
	    u32_to_cpu (inode.mode), u32_to_cpu (inode.offset),
	    u32_to_cpu (inode.csize), u32_to_cpu (inode.dsize),
	    u32_to_cpu (inode.isize));
    if (S_ISDIR (mode))
      printf ("    directory\n");
    if (S_ISREG (mode))
      printf ("    regular file\n");
    if (S_ISLNK (mode))
      printf ("    link\n");
    if (S_ISCHR (mode))
      printf ("    char dev\n");
    if (S_ISBLK (mode))
      printf ("    blk dev\n");
  }

  if (S_ISDIR (mode) || ino == 1) {
    int i = find_cached_parent_inode (ino, dirent_cache, iDirent);
    for (; i != -1 && i < iDirent && dirent_cache[i].pino == ino; ++i) {
      if (dirent_cache[i].ino == 0)
	continue;

      char rgb[dirent_cache[i].sizeof_dirent];
      struct dirent_node& d = *(dirent_node*) rgb;
      read_node (rgb, pv, cb,
		 dirent_cache[i].index_dirent,
		 dirent_cache[i].sizeof_dirent);

      printf ("  %*.*s", d.nsize, d.nsize, d.name );
      switch (dirent_cache[i].type) {
      case DT_DIR:
	printf ("/");
	break;
      case DT_LNK:
	printf ("@");
	break;
      case DT_REG:
	break;
      default:
	printf ("(%d)", dirent_cache[i].type);
	break;
      }
      printf ("\n");
    }
  }

}


int main (int argc, char** argv)
{
  if (argc > 3)
    usage ();

  int fh = open (argv[1], O_RDONLY);
  size_t cb = lseek (fh, 0, SEEK_END);
  void* pv = NULL;
  if (fh != -1)
    pv = mmap (NULL, cb, PROT_READ, MAP_PRIVATE, fh, 0);

  if (!pv) {
    printf ("unable to open and map file %s\n", argv[1]);
    exit (1);
  }

  char* szPath = NULL;
  if (argc == 3)
    szPath = argv[2];

  scan (pv, cb);
  if (szPath) {
    int ino = find_ino (pv, cb, szPath);
    show_ino (pv, cb, ino);
  }
  else
    dump (pv, cb);

  exit (0);
}
