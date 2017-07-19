// @(#) $Id: OpeLShift.h,v 1.3 2003/03/08 20:41:10 able Exp $
// o p e r a t o r < < datatype frequently
OPELSHIFT(ostream &c)
OPELSHIFT(const char *c)
OPELSHIFT(const int c)
OPELSHIFT(const unsigned int c)
OPELSHIFT(const I32_T c)
OPELSHIFT(const F64_T c)
#ifdef __BORLANDC__
 OPELSHIFT2(smanip_fill<char, char_traits<char> > c) //e.g. setfill(char)
#elif __GNUG__ < 3
# define ios ios_base
 OPELSHIFT(ios &(*c)(ios &))          //e.g. hex
#else
# define ios ios_base
 OPELSHIFT(ios &(*c)(ios &))          //e.g. hex
 OPELSHIFT(_Setfill<char> c)          //e.g. setfill
 OPELSHIFT(_Setprecision c)           //e.g. setprecision
#endif
#ifdef __BORLANDC__
#undef ios
#endif

#ifndef _MSC_VER
# if __GNUG__ < 3
 OPELSHIFT(smanip<int> c)             //e.g. setw(int)
# else
 OPELSHIFT(_Setw c)                   //e.g. setw(int)
# endif
#else
  OPELSHIFT(SMANIP(int) c)             //e.g. setw(int)
#endif

#ifndef OPELSHIFT_X
 OPELSHIFT(ostream &(*c)(ostream &))  //e.g. endl
#else
 OPELSHIFT_X(ostream &(*c)(ostream &))  //e.g. endl
#endif
