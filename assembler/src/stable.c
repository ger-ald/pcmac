#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>

#include "stable.h"

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
#include "tfunc.h"
#include "utilits.h"

static int hashpjw(char *s);
static void list_tree(struct symbol *j);
static void write_tree(struct symbol *j);




/*
 ** HASH function
 **
 ** I found this function on the page 436. of the dragon book.
 **
 ** The dragon book:
 ** Aho-Sethi-Ulman : Compilers
 **			Principles, techniques, and Tools
 ** Addison-Wesley Publishing Company 1986
 **
 ** The only modification what was made by me is the letter 'l'
 ** following the 0xf0000000 constant for the specifying it long
 ** explicitly. ( The constant PRIME can be found in the file defines.h
 ** either, but the redefinition of it is identical. )
 */
#define PRIME 211
static int hashpjw(char *s)
{
	char *p;
	unsigned long h = 0, g;
	for(p = s; *p != '\0'; p = p + 1)
	{
		h = (h << 4) + (*p);
		if(h & MASK)
		{
			g = h;
			h = h ^ (g >> 24);
			h = h ^ g;
		}
	}
	return h % PRIME;
}

/* This hash function is an extension of the hash function hashpjw()
 ** The parameter 'k' points out that we want the hash value of a
 ** syntax definition or a string which must match a syntax
 ** definition. The difference :
 **	switch( k ) {
 **		case 1: Convert every * to \*
 **				      \ to \\
**				      " to \"
 **			and stop when end of string or space.
 **		break;
 **		case 0: Do not convert but stop when
 **			End of string or
 **			space         or
 **			back-slash space pair.
 **		break;
 **		default: Internal compiler error.
 **		break;
 **		}
 ** So the syntax definition will get the same hash value as the line
 ** which match it. (This hash value is always case insensitive.)
 */
int machash(char *s, int k)
{
	char *p;
	unsigned long h = 0, g;
	int bslashwas; /* When calculating the hash value of a syntax definition. */
	switch(k)
	{
		case 1:/* The argument 's' is a line and not syntax definition. */
			for(p = s; *p != '\0' && !isspace(*p); p++)
			{
				/* Push a back slash before every *, \ and " */
				if(*p == '*' || *p == '\\' || *p == '\"')
				{
					h = (h << 4) + '\\';
					if(h & MASK)
					{
						g = h;
						h ^= g << 24;
//						h ^= g >> 24;//this is the changed line
						h ^= g;
					}
				}/* End if( *p == * or \ or " */
				h = (h << 4) + tolower((int)*p);
				if(h & MASK)
				{
					g = h;
					h ^= g << 24;
//					h ^= g >> 24;//this is the changed line
					h ^= g;
				}
			}/* End for */
			break;
		case 0:/* The argument 's' is a syntax definition. */
			bslashwas = 0;
			for(p = s; *p != '\0' && !isspace(*p) && !(*p == '\\' && isspace(p[1])) && !(*p == '*' && !bslashwas); p +=
					1)
			{
				bslashwas = (*p == '\\');
				h = (h << 4) + tolower((int)*p);
				if(h & MASK)
				{
					g = h;
					h ^= g << 24;
//					h ^= g >> 24;//this is the changed line
					h ^= g;
				}
			}/* End for */
			break;
		default:
			error("011 internal error!", INTERNAL);
			break;
	}/* End of switch( k ) */
	return h % PRIME;
}/*End of function machash() */

/*
 * Search in the table.
 *
 * not in there is make new.
 *
 * returns a pointer pointing to the searched item.
 */
//#define __FUNC__ "search_in_the_table"
struct symbol *search_in_the_table(char *s)
{
	struct symbol **work_pointer;
	int k;
	work_pointer = &(hashtable[hashpjw(s)]);
	while(*work_pointer && (k = strcmp(s, (*work_pointer)->name_of_the_symbol)))
	{
		work_pointer = k > 0 ? &((*work_pointer)->big_son) : &((*work_pointer)->small_son);
	}
	if(*work_pointer)
		return *work_pointer;
	*work_pointer = (struct symbol *)malloc(sizeof(struct symbol));
	if(!*work_pointer)
	{
		fprintf(stderr, "Not enough memory!\n");
		exit(1);
	}
	(*work_pointer)->name_of_the_symbol = strdup(s);
	if(!(*work_pointer)->name_of_the_symbol)
	{
		fprintf(stderr, "Not enough memory!\n");
		exit(1);
	}
	(*work_pointer)->type_of_the_symbol = UNDEFINED;
	(*work_pointer)->public = 0;
	(*work_pointer)->value_of_the_symbol = 0;
	(*work_pointer)->relocatable = 0;
	(*work_pointer)->big_son = (*work_pointer)->small_son = NULL;
	return *work_pointer;
}
//#undef __FUNC__

/*
 * Write symbol table to list file
 *
 * creates a listing of labels and variables in the symboltable (-s)
**/
void listsymbols(void)
{
	int i;
	fprintf(listfile, "\nSYMBOL TABLE:\n");
	fprintf(listfile, "-------------\n");
	for(i = 0; i < PRIME; i++) /* Go thru the hash table. */
		list_tree(hashtable[i]);
}
static void list_tree(struct symbol *j)
{
	if(j == NULL)
		return;
	list_tree(j->small_son);
	list_tree(j->big_son);
	switch(j->type_of_the_symbol)
	{
		case LABEL:
			fprintf(listfile, "LABEL    ");
			break;
		case VARIABLE:
			fprintf(listfile, "VARIABLE ");
			break;
	}
	switch(j->type_of_the_symbol)
	{
		case LABEL:
		case VARIABLE:
		{
			fprintf(listfile, "%32s %cR ", j->name_of_the_symbol, j->relocatable ? ' ' : 'N');
			fprintf(listfile, "%08lX\n", j->value_of_the_symbol);
		}
	}
}/* End of the function list_tree() */



static FILE *fp;
/*
 * Write header file
 *
 * creates the file for the create header option (-h)
**/
void write_header(void)
{
	int i;

	fp = fopen(outfilnam, "w");
	for(i = 0; i < PRIME; i++) /* Go thru the hash table. */
		write_tree(hashtable[i]);

	fclose(fp);
}
static void write_tree(struct symbol *j)
{
	if(j == NULL)
		return;
	write_tree(j->small_son);
	write_tree(j->big_son);
	switch(j->type_of_the_symbol)
	{
		case LABEL:
			fprintf(fp, "%-18s\tEQU\t", j->name_of_the_symbol);
			if(j->relocatable)
				fprintf(fp, " doreloc(");
			else
				fprintf(fp, "doureloc(");
			fprintf(fp, "0%08lXh )\n", j->value_of_the_symbol);
			break;
		default:
			break;
	}
}/* End of the function write_tree() */





