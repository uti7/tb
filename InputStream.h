// @(#) $Id: InputStream.h,v 1.3 2001/01/12 10:13:33 able Exp $
#ifndef __InputStream_h__
#define __InputStream_h__

#include "Generic.h"
#include "Message.h"
#include "StringOf.h"
#if 0
#include "OutputStream.h"
#endif

class InputStream {
public:
  InputStream() :
	isStdin_(0),
	isOpen_(0),
	ofs_(0) {}
  bool isOpen() { return isOpen_; }
  bool eof() { return (ifs().get() == EOF); }
  void open(const char *fn = 0);
  ~InputStream(){}

bool  bad(){
    return ifs().bad();
  }

char get(){
  ofs_++;
  return ifs().get();
}

char peek() { return ifs().peek(); }

bool isLittleEndian()
{
   int x = 1;
   if(*(char *)&x == 1) return 1;
  return 0;
}

	bool jump(signed j);

String<LETTER_T> &name() { return name_; }

bool get(I32_T &v, int &n);
bool get(I16_T &v, int &n);
bool get(I8_T &v, int &n);
bool get(F64_T &v, int &n);
bool get(const unsigned l, String<LETTER_T> &s, int &n);

  unsigned ofs() { return ofs_; }

  bool getToken(String<char> &);
  
  OutputStream &cout() { return *os_; }

  void  skipProlog(); // 2.0b27
private:
  size_t getl(String<char> &);  //2.0b27
#if __GNUG__ < 3
  istream &ifs() { return ifs_; }
#else
  istream &ifs() { if(isStdin_){ return cin; } return ifs_; }
#endif
  unsigned int ofs_;
  bool  isOpen_;
  ifstream ifs_;
  String<LETTER_T>  name_;
  bool  isStdin_;
  Ptr<OutputStream> os_;
};

#endif  //!__InputStream_h__
