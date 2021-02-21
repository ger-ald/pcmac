#include <stdio.h>
main()
{
FILE *fp;
int i;

fp = fopen("prt.tmp" , "w" );

for( i = 0 ; i <256 ; i++ ){
	fprintf(fp,"%d , ",i);
	if( !(i%8))fprintf(fp,"\n");
}
}