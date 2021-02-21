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
**
** This program generates a byte.
**
** Doesn't check wheather it is longer than 8 bits.
*/

void generate_byte( byte )
int byte;
{
	int i;
	static valtype predoll;/* The previous value of the dollar */


	byte &= BYTEMASK;
	if( first_flag )
		predoll = *dollar-1;
	if( first_flag && pass == 1 ) {
		dollmin = *dollar;
		dollmax = *dollar;
	}
	first_flag = 0;
	if( pass == 1 ) {
		if( *dollar < dollmin ) {
			dollmin = *dollar;
		}
		if( *dollar > dollmax ) {
			dollmax = *dollar;
		}
	}
	if( listswitch ) {
		/* nrprvl == number of printed values */
		if( nrprvl >= VALUESS || *dollar != predoll+1 ) {
			fprintf(listfile,"%s\n",listline);
			listline[ADDRESS+3*VALUESS+1] = '\0' ;
			strnset( listline , ' ' ,ADDRESS+3*VALUESS+1);
			nrprvl = 0;
		}
		if( !nrprvl ) {
			sprintf(name , "%0*X",ADDRESS, *dollar );
			for( i = 0 ; i<ADDRESS ; i ++ )
				listline[i] = name[i];
		}

		sprintf( name, "%02X", byte );
		for( i = 0 ; i < 2 ; i ++ )
			listline[ ADDRESS+1+3*nrprvl+i ] = name[i];
		nrprvl++;
	}/* End of if( listswitch ) */
	if( pass == 2 ) {
		if( *dollar>dollmax || *dollar<dollmin )
			error("Code generating error !",NORMAL);
		else
			if( objgen ){
				/* The maximal length is 255 because 256 == 0      */
				/* The linker can not handle such lenght. (Sorry.) */
				if( codepos == 255 || (codepos && *dollar != predoll+1) )
					flush_code();
				if( !codepos )
					fadblk =(unsigned long)*dollar;
				code[codepos++] = byte;
			}
			else
				code[*dollar-dollmin] = byte;
	}
	predoll = *dollar;
	(*dollar)++;
}

void generate_word( word )
int word;
{
	if( dwflag )generate_byte( word & INTMASK );

	generate_byte( (word>>BITSPERBYTE) & INTMASK );

	if( !dwflag )generate_byte( word & INTMASK );
}

void generate_dword( dword )
unsigned long dword;
{
	if( ddflag )generate_word( dword );

	generate_word( dword>>BITSPERINT );

	if( !ddflag )generate_word( dword );
}

void note_reloc( k )
int k;
{
	if( pass !=2 || !objgen )return;
		
	if( rebuffpos == 255 )
		/* The maximal legth is 255 because 256 == 0 */
		/* The linker can not handle 256 items long blocks. */
		/* Sorry. */
		flush_rebuff();
	if( k == DEFWORD )
		rebuff[rebuffpos].size = 0;
	else
		if( k == DEFDWORD )
			rebuff[rebuffpos].size = 0xf0;
		else
			error("014 internal error!",INTERNAL);
	rebuff[rebuffpos].address = *dollar;
	rebuffpos ++;
}
