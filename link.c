/*
 *
 *   This program is a linker for the PCMAC
 *   macro assemblers object file.
 *
 *   Written by: Peter Verhas  1988. july
 *
 *
 */
#include <stdio.h>
#include <ctype.h>

/*
** File opening constants.
*/
#ifdef __MSDOS__
#define WRITE "wb"
#define READ  "rb"
#else
#define WRITE "w"
#define READ  "r"
#endif
/*
** The block identifiers in the object file.
*/
#define CODE	0x81
#define RELOC	0x82
#define PUB	0x83
#define EXTDEF	0x84
#define EXTREF	0x85
/*
** The external type constants in the object file.
*/
#define BYTE	 1
#define WORD	 2
#define DWORD	 4
#define RBYTE	0xfe
#define RWORD	0xfd
#define RDWORD	0xfb
/*
** The constants that pas1 and pas2 return
*/
#define NOERR	0
#define BADF	1
/*
** The type of an identifier
** can be UNDEFINED if an external
** definition is given before the public
** defines the value of the identifier.
*/
#define UNDEFINED 0
#define DEFINED   1

#define EOMERR  { fprintf(stderr,"End of memory!\n"); exit(1); }
#define CHKERR  { fprintf(stderr,"Check summ error!\n");exit(1); }
/*
** HASH constant
*/
#define PRIME 211
/*
** The maximal length of an identifier.
*/
#define MAXIDTFLEN 100
/*
** Structure for an item in the symbol table.
*/
struct symbol {
	int   type_of_the_symbol;
	unsigned long value_of_the_symbol;
	char *name_of_the_symbol;
	int relocatable;
	struct symbol *small_son, *big_son;
} *hashtable[PRIME];
/*
** Structure for an item in the relocation array
*/
struct rl {
	/* Type can be word or long. */
	int type;
	unsigned long address;
}			*reloc;
/*
** Structure for an item of the external reference arrays.
*/
struct ext {
	/* The type can be byte, word, double word,
   and everyone absolute or relative.		*/
	int type;
	/* Every external definitions has an identifying number. */
	int idtfnum;
	/* The address, where the value is needed. */
	unsigned long address;
};
/*
** Structure for the information about a file
*/
struct fl {
/* Following the object code:
   where the code start (address of first byte)
   where the code ends  (address of last byte )
     [and not the first unused one!]
   Calculated value: dollstart where the code of
   the actual object will start in the tsk file.
*/
	unsigned long dollmin,dollmax,dollstart;
	/* A flag 1 while there was no code in the object file. */
	int firstflag;
	/* The number of the relocatable addresses. */
	unsigned long nreloc;
	/* The number of the public definitions. */
	unsigned long nrpub;
	/* The number of the external definitions. */
	unsigned long nredef;
	/* The number of the external references. */
	unsigned long nreref;
	/* Array for the external definitions. */
	/* This array is filled up following   */
	/* the given identifying numbers.      */
	struct symbol **extdef;
	/* Array for the external references.  */
	struct ext *extref;
};

/* Array for the public variables.
** This is a global variable, and
** not every file have such an array,
** because having read a file in the second pass
** and relocated the relocatable addresses and
** the publics we need not this array any longer.
** So we always have one publics array which stands
** always for the current file.
*/
struct symbol **public;
/* The name of the output file. */
char *outfilnam;
/* The base is given by the user in the command line. */
/* The default is zero */
unsigned long base;
/* An array for the code. */
unsigned char *code;
/* A help variable containing the name of an identifier. */
char name[MAXIDTFLEN+1];
/* The number of the errors.
** Since there are some errors which do not make
** the linker stop, finishing the work it has to
** know if there was any error. If there was it
** does not generate code.
*/
int nrerror=0;	/* Number of errors. */
/* The order how words and double words are
** treated in relocation and in filling in the
** places for the external. Normally both of them
** is zero. It means that the lower byte/word
** is on the lower anddress.
*/
int wordord,dwordord;

#ifdef __MSDOS__
/*
** Declare the functions.
*/
int strcmp(char *s1,char *s2);
struct symbol *search_in_the_table(char *s);
int hashpjw( char *s );
unsigned int getint(FILE *fp);
unsigned long getlong(FILE *fp );
void relext(struct fl *file);
void do_relocation(struct fl *file);
void pdw(unsigned long i,unsigned long j);
void pw(unsigned long i,unsigned long j);
unsigned long gdw(unsigned long i);
unsigned long gw(unsigned long i);
int pas2(FILE *fp,struct fl *file);
int pas1(FILE *fp,struct fl *file);

char *malloc(long l);
char *calloc(long l,long s);
void exit(int i);
void free(void *l);
char *strdup(char *s);
void usage(void);
#endif

main(argc,argv)int argc;char **argv;{
	unsigned int i;      /* Loop variable.                    */
	unsigned int nrofl;  /* The number of input files.        */
	unsigned int flidx;  /* File index.                       */
	FILE *fp;            /* We open only one file at a time.  */
	struct fl *file;/* Array for the information about files. */
	unsigned long csiz;  /* Code size.                        */
/*
** It is nice to declare every function we use.
*/
	int pas1(),pas2();
	char *malloc();
	void do_relocation(),relext(),usage();

	/* Default output file name. */
	outfilnam = "a.tsk";
	/* Default orders. */
	wordord = 0;
	dwordord = 0;
	/* No files yet. */
	nrofl = 0;
	/* Default base. */
	base = 0L;
/*
** Usage
*/ if( argc == 1 || ( argc == 2 && !strcmp(argv[1],"help")))usage();

/*
** Pass 0 only counting the file arguments.
*/
	for( i = 1 ; i<argc ; i++ )
		if( *(argv[i]) != '-' ) nrofl++;

	/* No use */
	if( nrofl == 0 ){
		fprintf(stderr,"No input file!\n");
		exit(1);
	}

	/* Creat some place for the array. */
	if( !(file = (struct fl *)malloc( sizeof( struct fl ) * nrofl ) ) )
		EOMERR;

/*
** HEADLINE
*/
	fprintf(stderr,"Versoft linker Version 1.0\n");
	fprintf(stderr,"Pass 1\n");

	flidx = 0; /* The file index. */
/*
** PASS 1
*/
	for( i = 1 ; i<argc ; i++ ){

		if( *(argv[i]) == '-' ){
			/* This is an option.	*/
			switch( argv[i][1] ){
			case 'B':/* Base address. */
				if( argv[i][2]=='x' || argv[i][2] == 'X' ){
					/* If the second character of the base is x */
					/* then it is given in hexadecimal form.    */
					register j = 3, c;
					base = 0;
					while( c = argv[i][j++] ){
						base *= 16;
#ifdef __MSDOS__
						c = tolower( c );
#else
						if( isupper( c ) )c = tolower( c );
#endif
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
					base = 0 ;
					while( c = argv[i][j++] ){
						base *= 10;
						base += c - '0';
						if( !isdigit(c)){
							fprintf(stderr,"Misspelled base!\n");
							exit(1);
						}
					}
				}
				break;
			case 'n':
				outfilnam = &(argv[i][2]);
				break;
			case 'w':
				wordord = 1;
				break;
			case 'd':
				dwordord = 1;
				break;
				/*User can give also invalid options.(But musn't!)*/
			default:
				fprintf(stderr,"Invalid option %c.\n",argv[i][1]);
				exit(1);
			}
		}
		else{
			/* This is a file. (Or at least must be. )*/
			if( fp = fopen( argv[i] , READ )){
				file[flidx].nreloc = 0L;
				file[flidx].nrpub  = 0L;
				file[flidx].nredef = 0L;
				file[flidx].nreref = 0L;
				file[flidx].firstflag = 1;
				switch( pas1( fp , &(file[flidx++]) ) ){
					/* pas1 can return two different values. */
				case NOERR:/*No error.*/
					break;
				case BADF:/*Bad format.*/
					fprintf(stderr,"Bad object format.\n");
					exit(1);
				default :
					fprintf(stderr,"Internal linker error.\n");
					exit(2);
				}
				fclose(fp);
			}
			else{
				fprintf(stderr,"I can not open the file %s.\n", argv[i]);
				exit(1);
			}
		}
	}
/*
** Calculate the code size and
** dollstart for every file.
*/
	csiz = base;/* We will subtract it at the end. */
	for( i = 0 ; i<flidx ; i++ ){
		file[i].dollstart = csiz;
		/* We must give plus one because dollmax is the last byte. */
		/* [and not the first unused] */
		csiz += 1+file[i].dollmax - file[i].dollmin;
	}
	/* We dont use the space from address 0 to the start of the program */
	csiz -= base;
	if( csiz == 0 ){
		fprintf(stderr,"Zero code error.\n");
		exit(1);
	}
	/* Make room for the code. */
	if(!(code = malloc( csiz * sizeof( char ) )))
		EOMERR;
/**
*** PASS 2
**/
	fprintf(stderr,"Pass 2.\n");

	flidx = 0;
	for( i = 1 ; i<argc ; i++ ){

		if( *(argv[i]) != '-' ){/* We are not interested in the
						   options any longer. */
			/* This is a file. */
			if( fp = fopen( argv[i] , READ )){
				/* Make room for the relocation information. */
				if( file[flidx].nreloc ){
					reloc = (struct rl *)malloc
					    (sizeof( struct rl)*file[flidx].nreloc );
					if( !reloc )EOMERR;
				}
				/* Make room for the publics. */
				if( file[flidx].nrpub ){
					public = (struct symbol**)malloc
					    (sizeof(struct symbol *)*file[flidx].nrpub);
					if( !public )EOMERR;
				}

				switch(pas2( fp , &(file[flidx]) ) ){
				case NOERR:/*No error.*/
					break;
					/*  case BADF: the first pass ougth to have detected it! */
				default :
					fprintf(stderr,"Internal linker error.\n");
					exit(2);
				}
	/* Do the relocation for the relocatable bytes, and
                   the publics. [It also frees the arrays.] */
				do_relocation( &(file[flidx++]));
			}
			else{
				/* To get such an error would be very interesting. */
				/* [Since the file was open in the first pass. ]   */
				/* {Computers are magic. } 		           */
				fprintf(stderr,"I can not open the file %s.\n", argv[i]);
				exit(1);
			}
			fclose(fp);
		}
	}
/*
** END OF THE SECOND PASS.
*/

/* Fill the external references in all objects.
*/
	for( i = 0 ; i<nrofl ; i++ )
		relext( &(file[i]) );
	/* if any error happend. */
	if( nrerror ){
		fprintf(stderr,"No .tsk file.\n");
		exit(1);
	}

	/* Open a stream for the output file.  */
	fp = fopen( outfilnam , WRITE );
	if( !fp ){
		fprintf(stderr,"Unable to open the file %s.\n",outfilnam);
		exit(1);
	}
	/* Create the tsk file. */
	for( i=0 ; i<csiz ; i++ )
		fprintf( fp ,"%c",code[i] );
	fclose(fp);
				/* This file write out could be better
				   with a handler, open, write, and close. */
}/*End of main()*/


unsigned int chk;/* A variable containing the check summ. */


/**
*** This function gets a file opened, and a structure.
*** In this pass, this function negligates the most of
*** the information in the file, and creates only the
*** number of the: relocatable addresses,
*** 			   public symbols,
***				   external definitions,
***				   external references,
*** This also creates the dollmax and dollmin.
*** We check the check summs in this pass only.
**/
int pas1( fp , file) FILE *fp;struct fl *file;{

	unsigned int i,getint();
	unsigned long address,getlong();
	unsigned int blen;
	unsigned int h;

	/* Every object file starts with an 0xaa55 word. */
	/* [In normal order.]*/
	i = getc( fp );
	if( i != 0x55 )
		return BADF;
	i = getc( fp );
	if( i != 0xaa )
		return BADF;

	while( (h = getc(fp)) != EOF ){
		switch( h ){
		case CODE:
			blen = getint( fp );
			chk = 0;
			address=getlong(fp);
			if( file->firstflag ){
				file->firstflag = 0;
				file->dollmin = address;
				file->dollmax = address;
			}
			if( address<file->dollmin ) file->dollmin = address;
			if( address>file->dollmax ) file->dollmax = address;
			h = getc( fp );
			chk ^= h;
			if( blen != h+5 )return BADF;
			address += h-1;
			if( address<file->dollmin ) file->dollmin = address;
			if( address>file->dollmax ) file->dollmax = address;
			for( i = 0 ; i<h ; i++ )/* Eat the bytes. */
				chk ^= getc( fp );
			if( chk != getc( fp ) ) CHKERR;
			break;
		case RELOC:
			blen = getint( fp );
			chk = 0;
			h = getc( fp );
			chk ^= h;
			if( blen != 5*h+1 )return BADF;
			file->nreloc +=h;
			for( i = 0 ; i<h ; i++ ){/*Eat the items. */
				chk ^= getc( fp );/* Size */
				getlong( fp );/*Address (thrown away)*/
			}
			if( chk != getc( fp ))CHKERR;
			break;
		case PUB:
			file->nrpub++;
			blen = getint( fp );
			chk = 0;
			chk ^= getc( fp );/* Eat the relocation flag. */
			blen --;
			i = getc(fp);
			chk ^= i;
			blen--;
			while( i ){
				i = getc( fp );
				chk ^= i;
				blen -- ;
			}
			if( blen != 4 ) return BADF;
			getlong( fp );/* Value. */
			if( chk != getc( fp ))CHKERR;
			break;
		case EXTDEF:
			blen = getint( fp );
			chk = 0;
			i = getc( fp );
			chk ^= i;
			blen --;
			while( i ){
				i = getc( fp );
				chk ^= i;
				blen --;
			}
			if( blen != 2 ) return BADF;
			file->nredef++;
			getint( fp );/* idtf number */
			if( chk != getc( fp )) CHKERR;
			break;
		case EXTREF:
			blen = getint( fp );
			chk = 0;
			h = getc( fp ); /* Number of items. */
			chk ^= h;
			if( blen != 1+ 7*h )return BADF;
			file->nreref += h;
			for( i = 0 ; i<h ; i++ ){
				getint( fp );  /* Idtf number */
				chk ^= getc( fp );   /* type*/
				getlong( fp ); /* address */
			}
			if( chk != getc( fp )) CHKERR;
			break;
		default:
			return BADF;
		}
	}
	return NOERR;
}

/**
*** This function gets a file opened, and a structure.
*** In this pass, this function takes care of the most of
*** the information in the file.
**/
int pas2( fp , file) FILE *fp; struct fl *file;{

	unsigned int i,getint(),j,rlidx=0,puidx=0,eridx=0;
	char *malloc();
	char *calloc();
	unsigned long address,getlong();
	struct symbol *search_in_the_table(),*symptr;
	unsigned int blen;
	unsigned int h;

	if( file->nredef ){
		file->extdef = (struct symbol **)calloc
	    	( sizeof(struct symbol *),file->nredef);
		if( !file->extdef )EOMERR;
	}

	if( file->nreref ){
		file->extref = (struct ext *)malloc
	    	( sizeof(struct ext)*file->nreref );
		if( !file->extref )EOMERR;
	}


	i = getc( fp );
	if( i != 0x55 )
		return BADF;
	i = getc( fp );
	if( i != 0xaa )
		return BADF;

	while( (h = getc(fp)) != EOF ){
		switch( h ){
		case CODE:
			blen = getint( fp );
			address=getlong(fp);
			h = getc( fp );
			if( blen != h+5 )return BADF;
			address += file->dollstart - file->dollmin - base;
			for( i = 0 ; i<h ; i++ )
				code[ address++] = getc( fp );
			getc( fp );/* Eat the check summ. */
			break;
		case RELOC:
			blen = getint( fp );
			h = getc( fp );
			if( blen != 5*h+1 )return BADF;
			for( i = 0 ; i<h ; i++ ){
				reloc[rlidx].type = getc( fp );/* Size */
				if( reloc[rlidx].type != 0x00 &&
				    reloc[rlidx].type != 0xf0 )return BADF;
				reloc[rlidx++].address = getlong( fp );/*Address */
			}
			getc( fp );/* Eat the check summ. */
			break;
		case PUB:
			blen = getint( fp );
			h = getc( fp );
			blen --;
			j = 0;
			do{
				i = getc( fp );
				blen -- ;
				name[j++] = i;
			}while( i );
			symptr = search_in_the_table( name );
			if( symptr->type_of_the_symbol != UNDEFINED ){
				fprintf(stderr,"WARNING Symbol %s duplicated.\n",name);
				nrerror++;
				break;
			}
			symptr->type_of_the_symbol = DEFINED;
			if( h == 0xf0 )
				symptr->relocatable = 1;
			else
				symptr->relocatable = 0;
			if( blen != 4 ) return BADF;
			symptr->value_of_the_symbol = getlong( fp );
			getc( fp );/*check summ.*/
			public[puidx++] = symptr;
			break;
		case EXTDEF:
			blen = getint( fp );
			j = 0;
			do{
				i = getc( fp );
				name[j++] = i;
				blen --;
			}while( i );
			if( blen != 2 ) return BADF;
			symptr = search_in_the_table( name );
			h = getint( fp );
			if( file->nredef>h  )return BADF;
			if( file->extdef[h] )return BADF;
			file->extdef[h] = symptr;
			getc( fp );/*check summ. */
			break;
		case EXTREF:
			blen = getint( fp );
			h = getc( fp );
			if( blen != 1+ 7*h )return BADF;
			for( i = 0 ; i<h ; i++ ){
				file->extref[eridx].idtfnum   = getint ( fp );
				file->extref[eridx].type      = getc   ( fp );
				file->extref[eridx++].address = getlong( fp );
			}
			getc( fp );/* check summ */
			break;
		default: /* It ought to come in the first pass. */
			fprintf(stderr,"Internal linker error.\n");
			exit(2);
		}
	}
	return NOERR;
}

/*
** get word from the memory
** The argument is not address,
** but a normal array index.
*/
unsigned long gw(i)unsigned long i;{

	if( wordord ){
		return code[i+1] +
		    0x100*code[i];
	}
	else {
		return code[i] +
		    0x100*code[i+1];
	}
}

/*
** get double word from the memory
** The argument is not address,
** but a normal array index.
*/
unsigned long gdw(i)unsigned long i;{

	if( dwordord){
		return gw( i+2 ) + 0x10000L * gw( i );
	}
	else{
		return gw( i ) + 0x10000L * gw( i+2 );
	}
}

/*
** put word into the memory.
** i is an index (not address)
** j is the value.
*/
void pw(i,j)unsigned long i,j;{

	if( wordord ){
		code[i+1] = j & 0xff;
		code[i] = (j>>8) & 0xff;
	}
	else {
		code[i] = j & 0xff;
		code[i+1] = (j>>8) & 0xff;
	}
}

/*
** put double word into the memory.
** i is an index (not address)
** j is the value.
*/
void pdw(i,j)unsigned long i,j;{

	if( dwordord ){
		pw(i+2,j&0xffff);
		pw( i , (j>>16)&0xffff );
	}
	else {
		pw(i,j&0xffff);
		pw( i+2 , (j>>16)&0xffff );
	}
}

/*
** Do the relocation in the file, that has been read in.
** When the code fragment has been relocated it frees the
** relocation array, so it need not, and should not be freed
** by the callee.
** The argument is an object file descriptor. (Not a standard file!)
*/
void do_relocation( file )struct fl *file;{
	unsigned int i;
	unsigned long hvar;
	unsigned long offset;

	offset = file->dollstart-file->dollmin ;
	for( i = 0 ; i<file->nreloc ; i++){
		if( reloc[i].type == 0x00 ){/*WORD*/
			hvar = gw( offset+reloc[i].address - base );
			hvar += offset;
			pw( offset+reloc[i].address  - base, hvar );
		}
		else{/*DWORD*/
			hvar = gdw(offset+reloc[i].address - base);
			hvar += offset;
			pdw(offset+reloc[i].address - base,hvar);
		}
	}
if( file->nreloc )
	free( reloc );

	for( i = 0 ; i<file->nrpub ; i++ )
		if( public[i]->relocatable )
			public[i]->value_of_the_symbol += offset;
if( file->nrpub )
	free( public );
}/* End of do_relocatin() */

void relext( file )struct fl *file;{
	unsigned int i;
	struct symbol *symptr;
	unsigned long rbase,raddr,value,offset;
	unsigned long rval;

	rbase = file->dollstart - base;
	offset = file->dollstart - file->dollmin;

	for( i = 0 ; i<file->nreref ; i++ ){
		raddr = rbase + file->extref[i].address;
		symptr = file->extdef[file->extref[i].idtfnum];
		if( symptr->type_of_the_symbol == UNDEFINED ){
			fprintf(stderr,"Undefined external %s.\n",
			    symptr->name_of_the_symbol);
			nrerror++;
			continue;
		}
		value = symptr->value_of_the_symbol;
		rval = value - (file->extref[i].address + offset);
		switch( file->extref[i].type ){
		case BYTE  :
			code[raddr  ] = (char )(value    )&0xff;
			if( value&(~0xff) )
				fprintf(stderr,"Byte big.\n");
			break;
		case WORD  :
			pw( raddr , value );
			if( value&(~0xffff) )
				fprintf(stderr,"Word big.\n");
			break;
		case DWORD :
			pdw( raddr , value );
			break;
		case RBYTE :
			if( rval<0 )rval +=0x100;
			value =rval - 1;
			code[raddr  ] = (char )(value    )&0xff;
			if( value&(~0xff) )
				fprintf(stderr,"Rbyte too far.\n");
			break;
		case RWORD :
			if( rval<0 )rval +=0x10000L;
			value =rval - 2;
			pw( raddr , value );
			if( value&(~0xffff) )
				fprintf(stderr,"Rword too far.\n");
			break;
		case RDWORD:
			value =rval - 4;
			pdw( raddr , value );
			break;
		default:
			fprintf(stderr,"Internal linker error.\n");
			exit(2);
		}
	}
}/* End of relext() */

/*
** get a long(4bytes) number from a file
** and maintain the global chk variable to
** hold always the exor check sum of
** the read bytes.
*/
unsigned long getlong( fp ) FILE *fp;
{
	unsigned long l;
	unsigned int c;

	l = getc( fp );
	chk ^= l;
	c = getc( fp );
	chk ^= c;
	l += 0x100L  *  c;
	c = getc( fp );
	chk ^= c;
	l += 0x10000L * c;
	c = getc( fp );
	chk ^= c;
	l += 0x1000000L * c;

	return l;
}

/*
** get an integer (2bytes) number from a file
** and maintain the global chk variable to
** hold always the exor check sum of
** the read bytes.
*/
unsigned int getint( fp ) FILE *fp;
{
	unsigned int l;
	unsigned int c;

	l = getc( fp ) ;
	chk ^= l;
	c = getc( fp );
	chk ^= c;
	l += 0x100 * c ;

	return l;
}

/*
** HASH function
**
** I found this function on the page 436. of the dragon book.
**
** The dragon book:
** Aho-Sethi-Ulman : Compilers
**			Principles, techniques, and Tools
** Addison-Wesley Publishing Company 1986
**
** The only modification that was made by me is the letter 'L'
** following the 0xf0000000 constant for the specifying it long
** explicitly. ( The constant PRIME can be found in the file defines.h
** either, but the redefinition of it is identical. )
*/
#define MASK	0xf0000000L
int hashpjw(s)
char   *s;
{
	char *p;
	unsigned long h = 0, g;
	for ( p = s; *p != '\0'; p = p+1 ) {
		h = (h << 4) + (*p);
		if (g = h&MASK) {
			h = h ^ (g >> 24);
			h = h ^ g;
		}
	}
	return h % PRIME;
}

/*
** Search in the table.
**
** returns a pointer pointing to the searched item.
*/
struct symbol *search_in_the_table( s )
char *s;
{
	struct symbol **work_pointer;
	char *malloc();
	int  strcmp();
	char *strdup();
	unsigned int k;
	work_pointer = &(hashtable[ hashpjw( s ) ]);
	while( *work_pointer && (k=strcmp(s,(*work_pointer)->name_of_the_symbol)) ) {
		work_pointer= k > 0 ? &((*work_pointer)->big_son)  :
		    &((*work_pointer)->small_son);

	}
	if( *work_pointer ) return *work_pointer;
	*work_pointer = (struct symbol *) malloc( sizeof( struct symbol ));
	if( !*work_pointer ) {
		fprintf(stderr,"Not enough memory!\n");
		exit(1);
	}
	(*work_pointer)->name_of_the_symbol = strdup( s );
	if( !(*work_pointer)->name_of_the_symbol )
		EOMERR;
	(*work_pointer)->type_of_the_symbol = UNDEFINED;
	(*work_pointer)->value_of_the_symbol = 0;
	(*work_pointer)->big_son =
	    (*work_pointer)->small_son = NULL;
	return *work_pointer;
}

#ifndef __MSDOS__
char *strdup( s )char *s;
{
	char *p,*malloc();
	int  strlen();
	void strcpy();

	p = (char *)malloc( strlen( s )+1 );
	if( p )
		strcpy( p , s );
	return p;
}
#endif
void usage(){
	fprintf(stderr,"Usage: link [options] input_file_name_list\n");
	fprintf(stderr,
		 "Options: -B[x]nnnn base address. (if x then in hex.)\n");
	fprintf(stderr,"n&name change default (a.tsk) outfilename\n");
	fprintf(stderr,"w reverse word order.\n");
	fprintf(stderr,"d reverse long word order.\n");
	fprintf(stderr,"\nThe & means: write the name without space.\n");
	exit(1);
}
