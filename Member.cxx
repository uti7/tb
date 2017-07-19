// @(#) $Id: Member.cxx,v 1.10 2003/03/08 20:41:10 able Exp $
#ifdef __GNUG__
#pragma implementation
#endif

#include "Generic.h"
#include "Message.h"
#include "Record.h"
#include "GenArg.h"
#include "Member.h"
#include "StdInputStream.h"
#include "OutputStream.h"
#include "InputStream.h"
#include "ArgParser.h"

#ifdef _MSC_VER
#pragma warning(disable:4018) /* warning C4018: '!=' : signed Ç∆ unsigned ÇÃêîílÇî‰ärÇµÇÊÇ§Ç∆ÇµÇ‹ÇµÇΩÅB */
#endif

Member::~Member() { base_ = UNKNOWN; }

unsigned Member::resolveRefNum(char ref)
{
	unsigned n;
	if(ref){
		n = ga.mkval(ref);

		if(hasCond_) {  // boolean type referer
			if(cond_ == n){
				n = 1;
			}else{
				if(ga.isV()){
					MSG(N_N) << "val referrer `$"
                             << cLit(ref)
                             << "'"
                             << " is false ("
                             << dec
                             << cond_
                             << ")."
                             << "(offset="
                             << MANIP_COFFSET
                             << ga.mkpos(ref)
                             << ")"
                             << endl;
				}
				n = 0;
			}
		}else if(!n && ga.isV()){
			MSG(N_N) << " val referrer `$" << cLit(ref) << "'" << " is zero."
			<< "(offset=" << MANIP_COFFSET << ga.mkpos(ref) << ")" << endl;

		}else if(n == (unsigned)-1){
			MSG(E_W) << "empty val referrer, `$" << cLit(ref) << "'"
			<< " not used or too much" << endl;
			n = 0;
		}
	}else{
		MSG(E_W) << "`$" << cLit(ref) << "'"
		<< " no variable name.(bug)" << endl;
		n = 0;
		DEBUG_ASSERT(0);
	}
	return n;
}

void Member::outComment(InputStream &is)
{
	if( comment_.size() > 1){
		String<LETTER_T> unfolded;
		if(ga.isNoSubstitute() == GenArg::NoSubstituteParameterAndCommand){
			unfolded = comment_;
		}else{
			unfoldComment(unfolded, is);
		}
		if(unfolded.size()){

			//2.0b10
			unfolded += '\0';
			String<LETTER_T> called;
			if(ga.isNoSubstitute() != GenArg::DoSubstitute){
				called = unfolded;
			}else{
				externalCallComment(unfolded, called);
			}
			is.cout().pad();
			if(called.size()){
				called += '\0';  //null term
//					is.cout() << ga.delm().data();

				if(ga.isV()){ //2.0b32
					is.cout() << MANIP_OFFSET << ofs_ << ga.delm().data();
				}
				is.cout() << called.data();
			}

			LETTER_T	tem[3];
			tem[2] =  '\0';
			if(mark_){
				is.cout() << ga.delm().data();
				tem[0] = '@';
				tem[1] = mark_;
				is.cout() << tem;
			}
			if(refMark_){
				is.cout() << ga.delm().data();
				tem[0] = '$';
				tem[1] = refMark_;
				is.cout() << tem;
			}
			is.cout() << endl;
			//is.cout().flush();  already processed from endl
		}
	}
}

void Member::unfoldComment(String<LETTER_T> &unfolded, InputStream &is)
{
	bool noMore = 0;
	size_t i = 0;
	while(!noMore){
		for(; i < APP_STRLEN(comment_.data()) &&  // null exclude
					!noMore;
				i++){ // pick precedence
			if(comment_[i] == *(LETTER_ES) &&
				 comment_[i+1] == *(LETTER_UNF)){
				//starting parameter substitution
				break;
			}
			unfolded += comment_[i];
		}
		if(i >= APP_STRLEN(comment_.data())){
			noMore =1 ;
			break;
		}
		i += 2;
		getReplacedComment(comment_, i ,unfolded, is);
	}
}

void Member::getReplacedComment(String<LETTER_T> &comment, size_t &i,
	String<LETTER_T> &unfolded, InputStream &is)
{
	String<LETTER_T> picked;
	for(; i < comment_.size(); i++){  // pick pramater name
		if(comment[i] != *(LETTER_USC) && !isalnum(comment[i])
		//2.0b30
		 && comment[i] != *(UNFOLD_VREF)){
			// end name letter (i.e.[^a-zA-Z0-9_])
			break;
		}
		picked += comment[i];
	}
	if(picked.size()){
		//if(i < APP_STRLEN(comment.data())) i++;
		picked += '\0';
		size_t gt = 16;
		if(strVal_.size() > gt){
			gt = strVal_.size();
		}
		if(is.name().size() > gt){
			gt = is.name().size();
		}
		LETTER_T *obuf = new LETTER_T[gt];
		ostrstream os(obuf,sizeof(LETTER_T) * gt);
		if(!APP_STRCMP(UNFOLD_FILENAME, picked.data())){
			os << is.name().data() << ends;
		}else if(!APP_STRCMP(UNFOLD_VAL, picked.data())){
			os << strVal_.data() << ends;
		}else if(!APP_STRCMP(UNFOLD_CNFR, picked.data())){
			//decimal
			os << dec << cnfr_ << ends;
		}else if(!APP_STRCMP(UNFOLD_CNF, picked.data())){
			//decimal
			os << dec << cnf_ << ends;
		}else if(!APP_STRCMP(UNFOLD_NR, picked.data())){
			//decimal
			os << dec << nr_ << ends;

		//2.0b26
		}else if(!APP_STRCMP(UNFOLD_NFR, picked.data())){
			//decimal
			os << dec << nfr_ << ends;
		}else if(!APP_STRCMP(UNFOLD_SNR, picked.data())){
			//decimal
			os << dec << snr_ << ends;

		//2.0b30
		}else if(*(UNFOLD_VREF) == picked[0]){
			//decimal
			os << dec << ga.mkval(picked[1]) << ends;

		}else if(!APP_STRCMP(UNFOLD_NF, picked.data())){
			//decimal
			os << dec << nf_ << ends;

		}else{
			MSG(E_W) << "illegal unfold literal. >>> "
							 << picked.data() << "<<<" << endl;
		}
		if(APP_STRLEN(obuf)){
			unfolded.append(obuf,APP_STRLEN(obuf));
			//unfolded += '\0';  //null term
		}
		delete [] obuf;
	}
}

//2.0b10
void Member::externalCallComment(String<LETTER_T> &unfolded,
	String<LETTER_T> &called)
{
	bool noMore = 0;
	size_t i = 0;
	while(!noMore){
		for(; i < APP_STRLEN(unfolded.data()) &&  // null exclude
				!noMore;
			i++){
			if(unfolded[i] == *(LETTER_ES) &&
				unfolded[i+1] == *(LETTER_XCL)){
				// starting commend substitution
				break;
			}
			called += unfolded[i];
		}
		if(i >= APP_STRLEN(unfolded.data())){
			noMore =1 ;
			break;
		}
		i += 2;
		getExecComment(unfolded, i ,called);
	}
}

void Member::getExecComment(String<LETTER_T> &unfolded, size_t &i,
	String<LETTER_T> &called)
{
	String<LETTER_T> picked;
	for(; i < unfolded.size(); i++){  // pick command
		if(unfolded[i] == *(LETTER_ES) &&
			 unfolded[i+1] == *(LETTER_XCL)){ // end command
			break;
		}
		picked += unfolded[i];
	}
	if(picked.size()){
		if((i+1) < APP_STRLEN(unfolded.data())){
			i += 2;
		}else if(i < APP_STRLEN(unfolded.data())){
			i++;
		}
		picked += '\0';
		LETTER_T obuf[2048];
#if __GNUG__ >= 3
        APP_MEMSET(obuf, '\0', sizeof(obuf));
#else
		ostrstream os(obuf,sizeof(obuf));
#endif
		FILE *fp = APP_POPEN(picked.data(),"r");
		if(!fp){
			MSG(E_W) << "pipe open failed. >>>"<<picked.data()<<"<<<"<<endl;
		}else{
#if __GNUG__ >= 3
            //__gnu_cxx::stdio_filebuf<char> istdin(fp, ios::in);
            // os << istdin.read(...) << ends;
            fgets(obuf, sizeof(obuf), fp);
#else
			istdiostream istdin(fp);
			os << istdin.rdbuf() << ends;
#endif
			// press line feed
			if(obuf[APP_STRLEN(obuf)-1]=='\n'){ obuf[APP_STRLEN(obuf)-1] = '\0'; }
			APP_PCLOSE(fp);
		}
		if(APP_STRLEN(obuf)){
			called.append(obuf,APP_STRLEN(obuf));
		}
	}
}

void Member::setMarkBase(unsigned val, unsigned ofs)
{
	if(mark_ && (base_&IS_LETTER)){
		MSG(E_W) << "val saving supported only integer." << endl;
		return;
	}
	ga.setMark(mark_, val, ofs);
}

////////////////////////////////////////////////////////////////////////
RecordMember::RecordMember() : nums_(0), width_(0), isRound_(0),
	fsSize_(0), Member(Member::REC) {}

//for root
RecordMember::RecordMember(unsigned s) : nums_(1), width_(0),
	isRound_(0), fsSize_(s),
	Member(Member::REC) {}

bool RecordMember::process(InputStream &is,
	size_t snr, size_t nr /*= 0*/, size_t nf /*= 0*/, size_t cnf /*= 0*/)
{
	nr_ = nr;
	nf_ = nf;
	cnf_ = cnf;
	if(nums_ == (unsigned)-1 || refMark_){ //unresolve
		nums_ = resolveRefNum(refMark_);
		//2.0b12
		if(width_){
			if(isRound_){
				if(nums_ % width_) nums_ += width_ - (nums_ % width_);
			}else
				nums_ = nums_ / width_;
		}
	}

	//2.0b26
	snr_ = nums_;

	bool ret = 1;
	for(unsigned i = 0; ret && i < nums_; i++){
		ofs_ = is.ofs();
		for(unsigned j = 0; j < sub_.size(); j++){
			if(!sub_[j]->process(is, snr_, i, sub_.size(), j)){
				if( j != sub_.size() -1){
					if(nums_ == (unsigned)-2 && j == 0){
						;
					}else{
					MSG(E_W) << "too short data, for a record." << endl;
					}
				}
ret = 0;
				break;
			}
		}
		if(ga.isV()){
			is.cout().pad();
			is.cout() << MANIP_OFFSET << ofs_ << NEWLINE_SEQ;
		}else if(base_ != NESTED_REC){
			is.cout() << NEWLINE_SEQ;
		}
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////
void  RepeatMember::outRemainder(InputStream &is, unsigned char *b,unsigned s)
{
	for(unsigned int i=0;i<s;i++)
		is.cout() << MANIP_BITS << (unsigned)b[i] << ga.delm().data();
	is.cout() << NEWLINE_SEQ;
}

RepeatMember::RepeatMember(LETTER_T *p, unsigned s, BaseType b,
	LETTER_T *c /*= 0*/)
: repeat_(0), width_(0), isRound_(0), Member(s,b,c)
{
	prefix_.assign(p,1);
}

bool RepeatMember::process(InputStream &is,
	size_t snr, size_t nr /* = 0*/, size_t nf /* = 0*/, size_t cnf /*= 0*/)
{
	snr_ = snr; //2.0b26
	nr_ = nr;
	nf_ = nf;
	cnf_ = cnf;
	if(repeat_ == (unsigned)-1 || refMark_){ //unresolve
		repeat_ = resolveRefNum(refMark_);
		//2.0b12
		if(width_){
			if(isRound_){
				if(repeat_ % width_) repeat_ += width_ - (repeat_ % width_);
			}else
				repeat_ = repeat_ / width_;
		}
	}

	//2.0b26
	nfr_ = repeat_;

	if(isLetter()) return this->out(is);

	for(unsigned i = 0; i < repeat_; i++){
		cnfr_ = i;
		if(!this->out(is)){
			if( 0 != repeat_ -1){
				MSG(E_W) << "too short data, for a record." << endl;
			}
			return 0;
		}
	}
	return 1;
}

////////////////////////////////////////////////////////////////////////
I32Member::I32Member(LETTER_T *p, BaseType b, LETTER_T *c /* = 0*/)
: IntMember(p, sizeof(I32_T),b,c) {}

bool I32Member::out(InputStream &is)
{
	int n;
	ofs_ = is.ofs();
	if(!is.get(val_,n)){
		if(n && n != sizeof(I32_T)){
			MSG(E_W) << "too short data, for a member `"
				<< prefix_.data() << "'." << endl;
			outRemainder(is, (unsigned char *)&val_,n);
		}
		return false;
	}else{
		switch(base_){
			case UNSIGNDED_DEC:
			{
				is.cout() << MANIP_I32UD << (unsigned)val_;
				strVal_.resize(10+1);
				ostrstream os(strVal_.begin(), strVal_.size());
				os << S_MANIP_I32UD << (unsigned)val_ << ends;
			}
				break;
			case SIGNED_DEC:
			{
		SI32_T tem = val_;
				is.cout() << MANIP_I32D << (signed)tem;
				strVal_.resize(10+1);
				ostrstream os(strVal_.begin(), strVal_.size());
				os << S_MANIP_I32D << (signed)tem << ends;
			}
				break;
			case OCT:
				is.cout() << MANIP_I32O << (unsigned)val_;
			{
				strVal_.resize(11+1);
				ostrstream os(strVal_.begin(), strVal_.size());
				os << "0" << S_MANIP_I32O << val_ << ends;
			}
				break;
			case HEX:
				is.cout() << MANIP_I32H << val_;
			{
				strVal_.resize(10+1);
				ostrstream os(strVal_.begin(), strVal_.size());
				os << "0x" << S_MANIP_I32H << val_ << ends;
			}
				break;
			default:
				DEBUG_ASSERT(0);
				break;
		}
		is.cout() << ga.delm().data();
		outComment(is);
		setMark();
	}
	return true;
}

////////////////////////////////////////////////////////////////////////
I16Member::I16Member(LETTER_T *p, BaseType b, LETTER_T *c /*= 0*/)
: IntMember(p, sizeof(I16_T),b,c) {}

bool I16Member::out(InputStream &is)
{
	int n;
	ofs_ = is.ofs();
	if(!is.get(val_,n)){
		if(n && n != sizeof(I16_T)){
			MSG(E_W) << "too short data, for a member `"
				<< prefix_.data() << "'." << endl;
			outRemainder(is, (unsigned char *)&val_,n);
		}
		return false;
	}else{
		switch(base_){
			case UNSIGNDED_DEC:
			{
				is.cout() << MANIP_I16UD << (unsigned)val_;
				strVal_.resize(5+1);
				ostrstream os(strVal_.begin(), strVal_.size());
				os << S_MANIP_I16UD << (unsigned)val_ << ends;
			}
				break;
			case SIGNED_DEC:
			{
		SI16_T tem = val_;
				is.cout() << MANIP_I16D << (signed)tem;
				strVal_.resize(5+1);
				ostrstream os(strVal_.begin(), strVal_.size());
				os << S_MANIP_I16D << (signed)tem << ends;
			}
				break;
			case OCT:
				is.cout() << MANIP_I16O << val_;
			{
				strVal_.resize(7+1);
				ostrstream os(strVal_.begin(), strVal_.size());
				os << "0" << S_MANIP_I16O << val_ << ends;
			}
				break;
			case HEX:
				is.cout() << MANIP_I16H << val_;
			{
				strVal_.resize(6+1);
				ostrstream os(strVal_.begin(), strVal_.size());
				os << "0x" << S_MANIP_I16H << val_ << ends;
			}
				break;
			default:
				DEBUG_ASSERT(0);
				break;
		}
		is.cout() << ga.delm().data();
		outComment(is);
		setMark();
	}
	return true;
}

////////////////////////////////////////////////////////////////////////
I8Member::I8Member(LETTER_T *p, BaseType b, LETTER_T *c /*= 0*/)
: IntMember(p, sizeof(I8_T),b,c) {}

bool I8Member::out(InputStream &is)
{
	int n;
	ofs_ = is.ofs();
	if(!is.get(val_,n)){
		if(n && n != sizeof(I8_T)){
			MSG(E_W) << "too short data, for a member `"
				<< prefix_.data() << "'." << endl;
			outRemainder(is, (unsigned char *)&val_,n);
		}
		return false;
	}else{
		switch(base_){
			case UNSIGNDED_DEC:
			{
				is.cout() << MANIP_I8UD << (unsigned)val_;
				strVal_.resize(3+1);
				ostrstream os(strVal_.begin(), strVal_.size());
				os << S_MANIP_I8UD << (unsigned)val_ << ends;
			}
				break;
			case SIGNED_DEC:
			{
		SI8_T tem = val_;
				is.cout() << MANIP_I8D << (signed)tem;
				strVal_.resize(3+1);
				ostrstream os(strVal_.begin(), strVal_.size());
				os << S_MANIP_I8D << (signed)tem << ends;
			}
				break;
			case OCT:
				is.cout() << MANIP_I8O << (unsigned)val_;
			{
				strVal_.resize(4+1);
				ostrstream os(strVal_.begin(), strVal_.size());
				os << "0" << S_MANIP_I8O << val_ << ends;
			}
				break;
			case HEX:
				is.cout() << MANIP_I8H << (unsigned)val_;
			{
				strVal_.resize(4+1);
				ostrstream os(strVal_.begin(), strVal_.size());
				os << "0x" << S_MANIP_I8H << (unsigned)val_ << ends;
			}
				break;
			default:
				DEBUG_ASSERT(0);
				break;
		}
		is.cout() << ga.delm().data();
		outComment(is);
		setMark();
	}
	return true;
}

////////////////////////////////////////////////////////////////////////
F64Member::F64Member(LETTER_T *p, BaseType b, LETTER_T *c /*= 0*/)
: RepeatMember(p, sizeof(F64_T),b,c) {}

bool F64Member::out(InputStream &is)
{
	int n;
	ofs_ = is.ofs();
	if(!is.get(val_,n)){
		if(n && n != sizeof(F64_T)){
			MSG(E_W) << "too short data, for a member `"
				<< prefix_.data() << "'." << endl;
			outRemainder(is,(unsigned char *)&val_,n);
		}
		return false;
	}else{
		switch(base_){
			case DOUBLE:
				is.cout() << MANIP_F64 << val_;
			{
				strVal_.resize(32+1);
				ostrstream os(strVal_.begin(), strVal_.size());
				os << S_MANIP_F64 << val_ << ends;
			}
				break;
			default:
				DEBUG_ASSERT(0);
				break;
		}
		is.cout() << ga.delm().data();
		outComment(is);
		setMark();	// NOTE: cast to integer
	}
	return true;
}

////////////////////////////////////////////////////////////////////////
LetterMember::LetterMember(LETTER_T *p, BaseType b, LETTER_T *c /*= 0*/)
: RepeatMember(p, (b==Member::NOTACCESS) ? 0 : sizeof(LETTER_T), b, c) {}

bool LetterMember::out(InputStream &is)
{
	int n;
	ofs_ = is.ofs();
	if(!is.get(repeat_,val_, n)){
		if(n && n != repeat_ * sizeof(I8_T)){
			MSG(E_W) << "too short data, for a member `"
				<< prefix_.data() << "'." << endl;
			outRemainder(is, (unsigned char *)val_.data(),n);
		}
		return false;
	}
	if(base_ == VAR_LETTER){
		is.cout() << MANIP_VARLET << repeat_ << ga.delm().data();
	}

	val_ = refine();
  is.cout() << val_.data();
	is.cout() << ga.delm().data();
	strVal_ = val_;
	strVal_ += '\0';
	outComment(is);
		setMark();	// NOTE: not supported, to out message
	return true;
}

String<LETTER_T> LetterMember::refine()
{
	String<LETTER_T> tem;
	bool infield = 0;
	for(unsigned int i = 0; i < val_.size(); i++){
		I8_T  oval;
		if( isascii(val_[i]) ){

			if(isprint(val_[i])){ //printable
				if( infield == 0 ){
					tem += *(LETTER_QUOTE);
					infield = 1;
				}
				if(val_[i] == *(LETTER_QUOTE)
					|| val_[i] == *(LETTER_ES)){
					tem += *(LETTER_ES);
				}
				tem += val_[i];

			}else{ //not printable
				if( infield == 1 ){
						tem += *(LETTER_QUOTE); //closE
						tem.append(ga.delm().data(), ga.delm().size()-1);
						infield = 0;
				}
//                tem.append(BITPREFIX,APP_STRLEN(BITPREFIX));
				LETTER_T  obuf[16];
				oval = val_[i];
				ostrstream oss(obuf,16);
				oss << MANIP_BITS << (unsigned)oval << ends;
				tem.append(obuf,APP_STRLEN(obuf));
				tem.append(ga.delm().data(), ga.delm().size()-1);
			}

		}else{ //multi byte

			if( val_.size() == i+1 ){ // odds
				if( infield == 1 ){
					tem += *(LETTER_QUOTE); //closE
					tem.append(ga.delm().data(), ga.delm().size()-1);
					infield = 0;
				}
//                tem.append(BITPREFIX,APP_STRLEN(BITPREFIX));
				LETTER_T  obuf[16];
				oval = val_[i];
				ostrstream oss(obuf,16);
				oss << MANIP_BITS << (unsigned)oval << ends;
				tem.append(obuf,APP_STRLEN(obuf));
				continue;
			}

			LETTER_T  obuf[16];
			wchar_t   wc[16];
			obuf[0] = val_[i]; obuf[1] = val_[i+1]; obuf[2] = '\0';
			int rtn = mbtowc( wc, obuf, 2 );
			if( rtn < 2 ){ //failure
				if( infield == 1 ){
						tem += *(LETTER_QUOTE); //closE
						tem.append(ga.delm().data(), ga.delm().size()-1);
						infield = 0;
				}
				ostrstream oss(obuf,16);
				oss.seekp(0);
#if 0
								oval = val_[i];
								oss << MANIP_BITS << (unsigned)oval << ga.delm().data() << ends;
								oval = val_[i+1];
								oss << MANIP_BITS << (unsigned)oval << ga.delm().data() << ends;
#else // OSTRSTREAM IMPL DIFFERENCE?
				oss << MANIP_BITS << (unsigned)(val_[i] & 0xffUL) << ga.delm().data()
						<< MANIP_BITS << (unsigned)(val_[i+1] & 0xffUL) << ga.delm().data()
						<< ends;
#endif
				tem.append(obuf,APP_STRLEN(obuf));

			}else if( iswprint(wc[0]) ){ //printable
				if( infield == 0 ){
					tem.append(ga.delm().data(), ga.delm().size()-1);
					tem += *(LETTER_QUOTE); //opeN
					infield = 1;
				}
				tem += val_[i ];
				tem += val_[i+1];

			}else{ // not printable
				if( infield == 1 ){
					tem += *(LETTER_QUOTE); //closE
					tem.append(ga.delm().data(), ga.delm().size()-1);
					infield = 0;
				}
				ostrstream oss(obuf,16);
				oss.seekp(0);
				oss << MANIP_BITS << val_[i  ] << ga.delm().data()
						<< MANIP_BITS << val_[i+1] << ga.delm().data() << ends;
				tem.append(obuf,APP_STRLEN(obuf));
			}
			i++;
		}
	}
	if( infield == 1 ){
		tem += *(LETTER_QUOTE); //closE
// 2.0b32	tem.append(ga.delm().data(), ga.delm().size()-1);
	}
	tem += (LETTER_T)'\0';
	return tem;
}

////////////////////////////////////////////////////////////////////////
VarLetterMember::VarLetterMember(LETTER_T *p, BaseType b, LETTER_T *c
/*= 0*/)
: LetterMember(p, b,c) {}

String<LETTER_T> VarLetterMember::refine()
{
	String<LETTER_T> tem;
	bool infield = 0;
	if( infield == 0 ){
			tem += *(LETTER_QUOTE);
			infield = 1;
	}
	for(unsigned int i = 0; i < val_.size(); i++){
		if(val_[i] == '\0') break;
				if( isascii(val_[i]) ){ //   /*£·£Û£„£È£È ∏ª˙* */

						if(val_[i] == *(LETTER_CR)){
							tem += *(LETTER_ES);
							tem += SEQ_CR;
							continue;
						}else if(val_[i] == *(LETTER_LF)){
							tem += *(LETTER_ES);
							tem += SEQ_LF;
							continue;
						}else if(val_[i] == *(LETTER_QUOTE)
							|| val_[i] == *(LETTER_ES)){
							tem += *(LETTER_ES);
						}
						tem += val_[i];

            }else{ // /* £øÙ•–•§•» ∏ª˙*/

						if( val_.size() == i+1 ){   // /*»æ√º ∏ª˙*/
								tem += val_[i];
								continue;
						}
						tem += val_[i];
						i++;
						tem += val_[i];
				}
	}
	if( infield == 1 ){
		tem += *(LETTER_QUOTE); //clos3
//2.0b32		tem.append(ga.delm().data(), ga.delm().size()-1);
	}
	tem += (LETTER_T)'\0';
	return tem;
}

////////////////////////////////////////////////////////////////////////
TokenLetterMember::TokenLetterMember(LETTER_T *p, BaseType b, LETTER_T *c
/*= 0*/)
: LetterMember(p, b,c) {}

bool TokenLetterMember::out(InputStream &is)
{
	int n;
	ofs_ = is.ofs();
	if(!is.get(repeat_,val_, n)){

#if 0
		if(n && n != repeat_ * sizeof(I8_T)){
			MSG(E_W) << "too short data, for a member `"
				<< prefix_.data() << "'." << endl;
			outRemainder(is, (unsigned char *)val_.data(),n);
		}
		return false;
#else
        if(!n){
            return false;
        }
#endif

	}

    size_t d = 0;
    size_t l = val_.size();
    LETTER_T *p;
    
    p = (LETTER_T*)memchr(val_.data(), delimiter_[d], l);
    while(p){
        d++;
        if(d >= delimiter_.size()){
            break;
        }
        p++;
        if((val_.data() + val_.size()) >= p){
            p = 0;
            break;
        }
        l = val_.size() - ((size_t)p - (size_t)val_.data());
        p = (LETTER_T*)memchr(p, delimiter_[d], l);
    }

    if(!p){
        MSG(E_W) << "token not found."
                 << endl;
    } else{
        l = ((size_t)p - (size_t)val_.data()) + 1;
        String<LETTER_T> brk(val_.data(), l);
        is.jump(l - val_.size());   // back for re-get
        val_ = brk;
    }


	val_ = refine();
    is.cout() << val_.data();
	is.cout() << ga.delm().data();
	strVal_ = val_;
	strVal_ += '\0';
	outComment(is);
		setMark();	// NOTE: not supported, to out message
	return true;
}
////////////////////////////////////////////////////////////////////////

NotAccessMember::NotAccessMember(LETTER_T *pfx, LETTER_T *c /*= 0*/)
: LetterMember(pfx, Member::NOTACCESS,c)
{}

bool NotAccessMember::out(InputStream &is)
{
	ofs_ = is.ofs();
	if(prefix_[0] == 'n'){  //indicate that new line
		is.cout() << NEWLINE_SEQ; // out new line seq.
	}
	outComment(is);
	return true;
}

////////////////////////////////////////////////////////////////////////
JumpMember::JumpMember(LETTER_T *pfx, LETTER_T *c /*= 0*/)
: width_(1), isRound_(0), val_(0), Member(0, Member::JUMP, c)
{
	prefix_.assign(pfx, APP_STRLEN(pfx) + 1);
}

bool JumpMember::process(InputStream &is,
	size_t snr, size_t nr /*= 0*/, size_t nf /*= 0*/, size_t cnf /*= 0*/)
{
	//2.0b26
	snr_ = snr;

	nr_ = nr;
	nf_ = nf;
	cnf_ = cnf;
	if(val_ == (unsigned)-1 && refMark_){ //unresolve
		val_ = resolveRefNum(refMark_);
		//2.0b12
		if(width_){
			if(isRound_){
				if(val_ % width_) val_ += width_ - (val_ % width_);
			}else
				val_ = val_ / width_;
		}
	}
	return out(is);
}

bool JumpMember::out(InputStream &is)
{
	ofs_ = is.ofs();
	if(!APP_STRCMP(&prefix_[1],"0x")){
		is.cout() << MANIP_JUMPH << val_;
	}else if(!APP_STRCMP(&prefix_[1],"0")){
		is.cout() << MANIP_JUMPO << val_;
	}else{
		is.cout() << MANIP_JUMPD << val_;
	}
	is.cout() << ga.delm().data();
	outComment(is);
	return is.jump(val_);
}

////////////////////////////////////////////////////////////////////////
IndifferentMember::IndifferentMember(LETTER_T *pfx, ArgParser &ap, bool isAbs,
	LETTER_T *c /*= 0*/)
: ap_(ap), isAbsolute_(isAbs), JumpMember(pfx, c)
{
	base_ = Member::INDIFFERENT;
}

bool IndifferentMember::out(InputStream &is)
{
	bool ret = true;
	ofs_ = is.ofs();
	unsigned quant = (unsigned)val_;
	LETTER_T tem[16];
#define OS_INIT memset(tem,0,sizeof(tem)); ostrstream os(tem,sizeof(tem))
	if(isAbsolute_){
		if(ofs_ < quant)
			quant -= ofs_;
		else{
			if(ga.isV()){
				OS_INIT;
				if(!APP_STRCMP(&prefix_[1],"0x")){
					os << MANIP_JUMPH << val_;
				}else if(!APP_STRCMP(&prefix_[1],"0")){
					os << MANIP_JUMPO << val_;
				}else{
					os << MANIP_JUMPD << val_;
				}
				MSG(N_N) << "absolute indifferent member("
						<< tem << ") was specified "
						<< "which already past offset" << endl;
			}
			quant = 0;
		}
	}
	String<LETTER_T> s;
	if(quant){
		s += '1';
		s += 'n';
		unsigned forward = (ofs_ % 16) ? 16 - (ofs_ % 16) : 0;
		MIN_IT(forward, quant);
if(forward){
		s.append("[1",2);
		if(forward/2){
			OS_INIT;
			os << "s0x" << forward / 2;
			s.append(tem, APP_STRLEN(tem));
		}
		if(forward%2){
			OS_INIT;
			os << "c0x";
			s.append(tem, APP_STRLEN(tem));
		}
		quant -= forward;
		s += ']';
}
		if(quant/16){
			OS_INIT;
			os << "[" << quant / 16 << "s0x8]";
			s.append(tem, APP_STRLEN(tem));
			quant -= (quant /16) * 16;
		}
if(quant){
		s.append("[1",2);
		if(quant/2){
			OS_INIT;
			os << "s0x" << quant / 2;
			s.append(tem, APP_STRLEN(tem));
		}
		if(quant%2){
			OS_INIT;
			os << "c0x";
			s.append(tem, APP_STRLEN(tem));
		}
		s += ']';
}
	}
	s += '\0';
#ifdef DEBUG
	MSG(E_D) << "indifferent part is " << s.data() << endl;
	ret = true;
#endif
if(APP_STRLEN(s.data())){
	waste_ = ap_.makeRecForm((char *)s.data());
	DEBUG_ASSERT(!waste_.isNull());
	ret = waste_->process(is, 1);
}
	is.cout() << ga.delm().data();
	outComment(is);
	return ret;
}
