#include <stdio.h>
#ifdef __MSDOS__
void exit(int);
#endif
#define fetch() (byte = getc( fp ))
#define fetcw() (byte = getc(fp),byte +=0x100*getc(fp))
#define fetcd() {l=getc(fp);\
		 byte_= getc(fp);l+=0x100L*byte_;\
		 byte_= getc(fp);l+=0x10000L*byte_;\
		 byte_= getc(fp);l+=0x1000000L*byte_;}
main( argc , argv ) int argc;
char **argv;
{
	FILE *fp;
	int byte,i,limit,byte_;
	long l;

	if( argc == 1 ) {
		fprintf(stderr,"odx object debugger.\n");
		fprintf(stderr,"Usage odx filaname\n");
		exit(1);
	}

#ifdef __MSDOS__
	fp = fopen( argv[1] , "rb" );
#else
	fp = fopen( argv[1] , "r" );
#endif

	if( !fp ){
		fprintf(stderr,"Unable to open the file.\n");
		exit(1);
	}

	byte = getc(fp);
	if( byte != 0x55 ){
		fprintf(stderr,"Bad object format!\n");
		fprintf(stderr,"First magic number missing.\n");
		exit(1);
	}
	byte = getc( fp );
	if( byte != 0xaa ){
		fprintf(stderr,"Bad object format!\n");
		fprintf(stderr,"Second magic number missing.\n");
		exit(1);
	}

	byte = getc( fp );
	while( byte != EOF ){
		switch( byte ){
		case 0x81:/* CODE */
			fetcw();
			limit = byte;
			printf("CODE\n");
			fetcd();
			printf("ADDRESS=%08lx\n",l);
			fetch();
			printf("Bytes No:%d",byte);
			if( limit != byte +5 ){
				fprintf(stderr,"Bad object format!\n");
				fprintf(stderr,"Number of the bytes, and the length of the block are inconsistent.\n");
				fprintf(stderr,"Block length=%d , Number of bytes = %d\n", limit, byte);
				fprintf(stderr,"The difference is not 5.\n");
				exit(1);
			}
			limit = byte;
			for( i = 0 ; i < limit ; i++ ){
				fetch();
				if( !(i%8) )printf("\n");
				printf("%02X ",byte);
				if( byte>' ' && byte<128 )
					printf("%c,",byte);
				else
					printf(" ,");
			}
			printf("\n");
			fetch();/*CHKSUM*/
			break;
		case 0x82:/*RELOC */
			fetcw();
			limit = byte;
			printf("RELOC\n");
			fetch();
			printf("No:%d\n",byte);
			if( limit != byte*5+1 ){
				fprintf(stderr,"Bad object format!\n");
				fprintf(stderr,"Number of the bytes, and the length of the block are inconsistent.\n");
				fprintf(stderr,"Block length=%d , Number of bytes = %d\n", limit, byte);
				fprintf(stderr," %d != %d*5+1\n",limit,byte);
				exit(1);
			}
			limit = byte;
			for( i = 0 ; i<limit ; i++ ){
				fetch();
				if( byte == 0 )
					printf("WORD  ");
				else
					if( byte == 0xf0 )
						printf("DWORD ");
					else{
						fprintf(stderr,"Bad object format!\n");
						fprintf(stderr,"Unknown relocation type:%02X\n",byte);
						exit(1);
					}
				fetcd();
				printf("%08lX\n",l);
			}
			fetch();/*CHKSUM*/
			break;
		case 0x83:/* PUB  */
			fetcw();
			limit = byte;
			printf("PUB\n");
			fetch();
			limit--;
			if( byte == 0 )
				printf("N ");
			else
				if( byte == 0xf0 )
					printf("R ");
				else{
					fprintf(stderr,"Bad object format!\n");
					fprintf(stderr,"Public relocation information is wrong.\n");
					fprintf(stderr,"%02X\n",byte);
					exit(1);
				}

			fetch();
			limit --;
			while( byte ){
				printf("%c",byte);
				fetch();
				limit--;
			}
			if( limit != 4 ){
				fprintf(stderr,"Bad object format!\n");
				fprintf(stderr,"Number of the bytes, and the length of the block are inconsistent.\n");
				fprintf(stderr,"Block length=%d != 4\n", limit);
				exit(1);
			}
			fetcd();
			printf("  %08X\n",l);
			fetch();/*CHKSUM*/
			break;
		case 0x84:/*EXTDEF*/
			fetcw();
			limit = byte;
			printf("EXTDEF\n");
			fetch();
			limit --;
			while( byte ){
				printf("%c",byte);
				fetch();
				limit --;
			}
			if( limit != 2 ){
				fprintf(stderr,"Bad object format!\n");
				fprintf(stderr,"Number of the bytes, and the length of the block are inconsistent.\n");
				fprintf(stderr,"Block length=%d != 2\n", limit);
				exit(1);
			}
			fetcw();
			printf("  No:%d\n",byte);
			fetch();/*CHKSUM*/
			break;
		case 0x85:/*EXTREF*/
			fetcw();
			limit = byte;
			printf("EXTREF\n");
			fetch();
			if( limit != 1+byte*7 ){
				fprintf(stderr,"Bad object format!\n");
				fprintf(stderr,"Number of the bytes, and the length of the block are inconsistent.\n");
				fprintf(stderr,"Block length=%d, Number of items=%d\n", limit,byte);
				fprintf(stderr,"%d != 1+%d*7\n",limit,byte);
				exit(1);
			}
			limit = byte;
			for( i = 0 ; i <limit ; i++ ){
				fetcw();
				printf("No:%d\n",byte);
				fetch();
				switch( byte ){
				case 1:
					printf("BYTE   ");
					break;
				case 2:
					printf("WORD   ");
					break;
				case 4:
					printf("DWORD  ");
					break;
				case 0xfe:
					printf("RBYTE  ");
					break;
				case 0xfd:
					printf("RWORD  ");
					break;
				case 0xfb:
					printf("RDWORD ");
					break;
				default:
					fprintf(stderr,"Bad object format!\n");
					fprintf(stderr,"Unknown external type: %02X\n", byte);
					exit(1);
				}
				fetcd();
				printf("%08X\n",l);
			}
			fetch();/*CHKSUM*/
			break;
		default:
			fprintf(stderr,"Bad object format!\n");
			fprintf(stderr,"Unknown block type: %02X\n",byte);
			exit(1);
		}
		fetch();
	}

}
