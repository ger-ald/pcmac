#include "makeflag.h"

#include "include.h"
#include "defines.h"
#include "types.h"
#include "functype.h"

#if !MAKE
#ifndef __MSDOS__
#include "tfunc.c"
#endif
#endif

#ifndef __MSDOS__
#include "tfunc.h"
#endif
#ifdef DEBUG
#if !MAKE
#endif
#endif

#if MAKE
#include "vars.h"

#else

#include "vars.c"
#include "stable.c"
#include "utilits.c"
#include "error.c"
#include "express.c"
#include "init.c"
#include "file.c"
#include "pass.c"
#include "getline.c"
#include "primitiv.c"
#include "generate.c"
#include "libmak.c"
#include "libget.c"
#include "objmad.c"
#endif


/*
** Function checkline returns TRUE if the line is
**  good for the sintax definition: 'sintax_definition'
*/

int checkline( line , sintax_definition , sets , member , k )
/* Contains the line we want to analize. */
char *line,

/* Contains the sintax definition.	      */
*sintax_definition;

/* Contains pointers.		      */
/* sets[i] points the i-th set.	      */
struct set *sets[];

/* The function gives back the value in  */
/* this array. member[i] will get the    */
/* value j if the i-th set j-th member   */
/* matches the i-th star in the line.    */
char *member[];

/* If k is zero then an expression can   */
/* not contain an undefined label.       */
/* If k is one then no matter.           */
int k;
{
	int sintax_index,line_index,currset,lexlen,line_end_indicator;

	if( !*line ) return FALSE; /* An empty line can not match a macro. */
	sintax_index=
	    line_index=
	    currset=0;
	line_end_indicator = 0;
	while( line[line_index] && !line_end_indicator )
	{
		switch( sintax_definition[sintax_index] )
		{
		case '*':  /* The star means that is must matches a    */
			/* member of the set given.		       */
			lexlen = serch_in_set( &(line[line_index]) ,
			    sets[currset] ,
			    &(member[currset]) , k );
			if( !lexlen ) {
				/* Free the previously allocated strings. */
				/* The last was not allocated! */
				/* member[currset] pointes nothing*/
				while( currset-- ){
					free(member[currset]);
				};
				return FALSE;
			}
			currset++;
			line_index += lexlen;
			break;
		case '\t':
		case ' ' : /* A space matches zero or more spaces.     */
			while( isspace(line[line_index]) )line_index++;
			break;
		case '\0':/* If we reached the end of the sintax defintion */
			/* but not of the line, then there must be spaces */
			/* at the end of the line, but nothing else.      */
			while( isspace(line[line_index]))line_index++;
			line_end_indicator = 1;
			break;
		case ESCAPECHARACTER:
			/* Backslash is the escape character.       */
			/* It means that the next character matches */
			/* only the same character even if that is  */
			/* space, star or another backslash.        */
			sintax_index++;
			/*            NO BREAK !!!!!                */
			/* You MUSTN'T insert other case statement. */
		default  : /* If it is not a special character or if   */
			/* previous  one  was  a '\'  then  this    */
			/* character matches only the same character*/
			if( !charcmp(sintax_definition[sintax_index] ,
			    line[line_index]) ) {
				/* Free the previously            */
				/* allocated strings. 		*/
				/* The last was not allocated!	*/
				/* member[currset] pointes nothing*/
				while( currset-- ){
					free(member[currset]);
				};
				return FALSE;
			}
			line_index++;
			break;
		}/* End of switch. */
		if( sintax_definition[sintax_index] )
					sintax_index++;
	}/* End of while. */
	while( isspace( sintax_definition[sintax_index] ) )
		sintax_index++;
	if( !sintax_definition[sintax_index] &&
	    !line[line_index] 			) return TRUE;
	else {
		/* There are extra character on the end of the line. */
		while( currset-- ){
			free(member[currset]);
		};
		return FALSE;
	}
}/* End of checkline. */

/*
**
** Function serch_in_set returns the length
**  of the lexeme it matches, or zero if it
**   matches no any one.
**
*/

int serch_in_set( lexeme , set_to_search_in , giveback , k )
/* The lexeme we have to search in the set. */
char *lexeme;

/* Pointer to the set we have to search in. */
struct set *set_to_search_in;

/* Pointer pointing a char* where we have to put 	*/
/* the &string of the lexeme. 			*/
char **giveback;

/* If k is zero then it does not accept an expression */
/* for a numeric set, if that contains an undefined label. */
/* If k is 1 then it accepts. */
int k;
{
	int number_of_the_current_member;
	char *current_character_in_the_set;
	int lexeme_index;
	int the_current_character_was_good;
	valtype value;
	int err;
	int relocat;

	number_of_the_current_member = 0;


	if( set_to_search_in->settype == STRINGSET )
	{
		current_character_in_the_set = set_to_search_in->members;
		number_of_the_current_member = 0;
		do{
			lexeme_index=0;
			do{
				switch( *current_character_in_the_set )
				{
				case '\t':
				case ' ' : /* A space matches zero or more spaces.     */
					while( isspace(lexeme[lexeme_index]) )lexeme_index++;
					the_current_character_was_good = TRUE;
					break;
				case ESCAPECHARACTER:
					/* Backslash is the escape character.       */
					/* It means that the next character matches */
					/* only the same character even if that is  */
					/* space, star or another backslash.        */
					current_character_in_the_set++;
					/*            NO BREAK !!!!!                */
					/* You MUSTN'T insert other case statement. */
				default  : /* If it is not a special character or if   */
					/* previous  one  was  a '\'  then  this    */
					/* character matches only the same character*/
					the_current_character_was_good =
					    charcmp(*current_character_in_the_set ,
					    lexeme[lexeme_index] );
					lexeme_index++;
					break;
				}/* End of switch. */
				current_character_in_the_set++;
			}while( the_current_character_was_good &&
			    *current_character_in_the_set != SETMEMBERSEPARATOR &&
			    *current_character_in_the_set   );


			/* If every character was matched. */
			if( ( !*current_character_in_the_set ||
			    *current_character_in_the_set == SETMEMBERSEPARATOR )
			    && the_current_character_was_good )
			{
				sprintf( name , "%u" , number_of_the_current_member );
				*giveback = strdup( name );
				if( !*giveback )
					error("End of memory!",FATAL);
				/* The length of the lexeme that was found. */
				return lexeme_index;
			}

			/* The next number. */
			number_of_the_current_member++;

			/* Did not match, so we go further till we find */
			/* the separator, or the end of the set.	  */
			while( *current_character_in_the_set != SETMEMBERSEPARATOR
			    && *current_character_in_the_set != '\0' )
				current_character_in_the_set++;

			/* If we found a separator then we step for the  */
			/* first character of the next member.	   */
			if( *current_character_in_the_set )
				current_character_in_the_set ++ ;

			/* While there is next member. */
		}while( *current_character_in_the_set );
		return 0; /* Not matched. */
	}
	else
		if( set_to_search_in->settype == NUMERICSET )
		{
			/* At first we do not accept definitions with */
			/* NUMERICSET, otherwise ld a,(hl) could be   */
			/* compiled to be ld a,(address) if hl is a   */
			/* label equ address.						  */
			if( !k )return 0;
			lexeme_index = expression( lexeme , &value , &err , &relocat );
			if( err == RBRAMISSING || err == SERROR )return 0;
			if( err == UNDEFLABEL && pass == 2 )
				error("Undefined label in the expression.",NORMAL);
			if( err == UNDEFLABEL )
				relocat = 0;
			if( relocat )
				sprintf( name , "doreloc(%lu)" , value );
			else
				sprintf( name , "%lu" , value );
			*giveback = (char *)malloc( strlen( name )+1 );
			if( !*giveback )
				error("End of memory!",FATAL);
			strcpy( *giveback , name );
			return lexeme_index;
		}
		else
			if( set_to_search_in->settype == STRING )
			{
				if( *lexeme !='\"' )
					return 0; /* It is not a string. */
				lexeme_index = 0;
				name[ lexeme_index ] =lexeme[ lexeme_index ];
				lexeme_index++;
				while( lexeme[ lexeme_index ] != '\"' && lexeme[lexeme_index] ) {
					name[ lexeme_index ] = lexeme[ lexeme_index ];
					if( lexeme[ lexeme_index ] == ESCAPECHARACTER ) {
						lexeme_index ++ ;
						name[ lexeme_index ] = lexeme[ lexeme_index ];
					}
					if( lexeme[ lexeme_index ] )
						lexeme_index ++ ;
				}
				if( lexeme[lexeme_index] == '\"' ) {
					name[ lexeme_index ] =lexeme[ lexeme_index ] ;
					lexeme_index ++ ;
					name[ lexeme_index ] = '\0';
					*giveback = (char *)malloc( strlen( name )+1 );
					if( !*giveback )
						error("End of memory!",FATAL);
					strcpy( *giveback , name );
					return lexeme_index;
				}
				error("String not terminated.",NORMAL);
				return 0;
			}
			else
				if( set_to_search_in->settype == LABELSET )
				{
					/* If it can not match a label, or if the		     */
					/* label sets are swithed off. First we don't 	     */
					/* accept label sets, because eg.: ld sh,hl would be */
					/* accepted, and a dw hl line would be generated.    */
					/* This would cause an undeflabel error.		     */
					if( !isIDalpha( *lexeme ) || !k ) return 0;

					for( lexeme_index = 0;
					    isIDchar( lexeme[lexeme_index] ) ;
					    lexeme_index++)
						name[ lexeme_index ] = lexeme[ lexeme_index ];
					name[ lexeme_index ] = '\0';
					*giveback = strdup( name );
					if( !*giveback )
						error("End of memory!",FATAL);
					return lexeme_index;
				}
				else
					if( set_to_search_in->settype == DUMMY_SET )
					{/* Dummy set comes from an earlier error.*/
						/* Does not match anything. 		  */
						return 0;
					}
	error("004 internal error!",INTERNAL);/* Invalid set type. */
	return 0;
}/* End of serch_in_set. */

/*
**
** Returns the pointer to the macro which matches the line.
** 'setcc' points the lexemes of the sets.
** If no macro matches the line it returns NULL and
**  					so points 'setcc'.
**
** The last argument k drives the acceptance of undefined labels.
** If this is 0 the no undefined label can be accepted.
** If this is 1 then it is OK.
*/
struct macro *ismacro( s , setcc , k )
char *s;
char ***setcc;
int k;
{
	struct macro *ptr;

	if( !*s )/* Empty line can not match any macro. */
		return NULL;
	/* Calculate the hash value of the line. */
	/* 1 == it is not a sintax definition, but a line. */
	macqueindx = machash( s , 1 );
	ptr = macroot[macqueindx];
	while(ptr){
		/* Make room for an array. Every item is a pointer to a string. */
		/* The number of the items is the number of the sets. 		*/
		/* Every string contains the current value of the actual set.   */
		if( ptr->nrofsets > 0 ){
			*setcc = (char **)malloc( ptr->nrofsets*sizeof( char * ) );
			if( !*setcc )
				error("End of memory!",FATAL);
			}
		else	*setcc = NULL;
		/* If the sintax definition, and the set definitions are good for */
		/* this line then we found it being a macro.                      */
		if( checkline( s , ptr->sdef , ptr->setpointer , *setcc , k ) ) break;
		/* If we found it not being this macro then we try the next. */
		ptr = ptr -> nextmacro;
		/* The next macro probably has different number of sets so */
		/* we free this, and allocate new area in the next turn.   */
		free( *setcc );
		*setcc = NULL;
	}
	/* If the loop finished because there was no more macro, than */
	/* the last *setcc was freed, and points nothing.             */
	if( !ptr ) *setcc = NULL;

	return ptr;
}/* End of function ismacro() */

/*
**
** This function gets the macro definition, and
** first_unused_character points the left bracket opening the definition
** This function schedules the macro definition into
** the macro definition lists.
** Return 1 if reached the end of the file.
*/
int macrodef()
{
	int i;
	int setnr;
	struct symbol *ptr;
	struct exeline **exeptr;
	SPACEAT; /* Eat the spaces before the ( */
	if( *first_unused_character != '(' )/* If there is no ( */
		error("( missing in the macro definition.",NORMAL);
	else
		first_unused_character++;/* Step over the ( */
	SPACEAT;/* Eat the spaces between the ( and the ". */
	if( *first_unused_character !='\"' )/* The sintax string must follow */
		error("\" missing in the macro definition.",NORMAL);
	else
		first_unused_character++;/* Step over the double quote */
	i=	  /* Index for the char array 'name' */
	setnr = 0;/* The numbers of the set stars in the sintax string. */

	/* While not end of line and not end of the string. */
	while( *first_unused_character != '\"' && *first_unused_character ) {
		/* We remove those back-slashes which are not needed. */
		/* It is necessary for the correct work of machash(). */
		if( *first_unused_character == '\\' &&
		    first_unused_character[1] != '\\' &&
		    first_unused_character[1] != '\"' &&
		    first_unused_character[1] != '*'  &&
		    first_unused_character[1] != ' ' )
			first_unused_character ++;
		name[i++] = *first_unused_character;/* Copy the character. */
		if( *first_unused_character == '*' )/* If this is a set star */
			setnr++;/* Count it. 	     */
		/* If the character is backslash and there is
					next character on the line. */
		if( *first_unused_character++ == ESCAPECHARACTER &&
		    *first_unused_character )
			/* Copy it, and do not check wheather it is a *
        				or " or anything else. */
			name[i++] = *first_unused_character++ ;
	}
	if( !*first_unused_character )/* If we found the end of the line without " */
		error("\" missing after the macro definition.",NORMAL);
	else
		first_unused_character ++; /* Step over the " */
	/* Terminate the sintax string with zero character. */
	name[i++] = '\0';
	/* Now we have the sintax string in the character array 'name' */

	SPACEAT;/* Step over the spaces after the sintax definition string. */
	/* Calculate the hash value of the sintax definition. */
	/* 0 == it is a sintax definition. */
	macqueindx = machash( name , 0 );
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
	maclast[macqueindx]->sdef = strdup( compress( name ) );
	if( !(maclast[macqueindx]->sdef) ) error("End of memory!",FATAL);

	maclast[macqueindx]->num1 = 0;
	maclast[macqueindx]->num2 = 0;

	/* Make room for the set array. */
	if( setnr > 0 ){
		maclast[macqueindx]->setpointer =
		    (struct set **)malloc( setnr*sizeof(struct set*) );
		if( !(maclast[macqueindx]->setpointer) ) error("End of memory!",FATAL);

		/* Make room for the sets. */
		maclast[macqueindx]->setsymbols =
		    (struct symbol **)malloc( setnr*sizeof(struct symbol *) );
		if( !(maclast[macqueindx]->setsymbols) ) error("End of memory!",FATAL);
		}
	else
		maclast[macqueindx]->setpointer = NULL ;
		maclast[macqueindx]->setsymbols = NULL ;

	/* The number of the sets was counted in setnr. */
	maclast[macqueindx]->nrofsets = setnr;

	i = 0 ;
	for(;;) {
		SPACEAT;
		if( *first_unused_character == ')' ) break; /* Escape from 'for(;;)' */

		if( i >= setnr ){
			error("Too many const definitions.",NORMAL);
			while( *first_unused_character &&
			       *first_unused_character != ')' )
				 first_unused_character ++;
			}
		if( *first_unused_character != ',' )
			error(", missing between consts.",NORMAL);
		else /* Step over the comma. */
			first_unused_character++;
		getsymbol();/* getsymbol eats the spaces. */
		if( symbol != IDENTIFIER ) {
			error("Identifier missing as a const definition.",NORMAL);
			break;/* Escape from 'for(;;)' */
		}
		/* Look up the set name in the symbol table. */
		ptr = search_in_the_table( name );
		if( ptr->type_of_the_symbol == UNDEFINED ) {/* The set must be
									defined previously. */
			/* Create an error string. */
			sprintf( name , "Set %s was not defined." , ptr->name_of_the_symbol );
			error( name , NORMAL );
			ptr = search_in_the_table("DUMMY_SET");
		}
		else
			if( ptr->type_of_the_symbol != SET ) {/* A set must be set. (HI)*/
				/* Create an error message. */
				sprintf( name ,"Identifier %s is not set.",
				    ptr->name_of_the_symbol);
				error( name , NORMAL );
				ptr = search_in_the_table("DUMMY_SET");
			}
		/* Copy the pointer to the set definition
					into the macro set array */
		(maclast[macqueindx]->setpointer)[i  ] = ptr->stpointer;
		/* and copy the place of the symbol of set. */
		(maclast[macqueindx]->setsymbols)[i++] = ptr;
	}/* End of for(;;) */
	if( i < setnr ) {
		error("Too few const definition.",NORMAL);
		ptr = dummy_set; /* Dummy set is created in the init section. */
	}
	while( i< setnr ) {
		/* Copy the pointer to the set definition
				into the macro set array */
		(maclast[macqueindx]->setpointer)[i  ] = ptr->stpointer;
		/* and copy the place of the symbol of set. */
		(maclast[macqueindx]->setsymbols)[i++] = ptr;
	}
	first_unused_character++; /* Step over the ) charcter. */
	SPACEAT;
	if( *first_unused_character && *first_unused_character != ';') {
		/* Create an error message. */
		sprintf( name ,"Unexpected characters on the end of the line: (%s).",
		    first_unused_character);
		error( name , NORMAL);
	}
	/* Now we are about to work up the macro lines are terminated by an endm . */
	exeptr = &(maclast[macqueindx]->firstline);
	for(;;) {
		/* It is faster to call next_line() and
		correct since macros may not be nested. */
		if( next_line() == EOF ){
			error("Macro definition not terminated.",NORMAL);
			return 1;
		}
		first_unused_character = readinbuff;
		getsymbol();
		if( symbol == DEFMACRO || symbol == DEFSET ) {
			switch( symbol ) {
			case DEFMACRO:
				error("Macro definition nesting is not allowed.",NORMAL);
				break;
			case DEFSET  :
				error("Const definition can not be inside of a macro.",NORMAL);
				break;
			}
			while(1) {/* Eat the rest of the macro definition. */
				next_line();
				first_unused_character = readinbuff;
				getsymbol();
				if( symbol == ENDMACRO ) break;
			}
		}/* End of error recovery of macro nesting. */
		/* If we reached the last macro definition line. */
		if( symbol == ENDMACRO ) break;
		*exeptr = (struct exeline *)malloc( sizeof( struct exeline) );
		if( !*exeptr ) error("End of memory!",FATAL);
		cut_comment( readinbuff );/* We do not store the comments. */
		/* Check the macro arguments, since there must not be #i
		where i >= setnr. */
		check_sharps( readinbuff , setnr );
		/* Make room for the line. */
		(*exeptr)->exl =strdup( readinbuff ) ;
		if( !((*exeptr)->exl) ) error("End of memory!",FATAL);
		/* Step for the next executable line. */
		exeptr = &((*exeptr)->nextexeline);
	}/* End of for(;;) work up the exelines. */
	*exeptr = NULL;/* No next line following the last. */
	return 0;
}/* End of macrodef() */

main(argc , argv, env ) int argc;
char **argv,**env;
{
	int i;
	FILE *fp;
	int fterror;
	int slist;
	int libraryswitch;
	int stop_before_start;

/** INIT __tolwer__ */
	initolo();

	/*#ifndef __MSDOS__
malloc_debug(2);
#endif*/
	/*

* set the default values

*/
	outfilnam = NULL;
	original_name = "pcmac.asm";
	casesen = 1;
	glistswitch = 0;
	headswitch = 0;
	ilistswitch = 0;
	objgen = 0;
	libraryswitch = 0;
	maclist = 0;
	tskswitch = 0;
	listfile = stdout;
	stdoutsw = 1;
	slist = 0;
	include_directory = "";
	stop_before_start = 0;/* Do not stop. */

	if( argc == 1 || (argc == 2 && (argv[1][0] == '?' || !stricmp(argv[1],"help") ))) {
		fprintf(stderr,"Macro assembler (C) Versoft Ltd. Hungary.\n");
		fprintf(stderr,"Usage: pcmac ['-' options ] filename\n");
		fprintf(stderr," Options:\n");
		fprintf(stderr,"l list on\n");
		fprintf(stderr,"ln&name listfilename+list on\n");
		fprintf(stderr,"m list macro extention(+list).\n");
		fprintf(stderr,"mn&name listfilename+macro listing+listing.\n");
		fprintf(stderr,"s symbol table\n");
		fprintf(stderr,"c case sensitivity\n");
		fprintf(stderr,"n&outfilename\n");
		fprintf(stderr,"K to make library.\n");
		fprintf(stderr,"o generate object.\n");
		fprintf(stderr,"t don't care publics and externals.\n");
		fprintf(stderr,"h generate header file.\n");
		fprintf(stderr,"v version number.\n");
		fprintf(stderr,"\n& means: write the name without any space!\n");
		exit(1);
	}
	/* We search the default enviroment. */
	while( *env ){
		first_unused_character = *env ++;
		getsymbol();
		if( symbol != IDENTIFIER )continue;
		if( !strcmp( name , "INCLUDE" ) ){
			while( isspace(*first_unused_character ))first_unused_character ++;
			if( *first_unused_character != '=' )continue;
			first_unused_character++;
			i = 0;
			while( *first_unused_character )name[i++] = *first_unused_character++;
			if( !i )continue;
#ifdef __MSDOS__
			if( name[i-1] != '\\' )
				name[i++] = '\\';
#else
			if( name[i-1] != '/' )
				name[i++] = '/';
#endif
			name[i] = '\0';
			include_directory = strdup( name );
			if( !include_directory )error("End of memory!",FATAL);
		}
	}

	while( --argc ) {
		argv++;
		if( **argv == '-' )/* Option. */
			switch( (*argv)[1] ) {
			case 'K':
				libraryswitch = 1;
				break;
			case 'o':
				objgen = 1;
				break;
			case 'h':
				headswitch = 1;
				break;
			case 'p':
				stop_before_start = 1;
				break;
			case 'm':
				maclist = 1;
				/* NO break; */
			case 'l':
				switch( (*argv)[2] ) {
				case '\0':
					glistswitch = 1;
					break;
				case 'n':
					listfile =
					    fopen( &((*argv)[3]) , "w" );
					if( listfile )
						stdoutsw = 0;
					glistswitch = 1;
					break;
				default:
					error("Invalid list sub option.",NORMAL);
					break;
				}
				break;
			case 's':
				switch( (*argv)[2] ) {
				case '\0':
					slist = 1;
					break;
				default:
					error("Invalid symbol table listing sub option.",NORMAL);
					break;
				}
				break;
			case 'c':
				switch( (*argv)[2] ) {
				case '\0':
					casesen = 0;
					break;
				default:
					error("Invalid case sub option.",NORMAL);
					break;
				}
				break;
			case 'n':
				outfilnam = &((*argv)[2]);
				break;
			case 't':
				tskswitch = 1;
				break;
			case 'v': write_version();
			default:
				error("Invalid option!",NORMAL);
				break;
			}
		else {
			register post;
			original_name = *argv;
			strcpy( name , *argv );
			for( i = 0 ; i<MAXLINLEN && name[i] && name[i] !='.' ; i++ )
				;
			if( name[i] == '.' && name[i+1] == 'o' && !name[i+2] ){
#ifdef __MSDOS__
				post = 'q';
#else
				post = 'O';
#endif
				error("Input file name : *.o",WARNING);
			}
			else
				post = 'o';
			if( ( !name[i] || name[i] == '.') && i<MAXLINLEN-10 ){
				name[i++] = '.';
				name[i++] = post;
				name[i] = '\0';
			}
			else
				error("Too long file name.\n",FATAL);
			if( !outfilnam ) outfilnam = (char *)malloc( i + 3 );
			if( !outfilnam )error("End of memory!",FATAL);
			strcpy( outfilnam , name );
		}
	}

	if( !outfilnam ) outfilnam = "pcmac.o\0  ";
	if( !objgen ){
		for( i = 0 ; outfilnam[i] != '.' ; i++ )
			;
		strcpy( &(outfilnam[i]) , ".tsk");
	}
	if( libraryswitch ){
		for( i = 0 ; outfilnam[i] != '.' ; i++ )
			;
		strcpy( &(outfilnam[i]) , ".lib");
	}
	else
		if( headswitch ){
			for( i = 0 ; outfilnam[i] != '.' ; i++ )
				;
			strcpy( &(outfilnam[i]) , ".h");
		}

	if( objgen && tskswitch ){
		tskswitch = 0;
		error("Inconsistent options -o -t.",WARNING);
	}
	if( objgen && headswitch ){
		headswitch = 0;
		error("Inconsistent options -o -h.",WARNING);
	}
	if( objgen && libraryswitch )
		error("Inconsistent options -o -K.",FATAL);
	if( stop_before_start ){
		fprintf(stderr,"-p option was given.\n");
#ifdef __MSDOS__
		fprintf(stderr,"Change diskettes you wish and ");
#endif
		fprintf(stderr,"press any key before continue.\n");
#ifdef __MSDOS__
		while( kbhit() )getch();
		getch();
#else
		getchar();
#endif
		}
	init_assembly_system();
	if( !listfile ){
		listfile = stdout;
		error("Unable to open the list file.",NORMAL);
	}
	pass = 1;
	printf("Versoft macro assembler.\nV 1.0\n");
	printf("Pass1\n");
	if( !stdoutsw ){
		fprintf(listfile,"Versoft macro assembler.\nV 1.0\n");
		fprintf(listfile,"Pass1\n");
	}
	nrerrors =
	    nrwarnings = 0;
	*dollar = 0;
	macextnum = 0;
	for( i = 0 ; i<256 ; i++ )char_code[i]=i;
	dopas();
	fterror = nrerrors;
	flusherr();
	printf("Error%c   : %u\n", nrerrors   >1 ? 's' : ' ' , nrerrors   );
	printf("Warning%c : %u\n", nrwarnings >1 ? 's' : ' ' , nrwarnings );
	if( !first_flag ){
		printf("Start of the code : %lu\n",dollmin);
		printf("End   of the code : %lu\n",dollmax);
	}
	else
		printf("No code.\n");
	if( !stdoutsw ){
		fprintf(listfile,"Error%c   : %u\n", nrerrors   >1 ? 's' : ' ' , nrerrors   );
		fprintf(listfile,"Warning%c : %u\n", nrwarnings >1 ? 's' : ' ' , nrwarnings );
		if( !first_flag ){
			fprintf(listfile,"Start of the code : %lu\n",dollmin);
			fprintf(listfile,"End   of the code : %lu\n",dollmax);
		}
		else
			fprintf(listfile,"No code.\n");
	}

	if( !first_flag && libraryswitch )
		error("Not only macros for macrolibrary.",FATAL);
	if( libraryswitch && !nrerrors ) {
		write_library();
		exit(0);
	}
	if( headswitch && !nrerrors ){
		write_header();
		exit(0);
	}
	if( headswitch || libraryswitch )exit(1);
	printf("Pass2\n");
	if( !stdoutsw )
		fprintf(listfile,"Pass2\n");
	nrerrors =
	    nrwarnings = 0;
	*dollar = 0;
	rebuffpos = 0;
	codepos = 0;
	*sympass = 2;
	pass = 2 ;
	first_flag = 1;
	*listline = '\0';
	fly_over_the_lines = 0;
	ondeep = 0;
	if( objgen ){
#ifdef __MSDOS__
		linkfp = fopen( outfilnam , "wb" );
#else
		linkfp = fopen( outfilnam , "w");
#endif
		if( !linkfp )
			error("Unable to open the object file.",FATAL);

		putc( 0x55 , linkfp );
		putc( 0xaa , linkfp );
		gextdef();
		code = (char *)malloc( 256*sizeof( char ) );
	}
	else{
		code = (char *)malloc(dollmax - dollmin+1);
	}
	if( !code )
		error("Not enough memory for the code!",FATAL);
	macextnum = 0;
	for( i = 0 ; i<256 ; i++ )char_code[i]=i;
	dopas();
	printf("Error%c   : %u\n", nrerrors   >1 ? 's' : ' ' , nrerrors   );
	printf("Warning%c : %u\n", nrwarnings >1 ? 's' : ' ' , nrwarnings );
	if( !nrerrors && fterror )
		printf("Error%c in the first pass!!\n",fterror>1 ? 's' :' ');
	if( !stdoutsw ) {
		fprintf(listfile,"Error%c   : %u\n",
		    nrerrors   >1 ? 's' : ' ' , nrerrors   );
		fprintf(listfile,"Warning%c : %u\n",
		    nrwarnings >1 ? 's' : ' ' , nrwarnings );
		if( !nrerrors && fterror )
			fprintf(listfile,"Error%c in the first pass!!\n",fterror>1 ? 's' :' ');
	}
	if( !nrerrors && !fterror && !objgen ) {
#ifdef __MSDOS__
		fp = fopen( outfilnam , "wb");
#else
		fp = fopen( outfilnam , "w" );
#endif

		if( !fp ) error("Unable to open the output file!",FATAL);

		for( i=0 ; i<=dollmax-dollmin ; i++ )
			fprintf(fp,"%c",code[i]);

		fclose( fp );
	}
	if( objgen ){
		flush_rebuff();
		flush_code();
	}
	if( slist )
		listsymbols();
	if( listfile && listfile != stdout )
		fclose( listfile );
}

/* This function writes the licence. */
void write_version(){

	fprintf(stderr,"Versoft macro assembler.\n");
	fprintf(stderr,"Version 1.0\n");
	exit(0);

	}
