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
** EXPRESSION EVALUATING!
** expression        ::= orexpression      { '||' orexpression  }.
** orexpression      ::= andexpression     { '&&' andexpression }.
** andexpression     ::= borexpression     { '|' borexpression  }.
** borexpression     ::= xorexpression     { '^' xorexpression  }.
** xorexpression     ::= bandexpression    { '&' bandexpression }.
** bandexpression    ::= neoreexpression   { ( '<>' | '=' )              neoreexpression   }.
** neoreexpression   ::= compareexpression { ( '<' | '>' | '<=' | '>=' ) compareexpression }.
** compareexpression ::= shiftexpression   {( '<<' | '>>' )              shiftexpression   }.
** shiftexpression   ::= addexpression     { ( '+' | '-' )               addexpression     }.
** addexpression     ::= multexpression    { ( '*' | '/' | '%' )         multexpression    }.
** multexpression    ::= { ( '!' | '~' | '-' | '+' ) } tag.
** tag               ::= number | label | '(' expression ')' |
**			 func '(' expression ')'.
**
** number ::= label | decimal number with or without d postfix |
**	     	binary number with b postfix |
**     		hexadecimal number with h postfix |
** 	    	octal number with o postfix	|
**     		ascii number, max 8 character long,
**     		may contain escape sequences,
**	     	closed between ' like 'AS'.
**
** The operations are the same as they are in C.
** They have the same precedence.
**
** Verhas Peter   1988. marcius 3.
**
*/



/*
** EXPRESSION EVALUATING.
** It returns the length of the string containing the expression.
*/
int expression( s , val , err , reloc )
char *s; /* The start of the expression. */
valtype *val; /* The value of the expression. */
int *err; /* The number of the error occured
	     while evaluating the expression. */
int *reloc; /* True if the expression is
	       relocatable and false otherwise. */
{
	errortype = 0;/* No error yet. */
	first_unused_character = s;
	while( isspace( *first_unused_character ) )
		first_unused_character ++;
	if(  *first_unused_character == '(' || *first_unused_character == '[' ) {
		*err = SERROR;
		*reloc = 0;
		*val = 0;
		return (first_unused_character - s );
	}
	getsymbol();
	evalswitch = 1; /* Turn evaluating switch ON. */
	_expression( val );
	*err = errortype;
	/* The default value of an errorful expression is zero.(FALSE) */
	if( errortype ) *val = 0;
	*reloc = reloctype;
	return (first_character_of_the_lexeme - s );
}
void _expression( val )
valtype *val;
{
	int saveeval;
	valtype value;
	saveeval = evalswitch;
	value = orexpression();
	while( symbol == ORSYMBOL ) {
		getsymbol();
		if( value ) {
			evalswitch = 0 ;
			orexpression();
		}
		else {
			value = orexpression();
		}
		reloctype = FALSE;
	}
	evalswitch = saveeval;
	*val = value;
}/* End of _expression() */

valtype orexpression()
{
	int saveeval;
	valtype value;
	saveeval = evalswitch;
	value = andexpression();
	while( symbol == ANDSYMBOL ) {
		getsymbol();
		if( !value ) {
			evalswitch = 0 ;
			andexpression();
		}
		else	{
			value = andexpression();
		}
		reloctype = FALSE;
	}
	evalswitch = saveeval;
	return value;
}/* End of orexpression() */

valtype andexpression()
{
	valtype value;
	value =borexpression();
	while( symbol == BORSYMBOL ) {
		getsymbol();
		value |= borexpression();
		reloctype = FALSE;
	}
	return value;
}/* End of andexpression() */

valtype borexpression()
{
	valtype value;
	value = xorexpression();
	while( symbol == BXORSYMBOL ) {
		getsymbol();
		value ^= xorexpression();
		reloctype = FALSE;
	}
	return value;
}/* End of borexpression. */
valtype xorexpression()
{
	valtype value;
	value = bandexpression();
	while( symbol == BANDSYMBOL ) {
		getsymbol();
		value &= bandexpression();
		reloctype = FALSE;
	}
	return value;
}/* End of xorexpression() */
valtype bandexpression()
{
	valtype value;
	value = neoreexpression();
	while( symbol == EQSYMBOL ||
	    symbol == NESYMBOL ) {
		switch( symbol ) {
		case EQSYMBOL: 
			getsymbol();
			value = (value == neoreexpression() );
			break;
		case NESYMBOL: 
			getsymbol();
			value = (value != neoreexpression() );
			break;
		}
		reloctype = FALSE;
	}
	return value;
}/* End of bandexpression() */
valtype neoreexpression()
{
	valtype value;
	value = compareexpression();
	while(	symbol == GTSYMBOL ||
	    symbol == GESYMBOL ||
	    symbol == LTSYMBOL ||
	    symbol == LESYMBOL  ) {
		switch( symbol ) {
		case GTSYMBOL :
			getsymbol();
			value = ( value >  compareexpression() );
			break;
		case GESYMBOL :	
			getsymbol();
			value = ( value >= compareexpression() );
			break;
		case LTSYMBOL :	
			getsymbol();
			value = ( value <  compareexpression() );
			break;
		case LESYMBOL : 
			getsymbol();
			value = ( value <= compareexpression() );
			break;
		}
		reloctype = FALSE;
	}
	return value;
}/* End of neoreexpression() */
valtype compareexpression()
{
	valtype value;
	value = shiftexpression();
	while(	symbol == SHRSYMBOL ||
	    symbol == SHLSYMBOL ) {
		switch( symbol ) {
		case SHRSYMBOL   : 
			getsymbol();
			value >>= shiftexpression();
			break;
		case SHLSYMBOL : 
			getsymbol();
			value <<= shiftexpression();
			break;
		}
		reloctype = FALSE;
	}
	return value;
}/* End of compareexpression() */
valtype shiftexpression()
{
	valtype value;
	int savereloc;
	value = addexpression();
	savereloc = !!reloctype;
	while(	symbol == ADDSYMBOL ||
	    symbol == MINUSSYMBOL ) {
		switch( symbol ) {
		case ADDSYMBOL   : 
			getsymbol();
			value += addexpression();
			if( reloctype )
				savereloc ++;
			break;
		case MINUSSYMBOL : 
			getsymbol();
			value -= addexpression();
			if( reloctype )
				savereloc--;
			break;
		}
	}
	reloctype =( savereloc == 1 );
	return value;
}/* End of compareexpression() */

valtype addexpression()
{
	valtype value;
	value = multexpression();
	while(	symbol == MULTSYMBOL ||
	    symbol == MODSYMBOL  ||
	    symbol == DIVSYMBOL   ) {
		switch( symbol ) {
		case MULTSYMBOL : 
			getsymbol();
			value *= multexpression();
			break;
		case MODSYMBOL  : 
			getsymbol();
			value %= multexpression();
			break;
		case DIVSYMBOL  :
			getsymbol();
			value /= multexpression();
			break;
		}
		reloctype = FALSE;
	}
	return value;
}/* End of addexpression() */

valtype multexpression()
{
	valtype value;
	switch( symbol ) {
	case NOTSYMBOL   : 
		getsymbol();
		value = !multexpression();
		reloctype = FALSE;
		break;
	case NEGSYMBOL   : 
		getsymbol();
		value = ~multexpression();
		reloctype = FALSE;
		break;
	case MINUSSYMBOL : 
		getsymbol();
		value = -multexpression();
		reloctype = FALSE;
		break;
	case ADDSYMBOL   : 
		getsymbol();
		value =  multexpression();
		break;
	default	         : 
		value = tag();
		break;
	}
	return value;
}/* End of multexpression() */

valtype tag()
{
	struct symbol *ptr;
	valtype value;
	switch( symbol ) {
	case LBRA : 
		getsymbol();
		_expression( &value );
		if( symbol != RBRA )
			errortype = RBRAMISSING;
		else
			getsymbol();
		break;
	case LPARENT : 
		getsymbol();
		_expression( &value );
		if( symbol != RPARENT )
			errortype = RBRAMISSING;
		else
			getsymbol();
		break;
	case NUMBER :
		value = number;
		getsymbol();
		reloctype = FALSE;
		break;
	case IDENTIFIER :
		ptr = search_in_the_table( name );
		if( ptr->type_of_the_symbol == UNDEFINED && evalswitch )
			errortype = UNDEFLABEL;
		if( ptr->type_of_the_symbol == EXTERNAL && evalswitch )
			errortype = EXTIDTF;
		value = ptr->value_of_the_symbol;
		reloctype = ptr->relocatable;
		getsymbol();
		break;
	case DORELOCFUN :
		getsymbol();
		if( symbol != LBRA )
			errortype = SERROR;
		else
			getsymbol();
		_expression( &value );
		if( symbol != RBRA )
			errortype = SERROR;
		else
			getsymbol();
		reloctype = TRUE;
		break;
	case POP	:
		getsymbol();
		if( symbol != LBRA )
			errortype = SERROR;
		else
			getsymbol();
		_expression( &value );
		if( symbol != RBRA )
			errortype = SERROR;
		else
			getsymbol();
		value = usrpop( (int)value );
		break;
	case TOS	:
		getsymbol();
		if( symbol != LBRA )
			errortype = SERROR;
		else
			getsymbol();
		_expression( &value );
		if( symbol != RBRA )
			errortype = SERROR;
		else
			getsymbol();
		value = usrtos( (int)value );
		break;
	case DOURELOCFUN :
		getsymbol();
		if( symbol != LBRA )
			errortype = SERROR;
		else
			getsymbol();
		_expression( &value );
		if( symbol != RBRA )
			errortype = SERROR;
		else
			getsymbol();
		reloctype = FALSE;
		break;
	case ISRELOCFUN :
		getsymbol();
		if( symbol != LBRA )
			errortype = SERROR ;
		else
			getsymbol();
		_expression( &value );
		if( symbol != RBRA )
			errortype = SERROR;
		else
			getsymbol();
		value = !!reloctype;
		reloctype = FALSE ;
		break;
	case DEFFUN :
		getsymbol();
		if( symbol != LBRA )
			errortype = SERROR;
		else
			getsymbol();
		if( symbol != IDENTIFIER ) {
			errortype = SERROR;
			if( symbol != RBRA )
				getsymbol();
		}
		else {
			ptr = search_in_the_table( name );
			value = (ptr->type_of_the_symbol != UNDEFINED);
			getsymbol();
		}
		if( symbol != RBRA )
			errortype = SERROR;
		reloctype = FALSE;
		break;
	case STRLENFUN :
		getsymbol();
		if( symbol != LBRA )
			errortype = SERROR;
		SPACEAT;
		if( *first_unused_character++ != '\"' ) {
			errortype = SERROR;
			reloctype = FALSE; /* It is not interesting. */
			break;
		}
		value = 0;
		while( *first_unused_character &&
		    *first_unused_character != '\"' ) {
			if( *first_unused_character == '\\' ){
				first_unused_character ++;
				if( isdigit( *first_unused_character ) ){
					/* The first character a digit. */
					first_unused_character++;
					if( *first_unused_character>='0' &&
					    *first_unused_character<asciiradix+'0'){
						/* The second character is also a good number. */
						first_unused_character++;
						if( *first_unused_character<'0' ||
						    *first_unused_character>asciiradix+'0'-1 )
							/* The third charcter is not good. */
							first_unused_character --;/*step back */
					} else first_unused_character--;
				}
			}
			if( *first_unused_character )
				first_unused_character++;
			value++;
		}
		if( *first_unused_character != '\"' )
			errortype = SERROR ;
		else
			first_unused_character++;
		getsymbol();
		if( symbol != RBRA )
			errortype = SERROR;
		else
			getsymbol();
		reloctype = FALSE;
		break;
	default : 	
		errortype = SERROR;
		break;
	}
	return value;
}/* End of tag() */
/*
** return TRUE if c is a valid character for an identifier
*/
int isIDchar( c )
char c;
{
	return	(c>='a' && c<='z') ||
	    (c>='A' && c<='Z') ||
	    (c>='0' && c<='9') ||
	    (c=='_' 	 ) ||
	    (c=='$'		 ) ||
	    (c=='@'		 )
	    ;
}
/*
** return TRUE if c is valid for the first character of an identifier
*/
isIDalpha( c )
char c;
{
	return	(c>='a' && c<='z') ||
	    (c>='A' && c<='Z') ||
	    (c=='_' 	 ) ||
	    (c=='$'		 ) ||
	    (c=='@'		 )
	    ;
}

/* Returns the value of a hexadecimal digit. */
valtype hex(c)
int c;
{
	int i;
	i=0;
	while( c != "0123456789ABCDEF"[i] && i<16 )
		i++;
	if( i != 16 )
		return i;
	i=10;
	while( c != "abcdef"[i-10] && i<16 )
		i++;
	if( i != 16 )
		return i;
	error("Wrong hexadecimal number!",NORMAL);
	return 0;
}

/* Returns the value of a binary digit. */
valtype bin(c)
int c;
{
	if( c == '0' ) return 0;
	if( c == '1') return 1;
	error("Wrong binary number!",NORMAL);
	return 0;
}

/* Returns the value of an oktal digit. */
valtype okt(c)
int c;
{
	int i;
	i=0;
	while( c != "01234567"[i] && i<8 )
		i++;
	if( i != 8 )
		return i;
	error("Wrong oktal number!",NORMAL);
	return 0;
}

/* Returns the value of a decimal digit. */
valtype dec(c)
int c;
{
	if( c>'9' || c<'0' )
	{
		error("Wrong decimal number!",NORMAL);
		return 0;
	}
	return c-'0';
}
/*
** LEXICAL ANALIZER
*/
void getsymbol()
{
	int i,j;
	char postfix;
	static struct {
		char *lexeme;
		int token;
	}		lexemes[]={
		/*   WARNING ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! !
** While constructing this table you must keep in mind that
** a lexeme A prefix of lexeme B must be peceeded here by B.
** Because of the linear search, and there is not stop symbol
** in the text.
*/
		","			,	COMMA		,
		"#"			,	SHARP		,
		":="		,	ASSIGN		,
		"||"		,	ORSYMBOL	,
		"&&"		,	ANDSYMBOL	,
		">>"		,	SHRSYMBOL	,
		"<<"		,	SHLSYMBOL	,
		"%"			,	MODSYMBOL	,
		"<>"		,	NESYMBOL	,
		"!="		,	NESYMBOL	,
		">="		,	GESYMBOL	,
		">"			,	GTSYMBOL	,
		"<="		,	LESYMBOL	,
		"<"			,	LTSYMBOL	,
		"=="		,	EQSYMBOL	,
		"="			,	EQSYMBOL	,
		"("			,	LBRA		,
		")"			,	RBRA		,
		"["			,	LPARENT		,
		"]"			,	RPARENT		,
		"!"			,	NOTSYMBOL	,
		"~"			,	NEGSYMBOL	,
		"&"			,	BANDSYMBOL	,
		"^"			,	BXORSYMBOL	,
		"|"			,	BORSYMBOL	,
		"*"			,	MULTSYMBOL	,
		"/"		  	,	DIVSYMBOL	,
		"+"			,	ADDSYMBOL	,
		"-"			,	MINUSSYMBOL	,
		NULL		,	0		/* Terminating the table */
		}, 
/*
** These tables contain the reserver words.
** None of the strings contains the first letter.
** If a new keyword starting with the letter XYZ then
** put into the block XYZ_reswords.
** If XYZ_reswords does not exist then create and define XYZ_START.
** If this is not defined then the big switch will not be 
** compiled correct.
*/
#define A_START	
		A_reswords[]={
		"ND"		,	ANDSYMBOL	,
		NULL		,	0			},

#define C_START	
		C_reswords[]={
		"ONST"		,	DEFSET		,
		NULL		,	0			},

#define D_START
		D_reswords[]={
		"B"		,	DEFBYTE		,
		"D"		,	DEFDWORD	,
		"ORELOC"	,	DORELOCFUN	,
		"OURELOC"	,	DOURELOCFUN	,
		"RB"		,	DEFRBYTE	,
		"RD"		,	DEFRDWORD	,
		"RW"		,	DEFRWORD	,
		"W"		,	DEFWORD		,
		NULL		,	0			},

#define E_START
		E_reswords[]={
		"NDM"		,	ENDMACRO	,
		"XTRN"		,	EXTERNAL	,
		"XTERN"	,	EXTERNAL	,
		NULL		,	0			},

#define I_START
		I_reswords[]={
		"SDEF"		,	DEFFUN		,
		"SRELOC"	,	ISRELOCFUN	,
		NULL		,	0			},

#define M_START
		M_reswords[]={
		"ACRO"		,	DEFMACRO	,
		"OD"		,	MODSYMBOL	,
		NULL		,	0			},

#define N_START
		N_reswords[]={
		"OT"		,	NOTSYMBOL	,
		NULL		,	0			},

#define O_START
		O_reswords[]={
		"R"		,	ORSYMBOL	,
		NULL		,	0		},

#define P_START
		P_reswords[]={
		"UBLIC"		,	PUBLIC		,
		"OP"		,	POP			,
		"USH"		,	PUSH		,
		NULL		,	0			},

#define S_START
		S_reswords[]={
		"HR"		,	SHRSYMBOL	,
		"HL"		,	SHLSYMBOL	,
		"TRLEN"		,	STRLENFUN	,
		NULL		,	0			},

#define T_START
		T_reswords[]={
		"OS"		,	TOS			,
		NULL		,	0			},

#define V_START
		V_reswords[]={
		"AR"		,	VARSYMBOL	,
		NULL		,	0			},

#define X_START
		X_reswords[]={
		"OR"		,	BXORSYMBOL	,
		NULL		,	0			},
/* This pointer is set in the big switch to point the 
   correct part of the table of the reserved words. */
		*reswords;



		first_character_of_the_lexeme = first_unused_character;
		/* Step over the spaces. */
		while( isspace( *first_unused_character ) )
			first_unused_character ++;


		if( *first_unused_character == '\'' ) {
			/* This must be an ASCII number like 65 == 'A' */
			symbol = NUMBER;
			number = 0;
			j=0;
			first_unused_character++;
			while( *first_unused_character &&
			    *(first_unused_character) != '\'' ) {
				if( *first_unused_character == '\\' )  {
					first_unused_character++;
					if( !*first_unused_character )
						break;
					switch( *first_unused_character )  {
					case 'n':
						number = 0x100*number +  '\n' ;
						first_unused_character++;
						j++;
						break;
					case 't':
						number = 0x100*number +  '\t' ;
						first_unused_character++;
						j++;
						break;
					case 'r':
						number = 0x100*number + '\r' ;
						first_unused_character++;
						j++;
						break;
					case '8':
					case '9':
						if( asciiradix == 8 )
							error(
							    "Non octal character in character constant.",
							    WARNING);
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
						i = *(first_unused_character++) -'0';
						if( !*first_unused_character ){
							number = 0x100*number + i;
							break;
						}
						if(   *first_unused_character<'0'
						    || *first_unused_character>
						    '0'+asciiradix-1){
							number = 0x100*number + i;
							if( asciiradix == 8
							    && *first_unused_character>'7'
							    && *first_unused_character<='9')
								error("Oktal number terminated by digit.",WARNING);
							break;
						}
						i = asciiradix*i +
						    *(first_unused_character++)-'0';
						if( !*first_unused_character ){
							number = 0x100*number + i;
							break;
						}
						if(   *first_unused_character<'0'
						    || *first_unused_character>
						    '0'+asciiradix-1){
							number = 0x100*number + i;
							if( asciiradix == 8
							    && *first_unused_character>'7'
							    && *first_unused_character<='9')
								error("Oktal number terminated by digit.",WARNING);
							break;
						}
						i = asciiradix*i +
						    *(first_unused_character++)-'0';
						number = 0x100*number + i;
						j++;
						if( asciiradix == 8
						    && *first_unused_character>'7'
						    && *first_unused_character<='9')
							error("Oktal number terminated by digit.",WARNING);
						break;

					}/* End of switch */
				}
				else
				{
					number = 0x100*number +  char_code[*first_unused_character];
					first_unused_character++;
					j++;
				}
			}/* End of the while */
			if( *first_unused_character )/* Step over the ' character. */
				first_unused_character++;
			else
				error("Character not terminated.",WARNING);
			if( j>MAXASCLEN )/* Too many character are in the char constant. */
				error("ASCII number too long.",WARNING);
			/* If we want to use non ASCII char codes. */
			number = number;
			return;
		}/* End of the ASCII number. */



		if( !isdigit( *first_unused_character ) &&
		    !isIDalpha( *first_unused_character )  ) {
			if( !*first_unused_character ){
				symbol = UNKNOWN;
				return;
			}
			/* Look it up in the table. */
			/* Not the reserwed words!  */
			for( i=0 ; lexemes[i].lexeme ; i++ ) {
				for( j = 0 ; lexemes[i].lexeme[j] ; j++ )
					if( lexemes[i].lexeme[j] != first_unused_character[j] )
						break; /* get off the inner loop (while) */
				/* If we found it. */
				if( !lexemes[i].lexeme[j] )
					break; /* get off the outer loop (for) */
			}
			if( lexemes[i].lexeme ) {/* we found the lexeme */
				/* Now it points the first character after the lexeme. */
				first_unused_character = &(first_unused_character[j]);

				symbol = lexemes[i].token;
				return;
			}
			symbol = UNKNOWN;
			return;
		}


		if( isIDalpha( *first_unused_character ) ) {
			j = 0;
			while( isIDchar( *first_unused_character )) {
				name[j] = *first_unused_character;
				first_unused_character++;
				j++;
			}
			name[j] = '\0';
			j = *name;
			if( islower(j)) j = toupper( j );
			switch( j ){
#ifdef A_START
			case 'A':reswords=A_reswords;break;
#endif
#ifdef B_START
			case 'B':reswords=B_reswords;break;
#endif
#ifdef C_START
			case 'C':reswords=C_reswords;break;
#endif
#ifdef D_START
			case 'D':reswords=D_reswords;break;
#endif
#ifdef E_START
			case 'E':reswords=E_reswords;break;
#endif
#ifdef F_START
			case 'F':reswords=F_reswords;break;
#endif
#ifdef G_START
			case 'G':reswords=G_reswords;break;
#endif
#ifdef H_START
			case 'H':reswords=H_reswords;break;
#endif
#ifdef I_START
			case 'I':reswords=I_reswords;break;
#endif
#ifdef J_START
			case 'J':reswords=J_reswords;break;
#endif
#ifdef K_START
			case 'K':reswords=K_reswords;break;
#endif
#ifdef L_START
			case 'L':reswords=L_reswords;break;
#endif
#ifdef M_START
			case 'M':reswords=M_reswords;break;
#endif
#ifdef N_START
			case 'N':reswords=N_reswords;break;
#endif
#ifdef O_START
			case 'O':reswords=O_reswords;break;
#endif
#ifdef P_START
			case 'P':reswords=P_reswords;break;
#endif
#ifdef Q_START
			case 'Q':reswords=Q_reswords;break;
#endif
#ifdef R_START
			case 'R':reswords=R_reswords;break;
#endif
#ifdef S_START
			case 'S':reswords=S_reswords;break;
#endif
#ifdef T_START
			case 'T':reswords=T_reswords;break;
#endif
#ifdef Z_START
			case 'Z':reswords=Z_reswords;break;
#endif
#ifdef X_START
			case 'X':reswords=X_reswords;break;
#endif
#ifdef Y_START
			case 'Y':reswords=Y_reswords;break;
#endif
#ifdef V_START
			case 'V':reswords=V_reswords;break;
#endif
#ifdef W_START
			case 'W':reswords=W_reswords;break;
#endif
			default: symbol = IDENTIFIER;return;
			}
			for( j=0 ; reswords[j].lexeme ; j++ )
				if( !stricmp(reswords[j].lexeme ,&(name[1]) ) ) {
					symbol = reswords[j].token;
					return;
				}
			symbol = IDENTIFIER;
			return;
		}



		if( isdigit( *first_unused_character ) ) {
			symbol = NUMBER;
			j=0;
			while( isxdigit( *first_unused_character )	) {
				name[j++] = *first_unused_character;
				first_unused_character++;
			}
			name[j] = '\0';
			postfix = *first_unused_character;
			if( postfix == 'H' || postfix == 'h' ||
			    postfix == 'O' || postfix == 'o'  )
				first_unused_character ++;
			else
				postfix = *(first_unused_character - 1 );
			number = 0;
			switch( postfix ) {
			case 'h':
			case 'H': 
				for( j=0 ; name[j] ; j++ )
					number = 16*number + hex( name[j] );
				return;
			case 'o':
			case 'O': 
				for( j=0 ; name[j] ; j++ )
					number =  8*number + okt( name[j] );
				return;
			case 'b':
			case 'B': 
				name[j-1] = '\0';
				for( j=0 ; name[j] ; j++ )
					number = 2*number + bin( name[j] );
				return;
			case 'd':
			case 'D': /* Decimal number. */
				name[j-1] = '\0';
			default:  /* Decimal number without postfix. */
				for( j=0 ; name[j] ; j++ )
					number = 10*number + dec( name[j] );
				return;
			}
		}

		symbol = UNKNOWN;
		return;
	}/* End of getsymbol(). */
