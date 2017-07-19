// @(#) $Id: ArgParser.cxx,v 1.10 2003/03/08 20:41:09 able Exp $
//#ifdef __GNUG__
//#pragma implementation
//#endif

#include "Generic.h"
#include "Member.h"
#include "Record.h"
#include "GenArg.h"
#include "ArgParser.h"
#include "Message.h"
#include "OutputStream.h"
#include "InputStream.h"

int ArgParser::ParseLevel::l_ = 0;

bool ArgParser::go(int ac, char **av, Ptr<Record> &parent, InputStream &is)
{
  ParseLevel  plv;
  bool isErr = 0;
  for(int i = 1; i < ac && !isErr; i++){
    if(av[i][0] == '-'){
      switch(av[i][1]){
        case 'v': ga.setV(); break; // like a tar
        case 'i': ga.setI(); break; // out arg info(format file debugging)
        case 'e': ga.endian.setE(GenArg::Endian::LITTLE); break;
        case 'E': ga.endian.setE(GenArg::Endian::BIG); break;
        case 'x': ga.setX(GenArg::NoSubstituteCommand); break;
        case 'X': ga.setX(GenArg::NoSubstituteParameterAndCommand); break;
        case 'k':	// justify comment
        {
          char *p;
          if(av[i][2])
            p = &av[i][2];
          else{
            i++;
            if(i >= ac){
              isErr = 1;
              break;
            }
            p = av[i];
          }
          int n;
          istrstream optArg(p, 0);
          optArg >> n;
          ga.setCCol(n);
        }
          break;
        case 'F': //like a awk
        {
          char *p;
          if(av[i][2])
            p = &av[i][2];
          else{
            i++;
            if(i >= ac){
              isErr = 1;
              break;
            }
            p = av[i];
          }
          ga.setF(p);
        }
          break;

        case 'h': //like almost commend
          MSG(E_H) << BTTA_REV << BTTA_COMPILED << endl; // help
          return 0; //bye

        case 'f': // like a awk
        {
          i++;
          if(i >= ac){
            isErr = 1;
            break;
          }
          // record-format-arg from file
          if(parent.isNull()){
            parent = new Record(strlen(av[i]));
          }
          if(!formatFromFile(av[i], parent, is)){
            isErr = 1;
            MSG(E_F) << i << "th argument(0-n) was wrong." << endl;
            break;
          }
          break;
        }

        default:
        {
          if(parent.isNull()){
            parent = new Record(strlen(av[i]));
          }
          Ptr<Member> rec = makeRecForm(av[i]);
          if(rec.isNull()){
            isErr = 1; 
            MSG(E_F) << i << "th argument(0-n) was wrong." << endl;
            break;
          }
          parent->append(rec);
          break;
        }
      }
    }else{
      if(i < (ac -1)){
        MSG(E_W) << av[i] << ": assume a name of input-file."
        << endl;
      }
      if(plv.level() == 1){
        is.open(av[i]);
        if(is.bad()){
          MSG(E_F) << av[i] << ":cannot open" << endl;  //2.0b7
          isErr = 1;
        }
      }
      break;  //for-loop
    }
  }

  if(ga.isArgInfo()){
    for(int ii = 0; ii<ac;ii++){
      //MSG(N_N) << "parse level=" << plv.level() << ": "
      MSG(E_W) << "parse level=" << plv.level() << ": "
               << "argv[" << ii << "]:[" << av[ii]<<"]"<<endl;
    }
  }

  if(plv.level() == 1){
    if(isErr){
      MSG(E_U) << BTTA_REV << BTTA_COMPILED << endl;
      return 0; //fatal
    }
  //
  //  d e f a u l t  f o r m a t
  //
  if(ga.rootRec().isNull()){
#ifdef MORE_DEBUG
    MSG(E_D) << "no format." << endl;
#endif
    ga.rootRec() = new Record(5);
    Ptr<Member> rec = makeRecForm((char *)"-s0x8");
    DEBUG_ASSERT(!rec.isNull());
    ga.rootRec()->append(rec);
  }

    if(is.isOpen()){
      return 1;
    }

    is.open();  // apply stdin
    if(is.bad()) return 0;    //fatal
  }

  return 1;
}

bool ArgParser::formatFromFile(const char *fname, Ptr<Record> &parent,
  InputStream &is)
{
  InputStream argFile;
  argFile.open(fname);
  if(argFile.bad()){
    MSG(E_F) << fname << ": cannot open" << endl;
    return 0;
  }
  String<char> tok;
  String<char *> av;

  av += (char *)"Dummy";  //cause av[0] is ignored
  argFile.skipProlog();
  String<char>  oneOfav;  //2.0b27
  for(; argFile.getToken(tok); tok.resize(0)){
    if(tok[0] == '-'){  //new format

      // store previous oneOfav to one av
      if(oneOfav.size()){
        char *p = new char[oneOfav.size()+1];
        ostrstream os(p,oneOfav.size()+1);
        oneOfav[oneOfav.size()] = '\0';
        os << oneOfav.data() << ends;
        av += p;
        oneOfav.resize(0);
        //delete [] p;
      }
      oneOfav.append(tok.data(), 1);  //(i.e `-')

#if 0
      //nested -f file
      if(argFile.peek() == 'f') { //nest file
MSG(E_D) << "peek ->f" <<endl;
        oneOfav += argFile.get();  //(i.e. `f')
MSG(E_D) << "f -> oneOfav" <<endl;
        oneOfav += '\0';
MSG(E_D) << "oneOfav is " << oneOfav.data() <<endl;
        {
        char *p = new char[oneOfav.size()];
        ostrstream os(p,oneOfav.size());
        os << oneOfav.data() << ends;
        av += p;
        delete [] p;
        }
        oneOfav.resize(0);

        if(argFile.getToken(oneOfav)){
          char *p = new char[oneOfav.size()];
          ostrstream os(p,oneOfav.size());
          os << oneOfav.data() << ends;
          av += p;
          delete [] p;
          oneOfav.resize(0);
        }else {}  //cannot get filename, but leave it
       
      }
#endif
    }else{
      oneOfav.append(tok.data(), tok.size());
    }
  }

  //
  //  make a @rgv
  //
  if(oneOfav.size()){ // bit of last
    char *p = new char[oneOfav.size()+1];
    ostrstream os(p,oneOfav.size()+1);
    oneOfav[oneOfav.size()] = '\0';
    os << oneOfav.data() << ends;
    oneOfav.resize(0);
    av += p;
  }
  
  //
  //  parse all aRGV
  //
  bool ret;
  if(av.size()>1){
    char **pp = new char *[av.size()];
    memcpy(pp, av.data(), sizeof(char *) * av.size());
    ret = go(av.size(), pp, parent, is);
    delete [] pp;
  }

  //
  //  delete each aRGV
  //
  for(int i = 1; i < (int)av.size(); i++){
#ifdef MORE_DEBUG
    MSG(E_D) << "av[" << i << "]:" << av[i] << endl;
#endif
    delete [] av[i];
  }
  return ret;
}

Ptr<Member> ArgParser::makeRecForm(char *av)
{
  bool isErr = 0;
  int j;
  FormatString fs;
    fs.set(av);
    Ptr<Member> rec = fs.getNum();
    if(rec.isNull()){   //fatal
      MSG(E_F) << "unrecognized token (n of rec).\n";
      isErr = 1;
      return rec;
    }

    for(j = 1; !isErr; j++){
      Ptr<Member> mem;
      int ret = fs.getMember(mem, rec, *this);
      if(ret == 0) break;                 //finish
      if(ret == -1){        //fatal
        MSG(E_F) << "unrecognized token (member).\n";
        isErr = 1;
        break;
      }
      rec->append(mem);
    }
  if(isErr){
    MSG(E_F) << "invalid record-format.\n"
      << "            " << fs.cur() << "th character(0-n)\n"
      << "            " << j << "th member(1-n)\n"
      << "            " << "nest level(i.e. [nested-rec...],1-n):"
                              << FormatString::level << "\n"
      << "            " << "parse level(1-n):"
                              << ParseLevel::level() << endl;
    fs.print("            ");
    rec.clear();
  }
  return rec;
}

//  static
int FormatString::level = 0;

FormatString::FormatString(char *s)
{
  cur_ = 0;
  STRDES(str_);
  if(s[0] == '-'){
    STRDUP(((str_)), &s[1]);
  }else{
    STRDUP(str_,s);
  }
  level++;
}

void FormatString::set(char *s)
{
  cur_ = 0;
  STRDES(str_);
  if(s[0] == '-'){
    STRDUP(str_,&s[1]);
  }else{
    STRDUP(str_,s);
  }
}

Ptr<Member> FormatString::getNum()
{
  char *p;
  Ptr<Record> rec(new Record);
  //2.0b11
  if(cc() == '*'){  // rec saving
    cur_++;
    if(!cc()){
      MSG(E_W) << "no variable name, after `*' (rec saving)."
               << endl;
    }else{
      ga.markedRec(cc()) = rec.pointer(); //store
      cur_++;
    }
  }

  // get number of record
  rec->nums_ = strtoul((char*)cs(),&p,0);
  if(!rec->nums_ ){
    if(cc() == '$'){  // val referrer
      cur_++;
      if(!cc()){
        MSG(E_W) << "no variable name, after `$' (val referrer)."
                 << endl;
        rec->nums_ = (unsigned)-2;  // infinite
        return rec.pointer();
      }
      rec->refMark_ = cc();
      rec->nums_ = ga.mkval(rec->refMark_); //if not read yet, then -1
      cur_++;

      // 2.0b12
      rec->width_ = getWidthSpec(rec->isRound_);

      //
      //  boolean type refer
      //
      if(cc() == '('){  // starting isEqual expression
        rec->nums_ = (unsigned)-1;  // that unresoluve
        cur_++;
        rec->cond_ = strtoul((char*)cs(),&p,0);
        rec->hasCond_ = 1;
        ff((unsigned char*)p);
        if(cc() == ')'){  //normal closed
          cur_++;
        }else{
          MSG(E_W) << "`$" << cLit(rec->refMark_)
                   << "' : there is no `)' ."
                   << endl;
        }
      }
    }else if(cc() != '0'){ //omitted
      rec->nums_ = (unsigned)-2;  // infinite
//MSG(E_D) << "rec->nums_=" << rec->nums_ << "\n";
    }else{  //zero intentional
      //as it is
    }
  }else{
    ff((unsigned char *)p);
  }
  return rec.pointer();
}

int FormatString::getMember(Ptr<Member> &mem, Ptr<Member> &rec,
  ArgParser &ap)
{
  char mk = 0;
  if(!getMark(mk)) {  //end
    return 0;
  }
  Member::BaseType t =  getSizeAndBaseType(mem,rec,ap);
  switch(t){
    case Member::UNKNOWN:
      return -1;
    case Member::END_NESTED_REC:
      return 0;   //end
    case Member::JUMP:
    {
      JumpMember *tem = mem->asJump();
      if(tem)
        setJumpOffset(tem);
      break;
    }
    default:      //  normal
    {
      if(mk) {   //m@rked
        mem->mark_ = mk;
      }
      RepeatMember  *tem = mem->asRepeat();
      if(tem)
        setRepeat(tem);
      break;
    }
  }
  if(!getComment(mem)){ //fatal
DEBUG_ASSERT(0);
    return -1;
  }
  return 1;
}

bool FormatString::getMark(char &m)
{
  switch(cc()){
    case 0:   //format ended
      return 0; //end
    case '@': // val saving
      cur_++;
      if(!cc()){
        MSG(E_W) << "no variable name, after `@' (val saving)."
                 << endl;
        return 0; //end
      }
      m = cc();
      cur_++;
      break;
    default: //no mark
      break;
  }
  return 1;
}

Member::BaseType FormatString::getSizeAndBaseType(
  Ptr<Member> &m,Ptr<Member> &rec, ArgParser &ap)
{
  Member::BaseType t = Member::UNKNOWN;

	// 2.0b24 `,' allowed, for delimiter
	while(cc() == ',') cur_++;

  switch(cc()){
    case 'C':
      t = Member::UNSIGNDED_DEC; //def@ult
      m = new I8Member((char*)cs(), t);;
      break;
    case 'c':
      t = Member::SIGNED_DEC;
      m = new I8Member((char*)cs(), t);;
      break;

    case 'S':
      t = Member::UNSIGNDED_DEC;
      m = new I16Member((char*)cs(), t);;
      break;
    case 's':
      t = Member::SIGNED_DEC;
      m = new I16Member((char*)cs(), t);;
      break;

    case 'L':
      t = Member::UNSIGNDED_DEC;
      m = new I32Member((char*)cs(), t);;
      break;
    case 'l':
      t = Member::SIGNED_DEC;
      m = new I32Member((char*)cs(), t);;
      break;

    case 'd':	//2.0b15
    case 'D':	//2.0b15
      t = Member::DOUBLE;
      m = new F64Member((char*)cs(), t);;
      break;

    case 'T':   // 2.0b34
    {
      t = Member::TOKEN_LETTER;
      m = new TokenLetterMember((char*)cs(), t);;
      Ptr<TokenLetterMember> tl = m->asTokenLetter();
      if(!getStringConstant(tl->delimiter_)){
        return Member::UNKNOWN;
      }
      Ptr<RepeatMember> rpm = m->asRepeat();
      rpm->prefix_ += '\0'; //actually no use
      break;
    }
    case 't':
    {
      t = Member::FIXED_LETTER;
      m = new LetterMember((char*)cs(), t);
      Ptr<RepeatMember> rpm = m->asRepeat();
      rpm->prefix_ += '\0'; //actually no use
      break;
    }

    case 'Z':
    case 'z':
    {
      t = Member::VAR_LETTER;
      m = new VarLetterMember((char*)cs(), t);;
      Ptr<RepeatMember> rpm = m->asRepeat();
      rpm->prefix_ += '\0';
      break;
    }
    case '[':
    {
      cur_++;
      char *p = (char*)cs();
      m = ap.makeRecForm(p);
      if(m.isNull()){
        return Member::UNKNOWN;
      }
      RecordMember *tem = m->asRecord();
      assert(tem);
      t = Member::NESTED_REC;
      tem->setBase(t);
      cur_ += tem->fsSize_;
      return t;
    }
    case ']':  
    {
      // knowledge of FormatString proceeding
      RecordMember *tem = rec->asRecord();
      assert(tem);
      cur_++;
      tem->fsSize_ = cur_;

//!!!
//      m = tem;
      return Member::END_NESTED_REC;
    }
    case 'N':
    case 'n':
    {
      LETTER_T  pfx = cc();
      m = new NotAccessMember(&pfx);
      t= Member::NOTACCESS;
      break;
    }
    case '&': //2.0b11
    {
      cur_++;
      if(!cc()){
      MSG(E_W) << "no variable name, after `&' (rec referrer)."
               << endl;
        return Member::UNKNOWN;
      }
      m = ga.markedRec(cc());
      if(m.isNull()){
        MSG(E_W) << "empty rec referrer, `&" << cLit(cc())
                 << "' not used."
                 << endl;
        return Member::UNKNOWN;
      }
      t = m->whatis();
      break;
    }

    // 2.0b16
    case  'J':
    case  'j':
    // 2.0b29
    case 'r':
    case 'R':
    {
      t = Member::JUMP;
      LETTER_T  prefix[10];
      prefix[0] = cc();
      cur_++;
      if(cc() == '0'){
        prefix[1] = cc();
        cur_++;
        if(cc()=='x' || cc()=='X'){
          prefix[2] = cc();
          prefix[3] = '\0';
          cur_ -= 1;
        }else{
          prefix[2] = '\0';
        }
      }else{
        prefix[1] = '\0';
      }
      if(prefix[0] == 'R')
        m = new IndifferentMember(prefix,ap,1);
      else if(prefix[0] == 'r')
        m = new IndifferentMember(prefix,ap,0);
      else
        m = new JumpMember(prefix);
      cur_--;
    }
    break;

    default:
//DEBUG_ASSERT(0);
      return Member::UNKNOWN; //fatal;
  }
  cur_++;
  if(t&Member::IS_LETTER ||
    t&Member::IS_NEST_TOK ||
    t&Member::IS_NOTACCESS){
    ;
  }else{
    LETTER_T  prefix[10];
    if(cc() == '0'){
      prefix[0] = cc();
      cur_++;
      if(cc()=='x' || cc()=='X'){
        prefix[1] = cc();
        prefix[2] = '\0';
        t = Member::HEX;
        cur_++;
      }else{
        prefix[1] = '\0';
        t= Member::OCT;
      }
    }else{
      prefix[0] = '\0';
    }
    m->setBase(prefix, t);
  }
  return t;
}

bool FormatString::getStringConstant(String<LETTER_T> &s)
{

#define CHECK_UNEXPECTED_EOF \
  if(!cc()){ \
    MSG(E_F) << "unexpected end-of-format." \
             << endl; \
    return 0; \
  }

  cur_++;
  if(cc() != *(LETTER_QUOTE)){
    MSG(E_F) << "not a delimiter-spec."
             << endl;
    return 0;
  }
  cur_++;

  CHECK_UNEXPECTED_EOF

  LETTER_T c, *p;

  while(cc() != *(LETTER_QUOTE)){
    CHECK_UNEXPECTED_EOF
    if(cc() == *(LETTER_ES)){
      cur_++;
      CHECK_UNEXPECTED_EOF
      switch(cc()){
      case SEQ_CR:
        s.append(LETTER_CR, 1);
        break;
      case SEQ_LF:
        s.append(LETTER_LF, 1);
        break;
      case SEQ_SPC:
        s.append(LETTER_WS, 1);
        break;
      case SEQ_HTAB:
        s.append(LETTER_HTAB, 1);
        break;
      case SEQ_ESCCHAR:
        s.append(LETTER_ES, 1);
        break;
      case SEQ_QUOTE:
        s.append(LETTER_QUOTE, 1);
        break;
      case SEQ_OCTNUM:
        cur_++;
        CHECK_UNEXPECTED_EOF
        c = strtol((char*)cs(), &p, 8);
        if((char*)cs() == p){
          MSG(E_F) << "illegal octal constants."
                   << endl;
          return 0;
        }
        s += c;
        break;
      case SEQ_HEXNUM:
        cur_++;
        CHECK_UNEXPECTED_EOF
        c = strtol((char*)cs(), &p, 16);
        if((char*)cs() == p){
          MSG(E_F) << "illegal hexadecimal constants."
                   << endl;
          return 0;
        }
        s += c;
        break;
      default:
        s += cc();
      }
      cur_++;
      continue;
    }
    s += cc();
    cur_++;
  }
  return 1;
}

//  note: decimal expression only
void FormatString::setRepeat(RepeatMember *mem)
{
  char *p;
  unsigned r = strtoul((char*)cs(),&p,10);
  if(!r){
    if(cc() == '0'){ //intentional
      ff((unsigned char*)p);
      return;
    }else if(cc() == '$'){
      cur_++;
      if(!cc()){
        MSG(E_W) << "no variable name, after `$'(val referrer)."
                 << endl;
        return;
      }
      mem->refMark_ = cc();
      mem->repeat_ = ga.mkval(mem->refMark_);
        //if not read yet, then -1)
      cur_++;

      // 2.0b12
      mem->width_ = getWidthSpec(mem->isRound_);

    }else { //omitted
      mem->repeat_ = 1;
      return;
    }
  }else{
      mem->repeat_ = r;
    ff((unsigned char*)p);
  }
}

void FormatString::setJumpOffset(JumpMember *mem)
{
  char *p;
  unsigned ofs = strtol((char*)cs(),&p,0);
  if(!ofs && errno == EINVAL){
    if(cc() == '0'){ //intentional
      ff((unsigned char*)p);
      return;
    }else if(cc() == '$'){  //»²¾È
      cur_++;
      if(!cc()){
        MSG(E_W) << "no variable name, after `$' (jump offset val referrer)."
                 << endl;
        return;
      }
      mem->refMark_ = cc();
      mem->val_ = ga.mkval(mem->refMark_);
        //if not read yet, then -1
      cur_++;

      // 2.0b12
      mem->width_ = getWidthSpec(mem->isRound_);

    }else { //omitted
      mem->val_ = 0;
      return;
    }
  }else{
      mem->val_ = ofs;
    ff((unsigned char*)p);
  }
}

size_t FormatString::getWidthSpec(bool &isRound)
{
  if(cc() != ',' && cc() != ':'){  //It is impossible what width specified
    return 0;
  }else if(cc() == ':') isRound = 1;
  cur_++;
  char *p;
  size_t r = strtoul((char*)cs(),&p,0);
  if(!r){ //that was zero or omitted
    if(cc() == '0'){  //intentionally
      ff((unsigned char*)p);
    }
    return 0;
  }
  ff((unsigned char*)p);
  return r;
}

bool FormatString::getComment(Ptr<Member> &m)
{
  if(cc() != '#'){  //no comment
    return 1;
  }
  m->comment_ += cc();
  cur_++;
  while(cc() && cc() != ';' && cc() != '#'){
    m->comment_ += cc();
    cur_++;
  }
  if(cc() == ';' || cc() == '#') cur_++;
  m->comment_ += '\0';
  return 1;
}

void FormatString::print(const char *indent)
{
#ifdef MORE_DEBUG
cerr <<APP_STRLEN(str_)<<endl;
#endif
  const size_t maxWidth = 40;
  MSG(E_F) << NEWLINE_SEQ << indent;
  unsigned int i,j;
  for(i = 0, j = i; i < APP_STRLEN(str_); i++, j++){
    if(i == cur_){
      cerr << NEWLINE_SEQ << indent << "  >>> " << str_[i] << " <<<  "
               << NEWLINE_SEQ << indent;
    }else{
      cerr << str_[i];
    }
    if(str_[i] == ';' || !((j+1) % maxWidth)){
      cerr << NEWLINE_SEQ << indent;
      j = 0;
    }
  }
  if((j % maxWidth)){
    cerr << NEWLINE_SEQ;
  }
}

