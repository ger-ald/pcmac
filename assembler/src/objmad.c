/*
 *
 * This file contains the functions and variables,
 * that are for making object file instead of
 * target.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>

#include "objmad.h"

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
#include "pass.h"
#include "primitiv.h"
#include "stable.h"
#include "tfunc.h"
#include "utilits.h"

/*
 **
 ** The format of the object file:
 **  Consists of blocks.
 **  Every block starts with an identifier byte.
 **  These are:
 **  81	LINKCODE
 **	82	LINKRELOC
 **	83	LINKPUB
 **	84	LINKEXTDEF
 **	85	LINKEXTREF
 **
 **	After it 2 bytes the length of the body of the block.
 **
 ** Block body.
 **
 ** 1 byte exor checksumm.
 **
 **
 ** The body of a block:
 ** CODE: 4ADDRESS 1NRofBYTES bytes
 ** RELOC: 1NRofITEMS ITEMS( 1SIZE[0W F0D] 4ADDRESS)
 ** PUB: 1(0=NREL F0=REL) name \0 4VALUE
 ** EXTDEF: name \0 2IDTFNUM
 ** EXTREF: 1NRofITEMS ITEMS(2IDTFNUM 1TYPE[
 **         1BYTE 2WORD 4DWORD feRBYTE fdRWORD fbRDWORD] 4ADDRESS)
 **
 */

#define putC( x ) putc( (x) , linkfp)
#define fill(c)	(linkbuff[linkbuffpos++] = (c))

char *split_long(unsigned long p)
{
	static char k[4];
	int i;

	for(i = 0; i < 4; i++)
	{
		k[i] = p % 0x100;
		p /= 0x100;
	}
	return k;
}

/*
 ** Opens a block to write.
 */
void open_block(int k)
{

	if(linkbuffpos)
		error("014 Internal error.", INTERNAL);
	if(linkbloctype)
		error("015 Internal error.", INTERNAL);
	linkbloctype = k;
}

void close_block(void)
{
	int i;
	int chk;
	char *p;

	chk = 0;
	for(i = 0; i < linkbuffpos; i++)
		chk ^= linkbuff[i];
	putC(linkbloctype);
	p = split_long((valtype)linkbuffpos);
	putC(p[0]);
	putC(p[1]);
	for(i = 0; i < linkbuffpos; i++)
		putC(linkbuff[i]);
	putC(chk);

	linkbuffpos = 0;
	linkbloctype = 0;
}

void external_reference(valtype idtf, int type)
{
	char *p;

	if(type != DEFBYTE && type != DEFWORD && type != DEFDWORD && type != DEFRBYTE && type != DEFRWORD
			&& type != DEFRDWORD)
		error("015 Internal error", INTERNAL);
	if(pass == 2)
	{
		open_block(LINKEXTREF);
		fill(1); /*number of the items*/
		p = split_long(idtf);
		fill(p[0]);
		fill(p[1]);
		switch(type)
		{
			case DEFBYTE:
				fill(0x01);
				break;
			case DEFWORD:
				fill(0x02);
				break;
			case DEFDWORD:
				fill(0x04);
				break;
			case DEFRBYTE:
				fill(0xfe);
				break;
			case DEFRWORD:
				fill(0xfd);
				break;
			case DEFRDWORD:
				fill(0xfb);
				break;
		}
		p = split_long(*dollar);
		fill(p[0]);
		fill(p[1]);
		fill(p[2]);
		fill(p[3]);
		close_block();
	}
}

/*
 ** This function generates the
 ** EXTDEF and the PUB blocks in the object file
 */
void gextdef(void)
{
	int i;
	for(i = 0; i < PRIME; i++) /* Go thru the hash table. */
		gext_tree(hashtable[i]);
}

/* Writes out the external and public symbols to the
 * object file from the tree j.
 */
void gext_tree(struct symbol *j)
{
	int i;
	char *p;

	if(j == NULL)
		return;
	gext_tree(j->small_son);
	gext_tree(j->big_son);
	if(j->type_of_the_symbol == EXTERNAL)
	{
		open_block( LINKEXTDEF);
		/* This block always has only one item. */
		for(i = 0; j->name_of_the_symbol[i]; i++)
			fill(j->name_of_the_symbol[i]);
		fill(0);/* Close the name*/
		p = split_long(j->value_of_the_symbol);
		fill(p[0]);
		fill(p[1]);
		close_block();
	}
	else if(j->public)
	{
		if(j->type_of_the_symbol == UNDEFINED)
		{
			error("Public symbol was not defined.", NORMAL);
			return;
		}
		open_block( LINKPUB);
		/* This block always has only one item.*/
		if(j->relocatable)
			fill(0xf0);
		else
			fill(0);
		for(i = 0; j->name_of_the_symbol[i]; i++)
			fill(j->name_of_the_symbol[i]);
		fill(0);/* Close the name*/
		p = split_long(j->value_of_the_symbol);
		fill(p[0]);
		fill(p[1]);
		fill(p[2]);
		fill(p[3]);
		close_block();

	}
}/* End of the function gext_tree()*/

void flush_rebuff(void)
{
	char *p;

	if(!rebuffpos)
		return;

	open_block( LINKRELOC);
	fill(rebuffpos);
	while(rebuffpos--)
	{
		fill(rebuff[rebuffpos].size);
		p = split_long(rebuff[rebuffpos].address);
		fill(p[0]);
		fill(p[1]);
		fill(p[2]);
		fill(p[3]);
	}
	close_block();
}
void flush_code(void)
{
	char *p;
	int i;

	if(!codepos)
		return;

	open_block( LINKCODE);
	p = split_long(fadblk);
	fill(p[0]);
	fill(p[1]);
	fill(p[2]);
	fill(p[3]);
	fill(codepos);
	for(i = 0; i < codepos; i++)
		fill(code[i]);
	close_block();
	codepos = 0;
}
