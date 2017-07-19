// @(#) $Id: Member.h,v 1.6 2003/03/08 20:41:10 able Exp $
#ifndef __Member_h__
#define __Member_h__

#ifdef __GNUG__
#pragma interface
#endif

#include "Generic.h"
#include "Resource.h"
#include "Ptr.h"
#include "Vector.h"
#include "StringOf.h"
#if 0
#include "InputStream.h"
#endif

class InputStream;
class ArgParser;
////////////////////////////////////////////////////////////////////////
#define MANIP_I32UD prefix_.data() << setw(10) << setfill(*(LETTER_WS)) << dec
#define MANIP_I32D  prefix_.data() << setw(10) << setfill(*(LETTER_WS)) << dec
#define MANIP_I32O  prefix_.data() << setw(10) << setfill('0') << oct
#define MANIP_I32H  prefix_.data() << setw(8) << setfill('0') << hex
#define MANIP_I16UD prefix_.data() << setw(5) << setfill(*(LETTER_WS)) << dec
#define MANIP_I16D  prefix_.data() << setw(5) << setfill(*(LETTER_WS)) << dec
#define MANIP_I16O  prefix_.data() << setw(6) << setfill('0') << oct
#define MANIP_I16H  prefix_.data() << setw(4) << setfill('0') << hex
#define MANIP_I8UD prefix_.data() << setw(3) << setfill(*(LETTER_WS)) << dec
#define MANIP_I8D  prefix_.data() << setw(3) << setfill(*(LETTER_WS)) << dec
#define MANIP_I8O  prefix_.data() << setw(3) << setfill('0') << oct
#define MANIP_I8H  prefix_.data() << setw(2) << setfill('0') << hex
#define MANIP_F64  prefix_.data() << setw(16) << setfill(*(LETTER_WS)) \
 << setprecision(16) << dec
#define MANIP_VARLET  prefix_.data() << dec
#define MANIP_JUMPH  MANIP_I32H
#define MANIP_JUMPO  MANIP_I32O
#define MANIP_JUMPD  MANIP_I32D
#define BITPREFIX (LETTER_T *)"c0x"
#define MANIP_BITS  BITPREFIX << setw(2) << setfill('0') << hex
#define MANIP_OFFSET  "# 0x" << setw(8) << setfill('0') << hex
#define MANIP_COFFSET  "0x" << setw(8) << setfill('0') << hex

//for value string  (\$V in comment)
#define S_MANIP_I32UD dec
#define S_MANIP_I32D  dec
#define S_MANIP_I32O  setw(10) << setfill('0') << oct
#define S_MANIP_I32H  setw(8) << setfill('0') << hex
#define S_MANIP_I16UD dec
#define S_MANIP_I16D  dec
#define S_MANIP_I16O  setw(6) << setfill('0') << oct
#define S_MANIP_I16H  setw(4) << setfill('0') << hex
#define S_MANIP_I8UD dec
#define S_MANIP_I8D  dec
#define S_MANIP_I8O  setw(3) << setfill('0') << oct
#define S_MANIP_I8H  setw(2) << setfill('0') << hex
#define S_MANIP_F64 setw(16) << setfill(*(LETTER_WS)) \
 << setprecision(16) << dec

////////////////////////////////////////////////////////////////////////
#define INIT_BUILTIN_VAR \
 snr_(0), nr_(0), nf_(0), nfr_(0), cnf_(0), cnfr_(0)

////////////////////////////////////////////////////////////////////////
class FormatString;
class RecordMember;
class RepeatMember;
class TokenLetterMember;
class JumpMember;

////////////////////////////////////////////////////////////////////////
inline char *cLit(char c)
{
  static char s[2];
  s[0] = c;
  s[1] = '\0';
  return s;
}

////////////////////////////////////////////////////////////////////////
class Member : public Resource {
public:
  enum BaseType {     //  including token type
    UNKNOWN             = 0000,
    UNSIGNDED_DEC       = 0101,
    SIGNED_DEC          = 0111,
    OCT                 = 0102,
    HEX                 = 0104,
    VAR_LETTER          = 0121,
    FIXED_LETTER        = 0122,
    TOKEN_LETTER        = 0124,
    NESTED_REC          = 0040,
    END_NESTED_REC      = 0041,
    REC                 = 0042,
    JUMP                = 0200,
    INDIFFERENT         = 0201,
    NOTACCESS           = 0300,
    DOUBLE              = 0511,

    IS_SIGNED           = 0010,
    IS_LETTER           = 0020,
    IS_NEST_TOK         = 0040,
    HAS_REPEAT          = 0100,
    IS_NOTACCESS        = 0200,
    IS_DOUBLE           = 0400,
  };
  Member() {}
  Member(BaseType b)
    : refMark_(0), size_(0), mark_(0), base_(b), ofs_(0),
      cond_(0), hasCond_(0),
      INIT_BUILTIN_VAR {}

  virtual ~Member();
  Member(unsigned s, BaseType b, LETTER_T *c = 0)
    : refMark_(0), size_(s), mark_(0), base_(b), ofs_(0),
      cond_(0), hasCond_(0),
      INIT_BUILTIN_VAR {
    if(c){
      comment_.assign(c, APP_STRLEN(c)+1);
    }
  }
  void setComment(LETTER_T *c) {
    comment_.assign(c, APP_STRLEN(c)+1);
  }
  void outComment(InputStream &);
  void unfoldComment(String<LETTER_T> &, InputStream &);
  void getReplacedComment(
    String<LETTER_T> &,
    size_t &,
    String<LETTER_T> &,
    InputStream &);
//2.0b10
  void externalCallComment(String<LETTER_T> &, String<LETTER_T> &);
  void getExecComment(
    String<LETTER_T> &, size_t &, String<LETTER_T> &);
  void setMarkBase(unsigned, unsigned);
  unsigned  size() const { return size_; }
  virtual void setBase(BaseType t) { base_ = t; }
  virtual void setBase(LETTER_T *p, BaseType t) { assert(0); }
//  virtual unsigned  &repeat() { assert(0); }
  virtual bool process(InputStream &,
      size_t = 0, size_t = 0, size_t = 0, size_t = 0) {
    assert(0); return 0; };
//  virtual void      out(InputStream &) { assert(0); };
  virtual RecordMember *asRecord() { return 0; }
  virtual RepeatMember *asRepeat() {return 0; }
  virtual TokenLetterMember *asTokenLetter() { return 0; }
  virtual JumpMember   *asJump() { return 0; }
  virtual void append(Ptr<Member> &) { assert(0); }
  bool isSigned() const { return (bool)(base_ & IS_SIGNED); }
  bool isLetter() const { return (bool)(base_ & IS_LETTER); }
  unsigned resolveRefNum(char);
  virtual void  outRemainder(InputStream &, unsigned char *,unsigned) { assert(0); }
  BaseType  whatis() { return base_; }
protected:
friend class FormatString;
  unsigned size_;
  BaseType base_;
  unsigned char     mark_;
  String<LETTER_T>  comment_;
  unsigned char     refMark_;
  unsigned          ofs_;
  bool              hasCond_;
  unsigned int      cond_;    // of integer value of boolean type 
  size_t            nr_;      // number of record
  size_t            nf_;      // number of field
  size_t            cnf_;     // current number of field
  size_t            cnfr_;    // current number of repeat of field
  //2.0b26
  size_t            snr_;     // sum of number of record
  size_t            nfr_;     // sum of number of repeat of field
  String<LETTER_T>  strVal_;
};


////////////////////////////////////////////////////////////////////////
class RecordMember : public Member {
  public:
    RecordMember();
    RecordMember(unsigned);
    ~RecordMember() {}
    bool process(InputStream &,
            size_t = 0, size_t = 0, size_t = 0, size_t = 0);
    void append(Ptr<Member> &m);
    RecordMember *asRecord() { return this; }
  private:
friend class FormatString;
    unsigned              nums_;    // of rec
    unsigned              width_;   //  of rec
    bool                  isRound_;  // 2.0b28
    Vector<Ptr<Member> >  sub_;
    unsigned              fsSize_;
};
 

inline
void RecordMember::append(Ptr<Member> &m) {
  size_ += m->size();
  sub_.push_back(m);
}

////////////////////////////////////////////////////////////////////////
class RepeatMember : public Member {
  friend class FormatString;
  public:
    RepeatMember() : repeat_(0),width_(0),isRound_(0) {}
    RepeatMember(LETTER_T *p, unsigned s, BaseType b, LETTER_T *c = 0);
    virtual ~RepeatMember() {}
    bool process(InputStream &,
          size_t = 0, size_t = 0, size_t = 0, size_t = 0);
    virtual bool out(InputStream &) { assert(0); return 0; }
    RepeatMember  *asRepeat() { return this; }
    void      setBase(LETTER_T *pfx){
      prefix_.append(pfx,APP_STRLEN(pfx)+1);}
    void      setBase(LETTER_T *b, BaseType t){
      prefix_.append(b,APP_STRLEN(b)+1); Member::setBase(t); }
  protected:
    void  outRemainder(InputStream &, unsigned char *,unsigned);
    unsigned          repeat_;
    unsigned          width_;   //  of rec
    bool              isRound_;  // 2.0b28
    String<LETTER_T>  prefix_;
};

////////////////////////////////////////////////////////////////////////
class IntMember : public RepeatMember {
  public:
    IntMember() {}
    IntMember(LETTER_T *p, unsigned s, BaseType b, LETTER_T *c = 0)
      : RepeatMember(p, s,b,c) {}
    ~IntMember() {}
//    virtual void     out() const {}
};

////////////////////////////////////////////////////////////////////////
class I32Member : public IntMember {
  public:
    I32Member() {}
    I32Member(LETTER_T *, BaseType b, LETTER_T *c = 0);
    ~I32Member() {}
    bool     out(InputStream &);
    void  setMark() { setMarkBase((unsigned)val_, ofs_); }
  private:
    I32_T val_;
};

////////////////////////////////////////////////////////////////////////
class I16Member : public IntMember {
  public:
    I16Member();
    I16Member(LETTER_T *, BaseType b, LETTER_T *c = 0);
    ~I16Member() {}
    bool     out(InputStream &);
    void  setMark() { setMarkBase((unsigned)val_, ofs_); }
  private:
    I16_T val_;
};

////////////////////////////////////////////////////////////////////////
class I8Member : public IntMember {
  public:
    I8Member();
    I8Member(LETTER_T *, BaseType b, LETTER_T *c = 0);
    ~I8Member() {}
    bool     out(InputStream &);
    void  setMark() { setMarkBase((unsigned)val_, ofs_); }
  private:
    I8_T val_;
};

////////////////////////////////////////////////////////////////////////
class F64Member : public RepeatMember {
  public:
    F64Member() {};
    F64Member(LETTER_T *, BaseType b, LETTER_T *c = 0);
    ~F64Member() {}
    bool     out(InputStream &);
    void  setMark() { setMarkBase((unsigned)val_, ofs_); }
  private:
    F64_T val_;
};

////////////////////////////////////////////////////////////////////////
class LetterMember : public RepeatMember {
  public:
    LetterMember() {};
    LetterMember(LETTER_T *, BaseType b, LETTER_T *c = 0);
    ~LetterMember() {}
    virtual bool     out(InputStream &);
    void  setMark() { setMarkBase(0, ofs_); }
  protected:
    virtual String<LETTER_T> refine();
    String<LETTER_T> val_;
};

////////////////////////////////////////////////////////////////////////
class VarLetterMember : public LetterMember {
  public:
    VarLetterMember() {}
    VarLetterMember(LETTER_T *, BaseType b, LETTER_T *c = 0);
    ~VarLetterMember() {}
    String<LETTER_T> refine();
};

////////////////////////////////////////////////////////////////////////
class TokenLetterMember : public LetterMember {
  public:
    TokenLetterMember() {}
    TokenLetterMember(LETTER_T *, BaseType b, LETTER_T *c = 0);
    ~TokenLetterMember() {}
    bool     out(InputStream &);
    // String<LETTER_T> refine();
    TokenLetterMember  *asTokenLetter() { return this; }
  protected:
friend class FormatString;
    String<LETTER_T> delimiter_; // R2.0b34
};

////////////////////////////////////////////////////////////////////////
class NotAccessMember : public LetterMember {
  public:
    NotAccessMember(LETTER_T *pfx, LETTER_T *c = 0);
    ~NotAccessMember() {}
    bool     out(InputStream &);
};

////////////////////////////////////////////////////////////////////////
class JumpMember : public Member {
  public:
    JumpMember(LETTER_T *pfx, LETTER_T *c = 0);
    ~JumpMember() {}
    bool process(InputStream &,
      size_t snr = 0, size_t nr = 0, size_t nf = 0, size_t cnf = 0);
    virtual bool     out(InputStream &);
    JumpMember *asJump() { return this; }
  protected:
friend class FormatString;
    String<LETTER_T>  prefix_;
    unsigned width_;
    bool     isRound_;  // 2.0b28
    OFF_T    val_;		// seek offset
};

////////////////////////////////////////////////////////////////////////
class IndifferentMember : public JumpMember {
  public:
    IndifferentMember(LETTER_T *pfx, ArgParser &ap, bool isAbs,
      LETTER_T *c = 0);
    ~IndifferentMember() {}
    bool     out(InputStream &);
  protected:
friend class FormatString;
    ArgParser &ap_;
    Ptr<Member> waste_;
    bool  isAbsolute_;
};

#endif  //! __Member_h__
