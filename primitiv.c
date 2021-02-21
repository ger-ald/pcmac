#include "makeflag.h"

#if MAKE
#include "include.h"
#include "defines.h"
#include "functype.h"
#include "types.h"
#include "vars.h"
#include "tfunc.h"
#endif

/* These variables were moved out the function do_primitive, because
 * TURBO C++ could not compile it (out of memory.)
 *  04-16-93
 */
static char *s;
static int err,reloc;
static int i;
static valtype value,_value;
static struct loopstack *lsptr;
static struct symbol *symptr;
static FILE *charfile;
#ifdef __MSDOS__
void errorcheck(int err);
int kbhit(void);
int getch(void);
#else
void errorcheck();
#endif
#define STEP while( isalpha( *s ) ) s++
#define CONON if( !fly_over_the_lines )
static void func_error(){
		CONON{
		STEP;
		while( *s && isspace( *s ) ) s++;
		if( *s != '\"' )
			error("\" missing after the \'error\' directive.",NORMAL);
		else {
			for( i = 0 , s++ ; *s && *s != '\"' ; i++, s++ )
				if( *s == '\\' && s[1] ) {
					*s++;
					name[i] = *s;
					}
				else
					name[i] = *s;
			if( *s != '\"' )
				error("\" missing after the message in an \'error\' directive.",NORMAL);
			name[i]=0;
			error( name , NORMAL );
			}
		}/* End of CONON */
}
static void func_warning(){
		CONON{
		STEP;
		while( *s && isspace( *s ) ) s++;
		if( *s != '\"' )
			error("\" missing after the \'warning\' directive.",NORMAL);
		else {
			for( i = 0 , s++ ; *s && *s != '\"' ; i++, s++ )
				if( *s == '\\' && s[1] ) {
					*s++;
					name[i] = *s;
					}
				else
					name[i] = *s;
			if( *s != '\"' )
				error("\" missing after the message in an \'warning\' directive.",NORMAL);
			name[i]=0;
			error( name , WARNING );
			}
		}/*End of CONON */
          }
static void func_fatal(){
		CONON{
		STEP;
		while( *s && isspace( *s ) ) s++;
		if( *s != '\"' )
			error("\" missing after the \'fatal\' directive.",NORMAL);
		else {
			for( i = 0 , s++ ; *s && *s != '\"' ; i++, s++ )
				if( *s == '\\' && s[1] ) {
					*s++;
					name[i] = *s;
					}
				else
					name[i] = *s;
			if( *s != '\"' )
				error("\" missing after the message in an \'fatal\' directive.",NORMAL);
			name[i]=0;
			error( name , FATAL );
			}
		}/*End of CONON */
        }
static void func_message(){
		CONON{
		STEP;
		while( *s && isspace( *s ) ) s++;
		if( *s != '\"' )
			error("\" missing after the \'message\' directive.",NORMAL);
		else {
			for( i = 0 , s++ ; *s && *s != '\"' ; i++, s++ )
				if( *s == '\\' && s[1] ) {
					*s++;
					name[i] = *s;
					}
				else
					name[i] = *s;
			if( *s != '\"' )
				error("\" missing after the message in an \'message\' directive.",NORMAL);
			name[i]=0;
			error( name , MESSAGE );
			}
		}/* End of CONON. */
        }
static void func_pause(){
		CONON {
#ifdef  __MSDOS__
			while( kbhit() )getch();
			while( getch() != '\n' )
			;
#else
			getchar();
#endif
			}
        }
static void func_if(){
		STEP;
		s += expression( s , &value , &err , &reloc );
		errorcheck( err );
		if( value ) {/* #if 1 */
			if( fly_over_the_lines ) {
				offdeep++;
				}
			else {
				ondeep++;
				}
			}
		else {/* #if 0 */
			if( fly_over_the_lines ) {
				offdeep++;
				}
			else {
				offdeep = 0;
				fly_over_the_lines = 1;
				}
			}
		STEP;
		while( isspace( *s) ) s++;
		if( *s ) {
			sprintf(name, "Unexpected characters after the #if directive (%s).",s);
			error(name,NORMAL);
			}
       }
static void func_endif(){
		STEP;
		if( fly_over_the_lines ) {
			if( !(offdeep--) ) fly_over_the_lines = 0;
			}
		else {
			if( !(ondeep--) ) {
				error("Dangling #endif.",NORMAL);
				ondeep = 0;
				}
			}
       }
static void func_else(){
		STEP;
		if( fly_over_the_lines ) {
			if( !offdeep ) {
				fly_over_the_lines = 0;
				ondeep++;
				}
			}
		else {
			offdeep = 0;
			fly_over_the_lines = 1;
			}
       }
static void elsi_func(){
		if( fly_over_the_lines ) {
			if( !offdeep ) {
				fly_over_the_lines = 0;
				ondeep++;
				}
			}
		else {
			offdeep = 0;
			fly_over_the_lines = 1;
			}
		STEP;
		s += expression( s , &value , &err , &reloc );
		errorcheck( err );
		if( value ) {/* #if 1 */
			if( fly_over_the_lines ) {
				offdeep++;
				}
			}
		else {/* #if 0 */
			if( !fly_over_the_lines ) {
				offdeep = 0;
				fly_over_the_lines = 1;
				}
			}
		STEP;
		while( isspace( *s) ) s++;
		if( *s ) {
			sprintf(name, "Unexpected characters after the #if directive (%s).",s);
			error(name,NORMAL);
			}
        }
static void ifde_func(){
		STEP;
		first_unused_character = s;
		getsymbol();
		if( symbol != IDENTIFIER )
				error(" #ifdef needs an identifier.",NORMAL);
		if( search_in_the_table(name)->
			type_of_the_symbol != UNDEFINED ) {/* #if 1 */
			if( fly_over_the_lines ) {
				offdeep++;
				}
			else {
				ondeep++;
				}
			}
		else {/* #if 0 */
			if( fly_over_the_lines ) {
				offdeep++;
				}
			else {
				offdeep = 0;
				fly_over_the_lines = 1;
				}
			}
		while( isspace( *first_unused_character ))
					first_unused_character++;
		if( *first_unused_character ) {
			sprintf(name,
			"Unexpected characters after the #ifdef directive (%s).",
						       first_unused_character);
			error(name,NORMAL);
			}
        }
static void func_stack(){
	CONON{
		STEP;
		first_unused_character = s;
		getsymbol();
		if( symbol != IDENTIFIER )
				error("#stack needs an identifier.",NORMAL);
		if( (symptr=search_in_the_table(name))->
			type_of_the_symbol != VARIABLE ){
			if( symptr->type_of_the_symbol == UNDEFINED ){
				error("Variable was not explicitly declared.",WARNING);
				symptr->type_of_the_symbol = VARIABLE;
				if( symptr->public ){
					error("Variable can not be public.",NORMAL);
					symptr->public = 0;
					}
				}
			else {
				error("#stack needs a variable.",NORMAL);
				return;
				}
			}
		symptr->value_of_the_symbol = (valtype)newstack();
		if( symptr->value_of_the_symbol == USRSTKSIZ )
			error("No more stacks.",FATAL);
		}
        }
static void ifnde_func(){
		STEP;
		first_unused_character = s;
		getsymbol();
		if( symbol != IDENTIFIER )
				error(" #ifndef needs an identifier.",NORMAL);
		if( search_in_the_table(name)->
			type_of_the_symbol == UNDEFINED ) {/* #if 1 */
			if( fly_over_the_lines ) {
				offdeep++;
				}
			else {
				ondeep++;
				}
			}
		else {/* #if 0 */
			if( fly_over_the_lines ) {
				offdeep++;
				}
			else {
				offdeep = 0;
				fly_over_the_lines = 1;
				}
			}
		while( isspace( *first_unused_character ))
					first_unused_character++;
		if( *first_unused_character ) {
			sprintf(name,
			"Unexpected characters after the #ifndef directive (%s).",
						       first_unused_character);
			error(name,NORMAL);
			}
        }
static void func_while(){
		CONON {
		if( !mac_nest_deep ) error("Loop directive outside of a macro.",NORMAL);
		else {
		STEP;
		s += expression( s , &value , &err , &reloc );
		errorcheck( err );
		if( value ) {/* #while TRUE */
			lsptr = (struct loopstack *)malloc( sizeof(struct loopstack) );
			if( !lsptr )error("End of memory!",FATAL);
			lsptr->prev = loopstacklast;
			loopstacklast = lsptr;
			lsptr->flin = cmline;
			lsptr->m_n_d = mac_nest_deep;
			lsptr->ltype = WHILE;
			}
		else {/* #while FALSE	*/
		/* If the while's condition is false then we do not
		   turn off the compilation and let it to fly over the
		   lines, but explicitly step forward for the #wend */
			value = 0; /* Counting the nested while-wend loops. */
			for( ;; ) {
				if( !mslast->mlin ) {
					error("#while has no #wend in the macro.",NORMAL);
					break;
					}
		   		strcpy( cline , mslast->mlin->exl );
				for( i = 0 ; isspace(cline[i]) ; i++ )
						;
			        if( cline[i++] == '#' ) {
					if( check_directive( &(cline[i]) , "while" )) {
						value++;
						}
					else
					if( check_directive( &(cline[i]) , "wend"  )) {
						if( !value ) {
							mslast->mlin = mslast->mlin->nextexeline;
							break; /* Escape from for( ;; ) */
							}
						else value--;
						}
					}
				mslast->mlin = mslast->mlin->nextexeline;

				}/*End of for( ;; ) */
			}
		}
		}
        }
static void func_wend(){
		CONON {
		if( !mac_nest_deep ) error("Loop directive outside of a macro.",NORMAL);
		else {
		lsptr = loopstacklast;
		if( !lsptr || lsptr->ltype != WHILE ) error("#wend without #while.",NORMAL);
		else
		if( lsptr->m_n_d != mac_nest_deep ) error("#wend for an outer #while.",NORMAL);
		else {
			mslast->mlin = lsptr->flin;
			loopstacklast = lsptr->prev;
			free( lsptr );
		}
		}
		}
        }
static void func_repeat(){
		CONON {
		if( !mac_nest_deep ) error("Loop directive outside of a macro.",NORMAL);
		else {
			lsptr = (struct loopstack *)malloc( sizeof(struct loopstack) );
			if( !lsptr )error("End of memory!",FATAL);
			lsptr->prev = loopstacklast;
			loopstacklast = lsptr;
			lsptr->flin = cmline;
			lsptr->m_n_d = mac_nest_deep;
			lsptr->ltype = REPEAT;
		}
		}
        }
static void func_until(){
		CONON {
		if( !mac_nest_deep ) error("Loop directive outside of a macro.",NORMAL);
		else {
		lsptr = loopstacklast;
		if( !lsptr || lsptr->ltype != REPEAT ) error("#until without #repeat",NORMAL);
		else
		if( lsptr->m_n_d != mac_nest_deep ) error("#until for an outer #repeat.",NORMAL);
		else {
			STEP;
			s += expression( s , &value , &err , &reloc );
			errorcheck( err );
			if( !value )/* If the expression is not TRUE then not exit. */
				    /* (PASCAL style repeat-until) */
				mslast->mlin = lsptr->flin;
			/* Clear TOS. */
			loopstacklast = lsptr->prev;
			free( lsptr );
		}
		}
		}
        }
static void func_lib(){
	CONON{
		if( pass == 1 ) {
	    	STEP;
			while( isspace( *s )) s++;
			libfile = open_a_file_for_read( s , O_TEXT );
			read_library();
			}
		}
        }
static void func_octal(){
	CONON{
		STEP;
		while( isspace( *s ))s++;
		if( *s ) {
			sprintf(name,"Unexpected characters (%s) after #octal.",s);
			error(name,NORMAL);
			}
		asciiradix = 8;
		}
        }
static void func_decimal(){
	CONON{
		STEP;
		while( isspace( *s ))s++;
		if( *s ) {
			sprintf(name,"Unexpected characters (%s) after #decimal.",s);
			error(name,NORMAL);
			}
		asciiradix = 10;
		}
        }
static void func_dw(){
	CONON{
		STEP;
		while( isspace( *s ))s++;
		if( check_directive( s , "lh" ) ) dwflag=1;
		else
		if( check_directive( s , "hl" ) ) dwflag=0;
		else
		error("Unknown byte order in the #dw directive.",NORMAL);
		}
        }
static void func_dd(){
	CONON{
		STEP;
		while( isspace( *s ))s++;
		if( check_directive( s , "lh" ) ) ddflag=1;
		else
		if( check_directive( s , "hl" ) ) ddflag=0;
		else
		error("Unknown word order in the #dd directive.",NORMAL);
		}
        }
static void func_macnum(){
	CONON{
		STEP;
		expression( s , &value , &err , &reloc );
		if( reloc )
			error("Relocatable value for macro number.",NORMAL);
		if( err )
			error("Syntax error in the expression.",NORMAL);
		if( 33>value || value>127 )
			error("Non ascii character for macro number.",NORMAL);
		else
		macnum = value;
		}
        }
static void func_char(){
	CONON{
		STEP;
		while( isspace(*s) ) s++;
		if( *s == '\"' || *s == '<' ){
		/* We read the character table from file. */
			for( i=0 ; *s ; i++ , s++ )
				name[i] = *s;
			name[i] = '\0';
			if(charfile = fopen(name,"rb")){
				for( i = 0 ; i<256 ; i++ )
					char_code[i] = getc(charfile);
				fclose(charfile);
			}
			else
			error("Unable to open the character table file.",FATAL);
		}
		else{
			expression( s , &value , &err , &reloc );
			i = 1;
			if( reloc ){
				error("Relocatable value for ASCII code.",NORMAL);
				i = 0;
			}
			if( err ){
				error("Syntax error in the first expression.",NORMAL);
				i = 0;
			}
			if( value>255 || value<0 ){
				error("ASCII code out of range.",NORMAL);
				i = 0;
			}
			while( isspace( *first_unused_character ) )
				first_unused_character ++;
			if( symbol != COMMA ){
				error(", missing between the expresions.",NORMAL);
				i = 0;
			}
			/* if there is more line, and there was no error. */
			if( *first_unused_character && i ){
				_value = value;
				expression( first_unused_character , &value , &err , &reloc );
				i = 1;
				if( reloc ){
					error("Relocatable value for ASCII code.",NORMAL);
					i = 0;
				}
				if( err ){
					error("Syntax error in the first expression.",NORMAL);
					i = 0;
				}
				if( value>255 || value<0 ){
					error("ASCII code out of range.",NORMAL);
					i = 0;
				}
				if( i )
					char_code[_value] = value ;
		}
		}
		}
        }
static void func_macarg(){
	CONON{
		STEP;
		expression( s , &value , &err , &reloc );
		if( reloc )
			error("Relocatable value for macro prefix.",NORMAL);
		if( err )
			error("Syntax error in the expression.",NORMAL);
		if( 33>value || value>127 )
			error("Non ascii character for macro prefix.",NORMAL);
		else
		macarg = value;
		}
        }
static void func_pop(){
	CONON{
		STEP;
		expression( s , &value , &err , &reloc );
		if( reloc )
			error("Relocatable value for stack number.",NORMAL);
		if( err )
			error("Syntax error in the expression.",NORMAL);
		else
		usrpop( (int) value );
		}
        }
static void func_push(){
	CONON{
		STEP;
		expression( s , &value , &err , &reloc );
		if( reloc )
			error("Relocatable value for stack number.",NORMAL);
		if( symbol != COMMA )
			error(
			  ", expected after the stack number in the #push directive.",
				      NORMAL);
		if( err )
			error("Syntax error in the expression.",NORMAL);

		else{
		expression( first_unused_character , &_value , &err, &reloc );
		if( err == SERROR || err == RBRAMISSING )
			error("Syntax error in the expression.",NORMAL);
		usrpush( _value , value );
		}
		}
        }
static void func_clostak(){
	CONON{
		STEP;
		expression( s , &value , &err , &reloc );
		if( reloc )
			error("Relocatable value for stack number.",NORMAL);
		if( err )
			error("Syntax error in the expression.",NORMAL);
		releasestack( value );
		}
        }
static void func_list(){
	CONON{
		STEP;
		expression( s , &value , &err , &reloc );
		if( reloc )
			error("Relocatable value for list switching.",WARNING);
		if( err )
			error("Syntax error in the expression.",NORMAL);
		else
		if( glistswitch && value )
						(mlistswitch = 1),
						(listswitch = 1);
			else
						(mlistswitch = 0),
						(listswitch = 0);
		}
        }
static void func_ilist(){
	CONON{
		STEP;
		expression( s , &value , &err , &reloc );
		if( reloc )
			error("Relocatable value for list switching.",WARNING);
		if( err )
			error("Syntax error in the expression.",NORMAL);
		else
		if( glistswitch && value )
						(ilistswitch = 1);
			else
						(ilistswitch = 0);
		}
        }
#undef STEP
#undef CONON
static void func_direc(){
	s++; /* Step over the sharp character. */
	while( isspace( *s ) ) s ++; /* Step over the spaces */
	if( check_directive( s , "error" ) ) {
                func_error();
		}
	else
	if( check_directive( s , "warning") ) {
                func_warning();
		}
	else
	if( check_directive( s , "fatal"  ) ) {
                func_fatal();
		}
	else
	if( check_directive( s , "message" ) ) {
                func_message();
		}
	else
	if( check_directive( s , "pause" ) ) {
                func_pause();
		}
	else
	if( check_directive( s , "if" ) ) {
                func_if();
		}
	else
	if( check_directive( s , "endif" ) ) {
                func_endif();
		}
	else
	if( check_directive( s , "else" ) ) {
                func_else();
		}
	else
	if( check_directive( s , "elsif" ) ) {
                elsi_func();
		}
	else
	if( check_directive( s , "ifdef" ) ) {
                ifde_func();
		}
	else
	if( check_directive( s , "stack" )){
                func_stack();
		}
	else
	if( check_directive( s , "ifndef" ) ) {
                ifnde_func();
		}
	else
	if( check_directive( s , "while" ) ) {
                func_while();
		}
	else
	if( check_directive( s , "wend"  ) ) {
                func_wend();
		}
	else
	if( check_directive( s , "repeat") ) {
                func_repeat();
		}
	else
	if( check_directive( s , "until" ) ) {
                func_until();
		}
	else
	if( check_directive( s , "lib" ) ) {
                func_lib();
		}
	else
	if( check_directive( s , "octal" ) ) {
                func_octal();
		}
    else
	if( check_directive( s , "decimal" ) ) {
                func_decimal();
		}
	else
	if( check_directive( s , "dw") ){
                func_dw();
		}
	else
	if( check_directive( s , "dd") ){
                func_dd();
		}
	else
	if( check_directive( s , "macnum" ) ) {
                func_macnum();
		}
	else
	if( check_directive( s , "char") ){
                func_char();
		}
	else
	if( check_directive( s , "macarg" ) ) {
                func_macarg();
		}
	else
	if( check_directive( s , "pop" ) ){
                func_pop();
		}
	else
	if( check_directive( s , "push" )){
                func_push();
		}
	else
	if( check_directive( s , "clostack" )){
                func_clostak();
		}
	else
	if( check_directive( s , "list" ) ) {
                func_list();
		}
	else
	if( check_directive( s , "ilist" ) ) {
                func_ilist();
		}
	else
	if( !lmori ){
		/* Unknown directive. */
		while( isspace( *s ) ) s++;
		i=0;
		while( isalpha( s[i] ) ) i++;
		s[i] = '\0';
		sprintf( name , "Unknown directive (%s).",s);
		error( name , NORMAL );
		}
        }
static void func_def_byte(){
        do{
                 s = first_unused_character;
                       getsymbol();
                       while( isspace( *first_unused_character ))
                                       first_unused_character++;
                       if( symbol == IDENTIFIER
                         && (*first_unused_character == ',' ||
                                 !*first_unused_character  )
                         && (symptr=search_in_the_table(name))->
                         type_of_the_symbol == EXTERNAL){
                       if( objgen )
                               external_reference(
                                       symptr->value_of_the_symbol,DEFBYTE);
                       else
                               error(
                                       "Referencing external(w) while no -o option.",
                                                       NORMAL);
                       generate_byte( 0 );
                       getsymbol();
                       }
                       else{
               first_unused_character = s;
               while( isspace( *first_unused_character ) )
                               first_unused_character++;
               if( *first_unused_character == '\"' ) {/*string*/
                       first_unused_character++;
                       while( *first_unused_character &&
                                  *(first_unused_character) != '\"' ) {
                                  if( *first_unused_character == '\\' )  {
                                          first_unused_character++;
                                               if( !*first_unused_character )
                                                       break;
                                               switch( *first_unused_character )  {
                                                       case 'n':
                                                        generate_byte( '\n' );
                                                        first_unused_character++;
                                                        break;
                                                       case 't':
                                                        generate_byte( '\t' );
                                                        first_unused_character++;
                                                        break;
                                                       case 'r':
                                                        generate_byte( '\r' );
                                                        first_unused_character++;
                                                        break;
                                                       case '\"':
                                                        generate_byte( '\"' );
                                                        first_unused_character++;
                                                        break;
                                                       case '8':
                                                       case '9':
                                                        if( asciiradix == 8 )
                                                        error("Non octal character in string number.",WARNING);
                                                       case '0':
                                                       case '1':
                                                       case '2':
                                                       case '3':
                                                       case '4':
                                                       case '5':
                                                       case '6':
                                                       case '7':
                                                                       i = *(first_unused_character++) -'0';
                                                       if( !*first_unused_character ){
                                                               generate_byte(i);
                                                               break;
                                                               }
                                                                       if(   *first_unused_character<'0'
                                                                          || *first_unused_character>
                                                                                                  '0'+asciiradix-1){
                                                                          generate_byte(i);
                                                                          if( asciiradix == 8
                                                                              &&
                                                                          *first_unused_character>'7'
                                                                                  &&
                                                                          *first_unused_character<='9'
                                                                          )
                                                                          error(
                                                                          "Oktal number terminated by digit."
                                                                          ,WARNING);
                                                                                               break;
                                                                               }
                                                                       i = asciiradix*i +
                                                                            *(first_unused_character++)-'0';
                                                       if( !*first_unused_character ){
                                                               generate_byte(i);
                                                               if( asciiradix == 8
                                                                              &&
                                                                          *first_unused_character>'7'
                                                                                  &&
                                                                          *first_unused_character<='9'
                                                                          )
                                                                          error(
                                                                          "Oktal number terminated by digit."
                                                                          ,WARNING);
                                                               break;
                                                               }
                                                                       if(   *first_unused_character<'0'
                                                                          || *first_unused_character>
                                                                                                  '0'+asciiradix-1){
                                                                          generate_byte(i);
                                                                          if( asciiradix == 8
                                                                              &&
                                                                          *first_unused_character>'7'
                                                                                  &&
                                                                          *first_unused_character<='9'
                                                                          )
                                                                          error(
                                                                          "Oktal number terminated by digit."
                                                                          ,WARNING);
                                                                       break;
                                                                               }
                                                                       i = asciiradix*i +
                                                                            *(first_unused_character++)-'0';
                                                                       generate_byte(i);
                                                                       if( asciiradix == 8
                                                                              &&
                                                                       *first_unused_character>'7'
                                                                                  &&
                                                                       *first_unused_character<='9'
                                                                          )
                                                                       error(
                                                                        "Oktal number terminated by digit."
                                                                        ,WARNING);
                                                                       break;
                                                               default:
                                                                generate_byte(
                                                                 *first_unused_character++
                                                                 );
                                                                break;
                                                       }/* End of switch */
                                               }
                                  else {
                                  generate_byte( (int)char_code[*first_unused_character]);
                                  first_unused_character++;
                                  }
                               }/* End of the while */
                               if( *first_unused_character )/*step over the " */
                                       first_unused_character++;
                               else
                                       error("String not terminated.",WARNING);
                               getsymbol();/* And get the symbol. */
                               }/*End of string generating. */
               else {/* It must be an expression! */
                       expression( first_unused_character,
                       &value,
                       &err,
                       &reloc);
         if( reloc )
               error("Relocatable data for a byte.",WARNING);
         if( (value < -128 || value > 255) && pass == 2) {
               sprintf( name ,"Not 8 bit value(%lx) for a byte.",value);
               error(name,NORMAL);
               }
         if( err == RBRAMISSING )
               error("Right bracket missing in the expression.",NORMAL);
         else
         if( err == SERROR )
               error("Syntax error in the expression.",NORMAL);
         if( err == EXTIDTF )
               error("External in an expression.",NORMAL);
         if( pass == 2 && err == UNDEFLABEL )
               error("Undefined label in the expression.",NORMAL);
         generate_byte( (int) (value<0 ? value +256 : value ));
         }/* End of string<->expression separating if. */
         }/* End of external<->nonexternal separating if. */
         }while( symbol == COMMA );
       if( symbol != UNKNOWN )
               error("Unexpected characters on the end of the line.",NORMAL);
        }
/*
**
** Compiles the line 's' if that consists of a primitive line.
**
** Like db, or dw, or dd
**
*/
void do_primitive( ss )
char *ss;
{
s = ss;
if( *s == '#' ) {/* The line contains a directive. */
        func_direc();
	}
else {/* The line contains not a directive. */
	first_unused_character = s;
	getsymbol();
	switch( symbol ) {
/******************************************************************/
		case EXTERNAL:
					if( !objgen && !tskswitch )
						error("External with no -o option.",NORMAL);
					if( pass == 1 && !tskswitch )
						build_up_a_var_list( EXTERNAL );
					break;
/******************************************************************/
		case PUBLIC:
					if( !objgen && !tskswitch )
						error("Public with no -o option.",WARNING);
					if( pass != 1 || tskswitch ) break;
					do{
					getsymbol();
					if( symbol != IDENTIFIER )
						error("Identifier expected after public.",
								NORMAL);
					symptr = search_in_the_table( name );
					if( symptr->type_of_the_symbol == VARIABLE )
						error("Variable can not be public.",NORMAL);
					else
						symptr->public = 1;
					getsymbol();
					}while( symbol == COMMA );
					break;
/******************************************************************/
                case DEFBYTE: 
                        func_def_byte();
				break;
/******************************************************************/

		case DEFRBYTE:do{
						s = first_unused_character;
						getsymbol();
						while( isspace( *first_unused_character ))
								first_unused_character++;
						if( symbol == IDENTIFIER
						  && (*first_unused_character == ',' ||
							  !*first_unused_character         )
						  && (symptr=search_in_the_table(name))->
						  type_of_the_symbol == EXTERNAL){
						if( objgen )
							external_reference(
								symptr->value_of_the_symbol,DEFRBYTE);
						else
							error(
								"Referencing external(w) while no -o option.",
										NORMAL);
						generate_byte( 0 );
						getsymbol();
						}
						else{
						first_unused_character = s;
						expression( first_unused_character,
						&value,
						&err,
						&reloc);
				  if( !reloc && pass == 2 )
					/* In pass 1 can be undefined=>nonreloc */
						error("Non relocatable for byte offset!",WARNING);
				  value -= *dollar+1;
				  if( (value < -128 || value > 255) && pass == 2) {
					sprintf( name ,"Not 8 bit value(%lx) for a byte.",value);
					error(name,NORMAL);
					}
				  if( err == RBRAMISSING || err == SERROR )
					error("Syntax error in the expression.",NORMAL);
				  if( pass == 2 && err == UNDEFLABEL )
					error("Undefined label in the expression.",NORMAL);
				  generate_byte( (int)(value < 0 ? value + 256 : value) );
				  }
				  }while( symbol == COMMA );
				if( symbol != UNKNOWN )
					error("Unexpected characters on the end of the line.",
												NORMAL);
				break;
/******************************************************************/
		case DEFRWORD : do{
						s = first_unused_character;
						getsymbol();
						while( isspace( *first_unused_character ))
								first_unused_character++;
						if( symbol == IDENTIFIER
						  && (*first_unused_character == ',' ||
							  !*first_unused_character         )
						  && (symptr=search_in_the_table(name))->
						  type_of_the_symbol == EXTERNAL){
						if( objgen )
							external_reference(
								symptr->value_of_the_symbol,DEFRWORD);
						else
							error(
								"Referencing external(w) while no -o option.",
										NORMAL);
						generate_word( 0 );
						getsymbol();
						}
						else{
						first_unused_character = s;
						expression( first_unused_character,
						&value,
						&err,
						&reloc);
				  if( !reloc && pass == 2 )
					/* In pass 1 can be undefned =>nonreloc */
						error("Non relocatable for word offset!",WARNING);
				  value -= *dollar+2;
				  if( (value < -32768l || value > 65535l ) && pass == 2)
					error("Not 16 bit value for a word.",NORMAL);
				  if( err == RBRAMISSING || err == SERROR )
					error("Syntax error in the expression.",NORMAL);
				  if( pass == 2 && err == UNDEFLABEL )
					error("Undefined label in the expression.",NORMAL);
				  generate_word( (int)(value < 0 ? value + 65536l : value) );
				  }
				  }while( symbol == COMMA );
				if( symbol != UNKNOWN )
					error("Unexpected characters on the end of the line.",
												NORMAL);
				break;
/******************************************************************/
		case DEFWORD :  do{
						s = first_unused_character;
						getsymbol();
						while( isspace( *first_unused_character ))
								first_unused_character++;
						if( symbol == IDENTIFIER
						  && (*first_unused_character == ',' ||
							  !*first_unused_character         )
						  && (symptr=search_in_the_table(name))->
						  type_of_the_symbol == EXTERNAL){
						if( objgen )
							external_reference(
								symptr->value_of_the_symbol,DEFWORD);
						else
							error(
								"Referencing external(w) while no -o option.",
										NORMAL);
						generate_word( 0 );
						getsymbol();
						}
						else{
						first_unused_character = s;
						expression( first_unused_character,
						&value,
						&err,
						&reloc);
				  if( reloc )
					note_reloc( DEFWORD );
				  if( (value < -32768l || value > 65535l ) && pass == 2)
					error("Not 16 bit value for a word.",NORMAL);
				  if( err == RBRAMISSING || err == SERROR )
					error("Syntax error in the expression.",NORMAL);
				  if( pass == 2 && err == UNDEFLABEL )
					error("Undefined label in the expression.",NORMAL);
				  generate_word( (int)(value < 0 ? value + 65536l : value) );
				  }
				  }while( symbol == COMMA );
				if( symbol != UNKNOWN )
					error("Unexpected characters on the end of the line.",NORMAL);
				break;
/*******************************************************************/

		case DEFRDWORD :do{
						s = first_unused_character;
						getsymbol();
						while( isspace( *first_unused_character ))
								first_unused_character++;
						if( symbol == IDENTIFIER
						  && ( *first_unused_character == ',' ||
							  !*first_unused_character 		    )
						  && (symptr=search_in_the_table(name))->
						  type_of_the_symbol == EXTERNAL){
						if( objgen )
							external_reference(
								symptr->value_of_the_symbol,DEFRDWORD);
						else
							error(
								"Referencing external(d) while no -o option.",
										NORMAL);
						generate_dword( 0 );
						getsymbol();
						}
						else{
						first_unused_character = s;
						expression( first_unused_character,
						&value,
						&err,
						&reloc);
				  if( !reloc && pass == 2)
					/* In pass 1 can be undefined =>nonreloc */
						error("Non relocatable for dword offset!",
															WARNING);
				  if( err == RBRAMISSING || err == SERROR )
					error("Syntax error in the expression.",NORMAL);
				  if( pass == 2 && err == UNDEFLABEL )
					error("Undefined label in the expression.",NORMAL);
				  value -= *dollar+4;
				  generate_dword( value );
				  }
				  }while( symbol == COMMA );
				if( symbol != UNKNOWN )
					error("Unexpected characters on the end of the line.",NORMAL);
				break;
/******************************************************************/
		case DEFDWORD :  do{
						s = first_unused_character;
						getsymbol();
						while( isspace( *first_unused_character ))
								first_unused_character++;
						if( symbol == IDENTIFIER
						  && ( *first_unused_character == ',' ||
							  !*first_unused_character 		    )
						  && (symptr=search_in_the_table(name))->
						  type_of_the_symbol == EXTERNAL){
						if( objgen )
							external_reference(
								symptr->value_of_the_symbol,DEFDWORD);
						else
							error(
								"Referencing external(d) while no -o option.",
										NORMAL);
						generate_dword( 0 );
						getsymbol();
						}
						else{
						first_unused_character = s;
						expression( first_unused_character,
						&value,
						&err,
						&reloc);
				  if( reloc )
					note_reloc( DEFDWORD );
				  if( err == RBRAMISSING || err == SERROR )
					error("Syntax error in the expression.",NORMAL);
				  if( pass == 2 && err == UNDEFLABEL )
					error("Undefined label in the expression.",NORMAL);
				  generate_dword( value );
				  }
				  }while( symbol == COMMA );
				if( symbol != UNKNOWN )
					error("Unexpected characters on the end of the line.",NORMAL);
				break;
/******************************************************************/
		default: sprintf( name , "Syntax error ??%s??",s);
			 error( name ,NORMAL );
			 break;
		}
	}
}

static void errorcheck(err)int err;{
	switch( err ){
		case 0: return;/* No error. */
		case RBRAMISSING:
		case SERROR:
			error("Syntax error in the expression.",NORMAL);
			break;
		case UNDEFLABEL:
			error("Undefined label in the expression.",NORMAL);
			break;
		case EXTIDTF:
			error("External identifier in the expression.",NORMAL);
			break;
		default:
			error("016 Internal error.",INTERNAL);
		}
}
