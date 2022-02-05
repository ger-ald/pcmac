/*
 * file.c
 *
 *  Created on: 23 okt. 2014
 *      Author: gerald
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>

#include "file.h"

#include "defines.h"
#include "types.h"
#include "vars.h"

#include "error.h"
#include "express.h"
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
#include "utilits.h"

/*
 **
 ** The lowest level file handler system in the assembler.
 **
 ** The function next_line() returns with the next
 ** line of the file in the text buffer: 'readinbuff'.
 ** If a line contains an #include
 ** preprocessor directive then it goes forward for the next file.
 ** If file not found then asks the user for a new name or retry
 ** after changing the disk, or abort assembly.
 **
 ** The init_assembly_system() initialises the file system.
 ** Before the first call the name of the original file
 ** must be in the variable 'name'.
 **
 ** The return value of this function is EOF if we reached the end of the file,
 ** and !EOF if we had line to be read in.
 **
 ** Verhas Peter    1988. marcius 13.
 **
 */
int next_line(void)
{
	int character;
	int i, j;
	static long lintable[MAXINCNEST];
	if(*listline && listswitch)
	{
		if(isspace(*listline))
		{
			if(fly_over_the_lines)
				*listline = '-';
			else
				*listline = '+';
		}
		fprintf(listfile, "%s\n", listline);
	}
	/* Write out the error messages. */
	flusherr();
	while(1)
	{
		/* We have to read in a new line. */
		if(!inputfile[incdepth])
		{
			if(!incdepth)
				*symtline = 0;
			else
			{
				lintable[incdepth - 1] = *symline;
				*symline = 0;
			}
			/* We have to open a new file to read. */
			inputfile[incdepth] = open_a_file_for_read(name, O_TEXT);
			continue;
		}
		/* There is file to read from. */
		character = getc(inputfile[incdepth]);
		if(character == EOF)
		{
			/* We reached the end of a file. */
			fclose(inputfile[incdepth]);
			inputfile[incdepth] = NULL;
			if(!incdepth)
				/* We are in the primary file. */
				return EOF;
			incdepth--;/* Step back to the previous file. */
			*symline = lintable[incdepth];
			if(!incdepth)
				listswitch = mlistswitch;
			continue;
		}
		for(buffpos = 0; character != '\n' && character != EOF; buffpos++)// can overflow
		{
			readinbuff[buffpos] = character;
			character = getc(inputfile[incdepth]);
		}
		/* The '\n' or the EOF is not copied. */
		readinbuff[buffpos] = '\0';
		/* Step the predefinit variables. */
		(*symline)++;
		(*symtline)++;
		/* Reset listswitch for disabling lists of include files. */
		if(!ilistswitch && incdepth)
			listswitch = 0;
		for(buffpos = 0; isspace(readinbuff[buffpos]); buffpos++)
			;
		if(readinbuff[buffpos++] == '#')
		{
			/* Preprocesszor directive maybe #INCLUDE or #MACROS */
			/* Step over the spaces. */
			while(isspace(readinbuff[buffpos]))
				buffpos++;
			/* Check if the directive is an #include or a #macros. */
			i = check_directive(&(readinbuff[buffpos]), "include");
			j = check_directive(&(readinbuff[buffpos]), "macros");

			if(i || (j && pass == 1))
			{
				while(isalpha(readinbuff[buffpos]))
					buffpos++;

				/* The word followoving the # was "include", */
				/* or "macros" and this is the 1 pass.       */
				/* Step over the spaces. */
				while(isspace(readinbuff[buffpos]))
					buffpos++;
				for(i = 0; readinbuff[buffpos] && !isspace(readinbuff[buffpos]); i++, buffpos++)
					name[i] = readinbuff[buffpos];
				name[i] = '\0';
				if(!fly_over_the_lines)
				{
					if(incdepth >= MAXINCNEST)
						error("Too deep include in the file system!", FATAL);
					incdepth++;
				}
				buffpos = 0;
				if(!*listline)
					strcpy(listline, "                            ");
				listline[ADDRESS + 3 * VALUESS + 1] = '\0';
				strnset(listline, ' ', ADDRESS+3*VALUESS);
				strcat(listline, "\t");
				strcat(listline, readinbuff);
				nrprvl = 0;
				lmori = 1;
				return !EOF;
			}
			else
			/* #macros and second pass */
			if(j && pass == 2)
			{
				buffpos = 0;
				if(!*listline)
					strcpy(listline, "                            ");
				listline[ADDRESS + 3 * VALUESS + 1] = '\0';
				strnset(listline, ' ', ADDRESS+3*VALUESS);
				strcat(listline, "\t");
				strcat(listline, readinbuff);
				nrprvl = 0;
				lmori = 1;
				return !EOF;
			}
		}
		buffpos = 0;
		if(!*listline)
			strcpy(listline, "                            ");
		listline[ADDRESS + 3 * VALUESS + 1] = '\0';
		strnset(listline, ' ', ADDRESS+3*VALUESS);
		strcat(listline, "\t");
		strcat(listline, readinbuff);
		nrprvl = 0;
		lmori = 0;
		return !EOF;
	}/* End of if( !readinbuff[buffpos] ) */
	return 0;
}/* End of function next_line() */

FILE *open_a_file_for_read(char *filename, int type)
{
	char fopenName[MAXLINLEN];
	char s1[MAXLINLEN];
	char *s;
	int i;
	FILE *fp = NULL;

	fopenName[0] = '\0';
	s = s1;
	strcpy(s, filename);

	if(*s == '<')
	{
		/* file should come from the include dir */

		strcpy(fopenName, include_directory);
		s++;
		i = strlen(s);
		while(i && isspace(s[i - 1]))
			i--;
		if(s[i - 1] != '>')
			error("Error in file name!", NORMAL);
		else
			s[i - 1] = '\0';
	}
	else if(*s == '\"')
	{
		/* filename has quotes to remove */

		s++;
		i = strlen(s);
		while(i && isspace(s[i - 1]))
			i--;
		if(s[i - 1] != '\"')
			error("Error in file name!", NORMAL);
		else
			s[i - 1] = '\0';
	}

	strcat(fopenName, s);
	switch(type)
	{
		case O_TEXT:
			fp = fopen(fopenName, "r");
			break;
		case O_BINARY:
			fp = fopen(fopenName, "rb");
			break;
		default:
			sprintf(name, "Invalid file open type=%d in 'open_a_file_for_read'!", type);
			error(name, INTERNAL);
	}
	if(fp)
		return fp;
	snprintf(name, sizeof(name), "Unable to open the file %s.", fopenName);
	error(name, FATAL);
	return NULL;
}/* End of function open_a_file_for_read() */
