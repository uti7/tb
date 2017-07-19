// @(#) $Id: StdInputStream.h,v 1.2 2000/10/21 08:11:52 able Exp $
#ifndef __StdInputStream_h__
#define __StdInputStream_h__

#ifdef __BORLANDC__

// BorlandC++ only
// instead of istdiostream */

#include <stdio.h>
#include <iostream.h>
#include <strstrea.h>

class stdin_stream : public istream
{
	public:
		stdin_stream(FILE *);
		//streambuf *rdbuf();
		char *rdbuf();
	private:
		stdin_stream();	//undefined
		stdin_stream(stdin_stream &);	//undefined
		ostrstream sb_;
};

#ifdef SP_DEFINE_TEMPLATES
stdin_stream::stdin_stream(FILE *fp)
: sb_(new char[BUFSIZ], BUFSIZ) {
	while(fgets(sb_.str(), BUFSIZ, fp));
}
#endif	// SP_DEFINE_TEMPLATES

inline
char *stdin_stream::rdbuf(){
	return sb_.str();
}

//
//
typedef stdin_stream istdiostream;

#endif // __BORLANDC__
#endif // !__StdInputStream_h__
