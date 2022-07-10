/*
 * utilits.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>

#include "utilits.h"

#include "defines.h"
#include "types.h"
#include "vars.h"

#include "error.h"
#include "express.h"
#include "file.h"
#include "generate.h"
#include "getline.h"
#include "init.h"
#include "libget.h"
#include "libmak.h"
#include "objmad.h"
#include "pass.h"
#include "primitiv.h"
#include "stable.h"
#include "tfunc.h"

/* Cut the comment from the end of the line.
 *
 * Returning value is the sum of these:
 *      1   String not terminated
 *      2   Character not terminated
 *	4   The very last character was a backslash
 *
 *	if no one then return zero .
 */
int cut_comment(char *s)
{
	int i;
	int instring, incharacter, bslashwas;
	instring = incharacter = bslashwas = 0;
	for(i = 0; s[i]; i++)
	{
		if(instring && !bslashwas && s[i] == '\"')
		{
			instring = 0;
			continue;
		}
		if(incharacter && !bslashwas && s[i] == '\'')
		{
			incharacter = 0;
			continue;
		}
		if(!instring && !incharacter && s[i] == '\"')
		{
			instring = 1;
			bslashwas = 0;
			continue;
		}
		if(!incharacter && !instring && s[i] == '\'')
		{
			incharacter = 1;
			bslashwas = 0;
		}
		if((instring || incharacter) && !bslashwas && s[i] == '\\')
		{
			bslashwas = 1;
			continue;
		}
		if(!instring && !incharacter && s[i] == ';')
		{
			s[i] = '\0';
			break;
		}
		bslashwas = 0;
	}
	return (instring) | (incharacter << 1) | ((s[i - 1] == '\\') << 2);
}/* End of cut_comment();*/

/*
 ** Check a directive.
 **
 ** returns TRUE if the line 's' contains the directive 'l'.
 **
 */
int check_directive(char *s, char *l)
{
	/* Eat the spaces before the directive. */
	while(isspace(*s))
		s++;
	while(isalpha(*s) && *l != '\0')
	{
//		if(*l != tolower(*s))
		if(*l != tolower((int)*s))// the changed line
			break;
		l++;
		s++;
	}
	return *l == '\0' && (isspace(*s) || !*s);
}/* End check_directive(). */

/*
 ** Split line. (splits line into label and rest of line. label (aka s) can be null)
 **
 ** Splits the line 'l' into 's' and 't'.
 ** 's' contains the label if was
 ** 't' contains the rest of the line
 **
 */
void split_the_line(char *l, char *s, char *t)
{
	int i, j;
	i = 0;

	while (isspace(l[i]))
		i++;/* Step over the spaces being */
			/* on the start of the line.  */
	if (isIDalpha(l[i]))
	{
		// The line probably has label.
		l = &(l[i]);/* Cut spaces from the start of the line. */
		i = 0;
		while (isIDchar(l[i]))
			i++; /* Step over the characters of the label. */
		j = i; /* j is length of the label. */
		while (isspace(l[i]))
			i++; /* Step over the spaces following the label. */
		if (l[i] == ':')
		{
			// The line probably has a label.
			//   'blabla: <maybe more line>'
			//   'blabla := 42' (is also seen as label)
			
			/* Throw away the colon. */
			i++;
			if (l[i] == '=')
				i--; /* If it is := then we don't */

			while (isspace(l[i]))
				i++; /* Step over the spaces */
			/* following the colon. (or not if ':=') */
			strncpy(s, l, j); /* Copy the label to s. */
			s[j] = '\0';
			strcpy(t, &(l[i])); /* Copy the rest of the line to t. */
			return;
		}
		else if (((l[i] == 'e') || (l[i] == 'E')) &&
				((l[i+1] == 'q') || (l[i+1] == 'Q')) &&
				((l[i+2] == 'u') || (l[i+2] == 'U')) &&
				isspace(l[i+3]))
		{
			// The line probably has a label.
			//   'blabla equ 42' (is also seen as label)

			// Don't throw away any chars from 'equ'.

			// Spaces have already been skipped.

			strncpy(s, l, j); // Copy the label to s.
			s[j] = '\0';
			strcpy(t, &(l[i])); // Copy the rest of the line to t.
			return;
		}
		else
		{
			// The line has no label.
			*s = '\0';
			strcpy(t, l);	/* Copy the whole line onto t */
							/* since it has no label.     */
			return;
		}
	}
	else
	{
		// The line has no label.
		strcpy(t, &(l[i]));	/* Copy the whole line onto t */
							/* since it has no label.     */
		*s = '\0';
		return;
	}
}/* End of the split_the_line() */

/*
 **
 ** Extends the line
 ** #0 is the first set
 ** #1 is the second and so on
 **
 **
 */
void extend_line(char *line, char **setstrings)
{
	static char buffer[MAXLINLEN];
	/* 8 character is totally enough for a number. */
	static char number[9];
	static char lnumber[9];
	int i, j, k;
	int bslw, ich, ist;
	int setnum;
	bslw = /* The previous character was back slash. */
	ich = /* We are in a character. */
	ist = 0;/* We are in a string. */
	i = j = 0;
	sprintf(number, "%d", macextnum);
	if(pass == 1)
		sprintf(lnumber, "%d", mslast->mcro->num1);
	else if(pass == 2)
		sprintf(lnumber, "%d", mslast->mcro->num2);
	else
		error("013 Internal error.", INTERNAL);

	while(line[i])
	{
		if(!ist && !ich && line[i] == MACARG && isdigit(line[i + 1]))
		{
			i++; /* Step over the sharp character onto */
			/* the first character of the number. */
			setnum = 0;/* Calculate the number of the set. */
			while(isdigit(line[i]))
				setnum = ((10 * setnum) + (line[i++] - '0'));
			k = 0;/* Copy the lexeme of the set to the target line. */
			buffer[j++] = setstrings[setnum][k++];
			while(setstrings[setnum][(k-1)])
				buffer[j++] = setstrings[setnum][k++];
			j--;
			continue;
		}
		if(!ist && !ich && line[i] == MACNUM && line[i + 1] == MACNUM)
		{
			/* Global macro number. */
			i++;
			i++;/* We step over the numbering characters. */
			k = 0;
			buffer[j++] = number[k++];
			while(number[(k-1)])
				buffer[j++] = number[k++];
			j--;
			continue;
		}
		if(!ist && !ich && line[i] == MACNUM && line[i + 1] != MACNUM)
		{
			/* Local macro number. */
			i++;/* We step over the numbering character. */
			k = 0;
			buffer[j++] = number[k++];
			while(number[(k-1)])
				buffer[j++] = number[k++];
			j--;
			continue;
		}
		if(line[i] == '\"' && !bslw)
		{
			buffer[j++] = line[i++];
			ist = !ist;
			continue;
		}
		if(line[i] == '\'' && !bslw)
		{
			buffer[j++] = line[i++];
			ich = !ich;
			continue;
		}
		if((ist || ich) && line[i] == '\\' && !bslw)
		{
			buffer[j++] = line[i++];
			bslw = 1;
			continue;
		}
		buffer[j++] = line[i++];
		bslw = 0;
	}/* End of while( line[i] ) */
	buffer[j] = '\0';
	strcpy(line, buffer);
}/* End of extend_line() */

/*
 **
 ** This function works up a variable list.
 ** The variable 'first_unused_character' must point after the
 ** .var keyword.
 **
 */
void build_up_a_var_list(int k)
{
	struct symbol *ptr;
	getsymbol();
	while(symbol == IDENTIFIER)
	{
		ptr = search_in_the_table(name);
		if(ptr->type_of_the_symbol != UNDEFINED)
		{
			if(ptr->type_of_the_symbol != k)
			{
				sprintf(name, "Redeclaring the variable %s.", ptr->name_of_the_symbol);
				error(name, WARNING);
			}
			else
			{
				sprintf(name, "Redefining the symbol %s .", ptr->name_of_the_symbol);
				error(name, NORMAL);
			}
		}
		if(ptr->public)
		{
			if(ptr->type_of_the_symbol == VARIABLE)
				error("Variable can not be public.", NORMAL);
			else if(ptr->type_of_the_symbol == EXTERNAL)
				error("External can not be public.", NORMAL);
		}
		ptr->type_of_the_symbol = k;
		if(k == EXTERNAL)
			ptr->value_of_the_symbol = nexternal++;
		else
			ptr->value_of_the_symbol = 0;
		ptr->relocatable = FALSE;
		SPACEAT;
		if(*first_unused_character != ',')
			break;
		first_unused_character++;
		if(!isIDalpha(*first_unused_character))
			break;
		getsymbol();
	}/* End of the while( symbol == IDENTIFIER ) */
	if(*first_unused_character && *first_unused_character != ';')
	{
		sprintf(name, "Unexpected characters on the end of the line: (%s).", first_unused_character);
		error(name, NORMAL);
	}
	if(symbol != IDENTIFIER)
	{
		snprintf(readinbuff, sizeof(name), "%s is reserved word.", name);
		error(readinbuff, NORMAL);
	}
}/* End of function build_up_a_var_list() */
/*
 **
 ** This function builts up a set.
 ** The variable 'first_unused_character' must point after the
 ** const keyword.
 **
 */
void build_up_a_set(void)
{
	struct symbol *sptr;
	int i;
	getsymbol();
	if(symbol != IDENTIFIER)
		error("Identifier expected in the CONST!", NORMAL);
	sptr = search_in_the_table(name);
	if(sptr->type_of_the_symbol != UNDEFINED)
		error("Redefining of an identifier!", NORMAL);
	sptr->type_of_the_symbol = SET;
	Nrofthesets++; /* We count how many sets we have. */
	getsymbol();
	if(symbol != ASSIGN)
		error(":= expected!", NORMAL);
	SPACEAT;
	if(*first_unused_character != '(')
		error("(expected!", NORMAL);
	i = 0;
	first_unused_character++;
	while(*first_unused_character && *first_unused_character != ')')
	{
		name[i++] = *first_unused_character;
		if(*first_unused_character++ == ESCAPECHARACTER && *first_unused_character)
			name[i++] = *first_unused_character++;
		if(*first_unused_character == ',')
		{
			name[i++] = SETMEMBERSEPARATOR;
			first_unused_character++;
		}
	}
	if(!*first_unused_character)
		error(") missing after the const definition", NORMAL);
	name[i++] = '\0';
	sptr->stpointer = (struct set *)malloc(sizeof(struct set));
	if(!(sptr->stpointer))
		error("End of memory!", FATAL);
	sptr->stpointer->members = (char *)malloc(i);
	if(!(sptr->stpointer->members))
		error("End of memory!", FATAL);
	strcpy(sptr->stpointer->members, name);
	sptr->stpointer->settype = STRINGSET;
}

/*
 **
 ** Checks the line s if there is a #i where i is => limit.
 ** Changes every # character to MACARG
 **
 */
void check_sharps(char *s, int limit)
{
	int setnum;
	while(*s)
	{
		if(*s == macarg && isdigit(s[1]))
		{/* We found a sharp */
			*s = MACARG;
			s++;
			setnum = 0;/* Calculate the number of the set. */
			while(isdigit(*s))
				setnum = 10 * setnum + *s++ - '0';
			if(setnum >= limit)
				error("Invalid const number.", NORMAL);
			continue;
		}
		if(*s == macnum)
		{/* We found a macro numbering character. */
			*s = MACNUM;
			s++;
			continue;
		}
		if(*s == '\"')
		{/* A string starts. */
			s++;
			while(*s != '\"')
			{/* While not end of the string. */
				/* If the current character is a backslash */
				/* and there is a next then step over them */
				if(*s++ == ESCAPECHARACTER && *s)
					s++;
			}
		}
		if(*s == '\'')
		{/* A character constant starts. */
			s++;
			while(*s != '\'')
			{/* While not end of the character. */
				/* If the current character is a backslash */
				/* and there is a next then step over them */
				if(*s++ == ESCAPECHARACTER && *s)
					s++;
			}
		}
		/* A backslash outside of a string or character constant. 
		 if( *s == ESCAPECHARACTER ) s++; does not mean anything	*/
		/* If there is more character then step for it. */
		if(*s)
			s++;
	}
}/* End of function check_sharps() */

/*
 ** Checks the macro if it is starting with a #
 ** and compresses it.
 **
 */
char *compress(char *s)
{
	int i;//, j;
//	char prv, prv1;
	/* Eat the starting spaces. */
	while(isspace(*s))
		s++;
	if(!*s)
	{
		error("Empty sintax definition!", NORMAL);
		return s;
	}
	/* If the sintax definition starts with a forced space,
	 then no any line can fit it, because the space before the
	 mnemonic separates the label and the mnemonic.
	 In other words: a mnemonic can not start with a space.
	 A mnemonic also can not start with a # character, because
	 then that is a directive.
	 */
	if((s[0] == '\\' && isspace(s[1])) || *s == '#')
		error("Unmatchable sintax definition.", NORMAL);
	for(i = 0; s[i]; i++)
		if(isspace(s[i]))
			s[i] = ' ';

/*	i = j = 0;
	prv = prv1 = 'A';
	while(s[j] == s[i])
	{

		if(prv != ' ' || prv1 == '\\' || s[j] != ' ')
			j++;
		prv1 = prv;
		prv = s[i++];
	}
*/
	return s;
}/* End of function compress() */

/*
 ** Returns a number for a new stack,
 ** and also occupies it.
 */
int newstack(void)
{
	int i;

	for(i = 0; i < USRSTKSIZ; i++)
		if(!usrsf[i])
			break;
	usrsf[i] = 1;/* Since now it is occupied. */
	return i;
}

/*
 ** Releases a stack.
 ** If it was not occupied it generates a warning.
 */
void releasestack(int k)
{
	struct usrs *p;
	if(!usrsf[k])
	{
		error("Not used user stack free.", NORMAL);
		return;
	}
	usrsf[k] = 0;
	/* Clear the stack */
	while(usrstk[k])
	{
		p = usrstk[k]->nxt;
		free(usrstk[k]);
		usrstk[k] = p;
	}
}

/*
 ** Push a value into the stack
 */
void usrpush(valtype v, int s)
{
	struct usrs *p;

	if(s >= USRSTKSIZ || s < 0 || !usrsf[s])
	{
		error("Wrong stack number.", NORMAL);
		return;
	}

	p = (struct usrs *)malloc(sizeof(struct usrs));
	if(!p)
		error("End of memory!", FATAL);
	p->value = v;
	p->nxt = usrstk[s];
	usrstk[s] = p;
}

/*
 ** Pops a value from the stack, and returns it.
 */
valtype usrpop(int s)
{
	struct usrs *p;
	valtype v;

	if(s >= USRSTKSIZ || s < 0 || !usrsf[s])
	{
		error("Wrong stack number.", NORMAL);
		return (valtype)0;
	}

	if(!usrstk[s])
	{
		error("Poping an empty stack.", NORMAL);
		return (valtype)0;
	}

	p = usrstk[s];
	usrstk[s] = p->nxt;
	v = p->value;
	free(p);
	return v;
}

/*
 ** Reads the top of a stack and returns it.
 */
valtype usrtos(int s)
{

	if(s >= USRSTKSIZ || s < 0 || !usrsf[s])
	{
		error("Wrong stack number.", NORMAL);
		return (valtype)0;
	}

	if(!usrstk[s])
	{
		error("Reading the TOS of an empty stack.", NORMAL);
		return (valtype)0;
	}
	return usrstk[s]->value;
}
