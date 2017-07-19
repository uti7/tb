// @(#) $Id: btta_inst.cxx,v 1.2 2000/10/21 08:11:52 able Exp $
#include "Generic.h"
#define SP_DEFINE_TEMPLATES
#include "Message.h"
#include "Resource.h"
#include "Ptr.h"
#include "Vector.h"
#include "Owner.h"
#include "StdInputStream.h"
#include "Record.h"
#include "GenArg.h"
#include "Member.h"
#include "OutputStream.h"
#undef SP_DEFINE_TEMPLATES


template class Ptr<Member>;
template class Vector<Ptr<Member> >;
template class Ptr<RecordMember>;
template class Ptr<RepeatMember>;
template class Ptr<TokenLetterMember>;
template class String<char>;
template class String<char *>;
template class Ptr<OutputStream>;
template class Owner<ostrstream>;

