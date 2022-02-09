/*
 * vars.h
 *
 *  Created on: 23 okt. 2014
 *      Author: gerald
 */

#ifndef VARS_H_
#define VARS_H_

#include "defines.h"
#include "types.h"

/*
 ** Global variables for the macro assembler.
 */
/* Points the first unused character for the lexical analyser. */
char *first_unused_character;
/* Points the first character of the lexeme. */
char *first_character_of_the_lexeme;
/* The last symbol was read by the getsymbol() */
int symbol;
/* The type of an error. */
int errortype;
/* Type of the expression. */
int reloctype;
/* It switches the evaluating of the
 expression for the short circuit boolean. */
int evalswitch;
/* The lexical analizer passes a numeric value. */
valtype number;
/* The lexical analyzer passes the
 lexeme of an identifier in this variable. */
char name[MAXLINLEN];
/* Contains the name of the original file. */
char *original_name;
/* The function get_line() gives back the line in this array. */
char cline[MAXLINLEN];
/* A line for buffering the input. */
char readinbuff[MAXLINLEN];
/* A line for listing .*/
char listline[MAXLINLEN + ADDRESS + 3 * VALUESS + 1];
/* Logical value switch by the #if directives. */
int fly_over_the_lines;
/* Points a string containing the name of the include directory. */
/* This string must contain the \ character either. 		 */
char *include_directory;
/* Pointer shows the position in the buffer. */
/* Points the first unused character. 	     */
int buffpos;
/* Points the depth of the include. */
/* So inputfile[incdepth] id the current file pointer. */
int incdepth;
/* This array contains the file pointers. */
FILE *inputfile[MAXINCNEST];
/* Points the root of the list of macros. */
struct macro *macroot[PRIME];
/* Points the last member of the macro definition chain. */
struct macro *maclast[PRIME];
/* A global variable for indexing maclast, and macroot. */
int macqueindx;
/*
 ** A global variable pointing the roots of the tables.
 */
struct symbol *hashtable[PRIME];
/* Contains 1 if pass is 1 and 2 if pass is 2. */
int pass;
/* Points the last member of the macro stack. */
struct macrostack *mslast;
/* Points in to the symbol table where the
 value of the "$" symbol was scheduled. */
unsigned valtype *dollar;
/* Points in to the symbol table where the
 value of the 'line' symbol was scheduled. */
unsigned valtype *symline;
/* Points in to the symbol table where the
 value of the 'tline' symbol was scheduled. */
unsigned valtype *symtline;
/* Points in to the symbol table where the
 value of the 'pass' symbol was scheduled. */
unsigned valtype *sympass;
/* To calculate the size of the generated code. */
unsigned long dollmin, dollmax;
/* Counting the conditional compiling levels. */
int ondeep, offdeep;
/* Counting the macro nesting deepness,
 for handling the not correct macro loops. */
int mac_nest_deep;
/* For handling macro loop stack. */
struct loopstack *loopstacklast;
/* It holds the current exeline.
 The previous of mslast->mlin. */
struct exeline *cmline;
/* A dummy variable for error handling, when
 in a macro sintax definition there are more *
 referring to a set then set name following the
 strin. */
struct symbol *dummy_set;
/* For handling phase error. */
int phaseoffset;
/* For listing. This variable shows, how many values were
 printed yet into the current line.	*/
int nrprvl;
/* Number of errors, warnings. */
int nrerrors, nrwarnings;
/* A pointer pointing to the array, where the code gets. */
char *code;
/* Switch for case sensitivity\insensitivity character comparing. */
int casesen;
/* File for listing. */
FILE *listfile;
/* For switching listing on, and off. */
int listswitch;
/* For witching the listing of macro extentions. */
int maclist;
/* If this switch is 1 then no errors for externals. */
int tskswitch;
/* Switch for making header file. */
int headswitch;
/* For switching listing on and off globally. */
int glistswitch;
/* For switching the list of includes. */
int ilistswitch;
int mlistswitch;
/* Counter for counting the nuber of the sets. */
int Nrofthesets;
/* A global file for writing the liobrary. */
FILE *libfile;
/* A variable to give a number for each set. */
int setnumbering;
/* A global pointer containing the file name of the output file. */
char *outfilnam;
/* A global variable containing the file name length of the output file. */
int outfilnamBuffLen;
/* It is zero while there was no byte generated. */
int first_flag;
/* These pointers point to the symbol table
 where the predefinit sets are. */
struct symbol *setptr000, *setptr001, *setptr002, *setptr003;
/* The root of the error stack, for postponing the error messages. */
struct errstack *erroot;
/* Contains the radix of the \nnn ascii characters. */
int asciiradix;
/* An integer which is 1 if and only if */
/* the file system readed a #macros or  */
/* #include directive				    */
int lmori;
/* Two flags driving the DW and DD which order
 ** to generate bytes, words.
 ** The default is: Lower Higher
 ** is set in init.c
 */
int dwflag, ddflag;
/* This variable contains the macro argumentum prefix. */
int macarg;
/* This variable contains the macro numbering character. */
int macnum;
/* This variable contains: how many macro started to extend. */
int macextnum;
/* points, if the error messages go to the stdout or not. */
int stdoutsw;
/* Roots of the user stack. */
struct usrs *usrstk[USRSTKSIZ];
/* Flag for every stack pointing if that is empty or not. */
/* (Used or unused.) */
char usrsf[USRSTKSIZ];
/* The buffer of the object maker. */
char linkbuff[2000];
/* Buffer position. */
int linkbuffpos;
/* The type of the opened block. */
int linkbloctype;
/* The file where the output goes. */
FILE *linkfp;
/* A variable counting the externals. */
int nexternal;
/* A flag for the mode that the assembler works. */
int objgen;
/* A buffer containing the relocatable addresses. */
struct reloc rebuff[256];
/* A position in the rebuff */
int rebuffpos;
/* A position in the array code if we make object. */
int codepos;
/* The first address of the code in a flush */
valtype fadblk;
/* An array for the correct, and fast tolower macro */
char __tolower__[0x100];
/* An array for the non ASCII character set. 			*/
/* Every element contains the code of the actual character.	*/
/* Eg.: char_code[65=='A'] = 65 if it is ASCII character set.	*/
/* This array is initiated before every pass by the line:	*/
/* for( i = 0 ; i<256 ; i++ )char_code[i]=i;			*/
char char_code[256];


#endif /* VARS_H_ */
