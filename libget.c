#include "makeflag.h"

#if MAKE
#include "include.h"
#include "defines.h"
#include "functype.h"
#include "types.h"
#include "vars.h"
#include "tfunc.h"
#endif
/*
** This program reads a macro library from the file libfile.
**
** The format of this file is readable:
**
**   |the macro argumentum character
**   |direction
**   |number of the sets (in decimal form)
**   |name of the 1st set
**   |the members of the 1st set
**   |name of the 2nd set
**   |the members of the 2nd set
**   | .
**   | .                        (everyone terminated by zero)
**   | .						(names are terminated by nl.)
**   |name of the last set
**   |the members of the last set
**   |the number of the sets that the 1st macro have ':' 
**    the row numbers of the sets separated by ','
**   |the sintax definition line
**   |the number of the executable lines (decimal form)
**   |the executable lines separated by new lines.
**   
**   |the same for the 2nd macro and so on.
**
**    Peter Verhas 1988. marcius+4 05. 
**
*/
void read_library()
{
	int Nrofsets;
	int Nrofcset;
	int exelinum;
	int setnumber;
	int c;
	int i,j;
	int macqueindx;
	struct symbol **setptr;
	struct exeline **exeptr;
	struct set    **ststpointer;
	struct symbol **sysypointer;

	/* The macro argumentum character. */
	macarg = getc( libfile );
	/* The macro numbering character.  */
	macnum = getc( libfile );
	/* The byte generation direction. */
	c = getc( libfile );
	dwflag = c &1;
	ddflag = c/2;
	/* The number of the sets. */
	getint(&Nrofsets);
	if( Nrofsets<3 )
		error("Bad macro library file.",FATAL);

	setptr = (struct symbol **)malloc( Nrofsets * sizeof(struct symbol *));

	c = getc(libfile);
	if( c != '\n' )
		error("Bad macro library file.",FATAL);

	/* Fill the pointers for the predefined sets. */
	setptr[0] = setptr000;
	setptr[1] = setptr001;
	setptr[2] = setptr002;
	setptr[3] = setptr003;

	for( i = 4 ; i <Nrofsets ; i++ ) {
		c = getc(libfile);

		j=0;
		/* Copy the name of the set into the variable name! */
		while( c!='\n' ) {
			name[j++] = c;
			c = getc(libfile);
		}
		name[j] = '\0';
		/* Put it into the symbol table. */
		setptr[i] = search_in_the_table( name );

		if( setptr[i]->type_of_the_symbol != UNDEFINED )
			error( "Bad macro library file.", FATAL);

		/* Read the members of the set. */
		j = 0;
		c = getc(libfile);
		while( c ) {
			name[j++] = c;
			c = getc(libfile);
		}
		name[j] = '\0';
		setptr[i]->stpointer = (struct set *)
		    malloc( sizeof(struct set));
		setptr[i]->stpointer->settype = STRINGSET;
		setptr[i]->stpointer->members = strdup( name );
		if( !(setptr[i]->stpointer->members) )error("End of memory!",FATAL);

	}/* End of working up the sets. */

	/* Now we read the macros. */
	c = !EOF;
	while( 1 ) {
		c = getc( libfile );
		if( c == EOF ) break;
		ungetc(c ,libfile);
		/* Read the number of the set in the current macro.*/
		getint(&Nrofcset);

		/* Make room for the set pointers. */
		ststpointer = (struct set **)
		    malloc( Nrofcset*sizeof(struct set *));
		/* Make room for the set symbol pointers. */
		sysypointer = (struct symbol **)
		    malloc( Nrofcset*sizeof(struct symbol *));

		/* Eat the colon. */
		c = getc( libfile );
		if( c != ':' )
			error("Bad macro library file.",FATAL);
		/* Fill up the ststpointer array. */
		for( i = 0 ; i < Nrofcset ; i++ ){
			getint(&setnumber);
			if( setnumber>=Nrofsets ){
				error("Bad macro library file.",
				    FATAL);
				return;
			}
			ststpointer[i] = setptr[setnumber]->stpointer;
			sysypointer[i] = setptr[setnumber];
			/* Eat the comma. */
			c = getc( libfile );
			if( c != ',' )
				error("Bad macro library file.",FATAL);
		}/* We filled up the stspointer array. */
		/* Read the sintax definition. */
		c = getc(libfile);
		if( c != '\n' )
			error("Bad macro library file.",FATAL);
		c = getc(libfile);
		j = 0;
		while( c != '\n' && c != EOF){
			name[j++] = c;
			c = getc( libfile );
		}
		name[j] = '\0';/* The sdef is in the 'name' */
		macqueindx = machash( name , 0 );

		/* This is the same procedure that is in the file  */
		/* masm.c inthe function macrodef();     		   */
		if( maclast[macqueindx] == NULL ) {
			/* If there is not any macro defined previously on this queue. */
			/* Open the macro queue. */
			macroot[macqueindx] =
			    maclast[macqueindx] = (struct macro *)malloc(sizeof(struct macro));
			if( !maclast[macqueindx] )
				error("End of memory!",FATAL);
			if( macqueindx == EMPTYHASH ) {
				/* We open the queue which will contain those macro definitions */
				/* which have zero length for the hash function. Every other    */
				/* queue must be continued with this queue.			*/
				for( i=0 ; i<PRIME ; i++ )
					if( i != EMPTYHASH )
						/* We append this queue after every queue which */
						/* is not this queue.                           */
						if( maclast[i] )
							maclast[i] -> nextmacro =
							    macroot[EMPTYHASH];
						else
							macroot[i] = macroot[EMPTYHASH];
			}
		}
		else {/* Add the new macro to the macro queue. */
			maclast[macqueindx]->nextmacro = (struct macro *)malloc(sizeof(struct macro));
			if( !(maclast[macqueindx]->nextmacro) ) error("End of memory!",FATAL);
			/* Since now this new macro is the last one. */
			maclast[macqueindx] = maclast[macqueindx]->nextmacro;
		}
		/* Since this is the last there is not succesor of it, or the */
		/* succesor of it is the EMTYHASH queue.                      */
		if( macqueindx == EMPTYHASH )
			/* This queue contains those macro definitions, which start        */
			/* with a set. Eg.: ("* *,*",SET1,SET2,SET3)		           */
			/* You can find concrete example in the Z80 macro definition file. */
			maclast[EMPTYHASH]->nextmacro = NULL;
		else
			/* We append those macro definitions which start with a set.    */
			/* The length of this definitions which is accepted by the hash */
			/* function is zero, and the value which is given back by       */
			/* hash function for az empty line is EMPTYHASH.                */
			maclast[macqueindx]->nextmacro = macroot[EMPTYHASH];

		/* Make a new copy the sintax definition, */
		/* We need not compress the name here. */
		maclast[macqueindx]->sdef = strdup(  name  );
		if( !(maclast[macqueindx]->sdef) ) error("End of memory!",FATAL);
		/* The number of the sets. */
		maclast[macqueindx]->nrofsets   = Nrofcset;
		maclast[macqueindx]->setpointer = ststpointer;
		maclast[macqueindx]->setsymbols = sysypointer;

		/* The number of the executable lines. */
		getint(&exelinum);
		/* Eat the new line character. */
		c = getc( libfile );
		if( c != '\n' ){
			error("Bad macro library file.",FATAL);
			return;
		}
		exeptr = &(maclast[macqueindx]->firstline);
		/* Create the exelines. */
		for( i = 0 ; i < exelinum ; i++ ){
			c = getc(libfile);
			j = 0;
			while( c != '\n' && c != EOF ) {
				name[j++] = c;
				c = getc(libfile);
			}
			name[j] = '\0';/* We have an exeline in the 'name'. */
			*exeptr = (struct exeline *)
			    malloc( sizeof(struct exeline ));
			if( !*exeptr )
				error("End of memory!",FATAL);
			/* Make room for the line and copy it. */
			(*exeptr)->exl = strdup( name );
			if( !(*exeptr)->exl )
				error("End of memory!",FATAL);
			exeptr = &((*exeptr)->nextexeline);
		}
		*exeptr = NULL; /* No following exeline. */
	}/* End of reading the macro definitions. */
}
/*
**
** This function reads an integer number from the 
** file libfile into its argument.
**
** The stop character is ungetched back to the file.
**
*/
void getint( a )
int *a;
{
	int  c;
	c = getc( libfile );
	while( isspace( c ) ) c = getc( libfile);

	if(!isdigit(c))
		error("Bad macro library file.",FATAL);
	*a = 0;
	while( isdigit( c ) ){
		*a *=10;
		*a += c-'0';
		c =getc( libfile );
	}
	ungetc(c ,libfile);
}
