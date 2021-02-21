/* TFUNC.H

   Macros, and type definition only for the case
   if we compile under UNIX and not with TURBOC.
*/
#ifndef __TURBOC__
#define strdup( s ) tfuncstrdup( s )
#define strnset( str , ch , n )  tfuncstrnset( str , ch , n )
#define stricmp( s1,s2 )  tfuncstricmp( s1,s2 )

char *tfuncstrdup();
#endif
