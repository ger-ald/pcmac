/* TFUNC.C

   Implementation of functions that are defined under TURBOC,
   but are not under UNIX.
   It is compiled only if the compiler is not TURBOC.

*/
#ifndef __TURBOC__

#include "makeflag.h"

#if MAKE
#include "include.h"
#include "defines.h"
#include "functype.h"
#include "types.h"
#include "vars.h"
#include "tfunc.h"
#endif
/* Put the string s
** into a newly created place
*/
char *tfuncstrdup( s )
char *s;
{
	char *ptr;

	ptr = (char *)malloc( 1+ strlen( s ) );
	if( ptr )
		strcpy( ptr , s );
	return ptr;
}

/* Sets the first n characters of
** string str containing the character
** ch. If n>strlen(str) then strlen is set to n.
*/
void tfuncstrnset( str , ch , n )
char *str;
char ch;
unsigned n;
{
	int flag;

	flag = 0;
	while( n ) {
		if( !*str ) flag = 1;
		*str = ch;
		str++;
		n--;
	}
	if( flag )
		*str = '\0';
}

/* compare s1 and s2 strings
** but no case sensytive
*/
int  tfuncstricmp( s1,s2 )
char *s1,*s2;
{
	while( *s1 && *s2 &&
	    tolower(*s1) == tolower(*s2) )s1++,s2++;
	return tolower(*s1)-tolower(*s2);
}

#endif