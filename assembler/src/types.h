/* Type definitions needed by the macro assembler. */

#ifndef TYPES_H_
#define TYPES_H_

#include "defines.h"

/*
 *
 * Contains a member of a list of the executable lines of a macro.
 *
 */
struct exeline
{
	char *exl;
	struct exeline *nextexeline;
};
/*
 ** information of a set
 */
struct set
{
	int settype;
	char *members;
};
/*
 **
 ** Contains the information of a macro
 **
 */
struct macro
{
	int nrofsets;
	struct set **setpointer;
	struct symbol **setsymbols;
	char *sdef;
	struct exeline *firstline;
	struct macro *nextmacro;
	int num1, num2;
};

/*
 ** The definition of the type of an item in the symbol table.
 */
struct symbol
{
	int type_of_the_symbol;
	unsigned long value_of_the_symbol;
	char *name_of_the_symbol;
	struct set *stpointer;
	int relocatable;
	int public;
	struct symbol *small_son, *big_son;
};
/*
 ** For macro nesting.
 */
struct macrostack
{
	struct exeline *mlin;
	struct macrostack *prevmac;
	char **currset;
	struct macro *mcro;
};

/*
 ** For handling macro loops.
 */
struct loopstack
{
	int m_n_d;
	struct exeline *flin;
	int ltype;
	struct loopstack *prev;
};

/* A structure for storing error messages while
 ** the line is listed.
 */
struct errstack
{
	struct errstack *nexterr;
	char *errmes;
};

/*
 ** A structure for storing an item for the user.
 */
struct usrs
{
	valtype value;
	struct usrs *nxt;
};

struct reloc
{
	char size;
	valtype address;
};

/* A macro for comparing characters. x is equal to y if x==y or
 both are some kind of spaces. If casesen is on then 'A' == 'a'. */
#define charcmp(x,y) ((x==y||(casesen&&tolower(x)==tolower(y))||(isspace(x)&&isspace(y))))


#endif /* TYPES_H_ */
