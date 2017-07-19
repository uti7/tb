// vi:set ts=2,sw=2,et:
// @(#) $Id: InputStream.cxx,v 1.3 2001/01/12 10:13:33 able Exp $

#include "Generic.h"
#if 0
#include "StringOf.h"
#endif
#include "Message.h"
#include "Record.h"
#include "GenArg.h"
#include "OutputStream.h"
#include "InputStream.h"
 
void InputStream::open(const char *fn) {
  if(fn){
    ifs_.open(fn,ios::in|ios::binary);
    if(ifs_.fail()){
      MSG(E_D) << fn << ":cannot open"<<endl;
    }
    if(ifs_.bad()){
      MSG(E_D) << fn << ":bad condition"<<endl;
    }
    name_.assign(fn, APP_STRLEN(fn));
    name_ += '\0';
    isStdin_ = 0;
  }else{
#if __GNUG__ < 3
    ifs_.attach(0);
#endif
    name_.assign("stdin", 5);
    name_ += '\0';
    isStdin_ = 1;
  }
  isOpen_ = 1;

  if(ga.nCCol()){
    os_ = new WMOutputStream(ga.nCCol());
  }else{
    os_ = new OutputStream;
  }
}

bool InputStream::get(I32_T &v, int &n)
{
  char  byte[sizeof(I32_T)];
  ifs().read(byte,sizeof(I32_T));
  n = ifs().gcount();
  ofs_ += n;
  v = 0;
  if(!ga.endian.isNative()){ // crossed endian
    if(ga.endian.system() == GenArg::Endian::LITTLE){
      for(int i = 0; i < n; i++){
        v |= (I8_T)byte[i];
        if(i < n-1) v = v << 8;
      }
    }else{
      for(int i = n; i > 0; i--){
        v |= (I8_T)byte[i-1];
        if(i > 1) v = v << 8;
      }
	}
  }else{  // native
    memcpy(&v,byte,sizeof(I32_T));
  }
  if(ifs().eof()) return false;
  if(ifs().fail()) return false;
  return true;
}

bool InputStream::get(I16_T &v, int &n)
{
  char  byte[sizeof(I16_T)];
  ifs().read(byte,sizeof(I16_T));
  n = ifs().gcount();
  ofs_ += n;
  v = 0;
  if(!ga.endian.isNative()){  // crossed endian
    if(ga.endian.system() == GenArg::Endian::LITTLE){
      for(int i = 0; i < n; i++){
        v |= (I8_T)byte[i];
        if(i < n-1) v = v << 8;
      }
    }else{
      for(int i = n; i > 0; i--){
        v |= (I8_T)byte[i-1];
        if(i > 1) v = v << 8;
      }
	}
  }else{  //native
    memcpy(&v,byte,sizeof(I16_T));
  }
  if(ifs().eof()) return false;
  if(ifs().fail()) return false;
  return true;
}

bool InputStream::get(I8_T &v, int &n)
{
  ifs().read((char *)&v,sizeof(I8_T));
  n = ifs().gcount();
  ofs_ += n;
  if(ifs().eof()) return false;
  if(ifs().fail()) return false;
  return true;
}

bool InputStream::get(F64_T &v, int &n)
{
  char  byte[sizeof(F64_T)];
  union {
    char  place[sizeof(F64_T)];
    F64_T placedVal;
  } buf;
  ifs().read(byte,sizeof(F64_T));
  n = ifs().gcount();
  ofs_ += n;
  v = 0;
  if(!ga.endian.isNative()){ // crossed endian
      int j = n;
      for(int i = 0; i < n; i++,j--){
        buf.place[j-1] = byte[i];
      }
      v = buf.placedVal;
  }else{  // native
    memcpy(&v,byte,sizeof(F64_T));
  }
  if(ifs().eof()) return false;
  if(ifs().fail()) return false;
  return true;
}

bool InputStream::get(const unsigned l, String<LETTER_T> &s, int &n)
{
  LETTER_T *p = new LETTER_T[l];
  ifs().read((char *)p,l * sizeof(LETTER_T));
  n = ifs().gcount();
  ofs_ += n;
  s.assign(p,n);
  delete [] p;
  if(ifs().eof()) return false;
  if(ifs().fail()) return false;
  return true;
}

bool InputStream::getToken(String<char> &s)
{
  char c;
  bool complete = 0;
  while(!complete && (c = get())!= EOF){
    switch(c){
      case '-': //start new format
        s += c;
        complete = 1;
        break;
      //end of a token
      case ' ':
      case '\t':
      case '\n':
      case '\r':
        if(s.size()){  //have any letter
          complete=1;
        }
        continue;
      case '/': // factor of file comment
        switch(peek()){
          case '/': //file comment (c++ style)
            while(get() != '\n'); // skip to line end
            break;
          case '*': //file comment (c style)
          {
            bool isEnd = 0;
            get();  //skip `*'
            while(!isEnd){
              switch(get()){
                case '*':  // factor of close `*/'
                  if(peek()=='/'){  //closed
                    get();  // skip (i.e. close `/')
                    isEnd=1;
                  }
                  break;
                default:
                  // to be continued.(c style comment)
                  break;
              }
            }
            break;
          }
          default:  //not comment
            break;
        }
        break;
      case '#': // formatting comment
        s+= c;
        while((c = get()) != EOF){
          if(c == ';' || c == '\r' || c == '\n'){ //end
            s += ';'; //endof formatting comment
						if(peek()=='-'){  //next is new format
							complete = 1;
						}
            break;
          }
          s += c; 
        } 
        break;
      default:
        s+= c;
        if(peek()=='-'){  //next is new format
          complete = 1;
        }
        break;
    }
  }
  if(!s.size()) return 0;
//  s += '\0';  //null terminate
  return 1;
} 

bool InputStream::jump(signed j)
{
  bool ok = 1;
#ifdef DEBUG
MSG(E_D) << "::jump("<<j<<"): isStdin_=" << (signed)isStdin_ << endl;
#endif
  if(!isStdin_){
		streampos spos = 0;
		if(j > 0){
				ifs().seekg(j, ios::cur);
				spos = ifs().tellg();
				if(spos != (streampos)(ofs_ + j) || eof() ){
          MSG(E_F) << "EOF detected, during jump." << endl;
#ifdef DEBUG
MSG(E_D) << "spos="<<(signed)spos<<",(ofs_+ j)=" << (ofs_ + j) << endl;
#endif
					return 0;
				}
				ifs().seekg(-1, ios::cur);
				ofs_ = spos;
		}else if(j < 0){
				ifs().seekg(j, ios::cur);
				spos = ifs().tellg();
				if(spos != (streampos)(ofs_ + j)){
          MSG(E_F) << "EOF detected, during jump." << endl;
#ifdef DEBUG
MSG(E_D) << "spos="<<(signed)spos<<",(ofs_+ j)=" << (ofs_ + j) << endl;
#endif
					return 0;
				}
				ofs_ = spos;
    }
  }else{
    if(j < 1){
      MSG(E_F) << "cannot back stdin." << endl;
      return 0;
    }else{
      char *discard = new char[j];
      ifs().read(discard,j);
      delete [] discard;
      ofs_ = ifs().tellg();
    }
  }
  return 1;
}

void InputStream::skipProlog()
{
  String<char> s;
  while(peek() == '#'){
    getl(s);
    s.resize(0);
  }
}

size_t InputStream::getl(String<char> &s)
{
  char c;
  while((c = get())!= EOF){
    s += c;
    // FIXME: cannot determine a file that cr only par line
    if(c == '\n') break;
  }
  return s.size();
}
