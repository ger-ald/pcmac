/* TFUNC.H

 Macros, and type definition only for the case
 if we compile under UNIX and not with TURBOC.
 */

#ifndef TFUNC_H_
#define TFUNC_H_

int tfuncstricmp(char *s1, char *s2);
void tfuncstrnset(char *str, char ch, unsigned n);

#define strnset( str , ch , n )  tfuncstrnset( str , ch , n )
#define stricmp( s1,s2 )  tfuncstricmp( s1,s2 )

#endif /* TFUNC_H_ */
