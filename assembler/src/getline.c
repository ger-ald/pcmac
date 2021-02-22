/*
 * getline.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>

#include "getline.h"

#include "defines.h"
#include "types.h"
#include "vars.h"

#include "error.h"
#include "express.h"
#include "file.h"
#include "generate.h"
#include "init.h"
#include "libget.h"
#include "libmak.h"
#include "objmad.h"
#include "pass.h"
#include "primitiv.h"
#include "stable.h"
#include "tfunc.h"
#include "utilits.h"

/*
 *
 * Gives back a line in the array 'cline'
 *
 * If there is a macro currently extending then
 * it extends the next line of the macro, and gives back that line.
 */
int get_line(void)
{
	int eofindicator;
	int i;
	while(1)
	{
		if(mslast)
		{/* There is a macro being extended. */
			if(*listline && listswitch && maclist)
			{
				/* - appears if compilation is switched off */
				/* and + appears if switched on, but no code*/
				if(isspace(*listline))
				{
					if(fly_over_the_lines)
						*listline = '-';
					else
						*listline = '+';
				}

				fprintf(listfile, "%s\n", listline);
				*listline = '\0';
			}
			/* Write out the error messages. */
			flusherr();
			if(!mslast->mlin)
			{
				/* We should use this way, and we must not pop a macro
				 before the very last line had been executed
				 because that very last line can be a closure of a loop. */
				pop_macro();
				continue;
			}
			strcpy(cline, mslast->mlin->exl);
			cmline = mslast->mlin;
			mslast->mlin = mslast->mlin->nextexeline;
			extend_line(cline, mslast->currset);
			if(listswitch && maclist)
			{
				if(!*listline)
					strcpy(listline, "                            ");
				listline[ADDRESS + 3 * VALUESS + 1] = '\0';
				strnset(listline, ' ', ADDRESS+3*VALUESS);
				strcat(listline, "\t");
				strcat(listline, cline);
				nrprvl = 0;
			}
			if(fly_over_the_lines)
			{
				/* If the compilation is off and the line is not
				 a directive then get the next line! */
				for(i = 0; isspace(cline[i]); i++)
					;
				if(cline[i] != '#')
					continue;
			}
			return !EOF;
		}
		else
		{/* There is not any macro currently extending. */
			eofindicator = next_line();
			if(fly_over_the_lines && eofindicator != EOF)
			{
				/* If the compilation is off, we have not reached the end of
				 the file and the line is not a directive then get the next line! */
				for(i = 0; isspace(readinbuff[i]); i++)
					;
				if(readinbuff[i] != '#')
					continue;
			}
			strcpy(cline, readinbuff);
			return eofindicator;
		}
	}/* End of the infinite loop */
	return 0;
}

/*
 * pushes a macro into the macro stack.
 *
 */
void push_macro(struct macro *mcr, char **sets_lexeme)
{
	//struct macro *mcr; Points to the macro in the symbol table.
	//char **sets_lexeme; Points the lexemes to extend the lines with.
	struct macrostack *ptr;

	/* Make room for the new item in the macro stack. */
	ptr = (struct macrostack *)malloc(sizeof(struct macrostack));
	if(!ptr)
		error("End of memory!", FATAL);
	/* mslast points the TOS (top of stack.) */
	ptr->prevmac = mslast;
	/* Macro lines starts whith the first line of the macro. */
	ptr->mlin = mcr->firstline;
	/* The current set array is the array which was given as an argument. */
	ptr->currset = sets_lexeme;
	/* The macro is the macro. */
	ptr->mcro = mcr;
	/* And since now this is the last. */
	mslast = ptr;
	mac_nest_deep++;
}/* End of the function push_macro(). */
/*
 * drop out the last macro from the macro stack
 */
void pop_macro(void)
{
	int i;
	struct macrostack *ptr;
	struct loopstack *lsptr;
	if(!mslast)
		error("005 internal error!", INTERNAL);

	while(loopstacklast && loopstacklast->m_n_d == mac_nest_deep)
	{
		/* Clear the unclosed loops from the stack. */
		error("Not closed loop in the macro.", NORMAL);
		lsptr = loopstacklast;
		loopstacklast = loopstacklast->prev;
		free(lsptr);
	}
	if(!mac_nest_deep--)
		error("008 internal error!", INTERNAL);
	ptr = mslast;
	mslast = mslast->prevmac;
	for(i = 0; i < ptr->mcro->nrofsets; i++)
		free(ptr->currset[i]);
	free(ptr->currset);
	free(ptr);
}
