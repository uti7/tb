// @(#) $Id: btta.cxx,v 1.2 2000/10/21 08:11:52 able Exp $
#include "Generic.h"
#include "Resource.h"
#include "Ptr.h"
#include "Vector.h"

#include "Message.h"
#include "Record.h"
#include "GenArg.h"
#include "ArgParser.h"
#include "OutputStream.h"
#include "InputStream.h"

Message _msg;
GenArg  ga;

int main(int c, char **v)
{
//  setlocale( LC_ALL, CATEGORY );
  if(getenv("LANG")){
    setlocale(LC_CTYPE, getenv("LANG"));
  }
//  GenArg ga;
  InputStream is;
  ArgParser ap;
  bool success = ap.go(c,v,ga.rootRec(),is);
  if(!success) return 2;
  if(!ga.rootRec()->process(is)) return 1;
  if(!is.eof()){
    MSG(E_W) << "not reached to EOF." << endl;
    return 3;
  }
  return 0;
}
