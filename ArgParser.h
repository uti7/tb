// @(#) $Id: ArgParser.h,v 1.3 2001/02/05 00:11:22 able Exp $
#ifndef __ArgParser_h__
#define __ArgParser_h__

#include "Generic.h"
#if 0
#include "GenArg.h"
#include "InputStream.h"
#endif

class ArgParser {
  public:
    ArgParser() { }
    ~ArgParser() { }
    bool go(int , char **, Ptr<Record> &, InputStream &);
    Ptr<Member> makeRecForm(char *);
  private:
    bool  formatFromFile(const char *, Ptr<Record> &, InputStream &);
  class ParseLevel {
    public:
      ParseLevel() { l_++; }
      ~ParseLevel() { l_--; }
      static int level() { return l_; }
    private:
      static int  l_;
  };

};

class FormatString { ////////////// : istrstream {
  public:
    FormatString() : cur_(0), str_(0) {level++;}
    FormatString(char *s);
    void set(char *s);
    ~FormatString() { STRDES(str_); level--; }
    Ptr<Member> getNum();
    int getMember(Ptr<Member> &, Ptr<Member> &, ArgParser &);
    bool getMark(char &);
    void setRepeat(RepeatMember *);
    void setJumpOffset(JumpMember *);
    Member::BaseType getSizeAndBaseType(
      Ptr<Member> &,Ptr<Member> &, ArgParser &);
    bool getComment(Ptr<Member> &);
    size_t getWidthSpec(bool &);
    unsigned cur() {return cur_; }

    static int  level;
    void print(const char *);
  private:
    char  cc() const { return str_[cur_]; }
    unsigned char  *cs() const { return &str_[cur_]; }
    unsigned char  *ff(unsigned char *p){
      cur_ = (uint64_t)p - (uint64_t)str_;
      return cs();
    }
    bool getStringConstant(String<LETTER_T> &);
    unsigned char  *str_;
    uint64_t cur_;
};


#endif //!__ArgParser_h__
