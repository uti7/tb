// @(#) $Id: Generic.h,v 1.9 2003/03/08 20:41:10 able Exp $
#ifndef __Generic_h__
#define __Generic_h__

#ifndef _MSC_VER
#include "@.h"
#else
#include <windows.h>
#include <iostream.h>
#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>
#include <strstrea.h>
#include <stdiostr.h>
#include <errno.h>
#include <assert.h>
#include <locale.h>
#include <stdio.h>
#define SP_API
#define SP_HAVE_BOOL
#define __FUNCTION__  "unknown"
#define istdiostream  stdiostream

#pragma warning(disable:4800)  //warning C4800: 'int' : ÌÞ°Ù’l‚ð 'true' ‚Ü‚½‚Í 'false' ‚É‹­§“I‚ÉÝ’è‚µ‚Ü‚· (Œx‚Ìˆ—)


#endif

typedef unsigned long	I32_T;
typedef unsigned short	I16_T;
typedef unsigned char	I8_T;
typedef double			F64_T;
typedef char LETTER_T;
typedef signed OFF_T;
typedef long	SI32_T;
typedef short	SI16_T;
typedef char	SI8_T;

#define APP_STRLEN(s)        strlen((const char*)s)
#define APP_STRCMP(s1,s2)    strcmp((const char*)s1, (const char*)s2)
#define APP_MEMCHR           memchr
#if defined(_MSC_VER)
#define APP_POPEN			_popen
#define APP_PCLOSE			_pclose
#elif !defined(__BORLANDC__)
#define APP_POPEN			popen
#define APP_PCLOSE			pclose
#else
#define APP_POPEN			_popen
#define APP_PCLOSE			_pclose
#endif
#if __GNUG__ >= 3
#define APP_MEMSET          memset
#endif

//#define NEWLINE_SEQ  (LETTER_T *)"\n"
#define NEWLINE_SEQ  endl
#define LETTER_WS    (LETTER_T *)" "     // white space
#define LETTER_QUOTE (LETTER_T *)"\""
#define LETTER_ES    (LETTER_T *)"\\" //esc-seq
#define LETTER_CR    (LETTER_T *)"\r"
#define LETTER_LF    (LETTER_T *)"\n"
#define LETTER_HTAB  (LETTER_T *)"\t"
#define LETTER_UNF   (LETTER_T *)"$"
#define LETTER_USC   (LETTER_T *)"_"
  //unfold separator(parameter substitution)
#define SEQ_CR       (LETTER_T)'r'
#define SEQ_LF       (LETTER_T)'n'
#define SEQ_SPC      (LETTER_T)'s'
#define SEQ_HTAB     (LETTER_T)'t'
#define SEQ_ESCCHAR  (LETTER_T)'\\'
#define SEQ_QUOTE    (LETTER_T)'"'
#define SEQ_OCTNUM   (LETTER_T)'0'
#define SEQ_HEXNUM   (LETTER_T)'x'
#define LETTER_XCL   (LETTER_T *)"`"
  //call separator(command substitution)
// built in variable
#define UNFOLD_FILENAME (LETTER_T *)"FILENAME"
#define UNFOLD_NR    (LETTER_T *)"NR"
#define UNFOLD_NF    (LETTER_T *)"NF"
#define UNFOLD_CNF   (LETTER_T *)"CNF"
#define UNFOLD_CNFR  (LETTER_T *)"CNFR"
#define UNFOLD_VAL   (LETTER_T *)"V"
//2.0b26
#define UNFOLD_SNR  (LETTER_T *)"SNR"
#define UNFOLD_NFR  (LETTER_T *)"NFR"
//2.0b30
#define UNFOLD_VREF  (LETTER_T *)"$"

#define MIN_IT(current, other)	if ((other) < (current)) (current) = (other)
////////////////////////////////////////////////////////////////////////
#define BTTA_REV "R2.0b33"
#ifdef __GNUG__
# define BTTA_COMPILED  "(gcc)"
#elif defined(__BORLANDC__)
# define BTTA_COMPILED  "(bcc)"
#else
#  define BTTA_COMPILED "(other)"
#endif

#define BTTA_PURPOSE  "bin2txta - formatted dump\n"
#define BTTA_GUIDE  "Type `btta -h' for more information about options.\n"
#define BTTA_USAGE0 \
"Usage: btta [-v] [-{E|e}] [-F char ] [-k column] [-f format-file]\n            [-{x|X}] [-record-format [...]] [file]\n"
#define BTTA_USAGE4 \
"  -record-format: [record-count]member[menber...]\n"
#define BTTA_USAGE4_0 \
"                      (if not given, then `s0x8' (like as \"od\" with -x opt.))\n"
#define BTTA_USAGE5 \
"     record-count: literal-number\n"
#define BTTA_USAGE5_0 \
"                   or ref-mark\n                   (if not given, then infinite)\n"
#define BTTA_USAGE6 \
"       member: output format of a member of record.\n"
#define BTTA_USAGE6_0 \
"               [mark]format[base][repeat-count][comment]\n"
#define BTTA_USAGE6_1 \
"               or nested-record-format\n"
#define BTTA_USAGE7 \
"       mark: uses this value of member for some other record-count,\n"
#define BTTA_USAGE7_0 \
"             or repeat-count. (`@'identifier)\n"
#define BTTA_USAGE8 \
"       format: member width and output form(required),\n"
#define BTTA_USAGE8_0 \
"               one of following char\n"
#define BTTA_USAGE9 \
"         `c': 8bit integer\n"
#define BTTA_USAGE10 \
"         `s': 16bit integer\n"
#define BTTA_USAGE11 \
"         `l': 32bit integer\n"
#define BTTA_USAGE12 \
"         `t': 8/16 bit letter(s)\n"
#define BTTA_USAGE12_0 \
"              (if no printable, then apply `c0x' by force)\n"
#define BTTA_USAGE13 \
"         `z': 8/16 bit null end letter(s)\n"
#define BTTA_USAGE13_0 \
"              (if no printable, then It as it)\n"
#define BTTA_USAGE14 \
"         `n': output new-line(no access input stream)\n"
#define BTTA_USAGE15 \
"       base: `0x' (hex), `0'(oct), or `(none)'(dec)\n"
#define BTTA_USAGE16 \
"       repeat-count: only decimal number([0-9]+) or ref-mark\n"
#define BTTA_USAGE16_0 \
"                     (if not given, then 1 time)\n"
#define BTTA_USAGE17 \
"       ref-mark: to use record-count or repeat-count\n"
#define BTTA_USAGE17_0 \
"                 for value of marked member by mark (`$'identifier)\n"
#define BTTA_USAGE18 \
"       comment: `#'your comment...`;'\n"
#define BTTA_USAGE19 \
"  literal-number: dec: [0-9]+, oct: 0[0-7]+, hex: 0x[0-9a-fA-F]+\n"
#define BTTA_USAGE20 \
"  identifier: any single character\n"
#define BTTA_USAGE21 \
"  nested-record-format: `['record-format`]'\n"
////////////////////////////////////////////////////////////////////////

#define STRDUP(__dest__,__src__) \
  assert(__src__); __dest__ = new unsigned char[strlen(__src__)+1]; \
  strcpy((char*)__dest__, __src__)
#define STRDES(__target__) if(__target__) delete [] __target__

#define DEBUG_ASSERT(cond)  debug_assert(cond)

#ifdef DEBUG
inline void  debug_assert(bool cond) { assert(cond); }
#else
inline void  debug_assert(bool cond) { }
#endif

#if 0
#if SunOS
#define LOCALE "japanese"
#elif HP_UX
#define LOCALE "ja_JP.eucJP"
#elif Linux
#define LOCALE "ja_JP.ujis"
#elif UNIX_SV
#define LOCALE "japan"
#else
#define LOCALE "POSIX"
#endif
#endif

#define DEFAULT_MAXBUFSIZE  1024
#endif /* __Generic_h__ */
