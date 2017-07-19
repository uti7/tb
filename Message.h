// @(#) $Id: Message.h,v 1.2 2000/10/21 08:11:52 able Exp $
#ifndef __Message_h__
#define __Message_h__

#define E_W Message::WARN
#define E_U Message::USAGE
#define E_F Message::FATAL
#define N_N Message::NOTE
#define E_D Message::DEBUGONLY
#define E_H Message::HELP

#define MSG(__type__) \
  _msg.setInfo(__FILE__,__LINE__,__FUNCTION__,__type__); \
  _msg << __type__

#define MAX_MSGLEN  1024

#ifndef MAX_PATH
#define MAX_PATH  256
#endif

class Message {
  public:
    enum MsgType {  //under 3 columns must be sequential val
      WARN       = 01000,
      USAGE      = 01001,
      FATAL      = 01002,
      NOTE       = 00003,
      DEBUGONLY  = 01004,
      HELP       = 01005,
      NLIMIT     = 00777,
      IsStdErr   = 01000,
    };
    Message() : line_(0), outstr_(str_, MAX_MSGLEN), type_(NOTE) {
//      str_ = new char[MAX_MSGLEN];
    }
    ~Message();
    void setInfo(const char *f,int l,const char *fn, MsgType t);
#define OPELSHIFT(_TYPE)		Message &operator<<(_TYPE);
#define OPELSHIFT2(_TYPE,_TYPE2)	Message &operator<<(_TYPE,_TYPE2);

OPELSHIFT(const Message::MsgType &)

#include "OpeLShift.h"
#undef OPELSHIFT
#undef OPELSHIFT2

  private:
    ostream &out1or2()
      { return (type_&IsStdErr) ? cerr : cout; }
    static const char *typeName_[];
    static const char *usageStr_[];
    char file_[MAX_PATH];
    char func_[MAX_PATH];
    char str_[MAX_MSGLEN];
    int line_;
    MsgType type_;
    ostrstream outstr_;
};

inline
Message::~Message() {} ///////{ if(str_) delete [] str_; }


#define _MSG  Message _msg;
extern Message _msg;


#ifdef SP_DEFINE_TEMPLATES
/*static*/
const char *Message::typeName_[] = {
      "WARN",
      "USAGE",
      "FATAL",
      "NOTE",
      "DEBUG",
      "OPTION",
    };
const char *Message::usageStr_[] = {
"bin2txta - formatted dump\n",
BTTA_USAGE0,
"  -v: verbosely print \n       file offset, warning etc...\n",
"  -e(-E): indicates whether integer value that 16bit and greater in file\n         should be processed little(big) endian(default: native)\n",
"  -F: delimiter spec, char: character(string) for delimiter(e.g. `,') \n",
"  -k column: alignment of comment column(default: no alignment)\n",
"  -f: specifies a record-format(s) descripted file\n",
"  -x: no substitute external command in comment\n",
"  -X: no substitute external command and built-in variable in comment\n",
0
};

#endif //SP_DEFINE_TEMPLATES

#endif //!__Message_h__
