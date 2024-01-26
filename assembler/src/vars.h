/*
 * vars.h
 *
 *  Created on: 23 okt. 2014
 *      Author: gerald
 */

#ifndef VARS_H_
#define VARS_H_

#include <stdio.h>
#include "defines.h"
#include "types.h"

/*
 ** Global variables for the macro assembler.
 */
/* Points the first unused character for the lexical analyser. */
extern char *first_unused_character;
/* Points the first character of the lexeme. */
extern char *first_character_of_the_lexeme;
/* The last symbol was read by the getsymbol() */
extern int symbol;
/* The type of an error. */
extern int errortype;
/* Type of the expression. */
extern int reloctype;
/* The lexical analizer passes a numeric value. */
extern valtype number;
/* The lexical analyzer passes the
 lexeme of an identifier in this variable. */
extern char name[MAXLINLEN];
/* Contains the name of the original file. */
extern char *original_name;
/* The function get_line() gives back the line in this array. */
extern char cline[MAXLINLEN];
/* A line for buffering the input. */
extern char readinbuff[MAXLINLEN];
/* A line for listing .*/
extern char listline[MAXLINLEN + ADDRESS + 3 * VALUESS + 1];
/* Logical value switch by the #if directives. */
extern int fly_over_the_lines;
/* Points a string containing the name of the include directory. */
/* This string must contain the \ character either. 		 */
extern char *include_directory;
/* Pointer shows the position in the buffer. */
/* Points the first unused character. 	     */
extern int buffpos;
/* Points the depth of the include. */
/* So inputfile[incdepth] id the current file pointer. */
extern int incdepth;
/* This array contains the file pointers. */
extern FILE *inputfile[MAXINCNEST];
/* Points the root of the list of macros. */
extern struct macro *macroot[PRIME];
/* Points the last member of the macro definition chain. */
extern struct macro *maclast[PRIME];
/* A global variable for indexing maclast, and macroot. */
extern int macqueindx;
/*
 ** A global variable pointing the roots of the tables.
 */
extern struct symbol *hashtable[PRIME];
/* Contains 1 if pass is 1 and 2 if pass is 2. */
extern int pass;
/* Points the last member of the macro stack. */
extern struct macrostack *mslast;
/* Points in to the symbol table where the
 value of the "$" symbol was scheduled. */
extern unsigned valtype *dollar;
/* Points in to the symbol table where the
 value of the 'line' symbol was scheduled. */
extern unsigned valtype *symline;
/* Points in to the symbol table where the
 value of the 'tline' symbol was scheduled. */
extern unsigned valtype *symtline;
/* Points in to the symbol table where the
 value of the 'pass' symbol was scheduled. */
extern unsigned valtype *sympass;
/* To calculate the size of the generated code. */
extern unsigned long dollmin, dollmax;
/* Counting the conditional compiling levels. */
extern int ondeep, offdeep;
/* Counting the macro nesting deepness,
 for handling the not correct macro loops. */
extern int mac_nest_deep;
/* For handling macro loop stack. */
extern struct loopstack *loopstacklast;
/* It holds the current exeline.
 The previous of mslast->mlin. */
extern struct exeline *cmline;
/* A dummy variable for error handling, when
 in a macro syntax definition there are more *
 referring to a set then set name following the
 strin. */
extern struct symbol *dummy_set;
/* For handling phase error. */
extern int phaseoffset;
/* For listing. This variable shows, how many values were
 printed yet into the current line.	*/
extern int nrprvl;
/* Number of errors, warnings. */
extern int nrerrors, nrwarnings;
/* A pointer pointing to the array, where the code gets. */
extern char *code;
/* Switch for case sensitivity\insensitivity character comparing. */
extern int casesen;
/* File for listing. */
extern FILE *listfile;
/* For switching listing on, and off. */
extern int listswitch;
/* For witching the listing of macro extentions. */
extern int maclist;
/* If this switch is 1 then no errors for externals. */
extern int tskswitch;
/* Switch for making header file. */
extern int headswitch;
/* For switching listing on and off globally. */
extern int glistswitch;
/* For switching the list of includes. */
extern int ilistswitch;
extern int mlistswitch;
/* Counter for counting the nuber of the sets. */
extern int Nrofthesets;
/* A global file for writing the liobrary. */
extern FILE *libfile;
/* A variable to give a number for each set. */
extern int setnumbering;
/* A global pointer containing the file name of the output file. */
extern char *outfilnam;
/* A global variable containing the file name length of the output file. */
extern int outfilnamBuffSize;
/* It is zero while there was no byte generated. */
extern int first_flag;
/* These pointers point to the symbol table
 where the predefinit sets are. */
extern struct symbol *setptr000, *setptr001, *setptr002, *setptr003;
/* The root of the error stack, for postponing the error messages. */
extern struct errstack *erroot;
/* Contains the radix of the \nnn ascii characters. */
extern int asciiradix;
/* An integer which is 1 if and only if */
/* the file system readed a #macros or  */
/* #include directive				    */
extern int lmori;
/* Two flags driving the DW and DD which order
 ** to generate bytes, words.
 ** The default is: Lower Higher
 ** is set in init.c
 */
extern int dwflag, ddflag;
/* This variable contains the macro argumentum prefix. */
extern int macarg;
/* This variable contains the macro numbering character. */
extern int macnum;
/* This variable contains: how many macro started to extend. */
extern int macextnum;
/* points, if the error messages go to the stdout or not. */
extern int stdoutsw;
/* Roots of the user stack. */
extern struct usrs *usrstk[USRSTKSIZ];
/* Flag for every stack pointing if that is empty or not. */
/* (Used or unused.) */
extern char usrsf[USRSTKSIZ];
/* The buffer of the object maker. */
extern char linkbuff[2000];
/* Buffer position. */
extern int linkbuffpos;
/* The type of the opened block. */
extern int linkbloctype;
/* The file where the output goes. */
extern FILE *linkfp;
/* A variable counting the externals. */
extern int nexternal;
/* A flag for the mode that the assembler works. */
extern int objgen;
/* A buffer containing the relocatable addresses. */
extern struct reloc rebuff[256];
/* A position in the rebuff */
extern int rebuffpos;
/* A position in the array code if we make object. */
extern int codepos;
/* The first address of the code in a flush */
extern valtype fadblk;
/* An array for the correct, and fast tolower macro */
extern char __tolower__[0x100];
/* An array for the non ASCII character set. 			*/
/* Every element contains the code of the actual character.	*/
/* Eg.: char_code[65=='A'] = 65 if it is ASCII character set.	*/
/* This array is initiated before every pass by the line:	*/
/* for( i = 0 ; i<256 ; i++ )char_code[i]=i;			*/
extern char char_code[256];


#endif /* VARS_H_ */
