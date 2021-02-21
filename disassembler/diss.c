#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <alloc.h>

/*
 We allocate from the far heap.
 */
#define malloc farmalloc
#define free   farfree
#define calloc farcalloc

/*
 Attribute flags.
 */
#define M_DATA		001
#define M_CODE		002
#define M_LABEL		004
#define M_WORD		010
#define M_STRING	020

/*
 The maximal number of enter points.
 */
#define MAXENTER   64
/*
 The maximal length of a line.
 */
#define MAXLINLEN  100

/*
  Global variables.
 */
char name[MAXLINLEN];
char *line;
unsigned long fflag,fcontinue,sflag,scontinue;
unsigned char *_code;
unsigned char code();
unsigned int dis();
unsigned char *attr;
unsigned long dollar;
int speak;
int simple;
int lowercas;
unsigned int counter;
unsigned int base,enter[MAXENTER];
unsigned int enterindex;
unsigned long length;

/*

  Dissasembler.

  It reads a code file, and generates a Z80 dissassembled file.
  The usage of the program:
      diss  [-options] inputfilename
  The options :
  -s Speak.      If this option is given the program
	         will write out what it does.
  -l simpLe.     If this option is given the program
	         won't analyse the code, but makes a simple dissassembly.
  -c lower Case. If this option is given the generated text will be almost
                 all in lower case.
  -n&name        You can specify an output file name with this option.
                 The name of the file should be glued, and the full name
                 has to be given. No implicit extention is supported.
                 The default output is the standard output.
  -B[x]baddress  You can specify the BASE address of the code where it
                 was loaded in the memory. You can use a decimal form, like
                 -B32000 or hexadecimal form -Bx7d00. The hexadecimal
                 digits start with an any case 'x'. The default BASE address
                 is zero.
  -E[x]eaddress  You can specify the ENTER address of the code where the
                 first executable byte of the program is. It is useful only
                 when we don't use the -l option. You can use decimal or
                 hexadecimal form as well as in the -B option with the same
                 syntax.
  -Fdriverfile   You can specify a driver file.

  The program was written by Peter Verhas. 1988. summer
  No rigths are reserved. This program can be broadcasted in any form,
  can be changed, can be even removed from your system.
  It's a present to you, budd.

  Compile with TURBO C 1.5 or later.

*/
void
main( argc , argv )int argc;char **argv;{
	FILE *fp,*ofp=stdout;
	FILE *dfile = NULL;
	void analise();
	unsigned int j,i,dis();
	void usage();
	int readline();
	char *fetch_number();

	if( argc == 1 ) usage();

	fp         = NULL;
	enterindex = 0;
	speak      = 0;
	simple     = 0;
	base       = 0;
	lowercas   = 0;

	for( i = 1 ; i<argc ; i++ )
		if( argv[i][0] == '-' )
			switch( argv[i][1]) {
			case 's':
				speak = 1;
				break;
			case 'l':
				simple = 1;
				break;
			case 'c':
				lowercas = 1;
				break;
			case 'n':
				ofp = fopen( &(argv[i][2]) , "w");
				if( !ofp ){
					fprintf(stderr,"Unable to open the output.\n");
					exit(1);
				}
				break;
			case 'F' :
				dfile = fopen( &(argv[i][2]) , "r" );
				if( !dfile ){
					fprintf(stderr,"Unable to open the driverfile.\n");
					exit(1);
				}
				break;
			case 'B':/* Base address. */
				if( argv[i][2] && argv[i][2]=='x' || argv[i][2] == 'X' ){
					/* If the second character of the base is x */
					/* then it is given in hexadecimal form.    */
					register j = 3, c;
					base = 0;
					while( (c = argv[i][j++]) != 0 ){
						base *= 16;
						if( isupper( c ) )c = tolower( c );
						if( isdigit( c ) )
							base += c - '0';
						else
							base += c -'a'+10;
						if( !isxdigit( c )){
							fprintf(stderr,"Misspelled base!\n");
							exit(1);
						}
					}
				}
				else{
					/* Decimal form of the base. */
					register j = 2 ,c;
					if( !argv[i][2] ){
						fprintf(stderr,"Base missing.\n");
						exit(1);
					}
					base = 0 ;
					while( (c = argv[i][j++]) != 0 ){
						base *= 10;
						base += c - '0';
						if( !isdigit(c)){
							fprintf(stderr,"Misspelled base!\n");
							exit(1);
						}
					}
				}
				if( !enterindex )enter[enterindex++] = base;
				break;

			case 'E':/* Enter address. */
				if( enterindex > MAXENTER ){
					fprintf(stderr,"Too many enter address.\n");
					exit(1);
					}
				if( argv[i][2]=='x' || argv[i][2] == 'X' ){
					/* If the second character of the enter is x */
					/* then it is given in hexadecimal form.    */
					register j = 3, c;
					enter[enterindex] = 0;
					while( (c = argv[i][j++]) != 0 ){
						enter[enterindex] *= 16;
						if( isupper( c ) )c = tolower( c );
						if( isdigit( c ) )
							enter[enterindex] += c - '0';
						else
							enter[enterindex] += c -'a'+10;
						if( !isxdigit( c )){
							fprintf(stderr,"Misspelled enter!\n");
							exit(1);
						}
					}
				}
				else{
					/* Decimal form of the enter. */
					register j = 2 ,c;
					enter[enterindex] = 0 ;
					while( (c = argv[i][j++]) != 0 ){
						enter[enterindex] *= 10;
						enter[enterindex] += c - '0';
						if( !isdigit(c)){
							fprintf(stderr,"Misspelled enter!\n");
							exit(1);
						}
					}
				}
				enterindex ++;
				break;

			default:
				fprintf(stderr,"Invalid option.\n");
				exit(1);
			}
		else{
			if( fp ){
				fprintf(stderr,"Too many file name!\n");
				exit(1);
			}
			fp = fopen( argv[i] , "rb" );
			if( !fp ){
				fprintf(stderr,"Unable to open the input file.\n");
				exit(1);
			}
		}

	if( !fp ){
		fprintf(stderr,"No file.\n");
		exit(1);
	}
	fseek( fp , 0L , 2 );
	length = ftell( fp );
	rewind( fp );

	if( length>0x10000L ){
		fprintf(stderr,"File too long.\n");
		exit(1);
	}

	_code =(unsigned char *) malloc( length*sizeof(char) );
	if( !_code ){
		fprintf(stderr,"No memory for the file.\n");
		exit(1);
	}
	attr =(unsigned char *) calloc( length,sizeof(char) );
	if( !attr ){
		fprintf(stderr,"No memory for the file.\n");
		exit(1);
	}

	for( i = 0 ; i<length ; i++ )
		_code[i] = getc( fp );

	fclose( fp );
	if( dfile )
		for(;;){
			if( EOF == readline(dfile,name) )break;
			switch( *name ){
				case 'l' :
				case 'L' :
					line = fetch_number( &(name[1]) , &i );
					if( !*line ){
						if( i<length+base && i>=base )
							attr[i-base] |= M_STRING;
						break;
						}
					fetch_number( line , &j );
					for( ; i<j ; i++ )
						if( i<length+base && i>=base )
							attr[i-base] |= M_LABEL;
					break;
				case 's' :
				case 'S' :
					line = fetch_number( &(name[1]) , &i );
					if( !*line ){
						if( i<length+base && i>=base )
							attr[i-base] |= M_STRING;
						break;
						}
					fetch_number( line , &j );
					for( ; i<j ; i++ )
						if( i<length+base && i>=base )
							attr[i-base] |= M_STRING;
					break;

				}/* End switch() */
		}
	analise();

	for( dollar = base ; dollar<length+base ; dollar +=i ){
		if( !(attr[(int)dollar-base]&M_CODE)){
			int byte;
			byte = code(dollar);
			if( byte<0 )byte +=0x100;
			if( attr[(int)dollar-base] & M_LABEL )
				if( lowercas )
					fprintf(ofp,"l%04x\n",dollar);
				else
					fprintf(ofp,"L%04X\n",dollar);
			if( (attr[(int)dollar-base]&M_WORD) && (dollar-base+1<length) &&
			    !(attr[(int)dollar-base+1]&M_CODE) ){
				byte += 0x100*_code[(int)dollar-base+1];
				if( attr[(int)dollar-base+1]&M_LABEL)
					if( lowercas )
						fprintf(ofp,"l%04x\tEQU\t$+1\n",dollar+1);
					else
						fprintf(ofp,"L%04X\tEQU\t$+1\n",dollar+1);
				if( lowercas )
					fprintf(ofp,"\tDW\t0%04xh\n",byte);
				else
					fprintf(ofp,"\tDW\t0%04XH\n",byte);
				i = 2;
			}
			else{
				if( lowercas )
					fprintf(ofp,"\tdb\t0%02xh",byte);
				else
					fprintf(ofp,"\tDB\t0%02XH",byte);
				if( attr[(int)dollar-base]&M_WORD )
					fprintf(ofp,"\t;Word reference also");
				fprintf(ofp,"\n");
				i = 1;
			}
			continue;
		}
		i = dis();
		if( !i ){
			int byte;
			byte = code(dollar);
			if( byte<0 )byte +=0x100;
			if( attr[(int)dollar-base] & M_LABEL )
				if( lowercas )
					fprintf(ofp,"l%04x\n",dollar);
				else
					fprintf(ofp,"L%04X\n",dollar);
			fprintf(ofp,"\t");
			if( lowercas )
				fprintf(ofp,"db\t0%02xh",byte);
			else
				fprintf(ofp,"DB\t0%02XH",byte);
			fprintf(ofp,"\t;***Reached code!***\n");
			i = 1;
		}
		else {
			register j;
			j = i-1;
			do{
				if( attr[(int)dollar+j-base]&M_LABEL )
					if( j )
						if( lowercas )
							fprintf(ofp,"l%04x\tEQU\t$+%d\n",dollar+j,j);
						else
							fprintf(ofp,"L%04X\tEQU\t$+%d\n",dollar+j,j);
					else
						if( lowercas )
							fprintf(ofp,"l%04x\n",dollar);
						else
							fprintf(ofp,"L%04X\n",dollar);
				if( !j )break;
				j--;
			}while( 1 );
			fprintf(ofp,"\t");
			if( lowercas ){
				register j=0;
				for(;name[j];j++)
					if( isupper(name[j]) )name[j]=tolower(name[j]);
			}
			fprintf(ofp,"%s",name);
			if( attr[(int)dollar-base]&M_DATA )
				fprintf(ofp,"\t;***Code&Data***");
			fprintf(ofp,"\n");
			if( !fflag || ((fcontinue != dollar +i) && (scontinue != dollar+i)))
				fprintf( ofp,";---------------------------------\n");
		}
	}

}/*End of main() */

/*
 Return a string that can be treated as a number or a label.
 If there is any reference to that value, then it returns a
 label, otherwise a string containing a hexadecimal number.
 The returned string is static!
 */
char *label(k)unsigned int k;
{
	static char p[7];

	if( lowercas ){
		sprintf(&(p[1]),"%04x",k);
		*p = 'l';
	}
	else {
		sprintf(&(p[1]),"%04X",k);
		*p = 'L';
	}
	if( k<length+base && k>=base ){
		attr[k-base] |= M_LABEL;
		return p;
	}
	*p = '0';
	if( lowercas )
		p[5] = 'h';
	else
		p[5] = 'H';
	p[6] = '\0';
	return p;

}/*End of label() */

/*
 Return a label pointing to an address.
 d is the current address, and k is the 8 bit offset.
 The returned string is static!
 */
char *rlabel( k , d )unsigned int k,d;
{
	if( k>0x7f )k=k+d-0x100;
	else	k=k+d;
	return label( k );
}/*End of rlabel() */

/*
 Register the address as a possible address to continue.
 Treat d as the current address and k as an 8 bit offset.
 */
void rcont( k , d )unsigned int k,d;
{
	void cont();
	if( k>0x7f )k=k+d-0x100;
	else	k=k+d;
	cont( k );
}/*End of rcont() */

/*
 Register the address as a location
 with data reference from somewhere.
 */
void data( address )unsigned int address;
{
	if( base<=address && address<base+length )
		attr[address-base] |=M_DATA;
}
/*
 Register the address as a location
 with WORD reference from somewhere.
 */
void word( address )unsigned int address;
{
	if( base<=address && address<base+length )
		attr[address-base] |=M_WORD;
}

/*
 Register the address as a possible address to continue.
 */
void cont( address )unsigned int address;
{
	/* If the address is out of the fragment then do nothing. */
	if( address>=length+base || address<base )return;

	if( fflag ) (scontinue = address) ,
		    (sflag     = 1      );
	else	(fcontinue = address) ,
		(fflag	   = 1      );
}

/*
 Analize the code if not simLe,
 otherwise only labell().
 */
void analise(){
	void analize();
	void labell();
	int i;

	if( simple ) {
		labell();
	}
	else {
		if( speak ){
			counter = 0;
			fprintf(stderr,"\nFULL > READY\n%04X > %04X",length,counter);
		}
		for( i = 0 ; i<enterindex ; i++ )
		analize( enter[i] );
	}
}

/*
 Analize the code.
 */
void analize( address )unsigned int address;
{
	unsigned int i;
	for(;;){
		if( attr[address-base] & M_CODE )break;
		dollar = address;
		if( speak )
			fprintf(stderr,"\b\b\b\b%04X",counter++);
		fflag = 0;
		sflag = 0;
		i = dis();
		if( !i ){
			i = 1;
			fflag = 1;
			fcontinue = dollar+1;
		}
		i--;
		do{
			if( address+i-base <length )
				attr[address+i-base] |= M_CODE;
		}while( i-- );
		if( sflag ){
			address =(int) scontinue  ;
			analize( fcontinue );
			continue;
		}
		else
			address =(int) fcontinue;
		if( !fflag )break;
	}
	return;
}


/*
 Return the value of the location d.
 */
unsigned char code( d )unsigned long d;
{
	return _code[(int)d-base];
}

/*
 Do not analize, treat every byte as executable code,
 and register the references.
 */
void labell(){
	register unsigned int i,j;

	for( dollar = base ; dollar<length+base ; dollar +=i ){
		i = dis(); /* i = the length of the instruction in bytes. */
		for( j=i ; i && j ; j-- )attr[(int)dollar-base+j-1] |= M_CODE;
		i += !i;
	}
}

/*
 Read a line from the file.
 */
int readline(file,line)FILE *file;char *line;{
	register i,c;

	c = getc(file);
	if( c == EOF ) return EOF;
	ungetc( c , file );

	for( i = 0 ;
	     (c = getc(file)) != '\n' && c != EOF && i<MAXLINLEN ;
	     i++ )
		line[i] = c;
	if( i == MAXLINLEN ){
		fprintf(stderr,"Line too long in driverfile.\n");
		exit(1);
		}

	/* Cut off the spaces from the end of the line. */
	while( i>0 && isspace(line[i-1]) ) i--;

	/* Close the line. */
	line[i] = '\0';

	return !EOF ;
}

/*
 Return a number given in the line.
 */
char *fetch_number( line , k )char *line;int *k;{
	int i;

	/* Step over the spaces. */
	while( isspace( *line ) ) line ++ ;

	if( *line == 'x' || *line == 'X' ){
		i = 0 ;
		line ++;
		while( *line ){
			i *= 16 ;
			if( isdigit( *line ) )
					i += *line - '0';
			else
			if( isupper( *line ) )
					i += *line - 'A' + 10 ;
				else
					i += *line - 'a' + 10 ;
			}
			line ++;
		}
	else{
		i = 0;
		while( *line ) i = 10 * i + *(line++) - '0' ;
			}
	*k = i ;
	/* Step over the spaces. */
	while( isspace( *line ) ) line ++ ;
	return line;
}


#include "instr.c"


void usage(){
	fprintf(stderr,"Usage: diss [options] inputfilename\n");
fprintf(stderr,"  The options :\n");
fprintf(stderr,"  -s Speak.      If this option is given the program\n");
fprintf(stderr,"	         will write out what it does.\n");
fprintf(stderr,"  -l simpLe.     If this option is given the program\n");
fprintf(stderr,"	         won't analyse the code, but makes a simple dissassembly.\n");
fprintf(stderr,"  -c lower Case. If this option is given the generated text will be almost\n");
fprintf(stderr,"                 all in lower case.\n");
fprintf(stderr,"  -n&name        You can specify an output file name with this option.\n");
fprintf(stderr,"                 The name of the file should be glued, and the full name\n");
fprintf(stderr,"                 has to be given. No implicit extention is supported.\n");
fprintf(stderr,"                 The default output is the standard output.\n");
fprintf(stderr,"  -B[x]baddress  You can specify the BASE address of the code where it\n");
fprintf(stderr,"                 was loaded in the memory. You can use a decimal form, like\n");
fprintf(stderr,"                 -B32000 or hexadecimal form -Bx7d00. The hexadecimal\n");
fprintf(stderr,"                 digits start with an any case 'x'. The default BASE address\n");
fprintf(stderr,"                 is zero.\n");
fprintf(stderr,"  -E[x]eaddress  You can specify the ENTER address of the code where the\n");
fprintf(stderr,"                 first executable byte of the program is. It is useful only\n");
fprintf(stderr,"                 when we don't use the -l option. You can use decimal or\n");
fprintf(stderr,"                 hexadecimal form as well as in the -B option with the same\n");
fprintf(stderr,"                 syntax.\n");
	exit(1);
}
