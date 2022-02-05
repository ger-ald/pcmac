/* The constants needed by the macro assembler. */

#ifndef DEFINES_H_
#define DEFINES_H_

/* The maximal length of a line. */
#define MAXLINLEN 500
/*
 ** TOKENS
 */
#define ORSYMBOL		1 // '||'
#define ANDSYMBOL		2 // '&&'
#define SHRSYMBOL		3
#define SHLSYMBOL		4
#define MODSYMBOL		5
#define EQSYMBOL		6
#define IDENTIFIER		7
#define NOTSYMBOL		8
#define NEGSYMBOL		9
#define MINUSSYMBOL		10
#define PLUSSYMBOL		11
#define BANDSYMBOL		12 // '&'
#define BORSYMBOL		13 // '|'
#define BXORSYMBOL		14
#define MULTSYMBOL		15 // '*'
#define ADDSYMBOL		16 // '+'
#define GTSYMBOL		17 // '>'
#define GESYMBOL		18 // '>='
#define LTSYMBOL 		19 // '<'
#define LESYMBOL 		20 // '<='
#define NESYMBOL		21 // '!='
#define DIVSYMBOL		22 // '/'
#define LPARENT			23 // '('
#define RPARENT			24 // ')'
#define LBRA			25 // '['
#define RBRA			26 // ']'
#define NUMBER			27
#define UNKNOWN			28
#define UNDEFINED		29
#define UNDEFLABEL		30
#define SERROR			31 // Syntax error
#define RPARENTMISSING	32
#define POP				33
#define PUSH			34
#define	PUBLIC			35
#define EXTERNAL		36
#define DEFEXTR			37
#define DEFWORD			38
#define DEFBYTE			39
#define ENDMACRO		40
#define VARSYMBOL		41
#define DEFSET			42
#define DEFMACRO		43
#define ASSIGN			44
#define DORELOCFUN		45
#define DOURELOCFUN		46
#define ISRELOCFUN		47
#define DEFFUN			48
#define STRLENFUN		49
#define COMMA			50
#define DEFDWORD		51
#define SHARP			52
#define TOS				53
#define EXTIDTF			54
#define DEFRBYTE		55
#define DEFRWORD		56
#define DEFRDWORD		57
#define STRIDXFUN		58

#define DOLLAR				"$"
#define SETMEMBERSEPARATOR	'\n'
#define ESCAPECHARACTER		'\\'
/* The macro prefix character in the library. */
/* Non real ascii character. */
#define MACARG			7
/* The macro nunbering prefix character in the library.*/
/* Non ascii, and not the same as MACARG! */
#define MACNUM			8
/* Universal constants. */
#ifndef FALSE
#define FALSE			0
#endif
#ifndef TRUE
#define TRUE			1
#endif

/* Types of the sets. */
#define NUMERICSET		0
#define STRINGSET		1
#define STRING			2
#define LABELSET		3
#define DUMMY_SET		4

/* Type of errors. */
#define NORMAL			0
#define FATAL			1
#define INTERNAL		2
#define WARNING			3
#define MESSAGE			4

/* Types of symbols. */
#define VARIABLE		0
#define SET				1
#define LABEL			3
/* #define EXTERNAL 36 already defined. */

/* Types of a macro loop. */
#define REPEAT			0
#define	WHILE			1

/* The maximum number of include files nested. */
#define MAXINCNEST		5
/* The prime for the hash function. */
#define PRIME			211
#define MASK			0xf0000000l
/* The value which is given back by the hash
   function for a zero length string. */
#define EMPTYHASH		0

#define BITSPERBYTE		8
#define BITSPERINT		16
#define BYTEMASK		0xff
#define INTMASK			0xffff

/* Constants for listing.
 */
/* Width for the address. */
#define ADDRESS			8
/* The number of value to print into the same line. */
#define VALUESS			4
/* The maximal length of an ascii number. */
#define MAXASCLEN		4
/* The number of the stacks for the user */
#define USRSTKSIZ		100

/*
 **OBJCODE CONSTANT
 */
#define LINKCODE		0x81
#define LINKRELOC		0x82
#define LINKPUB			0x83
#define LINKEXTDEF		0x84
#define LINKEXTREF		0x85

/* SPACEAT eats the spaces */
#define SPACEAT while( isspace(*first_unused_character) ) first_unused_character++

#define valtype long

#ifdef tolower
#undef tolower
#endif
#define tolower(x) __tolower__[x]

#endif /* DEFINES_H_ */
