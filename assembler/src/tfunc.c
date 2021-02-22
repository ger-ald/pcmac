/* TFUNC.C

 Implementation of functions that are defined under TURBOC,
 but are not under UNIX.
 It is compiled only if the compiler is not TURBOC.

 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>

#include "tfunc.h"

#include "defines.h"
#include "types.h"
#include "vars.h"

/* Sets the first n characters of
 ** string str containing the character
 ** ch. If n>strlen(str) then strlen is set to n.
 */
void tfuncstrnset(char *str, char ch, unsigned n)
{
	int flag;

	flag = 0;
	while(n)
	{
		if(!*str)
			flag = 1;
		*str = ch;
		str++;
		n--;
	}
	if(flag)
		*str = '\0';
}

/* compare s1 and s2 strings
 ** but no case sensytive
 */
int tfuncstricmp(char *s1, char *s2)
{
	while((int)*s1 && (int)*s2 && tolower((int)*s1) == tolower((int)*s2) )
	{
		s1++;
		s2++;
	}
	return (int)(tolower((int)*s1) - tolower((int)*s2));
}
