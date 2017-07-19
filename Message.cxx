// @(#) $Id: Message.cxx,v 1.3 2003/03/08 20:41:10 able Exp $
#include "@.h"
#include "Generic.h"
#include "Message.h"

void Message::setInfo(const char *f, int l, const char *fn, MsgType mt)
{
  memset(file_, 0, MAX_PATH);
  strncpy(file_, f, MAX_PATH - 1);
  memset(func_, 0, MAX_PATH);
  strncpy(func_, fn, MAX_PATH - 1);
  memset(str_, 0, MAX_MSGLEN);
  line_ = l;
  type_ = mt;
  outstr_.seekp(0);
  outstr_ << ends;
}

Message &Message::operator<<(const Message::MsgType &t)
{
  char work[MAX_MSGLEN];
  memset(work,0,MAX_MSGLEN);

  int i = 0;

  switch(t){
  case USAGE:
    out1or2() << BTTA_USAGE0 << BTTA_GUIDE;
    break;
  case HELP:
    for(/*int*/ i = 0;usageStr_[i] != 0; i++){
      out1or2() << usageStr_[i];
    }
    break;
  case WARN:
  case FATAL:
    outstr_ << "btta: " << typeName_[t & ~IsStdErr] << ":";
    out1or2() <<  "btta: " << typeName_[t & ~IsStdErr] << ":";
    break;
  case NOTE:
    outstr_ << "# btta: " << typeName_[t & ~IsStdErr] << ":";
    out1or2() << "# btta: " << typeName_[t & ~IsStdErr] << ":";
#if 0
    outstr_ << "#" << file_ << ":" <<  line_ << ":" 
            << func_ << ":" << typeName_[t & ~IsStdErr] << ":";
    out1or2() << "#" << file_ << ":" <<  line_ << ":" 
              << func_ << ":" << typeName_[t & ~IsStdErr] << ":";
#endif
	break;
  case DEBUGONLY:
#ifdef DEBUG
    outstr_ << "#" << file_ << ":" <<  line_ << ":" 
            << func_ << ":" << typeName_[t & ~IsStdErr] << ":";
    out1or2() << "#" << file_ << ":" <<  line_ << ":" 
              << func_ << ":" << typeName_[t & ~IsStdErr] << ":";
#endif
    break;
  default:
    assert(0);
  }
  return *this;
}

#ifdef DEBUG
#define OPELSHIFT(_TYPE) \
Message &Message::operator<<(_TYPE )\
{\
  outstr_ << c;\
  out1or2() << c;\
  return *this;\
}
#define OPELSHIFT2(_TYPE1,_TYPE2)\
Message &Message::operator<<(_TYPE1,_TYPE2)\
{\
  outstr_ << c;\
  out1or2() << c;\
  return *this;\
}
#include "OpeLShift.h"

#else //DEBUG
#define OPELSHIFT(_TYPE) \
Message &Message::operator<<(_TYPE )\
{\
  if(type_ != DEBUGONLY){\
    outstr_ << c;\
    out1or2() << c;\
  }\
  return *this;\
}
#define OPELSHIFT2(_TYPE1,_TYPE2)\
Message &Message::operator<<(_TYPE1,_TYPE2)\
{\
  if(type_ != DEBUGONLY){\
    outstr_ << c;\
    out1or2() << c;\
  }\
  return *this;\
}
#include "OpeLShift.h"
#endif	//DEBUG
#undef OPESHIFT
#undef OPESHIFT2
