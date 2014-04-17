/* exception.h

   written by Marc Singer
   22 Feb 2008

   Copyright (C) 2008 Marc Singer

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__EXCEPTION_H__)
#    define   __EXCEPTION_H__

/* ----- Includes */

#include <stdarg.h>

/* ----- Types */

struct Exception {
  char* sz;

  void initialize (const char* szFmt, va_list ap) {
    size_t cb = 256;
    sz = new char[cb];
    vsnprintf (sz, cb, szFmt, ap); }

  Exception () : sz (NULL) {}
  Exception (const char* szFmt, ...) {
    va_list ap;
    va_start (ap, szFmt);
    initialize (szFmt, ap);
    va_end (ap); }
  ~Exception () {
    if (sz)
      delete sz; }
};

struct ResultException : public Exception {
  int result;
  ResultException (int _result, const char* szFmt ...) {
    result = _result;
    va_list ap;
    va_start (ap, szFmt);
    initialize (szFmt, ap);
    va_end (ap); }
};


/* ----- Globals */

/* ----- Prototypes */



#endif  /* __EXCEPTION_H__ */
