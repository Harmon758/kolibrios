/*
** All character classification functions except isascii().
** Integer argument (c) must be in ASCII range (0-127) for
** dependable answers.
*/

#define __ALNUM     1
#define __ALPHA     2
#define __CNTRL     4
#define __DIGIT     8
#define __GRAPH    16
#define __LOWER    32
#define __PRINT    64
#define __PUNCT   128
#define __BLANK   256
#define __UPPER   512
#define __XDIGIT 1024

extern char __is[128];

#define isalnum(c)(__is[c] & __ALNUM ) /* 'a'-'z', 'A'-'Z', '0'-'9' */
#define isalpha(c)(__is[c] & __ALPHA ) /* 'a'-'z', 'A'-'Z' */
#define iscntrl(c)(__is[c] & __CNTRL ) /* 0-31, 127 */
#define isdigit(c)(__is[c] & __DIGIT ) /* '0'-'9' */
#define isgraph(c)(__is[c] & __GRAPH ) /* '!'-'~' */
#define islower(c)(__is[c] & __LOWER ) /* 'a'-'z' */
#define isprint(c)(__is[c] & __PRINT ) /* ' '-'~' */
#define ispunct(c)(__is[c] & __PUNCT ) /* !alnum && !cntrl && !space */
#define isspace(c)(__is[c] & __BLANK ) /* HT, LF, VT, FF, CR, ' ' */
#define isupper(c)(__is[c] & __UPPER ) /* 'A'-'Z' */
#define isxdigit(c)(__is[c] & __XDIGIT) /* '0'-'9', 'a'-'f', 'A'-'F' */

#define isascii(c) (!((c)&(~0x7f)))
#define toascii(c) ((c)&0x7f)

