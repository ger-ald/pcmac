#include <stdio.h>

void prtchar(int k){
	if( k>31 && k<='z')
	  printf("%c",k);
	else
	  printf(" ");
	}
main( argc, argv )
int argc;
char **argv;
{
	FILE *f;
	int c;
	char line[16];
	int j;
	unsigned long i=0;
	if( argc != 2 || *argv[1] == '?' ) {
		fprintf(stderr,"od makes a hexa dump.\n");
		fprintf(stderr,"Usage: od inputfilename\n");
		exit(1);
	}
	f = fopen(argv[1],"rb");
	if( !f ){
		fprintf(stderr,"No file.\n");
		exit(1);
	}
	c = getc(f);
	printf("\n00000000 ");
	while( c!=EOF ) {
		printf("%02X ",c);
		line[i&15]=c;
		i++;
		if( !(i%16) ){
		  for( j=0 ; j<16 ; j++ )prtchar(line[j]);
		  printf("\n%08lX ",i);
		  }
		c = getc(f);
	}
	printf("\n");
}
