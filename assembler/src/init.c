/*
 * init.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>

#include "init.h"

#include "defines.h"
#include "types.h"
#include "vars.h"

#include "error.h"
#include "express.h"
#include "file.h"
#include "generate.h"
#include "getline.h"
#include "libget.h"
#include "libmak.h"
#include "objmad.h"
#include "pass.h"
#include "primitiv.h"
#include "stable.h"
#include "tfunc.h"
#include "utilits.h"

static void init_symbol_table(void);;
static void init_file_system(void);


/*
 * INITIALISES THE SYMBOL TABLE
 */
static void init_symbol_table(void)
{
	int i;
	struct symbol *ptr;

	for(i = 0; i < PRIME; i++)
	{
		hashtable[i] = NULL;
		macroot[i] = NULL;
		maclast[i] = NULL;
	}

	/* Fill into the table the predefined symbols! */
	ptr = search_in_the_table("STRING");
	if(ptr->type_of_the_symbol != UNDEFINED)
		error("001 internal error!", INTERNAL);
	setptr000 = ptr;
	ptr->type_of_the_symbol = SET;
	ptr->value_of_the_symbol = 0; /* For library making. */
	ptr->stpointer = (struct set *)malloc(sizeof(struct set));
	if(!ptr->stpointer)
		error("End of memory!", FATAL);
	ptr->stpointer->settype = STRING;
	ptr->stpointer->members = NULL;

	ptr = search_in_the_table("NUMERIC");
	if(ptr->type_of_the_symbol != UNDEFINED)
		error("002 internal error!", INTERNAL);
	setptr001 = ptr;
	ptr->type_of_the_symbol = SET;
	ptr->value_of_the_symbol = 1;
	ptr->stpointer = (struct set *)malloc(sizeof(struct set));
	if(!ptr->stpointer)
		error("End of memory!", FATAL);
	ptr->stpointer->settype = NUMERICSET;
	ptr->stpointer->members = NULL;
	erroot = NULL;
	asciiradix = 8;
	dwflag = 1;
	ddflag = 1;
	macarg = '#';
	macnum = '@';
	nexternal = 0;
	listswitch = mlistswitch = glistswitch;

	ptr = search_in_the_table("LABEL");
	if(ptr->type_of_the_symbol != UNDEFINED)
		error("003 internal error!", INTERNAL);
	setptr002 = ptr;
	ptr->type_of_the_symbol = SET;
	ptr->value_of_the_symbol = 2;
	ptr->stpointer = (struct set *)malloc(sizeof(struct set));
	if(!ptr->stpointer)
		error("End of memory!", FATAL);
	ptr->stpointer->settype = LABELSET;
	ptr->stpointer->members = NULL;

	ptr = search_in_the_table("DUMMY_SET");
	if(ptr->type_of_the_symbol != UNDEFINED)
		error("009 internal error!", INTERNAL);
	setptr003 = ptr;
	ptr->type_of_the_symbol = SET;
	ptr->value_of_the_symbol = 3;
	ptr->stpointer = (struct set *)malloc(sizeof(struct set));
	if(!ptr->stpointer)
		error("End of memory!", FATAL);
	ptr->stpointer->settype = DUMMY_SET;
	ptr->stpointer->members = NULL;
	dummy_set = ptr;

	ptr = search_in_the_table(DOLLAR);
	if(ptr->type_of_the_symbol != UNDEFINED)
		error("007 internal error!", INTERNAL);
	ptr->type_of_the_symbol = VARIABLE;
	ptr->value_of_the_symbol = 0;
	ptr->relocatable = TRUE;
	dollar = &(ptr->value_of_the_symbol);

	ptr = search_in_the_table("line");
	if(ptr->type_of_the_symbol != UNDEFINED)
		error("008 internal error!", INTERNAL);
	ptr->type_of_the_symbol = VARIABLE;
	ptr->value_of_the_symbol = 0;
	ptr->relocatable = FALSE;
	symline = &(ptr->value_of_the_symbol);

	ptr = search_in_the_table("tline");
	if(ptr->type_of_the_symbol != UNDEFINED)
		error("011 internal error!", INTERNAL);
	ptr->type_of_the_symbol = VARIABLE;
	ptr->value_of_the_symbol = 0;
	ptr->relocatable = FALSE;
	symtline = &(ptr->value_of_the_symbol);

	ptr = search_in_the_table("pass");
	if(ptr->type_of_the_symbol != UNDEFINED)
		error("010 internal error!", INTERNAL);
	ptr->type_of_the_symbol = VARIABLE;
	ptr->value_of_the_symbol = 1;
	ptr->relocatable = FALSE;
	sympass = &(ptr->value_of_the_symbol);

	ptr = search_in_the_table("on");
	if(ptr->type_of_the_symbol != UNDEFINED)
		error("012 internal error!", INTERNAL);
	ptr->type_of_the_symbol = VARIABLE;
	ptr->value_of_the_symbol = 1;
	ptr->relocatable = FALSE;

	ptr = search_in_the_table("off");
	if(ptr->type_of_the_symbol != UNDEFINED)
		error("013 internal error!", INTERNAL);
	ptr->type_of_the_symbol = VARIABLE;
	ptr->value_of_the_symbol = 0;
	ptr->relocatable = FALSE;
}

static void init_file_system(void)
{
	/* We are on the highest level of macro nesting. */
	mslast = NULL;
}

void init_input_system(void)
{
	int i;
	/* No files opened. */
	for(i = 0; i < MAXINCNEST; i++)
		inputfile[i] = NULL;
	buffpos = 0;
	/* The line is empty. */
	readinbuff[0] = '\0';
}

/* Initialises the assembly system. */
void init_assembly_system(void)
{
	flusherr();
	init_symbol_table();
	init_input_system();
	init_file_system();
	fly_over_the_lines = 0;
	ondeep = 0;
	first_flag = 1;
	mac_nest_deep = 0;
	loopstacklast = NULL;
	*listline = '\0';
	Nrofthesets = 4;
}

void initolo(void)
{
	int i;
	for(i = 0; i < 0x100; i++)
		if(isupper(i))
			__tolower__[i] = i - 'A' + 'a';
		else
			__tolower__[i] = i;
}
