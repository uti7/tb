// @(#) $Id: OutputStream.h,v 1.3 2003/03/08 20:41:10 able Exp $
#ifndef __OutputStream_h__
#define __OutputStream_h__

#include "Generic.h"
#include "Resource.h"
#include "Owner.h"
#include "StringOf.h"
#if 0
#include "GenArg.h"
#endif

// COUT hooker

// basic
class OutputStream : public Resource {
	public:
    OutputStream() {}
    virtual ~OutputStream() {}
    virtual void pad(){}
    virtual void flush(bool b = 1){}
#define OPELSHIFT(_TYPE)	\
  virtual OutputStream &operator<<(_TYPE) { cout << c; return *this; }
#define OPELSHIFT2(_TYPE,_TP2)	\
virtual OutputStream &operator<<(_TYPE, _TP2) {\
  cout << c; return *this; }

#include "OpeLShift.h"

#undef OPELSHIFT
#undef OPELSHIFT2

	private:
		OutputStream(OutputStream &);	// undefined
};


//
//
//
class WMOutputStream : public OutputStream {
	public:
		WMOutputStream(int col = 0) : col_(col), os_(new ostrstream) {
    }
		virtual ~WMOutputStream() {
      if(os_ && (*os_).pcount()){
        flush(0);
      }
    }

#define OPELSHIFT(_TYPE)	OutputStream &operator<<(_TYPE);
#define OPELSHIFT2(_TYPE, _TP2)	OutputStream &operator<<(_TYPE, _TP2);

#include "OpeLShift.h"

#undef OPELSHIFT
#undef OPELSHIFT2

    void pad();
    void flush(bool isReNew = 1){
      *os_ << ends;
      cout << (*os_).str();
      cout.flush();
#ifndef _MSC_VER
      (*os_).freeze(0);
#else
       (*os_).rdbuf()->freeze(0);
#endif
      if(isReNew){
        os_.clear();
        os_ = new ostrstream;
      }
    }

	private:
		int col_;
    Owner<ostrstream> os_;
};

#ifdef SP_DEFINE_TEMPLATES

void WMOutputStream::pad()
{
  for(size_t i = (*os_).pcount();i < col_; i++){
	*os_ << LETTER_WS;
  }
}

#define OPELSHIFT(_TYPE) \
OutputStream &WMOutputStream::operator<<(_TYPE){\
  *os_ << c;\
  return *this;\
}
#define OPELSHIFT2(_TYPE,_TYPE2) \
OutputStream &WMOutputStream::operator<<(_TYPE,_TYPE2){\
  *os_ << c;\
  return *this;\
}
#define OPELSHIFT_X(_TYPE) \
OutputStream &WMOutputStream::operator<<(_TYPE){\
  *os_ << c;\
  if((*os_).pcount()){\
    flush();\
  }\
  return *this;\
}

#if 0
cerr<<"DEBUG:"<< #_TYPE << "," << #_TYPE2 <<endl;
cerr<<"\tDEBUG:os_="<< (*os_).str() <<endl;
cerr<<"\t\tDEBUG:str_="<< str_.data() <<endl;
#endif

#include "OpeLShift.h"

#undef OPELSHIFT
#undef OPELSHIFT2
#undef OPELSHIFT_X

#endif	// SP_DEFINE_TEMPLATES

#endif  //!__OutputStream_h__
