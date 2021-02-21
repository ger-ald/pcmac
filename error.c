/* ERROR.C

   Functions for error handling.

*/
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
** Sends an error message for the standard error.
**
** If the type of the error is FATAL or INTERNAL
** then aborts the assembly.
**
*/
void error(msg,type)
char *msg;
int type;
{
	struct errstack *ptr;
	char errbuf[100];
	char *rest;

	switch(type)
	{
	case INTERNAL: 
		sprintf(errbuf,"INTERNAL *** ");
		break;
	case NORMAL:   
		sprintf(errbuf,"ERROR ****** ");
		break;
	case FATAL:    
		sprintf(errbuf,"FATAL ****** ");
		break;
	case WARNING:  
		sprintf(errbuf,"WARNING **** ");
		break;
	case MESSAGE:  
		sprintf(errbuf,"MESSAGE **** ");
		break;
	default:
		fprintf(stderr,"INTERNAL ***");
		exit(1);
	}
	rest = errbuf;
	while( *rest ) rest++;
	sprintf(rest,"%s",msg);
	ptr =(struct errstack *)malloc( sizeof(struct errstack));
	ptr->errmes = strdup(errbuf);
	ptr->nexterr = erroot;
	erroot = ptr;
	switch(type)
	{
	case INTERNAL: 
		flusherr();
		exit(1);
	case NORMAL:
		nrerrors++;
		break;
	case FATAL: 
		flusherr();
		exit(1);
	case WARNING:
		nrwarnings++;
		break;
	case MESSAGE: 
		break;
	default:
		fprintf(stderr,"000 internal error.\n");
		exit(1);
	}
}
void flusherr()
{
	struct errstack *ptr;

	while( erroot ){
		fprintf(stderr,"%s\n",erroot->errmes);
		if( !stdoutsw )
			fprintf(listfile,"%s\n",erroot->errmes);
		ptr = erroot;
		erroot = erroot->nexterr;
		free( ptr );
	}
}
