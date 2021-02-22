/*
 * libmak.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>

#include "libmak.h"

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
#include "objmad.h"
#include "pass.h"
#include "primitiv.h"
#include "stable.h"
#include "tfunc.h"
#include "utilits.h"

/*
 * This function writes a macro library file.
 *
 * The format of this file is readable:
 *
 *   |macroarg character
 *   |directions
 *   |number of the sets (in decimal form)
 *   |name of the 1st set
 *   |the members of the 1st set
 *   |name of the 2nd set
 *   |the members of the 2nd set
 *   | .
 *   | .                        (everyone terminated by zero)
 *   | .						(names are terminated by nl.)
 *   |name of the last set
 *   |the members of the last set
 *   |the number of the sets that the 1st macro have ':'
 *    the row numbers of the sets separated by ','
 *   |the syntax definition line
 *   |the number of the executable lines (decimal form)
 *   |the executable lines separated by new lines.
 *
 *   |the same for the 2nd macro and so on.
 *
 *    Peter Verhas 1988. july 05.
 *
 */
void write_library(void)
{
	int i, j;
	int number_of_the_exelines;
	struct exeline *exeptr;
	struct macro *macptr;

	libfile = fopen(outfilnam, "w");
	if(!libfile)
		error("Unable to open the library file.", FATAL);
	/* The first item in the library   */
	/* The macro argumentum character. */
	fprintf(libfile, "%c", macarg);
	/* The macro numbering character. */
	fprintf(libfile, "%c", macnum);
	/* The direction. */
	fprintf(libfile, "%c", dwflag + 2 * ddflag);
	/* file is the number of the sets. */
	fprintf(libfile, "%d\n", Nrofthesets);

	/* We write out the sets. */
	setnumbering = 4;/* The predefined set have the number 0,1,2,3. */
	/* So we must start whith the value 4. 		*/
	for(i = 0; i < PRIME; i++)
	{
		libsetlis(hashtable[i]);
	}

	/* We write out the macros. */
	for(i = 0; i < PRIME; i++)
	{
		macptr = macroot[i];
		if(maclast[i])
		{/* We unlink the empty hash, if it is linked. */
			/* And we go thru the list   */
			/* only in case if there is. */
			maclast[i]->nextmacro = NULL;
			while(macptr)
			{
				fprintf(libfile, "%d:", macptr->nrofsets);
				for(j = 0; j < macptr->nrofsets; j++)
					fprintf(libfile, "%ld,", ((macptr->setsymbols)[j])->value_of_the_symbol);

				fprintf(libfile, "\n%s\n", macptr->sdef);

				/* We count the number of the exelines. */
				for(number_of_the_exelines = 0, exeptr = macptr->firstline; exeptr;
						exeptr = exeptr->nextexeline, number_of_the_exelines++)
					;

				fprintf(libfile, "%d\n", number_of_the_exelines);

				for(exeptr = macptr->firstline; exeptr; exeptr = exeptr->nextexeline)
					fprintf(libfile, "%s\n", exeptr->exl);

				macptr = macptr->nextmacro;
			}/* End of while(macros). */
		}/* We finished an item in the hash table. */
	}/* We finished all the macros. */
}/* End of function write library. */

void libsetlis(struct symbol *l)
{
	if(!l)
		return;
	if(l->type_of_the_symbol == SET && l->stpointer->settype == STRINGSET)
	{
		l->value_of_the_symbol = setnumbering++;
		fprintf(libfile, "%s\n", l->name_of_the_symbol);
		fprintf(libfile, "%s%c", l->stpointer->members, 0);
	}
	libsetlis(l->small_son);
	libsetlis(l->big_son);
	return;
}/*End of recursive function libsetlis() */
