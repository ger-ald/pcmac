/*
 * pass.c
 *
 *  Created on: 24 okt. 2014
 *      Author: gerald
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>

#include "pass.h"

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
#include "libmak.h"
#include "objmad.h"
#include "primitiv.h"
#include "stable.h"
#include "tfunc.h"
#include "utilits.h"

//from main projectfile
struct macro *ismacro(char *, char ***, int);
int macrodef(void);

/*
 *
 * dopas() makes a pass of compiling.
 *
 *
 */
//#define __FUNC__ "dopas"
void dopas(void)
{
	char **sets_lexeme;
	char label[MAXLINLEN], line[MAXLINLEN];
	struct symbol *ptr = NULL;
	struct macro *macptr;
	valtype value_of_the_expression;
	int err, reloc;
	int i;
	int length_of_the_expression;

	phaseoffset = 0;
	init_input_system();
	strcpy(name, original_name);
	while(1)
	{
		if(get_line() == EOF)
			break; /* Finish if end of file! */
		/* The line is in cline. */
		first_unused_character = cline;
		getsymbol();
		if(symbol == DEFMACRO)
		{
			/* Since macrodef does not work up nested macros,
			 there must not be symbol == DEFMACRO when a macro
			 is currently being extended. */
			if(mslast /* cline contains a line from a macro. */)
				error("006 internal error!", INTERNAL);
			if(pass == 1)/* macrodef() return 1 if reaches the EOF */
			{
				if(macrodef())
				{
					return;
				}
			}
			else
			{/* If pass == 2 then we eat the lines of the macro. */
				err = 0; /* Counting the nested macros. */
				/* Note, that nesting macros is invalid!   */
				while(1)
				{
					if(next_line() == EOF)
						return;
					first_unused_character = readinbuff;
					getsymbol();
					if(symbol == DEFMACRO)
						err++;
					if(symbol == ENDMACRO && !err--)
						break;
				}
			}
			continue;
		}

		if(symbol == DEFSET)
		{
			/* Since macrodef does not work up a macro
			 containing a set definition
			 there must not be symbol == DEFSET when a macro
			 is currently being extended. */
			if(mslast)
				error("007 internal error!", INTERNAL);
			if(pass == 1)/* In pass 2 we negligate every set definition. */
				build_up_a_set();
			continue;
		}

		if(symbol == VARSYMBOL)
		{
			if(pass == 1)/* In pass 2 we negligate every variable definition. */
				build_up_a_var_list(VARIABLE);
			continue;
		}

		split_the_line(cline, label, line);
		cut_comment(line);
		first_unused_character = line;
		getsymbol();
		if(symbol != DEFBYTE && symbol != DEFWORD && symbol != DEFDWORD && symbol != ASSIGN && symbol != SHARP
				&& ((macptr = ismacro(line, &sets_lexeme, 0)) || (macptr = ismacro(line, &sets_lexeme, 1))))
		{
			if(*label)
			{
				ptr = search_in_the_table(label);
				if(ptr->type_of_the_symbol != UNDEFINED && pass == 1)
				{
					sprintf(name, "Redefining the label %s.", ptr->name_of_the_symbol);
					error(name, NORMAL);
				}
				ptr->type_of_the_symbol = LABEL;
				if(pass == 1)
					ptr->value_of_the_symbol = *dollar;
				else
				{
					if(ptr->value_of_the_symbol - *dollar != phaseoffset)
					{
						phaseoffset = ptr->value_of_the_symbol - *dollar;
						sprintf(name, "Phase error between passes!\nThe %s symbol was %ld and now it is %ld!",
								ptr->name_of_the_symbol, ptr->value_of_the_symbol, *dollar);
						error(name, NORMAL);
					}
				}
				ptr->relocatable = TRUE;
			}
			push_macro(macptr, sets_lexeme);
			if(pass == 1)
				macptr->num1++;
			else if(pass == 2)
				macptr->num2++;
			else
				error("012 Internal error.", INTERNAL);
			macextnum++;
			continue;
		}
		if(line[0] == ':' && line[1] == '=')
		{
			if(!*label)
			{
				error("No variable whom assing the value to.", NORMAL);
				strcpy(label, "__DUMMY__");
				ptr = search_in_the_table(label);
				ptr->type_of_the_symbol = VARIABLE;
				ptr->public = 0;
			}
			if((ptr = search_in_the_table(label))->type_of_the_symbol == UNDEFINED)
			{
				sprintf(name, "Variable %s was not explicitly declared.", ptr->name_of_the_symbol);
				error(name, WARNING);
				ptr->type_of_the_symbol = VARIABLE;
				if(ptr->public)
					error("Variable can not be public.", NORMAL);
				ptr->public = 0;
			}
			if(ptr->type_of_the_symbol != VARIABLE)
				error("Assign for a non variable symbol!", NORMAL);
			length_of_the_expression = expression(&(line[2]), &value_of_the_expression, &err, &reloc) + 2;
			if(err)
			{
				if(err == UNDEFLABEL && pass == 1)
					error("Expression must be predefined.", NORMAL);
				else
					error("Sintax error in the expression.", NORMAL);
			}
			ptr->relocatable = reloc;
			ptr->value_of_the_symbol = value_of_the_expression;
			while(line[length_of_the_expression] && isspace(line[length_of_the_expression]))
				length_of_the_expression++;

			if(line[length_of_the_expression] && line[length_of_the_expression] != ';')
			{
				sprintf(name, "Unexpected characters after the expression (%s).", &(line[length_of_the_expression]));
				error(name, NORMAL);
			}
			continue;
		}/* End of assign value to a variable. */

		if((line[0] == 'e' || line[0] == 'E') && (line[1] == 'q' || line[1] == 'Q')
				&& (line[2] == 'u' || line[2] == 'U') && !isIDchar(line[3]))
		{
			if(!*label)
				error("No label whom assing the value to.", NORMAL);
			else if((ptr = search_in_the_table(label))->type_of_the_symbol != UNDEFINED && pass == 1)
				error("Redefining of a symbol", NORMAL);
			length_of_the_expression = expression(&(line[3]), &value_of_the_expression, &err, &reloc) + 3;
			if(err)
			{
				if(err == UNDEFLABEL && pass == 1)
					error("Expression must be predefined.", NORMAL);
				else
					error("Sintax error in the expression.", NORMAL);
			}
			ptr->relocatable = reloc;
			if(pass == 1)
				ptr->value_of_the_symbol = value_of_the_expression;
			else
			{
				if(ptr->value_of_the_symbol - value_of_the_expression != phaseoffset)
				{
					phaseoffset = ptr->value_of_the_symbol - value_of_the_expression;
					sprintf(name, "Phase error between passes!\nThe %s symbol was %ld and now it is %ld!",
							ptr->name_of_the_symbol, ptr->value_of_the_symbol, value_of_the_expression);
					error(name, NORMAL);
				}
			}
			ptr->type_of_the_symbol = LABEL;
			while(line[length_of_the_expression] && isspace(line[length_of_the_expression]))
				length_of_the_expression++;

			if(line[length_of_the_expression] && line[length_of_the_expression] != ';')
			{
				sprintf(name, "Unexpected characters after the expression (%s).", &(line[length_of_the_expression]));
				error(name, NORMAL);
			}
			if(!mslast || maclist)
			{/* If we are not in a macro. */
				/* Or macro listing is on.   */
				sprintf(name, "%0*lX", ADDRESS, value_of_the_expression);
				for(i = 0; i < ADDRESS; i++)
					listline[i] = name[i];
			}
			continue;
		}/* End of assign value to a label with EQU. */
		if(*label)
		{
			ptr = search_in_the_table(label);
			if(ptr->type_of_the_symbol != UNDEFINED && pass == 1)
				error("Redefining a label.", NORMAL);
			ptr->type_of_the_symbol = LABEL;
			if(pass == 1)
			{
				ptr->value_of_the_symbol = *dollar;
				/* This is used for the listing. */
				value_of_the_expression = *dollar;
			}
			else
			{
				if(ptr->value_of_the_symbol - *dollar != phaseoffset)
				{
					phaseoffset = ptr->value_of_the_symbol - *dollar;
					sprintf(name, "Phase error between passes!\nThe %s symbol was %ld and now it is %ld!",
							ptr->name_of_the_symbol, ptr->value_of_the_symbol, *dollar);
					error(name, NORMAL);
				}
			}
			ptr->relocatable = TRUE;
		}

		if(*line)/* If there is line at all. */
			do_primitive(line);
		else if(*label && (!mslast || maclist))
		{
			/* If there is no line but there is label. */
			sprintf(name, "%0*lX", ADDRESS, value_of_the_expression);
			for(i = 0; i < ADDRESS; i++)
				listline[i] = name[i];
		}

	}/* End of the infinite loop. */
	if(fly_over_the_lines && pass == 1)/* This switch must   */
		/* be off at the end of the assembly. */
		error("Unbalanced #ifs.", NORMAL);
}/* End of function dopas() */
//#undef __FUNC__
