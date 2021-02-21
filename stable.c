#include "makeflag.h"

#if MAKE
#include "include.h"
#include "defines.h"
#include "types.h"
#include "functype.h"
#include "vars.h"
#include "tfunc.h"
#endif
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
int hashpjw(s)
char   *s;
{
	char *p;
	unsigned long h = 0, g;
	for ( p = s; *p != '\0'; p = p+1 ) {
		h = (h << 4) + (*p);
		if (g = h&MASK) {
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
**		brek;
**		case 0: Do not convert but stop when
**			End of string or
**			space         or
**			back-slash space pair.
**		brek;
**		default: Internal compiler error.
**		brek;
**		}
** So the sintax definition will get the same hash value as the line
** which match it. (This hash value is always case insensitive.)
*/
int machash( s , k )
char *s; 
int k;
{
	char *p;
	unsigned long h=0, g;
	int bslashwas; /* When calculating the hash value of a sintax definition. */
	switch( k ) {
	case 1:/* The argument 's' is a line and not sintax definition. */
		for( p = s ; *p != '\0' && !isspace(*p) ; p++ ) {
			/* Push a back slash before every *, \ and " */
			if( *p == '*' || *p == '\\' || *p == '\"' ) {
				h = (h<<4) +'\\';
				if( g = h&MASK ) {
					h ^= g<<24 ;
					h ^= g;
				}
			}/* End if( *p == * or \ or " */
			h = (h<<4) + tolower(*p);
			if( g = h&MASK ) {
				h ^= g<<24 ;
				h ^= g;
			}
		}/* End for */
		break;
	case 0:/* The argument 's' is a sintax definition. */
		bslashwas = 0;
		for( p = s ; *p != '\0' && !isspace(*p) &&
		    !(*p == '\\' && isspace(p[1])) &&
		    !(*p == '*' && !bslashwas)  ; p +=1 ) {
			bslashwas = ( *p == '\\' );
			h = (h<<4) + tolower(*p);
			if( g = h&MASK ) {
				h ^= g<<24 ;
				h ^= g;
			}
		}/* End for */
		break;
	default:
		error("011 internal error!",INTERNAL);
		break;
	}/* End of switch( k ) */
	return h % PRIME ;
}/*End of function machash() */



/*
** Search in the table.
**
** returns a pointer pointing to the searched item.
*/
struct symbol *search_in_the_table( s )
#define __FUNC__ "search_in_the_table"
char *s;
{
	struct symbol **work_pointer;
	int k;
	work_pointer = &(hashtable[ hashpjw( s ) ]);
	while( *work_pointer && (k=strcmp(s,(*work_pointer)->name_of_the_symbol)) ) {
		work_pointer= k > 0 ? &((*work_pointer)->big_son)  :
		    &((*work_pointer)->small_son);

	}
	if( *work_pointer ) return *work_pointer;
	*work_pointer = (struct symbol *) malloc( sizeof( struct symbol ));
	if( !*work_pointer ) {
		fprintf(stderr,"Not enough memory!\n");
		exit(1);
	}
	(*work_pointer)->name_of_the_symbol = strdup( s );
	if( !(*work_pointer)->name_of_the_symbol ) {
		fprintf(stderr,"Not enough memory!\n");
		exit(1);
	}
	(*work_pointer)->type_of_the_symbol = UNDEFINED;
	(*work_pointer)->public = 0;
	(*work_pointer)->value_of_the_symbol = 0;
	(*work_pointer)->relocatable = 0;
	(*work_pointer)->big_son =
	    (*work_pointer)->small_son = NULL;
	return *work_pointer;
}
#undef __FUNC__

void listsymbols()
{
	int i;
	fprintf(listfile,"SYMBOL TABLE:\n");
	fprintf(listfile,"-------------\n");
	for( i = 0 ; i < PRIME ; i++ ) /* Go thru the hash table. */
		list_tree( hashtable[i]);
}
void list_tree( j )
struct symbol *j;
{
	if( j == NULL ) return;
	list_tree( j->small_son );
	list_tree( j->big_son );
	switch( j->type_of_the_symbol ) {
	case LABEL:   
		fprintf(listfile,"LABEL    ");
		break;
	case VARIABLE:
		fprintf(listfile,"VARIABLE ");
		break;
	}
	switch( j->type_of_the_symbol )
	case LABEL:
	case VARIABLE:
		{	
			fprintf(listfile,"%20s %cR ",j->name_of_the_symbol,
			    j->relocatable ? ' ' : 'N');
			fprintf(listfile,"%08X\n",j->value_of_the_symbol); 
		}
}/* End of the function list_tree() */

static FILE *fp;
void write_header()
{
	int i;

	fp = fopen( outfilnam , "w" );
	for( i = 0 ; i < PRIME ; i++ ) /* Go thru the hash table. */
		write_tree( hashtable[i]);

	fclose( fp );
}
void write_tree( j )
struct symbol *j;
{
	if( j == NULL ) return;
	write_tree( j->small_son );
	write_tree( j->big_son );
	switch( j->type_of_the_symbol )
	case LABEL:
		{ 
			fprintf(fp,"%-18s\tEQU\t",j->name_of_the_symbol);
			if( j->relocatable )
				fprintf(fp," doreloc(");
			else
				fprintf(fp,"doureloc(");
			fprintf(fp,"0%08lXh )\n",j->value_of_the_symbol);
		}
}/* End of the function write_tree() */
