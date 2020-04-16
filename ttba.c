/* vi:set ts=4 sw=4 et: */
/* @(#) $Id: ttba.c,v 1.9 2003/03/08 20:41:10 able Exp $ */
/*********************************
*
* Name:     txt2bin-a
*  (from text_data to binary_data)
*                 
* History 1997.??.?? programmed by souma
* Update 1997.11.30 stdin/out support
*       R1.2    exit-code
*       R1.3    support "z" (null termtate string)
*       R1.4    support "\"" (double quotes char ESC SEQ.)
                                with "-E" opt
        R1.5    double support (to use, `d') 2k.06.03
        R1.51   jump support (to use, `j') 2k.06.07
        R1.6.0  for Borland C
        R2.0b1  add gET_iNTEGER(), instead of fSCNAF
                cease "-E" opt
**********************************/
#ifndef _MSC_VER
#include "@.h"
#else
#include <windows.h>
#include <stdio.h>
#include <errno.h>
#define NO_GETOPT
#endif

#define ERR_POS "%s: offset(nearby)=%d(%#x).\n",\
    _fnin,ftell(_fin)-1,ftell(_fin)-1
#define USAGE   "\
txt2bin-a - convert text-scritpt to binary-data\n\
Usage: ttba [-{e|E}] [-m marge_src_file] infile outfile\n\
       ttba [-{e|E}] -c [infile]\n\
       -m   : merge the contents of infile into marge_src_file and output as outfile\n\
       -c   : send to stdout\n\
       -e(E): indeicate whether numeric value that 16bit and greater\n\
              in infile should be processed little(big) endian\n\
              (default: native)\n\
%s%s\n"
# define TTBA_REV "R2.1b1"

#ifdef __BORLANDC__
# define TTBA_COMPILED "(bcc)"
# define INT_SCAN_FORM "%li"
#elif defined(__GNUC__)
# define TTBA_COMPILED "(gcc)"
# define INT_SCAN_FORM "%0li"
#elif defined(_MSC_VER)
# define TTBA_COMPILED "(vc)"
# define INT_SCAN_FORM "%0li"
#else
# define TTBA_COMPILED "(other)"
# define INT_SCAN_FORM "%0li"
#endif

    FILE            *_fin = NULL, *_fout = NULL, *_fmarge = NULL;;
    char            *_fnin = NULL, *_fnout = NULL, *_fnmarge= NULL;;

#ifdef NO_GETOPT
int            optind = 1;
static char    optargbuf[BUFSIZ];
char        *optarg = optargbuf;

int getopt(int c, char *v[], const char *s)
{
    extern int        optind, retopt;
    extern char     *optarg;
    static unsigned int vpos = 0;
    char tok[2],*p = NULL;
    static int        isOptArg = 0;
    /* FIXME: cannot determine multi parameter in arg */
    if(isOptArg) optind++;
    while(1){
        isOptArg = 0;
        if(optind >= c || (vpos == 0 && v[optind][vpos] != '-')) return EOF;
        vpos++;
        if(vpos >= strlen(v[optind])){
            optind++;
            vpos = 0;
            continue;
        }
        sprintf(tok,"%1c", v[optind][vpos]);
        if((p = strstr(s, tok)) == NULL || *(p+1) != ':') break;
        optind++;
        isOptArg = 1;
        vpos = 0;
        strcpy(optarg, v[optind]);
        break;
    }
    return (int)tok[0];
}
#endif    /* NO_GETOPT */

#define MAX_SLEN 16

int get_integer(unsigned long *val)
{
    int     c = 0;
    int     is_err = 0;
    size_t  slen = 0;
/*    static const size_t max_slen = 16; */
    char    s[MAX_SLEN], *p = 0;

    *val  = 0;
    memset(s, 0, sizeof(s));
    /* get token of factor of integer */
    while(slen < MAX_SLEN - 1 && (c = fgetc(_fin)) != EOF){
        if(c == '\n' || c == '\t' || c == ' ' || c == '\r'){
            if(slen == 0) continue;
            break;
        }else if(c >= '0' && c <= '9'
            || c == '-'
            || toupper(c) >= 'A' && toupper(c) <= 'F'
            || toupper(c) == 'X'){
            s[slen] = c;
            slen++;
            continue;
        }
        break;
    }
    if(slen == 0){
        fprintf(stderr,"cannot get integer literal.\n");
        fprintf(stderr,ERR_POS);
        return -2;
    }
    *val = strtoul(s, &p, 0);
    if((size_t)p != (size_t)s + slen){
        fprintf(stderr,"unexpected letter.(%#x,`%1c')\n", *p, *p);
        fprintf(stderr,ERR_POS);
        return -2;
    }
    return c;
}

enum {
    BIG,
    LITTLE,
    NATIVE };

int isNative(int e)
{
    int sys_;
    int     i = 0x1;
    if( *(char *)&i == 1)
        sys_ = LITTLE;
    else
        sys_ = BIG;
    if(e ==sys_)
        return 1;
    else
        return 0;
}

unsigned short sreverse(unsigned short s)
{
    unsigned short ret=0;
    ret = (s & 0xff) << 8;
    ret |= (s & 0xff00) >> 8;
    return ret;
}

unsigned long lreverse(unsigned long s)
{
    unsigned long ret=0;
    ret = (s & 0xff) << 24;
    ret |= (s & 0xff00) << 8;
    ret |= (s & 0xff0000) >> 8;
    ret |= (s & 0xff000000) >> 24;
    return ret;
}

/*int errno = 0;*/

int main(int argc, char **argv)
{
    int             c = 0;
    int             db,esc_f;
    unsigned long   val = 0;
    unsigned short  sval;
    unsigned char   cval;
    char            discard[BUFSIZ];
    int             exit_code = 0;
    int             opt, is_pipe = 0;
    size_t          pbyte,rbyte,zbyte = 0;
    extern int      optind;
    extern char     *optarg;

    int             isNativeEndian = 1;
    struct stat     s;
    errno = 0;
    while ((opt = getopt(argc, argv, "eEh?cm:")) != EOF){
        switch (opt) {
        case 'e':
            isNativeEndian = isNative(LITTLE);
            break;
        case 'E':
            isNativeEndian = isNative(BIG);
            break;
        case 'c':
            is_pipe = 1;
            break;
        case 'm':
            _fnmarge = strdup(optarg);
            break;

        case '?':
        case 'h':
            fprintf(stderr,USAGE,TTBA_REV,TTBA_COMPILED);
            exit_code = 0;
            goto END;

        default:

            errno = EINVAL;
        }
    }
    if (errno) {
        fprintf(stderr,USAGE,TTBA_REV,TTBA_COMPILED);
        exit_code = 2;
        goto END;
    }

    if(_fnmarge){
        _fmarge = fopen( _fnmarge, "rb" );
        if( !_fmarge ){
            perror(_fnmarge);
            exit_code = 1;
            goto END;
        }
        if(stat(_fnmarge, &s) != 0){
            perror(_fnmarge);
            goto END;
        }

    }

    if(is_pipe){
        if( optind < argc ){
            _fnin = strdup(argv[optind]);
            _fin = fopen( _fnin, "rb" );
            if( !_fin ){
                perror(_fnin);
                exit_code = 1;
                goto END;
            }
        }else{
            _fin = stdin;
            _fnin = (char *)malloc(10);
            if(!_fnin){
                perror("malloc");
                exit_code = 1;
                goto END;
            }
            strcpy( _fnin, "stdin" );
        }
        _fout = stdout;
        _fnout = (char *)malloc(10);
        if(!_fnout){
            perror("malloc");
            exit_code = 1;
            goto END;
        }
        strcpy( _fnout, "stdout" );
    }else{
        if( optind >= argc ){
            errno = EINVAL;
            fprintf(stderr,USAGE,TTBA_REV,TTBA_COMPILED);
            exit_code = 2;
            goto END;
        }
        _fnin = strdup(argv[optind]);
        _fin = fopen( _fnin, "rb" );
        if( !_fin ){
            perror(_fnin);
            exit_code = 1;
            goto END;
        }
        optind++;
        if( optind >= argc ){
            errno = EINVAL;
            fprintf(stderr,USAGE,TTBA_REV,TTBA_COMPILED);
            exit_code = 2;
            goto END;
        }
        _fnout = strdup(argv[optind]);
        _fout = fopen( _fnout, "wb" );
        if(!_fout){
            perror(_fnout);
            exit_code = 1;
            goto END;
        }
    }

while(!feof(_fin)){
    if(!c) c = fgetc(_fin);
#ifdef MORE_DEBUG
fprintf(stderr,"DEBUG: c=%d\n",c);
#endif
    if(c == EOF){
        exit_code = 0;
        goto END;
    }
    switch(c){
    case 'c':
    case 'C':
        if((c = get_integer(&val)) == -2) goto END;
        if(val > 0xff){
            fprintf(stderr,"%d(%#x): data incorrect, `c' is 8bit integer\n",
                val,val);
            fprintf(stderr,ERR_POS);
            exit_code = 1;
            goto END;
        }
        cval = (char)val;
        fwrite(&cval, 1, sizeof(cval), _fout);
        if(_fmarge) fseek(_fmarge, sizeof(cval), SEEK_CUR);
        break;
    case 's':
    case 'S':
        if((c = get_integer(&val)) == -2) goto END;
        if(val > 0xffff){
            fprintf(stderr,"%d(%#x): data incorrect, `s' is 16bit integer\n",
                val,val);
            fprintf(stderr,ERR_POS);
            exit_code = 1;
            goto END;
        }
        sval = (unsigned short)val;
        if(!isNativeEndian)
            sval=sreverse(sval);

        fwrite(&sval, 1, sizeof(sval), _fout);
        if(_fmarge) fseek(_fmarge, sizeof(sval), SEEK_CUR);
        break;
    case 'l':
    case 'L':
    {
        if((c = get_integer(&val)) == -2) goto END;
#ifdef DEBUG
fprintf(stderr, "DEBUG: scan result, long     (%d)=%#x\n",
    sizeof(long), val);
fprintf(stderr,"DEBUG: fscanf ret is =%d\n",c);
#endif
        if(!isNativeEndian)
            val=lreverse(val);

        fwrite(&val, 1, sizeof(val), _fout);
        if(_fmarge) fseek(_fmarge, sizeof(val), SEEK_CUR);
    }
        break;
    case 'J':
    case 'j':
    {
        void    *buf = NULL;
        int     distance = 0;
        int     is_abs_offset = (c == 'J') ? 1 : 0;

        if((c = get_integer(&val)) == -2) goto END;
        if(_fmarge){
            if((long)val == -1){
                /* jump to eof */
                distance = s.st_size - ftell(_fmarge);
            }else{
                distance = (is_abs_offset) ? val - ftell(_fmarge) : val;
            }
            if(distance < 0){
                fprintf(stderr,"%d(%#x): can't jump backwards. current offset: %d(%#x)\n",
                    val,val,ftell(_fmarge),ftell(_fmarge));
                fprintf(stderr,ERR_POS);
                exit_code = 1;
                goto END;
            }
            else if((distance + ftell(_fmarge)) > s.st_size ){
                fprintf(stderr,"%d(%#x): can't jump forwards. current offset: %d(%#x)\n",
                    val,val,ftell(_fmarge),ftell(_fmarge));
                fprintf(stderr,ERR_POS);
                exit_code = 1;
                goto END;
            }

            if(distance == 0)
                break;

            buf = malloc(distance);
            if(fread(buf, 1, distance, _fmarge) != distance){
                fprintf(stderr,"%d(%#x): jump failed, that marge src reading. \n",
                    val,val);
                fprintf(stderr,ERR_POS);
                exit_code = 1;
                free(buf);
                goto END;
            }
            fwrite(buf, 1, distance, _fout);
            free(buf);
        }else{
            if((long)val == -1){
                fprintf(stderr,"%d(%#x): jump distance is invalid. \n",
                    val,val);
                fprintf(stderr,ERR_POS);
                exit_code = 1;
                goto END;
            }
            fseek(_fout, distance, SEEK_CUR);
        }
        break;
    }
    case 'd':
    case 'D':
    {
        double w = 0;
        fscanf(_fin,"%le",&w);
        fwrite(&w, 1, sizeof(double), _fout);
        if(_fmarge) fseek(_fmarge, sizeof(w), SEEK_CUR);
        c = 0;
    }
        break;
    case '#':
        fgets(discard,BUFSIZ,_fin);
        c = 0;
        break;
    case 'z':
    case 'Z':
        if((c = get_integer((unsigned long *)&zbyte)) == -2) goto END;
        if(!zbyte){
            fprintf(stderr,"`z': size not found.\n");
            fprintf(stderr,ERR_POS);
            exit_code = 1;
            goto END;
        }
        break;
    case '"':
    {
        esc_f = 0;
        for(rbyte = 0; ; ){
            cval = fgetc(_fin);
            if(feof(_fin)){
                fprintf(stderr,"unexpected EOF, during text processing\n");
                exit_code = 1;
                goto END;
            }
            if(!esc_f && cval == '\\'){
                esc_f = 1;
                continue;
            }
            if(esc_f){
                switch(cval){
                case '"':
                case '\\':
                    break;
                case 'r':
                    cval = '\r';
                    break;
                case 'n':
                    cval = '\n';
                    break;
                default:
                    fprintf(stderr, "WARN: unknown escape sequence `\\%1c', will output as is.\n", cval);
                    fprintf(stderr,ERR_POS);
                    fprintf(_fout, "\\");
                    rbyte++;
                    /*exit_code = 1;
                    goto END;*/
                }
            }
            rbyte++;
            if(!esc_f && cval == '"'){
                /* null padding */
                cval = '\0';
                for( pbyte = rbyte; pbyte <= zbyte; pbyte++ ){
                    fprintf(_fout,"%1c",cval);
                    if(_fmarge) fseek(_fmarge, sizeof(cval), SEEK_CUR);
                }
                zbyte = 0;
                break;
            }else if( zbyte && rbyte > zbyte ){
                fprintf(stderr,
                    "ERROR: too long text-string for `Z'spec. specified=%d, actual=%d\n",
                    zbyte, rbyte);
                fprintf(stderr,ERR_POS);
                exit_code = 1;
                goto END;
            }
            fprintf(_fout,"%1c",cval);
            if(_fmarge) fseek(_fmarge, sizeof(cval), SEEK_CUR);
            esc_f = 0;
        }
        c = 0;
    }
        break;
    default:
        if(!ispunct(c) && !isspace(c)){
            fprintf(stderr,"ileagal letter(%#x,`%1c')\n", c, (char)c);
            fprintf(stderr,ERR_POS);
            exit_code = 1;
            goto END;
        }
        c = 0;
        break;
    }
}

END:
if(_fin) fclose(_fin);
if(_fout) fclose(_fout);
if(_fmarge) fclose(_fmarge);
if(_fnin) free(_fnin);
if(_fnout) free(_fnout);
if(_fnmarge) free(_fnmarge);
return exit_code;
}
