// @(#) $Id: GenArg.h,v 1.2 2000/10/21 08:11:52 able Exp $
#ifndef __GenArg_h__
#define __GenArg_h__

#include "Generic.h"
#include "Ptr.h"
#if 0
#include "StringOf.h"
#include "Record.h"
#endif


class GenArg {
  public:
    struct _MarkTbl {
      _MarkTbl() : refpos_((unsigned)-1), val_((unsigned)-1) {}
      unsigned  refpos_;
      unsigned  val_;
    };

    GenArg() : verbose_(0), nCCol_(0), argInfo_(0),
        isNoSubstitute_(DoSubstitute) {
      delm_.assign(LETTER_WS,APP_STRLEN(LETTER_WS)+1);
      char *p = getenv("MAXBUFSIZE");
      if(p){
        maxBufSize_ = strtol(p,0,0);
      }else{
        maxBufSize_ = DEFAULT_MAXBUFSIZE;
      }
    }
    ~GenArg() {}
    Ptr<Record> &rootRec() { return recForm_; }
    void setF(LETTER_T *c) { delm_.assign(c,APP_STRLEN(c)+1); }
    void setV() { verbose_ = 1; };
    void setI() { argInfo_ = 1; };
    void setCCol(int n) { nCCol_ = n; };
    void setX(unsigned n) { isNoSubstitute_ = n; };
    unsigned mkval(unsigned char c) { return mkt_[c].val_; }
    unsigned mkpos(unsigned char c) { return mkt_[c].refpos_; }
    void setMark(unsigned char c,unsigned v, unsigned ofs){
      mkt_[c].val_ = v;
      mkt_[c].refpos_ = ofs;
    }
    bool isV() { return verbose_; }
    bool isArgInfo() { return argInfo_; }
    int &nCCol() { return nCCol_; }
    unsigned isNoSubstitute() { return isNoSubstitute_; }
    enum { DoSubstitute,
           NoSubstituteCommand,
           NoSubstituteParameterAndCommand };
    String<LETTER_T> &delm() { return delm_; }

    int maxBufSize(){return maxBufSize_;}

    class Endian {
      public:
        enum { BIG,LITTLE };
        Endian(){
          //exa system
          int i = 0x1;
          if( *(char *)&i == 1)
            sys_ = LITTLE;
          else
            sys_ = BIG;
          file_ = sys_;
        }
        void setE(int e) { file_ = e; }
        bool isNative() { return (sys_ == file_); }
        int  system() { return sys_; }
      private:
        int sys_;
        int file_;
    } endian;
    Ptr<Member> &markedRec(int n) { return markedRec_[n]; }  //20b11
  private:
    bool      verbose_;
    bool      argInfo_;
    int  nCCol_;
    unsigned  isNoSubstitute_;
    String<LETTER_T>      delm_;
    Ptr<Record> recForm_;
    _MarkTbl  mkt_[256];
    Ptr<Member> markedRec_[256];  //20b11
    int maxBufSize_;
};


extern GenArg ga;

#endif /* __GenArg_h__ */

