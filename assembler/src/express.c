/*
 * express.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>

#include "express.h"

#include "defines.h"
#include "types.h"
#include "vars.h"

#include "error.h"
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
#include "utilits.h"

/*
 * EXPRESSION EVALUATING!
 * expression        ::= orexpression      { '||' orexpression  }.
 * orexpression      ::= andexpression     { '&&' andexpression }.
 * andexpression     ::= borexpression     { '|' borexpression  }.
 * borexpression     ::= xorexpression     { '^' xorexpression  }.
 * xorexpression     ::= bandexpression    { '&' bandexpression }.
 * bandexpression    ::= neoreexpression   { ( '<>' | '=' )              neoreexpression   }.
 * neoreexpression   ::= compareexpression { ( '<' | '>' | '<=' | '>=' ) compareexpression }.
 * compareexpression ::= shiftexpression   {( '<<' | '>>' )              shiftexpression   }.
 * shiftexpression   ::= addexpression     { ( '+' | '-' )               addexpression     }.
 * addexpression     ::= multexpression    { ( '*' | '/' | '%' )         multexpression    }.
 * multexpression    ::= { ( '!' | '~' | '-' | '+' ) } tag.
 * tag               ::= number | label | '(' expression ')' |
 *			 func '(' expression ')'.
 *
 * number ::= label | decimal number with or without d postfix |
 *	     	binary number with b postfix |
 *     		hexadecimal number with h postfix |
 * 	    	octal number with o postfix	|
 *     		ascii number, max 8 character long,
 *     		may contain escape sequences,
 *	     	closed between ' like 'AS'.
 *
 * The operations are the same as they are in C.
 * They have the same precedence.
 *
 * Verhas Peter   1988. marcius 3.
 *
 */


static void _expression(valtype *val);
static valtype orexpression(void);
static valtype andexpression(void);
static valtype borexpression(void);
static valtype xorexpression(void);
static valtype bandexpression(void);
static valtype neoreexpression(void);
static valtype compareexpression(void);
static valtype shiftexpression(void);
static valtype addexpression(void);
static valtype multexpression(void);
static valtype tag(void);

static valtype hex(int c);
static valtype bin(int c);
static valtype okt(int c);
static valtype dec(int c);


/*
 * EXPRESSION EVALUATING.
 * It returns the length of the string containing the expression.
 */
int expression( s , val , err , reloc )
	char *s; /* The start of the expression. */
	valtype *val; /* The value of the expression. */
	int *err; /* The number of the error occured while evaluating the expression. */
	int *reloc; /* True if the expression is relocatable and false otherwise. */
{
	//char *s; The start of the expression.
	//valtype *val; The value of the expression.
	//int *err; The number of the error occured while evaluating the expression.
	//int *reloc; True if the expression is relocatable and false otherwise.

	errortype = 0;// No error yet.
	first_unused_character = s;
	SPACEAT;
	if(*first_unused_character == '[')
	{
		*err = SERROR;
		*reloc = 0;
		*val = 0;
		return (first_unused_character - s);
	}
	getsymbol();
	_expression(val);
	*err = errortype;
	// The default value of an errorful expression is zero.(FALSE)
	if(errortype)
		*val = 0;
	*reloc = reloctype;
	return (first_character_of_the_lexeme - s);
}

static void _expression(valtype *val)
{
	valtype value;
	value = orexpression();
	while(symbol == ORSYMBOL)
	{
		getsymbol();
		//func before '||' or it won't be called
		value = orexpression() || value;
		reloctype = FALSE;
	}
	*val = value;
}/* End of _expression() */

//or
static valtype orexpression(void)
{
	valtype value;
	value = andexpression();
	while(symbol == ANDSYMBOL)
	{
		getsymbol();
		//func before '&&' or it won't be called
		value = andexpression() && value;
		reloctype = FALSE;
	}
	return value;
}/* End of orexpression() */

//and
static valtype andexpression(void)
{
	valtype value;
	value = borexpression();
	while(symbol == BORSYMBOL)
	{
		getsymbol();
		value |= borexpression();
		reloctype = FALSE;
	}
	return value;
}/* End of andexpression() */

//bor
static valtype borexpression(void)
{
	valtype value;
	value = xorexpression();
	while(symbol == BXORSYMBOL)
	{
		getsymbol();
		value ^= xorexpression();
		reloctype = FALSE;
	}
	return value;
}/* End of borexpression. */

//xor
static valtype xorexpression(void)
{
	valtype value;
	value = bandexpression();
	while(symbol == BANDSYMBOL)
	{
		getsymbol();
		value &= bandexpression();
		reloctype = FALSE;
	}
	return value;
}/* End of xorexpression() */

//band
static valtype bandexpression(void)
{
	valtype value;
	value = neoreexpression();
	while(symbol == EQSYMBOL || symbol == NESYMBOL)
	{
		switch(symbol)
		{
			case EQSYMBOL:
				getsymbol();
				value = (value == neoreexpression());
				break;
			case NESYMBOL:
				getsymbol();
				value = (value != neoreexpression());
				break;
		}
		reloctype = FALSE;
	}
	return value;
}/* End of bandexpression() */

//neore
static valtype neoreexpression(void)
{
	valtype value;
	value = compareexpression();
	while(symbol == GTSYMBOL || symbol == GESYMBOL || symbol == LTSYMBOL || symbol == LESYMBOL)
	{
		switch(symbol)
		{
			case GTSYMBOL:
				getsymbol();
				value = (value > compareexpression());
				break;
			case GESYMBOL:
				getsymbol();
				value = (value >= compareexpression());
				break;
			case LTSYMBOL:
				getsymbol();
				value = (value < compareexpression());
				break;
			case LESYMBOL:
				getsymbol();
				value = (value <= compareexpression());
				break;
		}
		reloctype = FALSE;
	}
	return value;
}/* End of neoreexpression() */

//compare
static valtype compareexpression(void)
{
	valtype value;
	value = shiftexpression();
	while(symbol == SHRSYMBOL || symbol == SHLSYMBOL)
	{
		switch(symbol)
		{
			case SHRSYMBOL:
				getsymbol();
				value >>= shiftexpression();
				break;
			case SHLSYMBOL:
				getsymbol();
				value <<= shiftexpression();
				break;
		}
		reloctype = FALSE;
	}
	return value;
}/* End of compareexpression() */

//shift
static valtype shiftexpression(void)
{
	valtype value;
	int savereloc;
	value = addexpression();
	savereloc = !!reloctype;
	while(symbol == ADDSYMBOL || symbol == MINUSSYMBOL)
	{
		switch(symbol)
		{
			case ADDSYMBOL:
				getsymbol();
				value += addexpression();
				if(reloctype)
					savereloc++;
				break;
			case MINUSSYMBOL:
				getsymbol();
				value -= addexpression();
				if(reloctype)
					savereloc--;
				break;
		}
	}
	reloctype = (savereloc == 1);
	return value;
}/* End of compareexpression() */

//add
static valtype addexpression(void)
{
	valtype value;
	value = multexpression();
	while(symbol == MULTSYMBOL || symbol == MODSYMBOL || symbol == DIVSYMBOL)
	{
		switch(symbol)
		{
			case MULTSYMBOL:
				getsymbol();
				value *= multexpression();
				break;
			case MODSYMBOL:
				getsymbol();
				value %= multexpression();
				break;
			case DIVSYMBOL:
				getsymbol();
				value /= multexpression();
				break;
		}
		reloctype = FALSE;
	}
	return value;
}/* End of addexpression() */

//multiply
static valtype multexpression(void)
{
	valtype value;
	switch(symbol)
	{
		case NOTSYMBOL:
			getsymbol();
			value = !multexpression();
			reloctype = FALSE;
			break;
		case NEGSYMBOL:
			getsymbol();
			value = ~multexpression();
			reloctype = FALSE;
			break;
		case MINUSSYMBOL:
			getsymbol();
			value = -multexpression();
			reloctype = FALSE;
			break;
		case ADDSYMBOL:
			getsymbol();
			value = multexpression();
			break;
		default:
			value = tag();
			break;
	}
	return value;
}/* End of multexpression() */

//tag
static valtype tag(void)
{
	struct symbol *ptr;
	valtype value;
	switch(symbol)
	{
		case LBRA: // '['
			getsymbol();
			_expression(&value);
			if(symbol != RBRA)
				errortype = RPARENTMISSING;
			else
				getsymbol();
			break;
		case LPARENT: // '('
			getsymbol();
			_expression(&value);
			if(symbol != RPARENT)
				errortype = RPARENTMISSING;
			else
				getsymbol();
			break;
		case NUMBER:
			value = number;
			getsymbol();
			reloctype = FALSE;
			break;
		case IDENTIFIER:
			ptr = search_in_the_table(name);
			if(ptr->type_of_the_symbol == UNDEFINED && (pass != 1))
				errortype = UNDEFLABEL;
			if(ptr->type_of_the_symbol == EXTERNAL && (pass != 1))
				errortype = EXTIDTF;
			value = ptr->value_of_the_symbol;
			reloctype = ptr->relocatable;
			getsymbol();
			break;
		case DORELOCFUN:
			getsymbol();
			if(symbol != LPARENT)
				errortype = SERROR;
			else
				getsymbol();
			_expression(&value);
			if(symbol != RPARENT)
				errortype = SERROR;
			else
				getsymbol();
			reloctype = TRUE;
			break;
		case POP:
			getsymbol();
			if(symbol != LPARENT)
				errortype = SERROR;
			else
				getsymbol();
			_expression(&value);
			if(symbol != RPARENT)
				errortype = SERROR;
			else
				getsymbol();
			value = usrpop((int)value);
			break;
		case TOS:
			getsymbol();
			if(symbol != LPARENT)
				errortype = SERROR;
			else
				getsymbol();
			_expression(&value);
			if(symbol != RPARENT)
				errortype = SERROR;
			else
				getsymbol();
			value = usrtos((int)value);
			break;
		case DOURELOCFUN:
			getsymbol();
			if(symbol != LPARENT)
				errortype = SERROR;
			else
				getsymbol();
			_expression(&value);
			if(symbol != RPARENT)
				errortype = SERROR;
			else
				getsymbol();
			reloctype = FALSE;
			break;
		case ISRELOCFUN:
			getsymbol();
			if(symbol != LPARENT)
				errortype = SERROR;
			else
				getsymbol();
			_expression(&value);
			if(symbol != RPARENT)
				errortype = SERROR;
			else
				getsymbol();
			value = !!reloctype;
			reloctype = FALSE;
			break;
		case DEFFUN:
			getsymbol();
			if(symbol != LPARENT)
				errortype = SERROR;
			else
				getsymbol();
			if(symbol != IDENTIFIER)
			{
				errortype = SERROR;
				if(symbol != RPARENT)
					getsymbol();
			}
			else
			{
				ptr = search_in_the_table(name);
				value = (ptr->type_of_the_symbol != UNDEFINED);
				getsymbol();
			}
			if(symbol != RPARENT)
				errortype = SERROR;
			reloctype = FALSE;
			break;
		case STRLENFUN:
			getsymbol();
			if(symbol != LPARENT)
				errortype = SERROR;
			SPACEAT;
			if(*first_unused_character++ != '\"')
			{
				errortype = SERROR;
				reloctype = FALSE; /* It is not interesting. */
				break;
			}
			value = 0;
			while(*first_unused_character && *first_unused_character != '\"')
			{
				if(*first_unused_character == '\\')
				{
					first_unused_character++;
					if(isdigit(*first_unused_character))
					{
						/* The first character a digit. */
						first_unused_character++;
						if(*first_unused_character >= '0' && *first_unused_character < asciiradix + '0')
						{
							/* The second character is also a good number. */
							first_unused_character++;
							if(*first_unused_character < '0' || *first_unused_character > asciiradix + '0' - 1)
								/* The third charcter is not good. */
								first_unused_character--;/*step back */
						}
						else
							first_unused_character--;
					}
				}
				if(*first_unused_character)
					first_unused_character++;
				value++;
			}
			if(*first_unused_character != '\"')
				errortype = SERROR;
			else
				first_unused_character++;
			getsymbol();
			if(symbol != RPARENT)
				errortype = SERROR;
			else
				getsymbol();
			reloctype = FALSE;
			break;
		case STRIDXFUN:
			getsymbol();
			if (symbol != LPARENT)
				errortype = SERROR;
			getsymbol();
			valtype indexToGet;
			_expression(&indexToGet);

			SPACEAT;
			if ((symbol != COMMA) || (*first_unused_character++ != '\"'))
			{
				errortype = SERROR;
				reloctype = FALSE; /* It is not interesting. */
				break;
			}
			
			// Now go trough the string to take an indexed char of
			value = 0;
			int currentIdx = 0;
			int charAtIdx = 0;
			int octalWarn;//0=no, (1<<1)=warn term. by digit, (1<<2)=non octal char
			while(*first_unused_character && *(first_unused_character) != '\"')
			{
				octalWarn = 0;
				if(*first_unused_character == '\\')
				{
					first_unused_character++;
					if(!*first_unused_character)
						break;
					switch(*first_unused_character)
					{
						case 'n':
							charAtIdx = '\n';
							first_unused_character++;
							break;
						case 't':
							charAtIdx = '\t';
							first_unused_character++;
							break;
						case 'r':
							charAtIdx = '\r';
							first_unused_character++;
							break;
						case '\"':
							charAtIdx = '\"';
							first_unused_character++;
							break;
						case '8':
						case '9':
							if(asciiradix == 8)
								octalWarn |= (1<<1);
							// no break
						case '0':
						case '1':
						case '2':
						case '3':
						case '4':
						case '5':
						case '6':
						case '7':
						{
							int i = *(first_unused_character++) - '0';
							if (!*first_unused_character)
							{
								charAtIdx = i;
								break;
							}
							if (*first_unused_character < '0' || *first_unused_character > '0' + asciiradix - 1)
							{
								charAtIdx = i;
								if (asciiradix == 8 && *first_unused_character > '7' &&
										*first_unused_character <= '9')
									octalWarn |= (1<<2);
								break;
							}
							i = asciiradix * i + *(first_unused_character++) - '0';
							if (!*first_unused_character)
							{
								charAtIdx = i;
								if (asciiradix == 8 && *first_unused_character > '7' &&
										*first_unused_character <= '9')
									octalWarn |= (1<<2);
								break;
							}
							if (*first_unused_character < '0' || *first_unused_character > '0' + asciiradix - 1)
							{
								charAtIdx = i;
								if (asciiradix == 8 && *first_unused_character > '7' &&
										*first_unused_character <= '9')
									octalWarn |= (1<<2);
								break;
							}
							i = asciiradix * i + *(first_unused_character++) - '0';
							charAtIdx = i;
							if (asciiradix == 8 && *first_unused_character > '7' && *first_unused_character <= '9')
								octalWarn |= (1<<2);

						}
						break;

						default:
							charAtIdx = (*first_unused_character++);
							break;
					}/* End of switch */
				}
				else
				{
					charAtIdx = ((int)char_code[(int)*first_unused_character]);
					first_unused_character++;
				}
				if (indexToGet == currentIdx)
				{
					// We got the index we need
					value = charAtIdx;

					// Warn if nessesary
					if ((octalWarn & (1<<1)) != 0) error("Non octal character in string number.", WARNING);
					if ((octalWarn & (1<<2)) != 0) error("Oktal number terminated by digit.", WARNING);

					// Run to the end of the string
					while(*first_unused_character && *(first_unused_character) != '\"')
					{
						first_unused_character++;
					}/* End of the while */
					break;
				}
				currentIdx++;
			}/* End of the while */
			if(*first_unused_character != '\"')
				errortype = SERROR;
			else
				first_unused_character++;
			getsymbol();
			if(symbol != RPARENT)
				errortype = SERROR;
			else
				getsymbol();
			reloctype = FALSE;
			break;
		default:
			errortype = SERROR;
			break;
	}
	return value;
}/* End of tag() */

/* Returns the value of a hexadecimal digit. */
static valtype hex(int c)
{
	int i;
	i = 0;
	while(c != "0123456789ABCDEF"[i] && i < 16)
		i++;
	if(i != 16)
		return i;
	i = 10;
	while(c != "abcdef"[i - 10] && i < 16)
		i++;
	if(i != 16)
		return i;
	error("Wrong hexadecimal number!", NORMAL);
	return 0;
}

/* Returns the value of a binary digit. */
static valtype bin(int c)
{
	if(c == '0')
		return 0;
	if(c == '1')
		return 1;
	error("Wrong binary number!", NORMAL);
	return 0;
}

/* Returns the value of an oktal digit. */
static valtype okt(int c)
{
	int i;
	i = 0;
	while(c != "01234567"[i] && i < 8)
		i++;
	if(i != 8)
		return i;
	error("Wrong oktal number!", NORMAL);
	return 0;
}

/* Returns the value of a decimal digit. */
static valtype dec(int c)
{
	if(c > '9' || c < '0')
	{
		error("Wrong decimal number!", NORMAL);
		return 0;
	}
	return c - '0';
}




/*
 ** return TRUE if c is a valid character for an identifier
 */
int isIDchar(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || (c == '_') || (c == '$')
			|| (c == '@');
}

/*
 ** return TRUE if c is valid for the first character of an identifier
 */
int isIDalpha(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_') || (c == '$') || (c == '@');
}

/*
 ** LEXICAL ANALIZER
 */
void getsymbol(void)
{
	int i, j;
	char postfix;

	// WARNING ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! !
	// While constructing this table you must keep in mind that
	// a lexeme A prefix of lexeme B must be preceded here by B.
	// Because of the linear search, and there is not stop symbol
	// in the text.
	static struct
	{
		char *lexeme;
		int token;
	} lexemes[] =
	{
		{",", COMMA}, {"#", SHARP}, {":=", ASSIGN},
		{"||", ORSYMBOL}, {"&&", ANDSYMBOL}, {">>", SHRSYMBOL},
		{"<<", SHLSYMBOL}, {"%", MODSYMBOL}, {"<>", NESYMBOL},
		{"!=", NESYMBOL}, {">=", GESYMBOL}, {">", GTSYMBOL},
		{"<=", LESYMBOL}, {"<", LTSYMBOL},
		{"==", EQSYMBOL}, {"=", EQSYMBOL},
		{"(", LPARENT}, {")", RPARENT},
		{"[", LBRA}, {"]", RBRA},
		{"!", NOTSYMBOL}, {"~", NEGSYMBOL},
		{"&", BANDSYMBOL}, {"^", BXORSYMBOL}, {"|", BORSYMBOL},
		{"*", MULTSYMBOL}, {"/", DIVSYMBOL},
		{"+", ADDSYMBOL}, {"-", MINUSSYMBOL},
		{NULL, 0} // Terminating the table
	},

	// These tables contain the reserver words.
	// None of the strings contains the first letter.
	// If a new keyword starting with the letter XYZ then
	// put into the block XYZ_reswords.
	// If XYZ_reswords does not exist then create and define XYZ_START.
	// If this is not defined then the big switch will not be
	// compiled correct.
	A_reswords[] = {{"ND", ANDSYMBOL},	{NULL, 0}},
	C_reswords[] = {{"ONST", DEFSET},	{NULL, 0 }},

	D_reswords[] = {{ "B", DEFBYTE},	{"D", DEFDWORD}, {"ORELOC", DORELOCFUN}, {"OURELOC", DOURELOCFUN},
					{"RB", DEFRBYTE},	{"RD", DEFRDWORD},	{"RW", DEFRWORD}, {"W", DEFWORD}, {NULL, 0}},

	E_reswords[] = {{"NDM", ENDMACRO},	{"XTRN", EXTERNAL},	{"XTERN", EXTERNAL},	{NULL, 0}},
	I_reswords[] = {{"SDEF", DEFFUN},	{"SRELOC", ISRELOCFUN}, {NULL, 0}},
	M_reswords[] = {{"ACRO", DEFMACRO},	{"OD", MODSYMBOL},	{NULL, 0}},
	N_reswords[] = {{"OT", NOTSYMBOL},	{NULL, 0}},
	O_reswords[] = {{"R", ORSYMBOL},	{NULL, 0}},
	P_reswords[] = {{"UBLIC", PUBLIC},	{"OP", POP}, {"USH", PUSH}, {NULL, 0}},
	S_reswords[] = {{"HR", SHRSYMBOL},	{"HL", SHLSYMBOL},	{"TRLEN", STRLENFUN},	{"TRIDX", STRIDXFUN},	{NULL, 0}},
	T_reswords[] = {{"OS", TOS},		{NULL, 0}},
	V_reswords[] = {{"AR", VARSYMBOL},	{NULL, 0}},
	X_reswords[] = {{"OR", BXORSYMBOL},	{NULL, 0}},

	// This pointer is set in the big switch to point the
	// correct part of the table of the reserved words.
	*reswords;
	// struct end

	first_character_of_the_lexeme = first_unused_character;
	/* Step over the spaces. */
	SPACEAT;

	if(*first_unused_character == '\'')
	{
		/* This must be an ASCII number like 65 == 'A' */
		symbol = NUMBER;
		number = 0;
		j = 0;
		first_unused_character++;
		while(*first_unused_character && *(first_unused_character) != '\'')
		{
			if(*first_unused_character == '\\')
			{
				first_unused_character++;
				if(!*first_unused_character)
					break;
				switch(*first_unused_character)
				{
					case 'n':
						number = 0x100 * number + '\n';
						first_unused_character++;
						j++;
						break;
					case 't':
						number = 0x100 * number + '\t';
						first_unused_character++;
						j++;
						break;
					case 'r':
						number = 0x100 * number + '\r';
						first_unused_character++;
						j++;
						break;
					case '8':
					case '9':
						if(asciiradix == 8)
							error("Non octal character in character constant.", WARNING);
						// no break
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
						i = *(first_unused_character++) - '0';
						if(!*first_unused_character)
						{
							number = 0x100 * number + i;
							break;
						}
						if(*first_unused_character < '0' || *first_unused_character > '0' + asciiradix - 1)
						{
							number = 0x100 * number + i;
							if(asciiradix == 8 && *first_unused_character > '7' && *first_unused_character <= '9')
								error("Oktal number terminated by digit.", WARNING);
							break;
						}
						i = asciiradix * i + *(first_unused_character++) - '0';
						if(!*first_unused_character)
						{
							number = 0x100 * number + i;
							break;
						}
						if(*first_unused_character < '0' || *first_unused_character > '0' + asciiradix - 1)
						{
							number = 0x100 * number + i;
							if(asciiradix == 8 && *first_unused_character > '7' && *first_unused_character <= '9')
								error("Oktal number terminated by digit.", WARNING);
							break;
						}
						i = asciiradix * i + *(first_unused_character++) - '0';
						number = 0x100 * number + i;
						j++;
						if(asciiradix == 8 && *first_unused_character > '7' && *first_unused_character <= '9')
							error("Oktal number terminated by digit.", WARNING);
						break;

				}/* End of switch */
			}
			else
			{
				number = 0x100 * number + char_code[(int)*first_unused_character];
				first_unused_character++;
				j++;
			}
		}/* End of the while */
		if(*first_unused_character)/* Step over the ' character. */
			first_unused_character++;
		else
			error("Character not terminated.", WARNING);
		if(j > MAXASCLEN)/* Too many character are in the char constant. */
			error("ASCII number too long.", WARNING);
		/* If we want to use non ASCII char codes. */
		//number = number;
		return;
	}/* End of the ASCII number. */

	if(!isdigit(*first_unused_character) && !isIDalpha(*first_unused_character))
	{
		if(!*first_unused_character)
		{
			symbol = UNKNOWN;
			return;
		}
		/* Look it up in the table. */
		/* Not the reserved words!  */
		for(i = 0; lexemes[i].lexeme; i++)
		{
			for(j = 0; lexemes[i].lexeme[j]; j++)
				if(lexemes[i].lexeme[j] != first_unused_character[j])
					break; /* get off the inner loop (for) */
			/* If we found it. */
			if(!lexemes[i].lexeme[j])
				break; /* get off the outer loop (for) */
		}
		if(lexemes[i].lexeme)
		{/* we found the lexeme */
			/* Now it points the first character after the lexeme. */
			first_unused_character = &(first_unused_character[j]);

			symbol = lexemes[i].token;
			return;
		}
		symbol = UNKNOWN;
		return;
	}

	if(isIDalpha(*first_unused_character))
	{
		j = 0;
		while(isIDchar(*first_unused_character))
		{
			name[j] = *first_unused_character;
			first_unused_character++;
			j++;
		}
		name[j] = '\0';
		j = *name;
		if(islower(j))
			j = toupper(j);
		switch(j)
		{
			case 'A':
				reswords = A_reswords;
				break;
			case 'C':
				reswords = C_reswords;
				break;
			case 'D':
				reswords = D_reswords;
				break;
			case 'E':
				reswords = E_reswords;
				break;
			case 'I':
				reswords = I_reswords;
				break;
			case 'M':
				reswords = M_reswords;
				break;
			case 'N':
				reswords = N_reswords;
				break;
			case 'O':
				reswords = O_reswords;
				break;
			case 'P':
				reswords = P_reswords;
				break;
			case 'S':
				reswords = S_reswords;
				break;
			case 'T':
				reswords = T_reswords;
				break;
			case 'V':
				reswords = V_reswords;
				break;
			case 'X':
				reswords = X_reswords;
				break;
			default:
				symbol = IDENTIFIER;
				return;
		}
		for(j = 0; reswords[j].lexeme; j++)
			if(!stricmp(reswords[j].lexeme, &(name[1])))
			{
				symbol = reswords[j].token;
				return;
			}
		symbol = IDENTIFIER;
		return;
	}

	if(isdigit(*first_unused_character))
	{
		symbol = NUMBER;
		j = 0;
		while(isxdigit(*first_unused_character))
		{
			name[j++] = *first_unused_character;
			first_unused_character++;
		}
		name[j] = '\0';

		postfix = *first_unused_character;
		if(postfix == 'H' || postfix == 'h' || postfix == 'O' || postfix == 'o')
			first_unused_character++;//'h' and 'o' (because isxdigit() stopped counting it as number)
		else
			postfix = *(first_unused_character - 1);//'b' and 'd' (because isxdigit() added them to the number)

		number = 0;
		switch(postfix)
		{
			case 'h':
			case 'H':
				for(j = 0; name[j]; j++)
					number = 16 * number + hex(name[j]);
				return;
			case 'o':
			case 'O':
				for(j = 0; name[j]; j++)
					number = 8 * number + okt(name[j]);
				return;
			case 'b':
			case 'B':
				name[j - 1] = '\0';//because isxdigit() added 'b' to the number
				for(j = 0; name[j]; j++)
					number = 2 * number + bin(name[j]);
				return;
			case 'd':
			case 'D': /* Decimal number. */
				name[j - 1] = '\0';//because isxdigit() added 'd' to the number
				// no break
			default: /* Decimal number without postfix. */
				for(j = 0; name[j]; j++)
					number = 10 * number + dec(name[j]);
				return;
		}
	}

	symbol = UNKNOWN;
	return;
}/* End of getsymbol(). */



