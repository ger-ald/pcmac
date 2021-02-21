#include <stdio.h>
#include <ctype.h>
#define LINELEN 200

FILE *fin,*fout;
char line[LINELEN];
char *fuc;
int deep;
int instring;
int inchar;

main( argc, argv )
int argc;
char **argv;
{
	int c;
	int i;

	if( argc == 1 ) {
		fprintf(stderr,"Usage: extmaker inputfile outputfile\n");
		exit(1);
	}
	if( argc == 2 ) {
		fprintf(stderr,"Too few file definitions.\n");
		exit(1);
	}
	fin = fopen(argv[1],"r");
	fout = fopen( argv[2] ,"w");
	if( !fin ) {
		fprintf(stderr,"Unable to open the file%s\n",argv[1]);
		exit(1);
	}
	if( !fout ) {
		fprintf(stderr,"Unable to open the file %s\n",argv[2]);
		exit(1);
	}

	deep = 0;
	instring = 0;
	inchar = 0;
	while( get_a_line() != EOF ) {
		fuc = line;
		while( *fuc && isspace( *fuc ) ) fuc++;
		if( *fuc && *fuc != '#' )
			fprintf(fout,"extern %s\n",fuc);
	}
}

int get_a_line()
{
	int i;
	int c;

	c = Getc();
	if( c == EOF )
		return EOF;

	for( i = 0 ; i<LINELEN && c != '\n' ; i++ ){
		line[i] = c;
		c = Getc();
	}
	line[i] = '\0';
}

int Getc()
{
	char c;
	while(1) {
		c = getc( fin );
		if( c == '/' ) {
			c = getc(fin);
			if( c != '*' ) {
				ungetc( c, fin );
				return '/';
			}
			c = getc( fin );
			while(1)
				if( c == '*' ) {
					c = getc( fin );
					if ( c== '/' ) break;
				}
				else
					c = getc( fin );
			continue;
		}
		else {
			if( c == '{' && !instring && !inchar )deep++;
			if( c == '}' && !instring && !inchar )deep--;
			return c;
		}
	}
}
