/*
** Function type definitions.
*/
#ifdef __MSDOS__
int checkline
	( char *line,char *sintax_definition, struct set *sets[], char *member[],int k);
int serch_in_set
	(char *lexeme, struct set *set_to_search_in, char **giveback,int k);
struct macro *ismacro(char *s, char ***setcc, int k);
int macrodef(void);
void write_header( void );
void write_tree(struct symbol *j);
void error( char *msg, int type);
void flusherr(void);
void dopass(void);
int get_line(void);
void push_macro(struct macro *mcr, char **sets_lexeme);
void pop_macro(void);
void do_primitive( char *s );
void listmacros( void );
void listsymbols( void );
void list_tree( struct symbol *j );
int cut_comment( char *s );
int check_directive(char *s, char *l);
void split_the_line(char *l ,char *s ,char *t );
void extend_line(char *line , char **setstrings);
void build_up_a_set(void);
void build_up_a_var_list(int k);
void check_sharps(char *s, int limit);
char *compress( char *s );
int hashpjw(char *s);
struct symbol *search_in_the_table(char *s );
int next_line(void);
FILE *open_a_file_for_read(char *s,int type);
void init_symbol_table(void);
void init_input_system(void);
void init_assembly_system(void);
void init_file_system(void);
int expression(char *s, valtype *val, int *err, int *reloc);
void _expression(valtype *val);

valtype orexpression(void);
valtype andexpression(void);
valtype borexpression(void);
valtype xorexpression(void);
valtype bandexpression(void);
valtype neoreexpression(void);
valtype compareexpression(void);
valtype shiftexpression(void);
valtype addexpression(void);
valtype multexpression(void);
valtype tag(void);

int isIDchar( char c );
int isIDalpha( char c );
valtype hex( int c );
valtype bin( int c );
valtype okt( int c );
valtype dec( int c );
void getsymbol(void);


int strcmp(char *s1, char *s2);
int strlen( char *s );
void strcpy( char *s1, char *s2 );
void strncpy( char *s1, char *s2, int i );
void strnset( char *s , char c , int n);
void strcat( char *s1, char *s2 );
char *strdup(char *s );
void system( char *s );
void generate_byte( int byte );
void generate_word( int word );
void generate_dword( unsigned long dword );
void note_reloc( int k );
void write_library(void);
void libsetlis(struct symbol *l);
void read_library(void);
void getint(int *a );

char *strccchr( char *s, char c1, char c2);

void *malloc( int size );
void *calloc( int size, int items);
void *realloc( void *ptr, int size);
void *free(void *ptr);

valtype usrtos(int s);
valtype usrpop(int s);
void usrpush(valtype v,int s);
void releasestack(int k);
int newstack();

void gext_tree(struct symbol *j);
void getxdef(void);
void flush_rebuff(void);
void flush_code(void);
char *split_long( unsigned long k);
int machash( char *c,int k);
int newstack(void);
int stricmp(char *k1,char *k2);
void dopas(void);
void open_block(int k);
void close_block(void);
void exit(int i);
void gextdef(void);
void external_reference(valtype idtf,int type);
void initolo(void);
void write_version(void);
#else
/*
** Function type definitions, if this is not MSDOS system.
*/
extern int checkline();
extern int serch_in_set();
extern struct macro *ismacro();
extern int macrodef();
extern void write_header();
extern void write_tree();
extern void error();
extern void flusherr();
extern void dopass();
extern int get_line();
extern void push_macro();
extern void pop_macro();
extern void do_primitive();
extern void listmacros();
extern void listsymbols();
extern void list_tree();
extern int cut_comment();
extern int check_directive();
extern void split_the_line();
extern void extend_line();
extern void build_up_a_set();
extern void build_up_a_var_list();
extern void check_sharps();
extern char *compress();
extern int hashpjw();
extern struct symbol *search_in_the_table();
extern int next_line();
extern FILE *open_a_file_for_read();
extern void init_symbol_table();
extern void init_input_system();
extern void init_assembly_system();
extern void init_file_system();
extern int expression();
extern void _expression();

extern valtype orexpression();
extern valtype andexpression();
extern valtype borexpression();
extern valtype xorexpression();
extern valtype bandexpression();
extern valtype neoreexpression();
extern valtype compareexpression();
extern valtype shiftexpression();
extern valtype addexpression();
extern valtype multexpression();
extern valtype tag();

extern int isIDchar();
extern int isIDalpha();
extern valtype hex();
extern valtype bin();
extern valtype okt();
extern valtype dec();
extern void getsymbol();

extern int strcmp();
extern int strlen();
extern void strncpy();
extern void strnset() ;
extern void strcat( );
extern char *tfuncstrdup();
extern void system( );
extern int tfuncstricmp();

extern void generate_byte();
extern void generate_word();
extern void generate_dword();
extern void note_reloc();
extern void write_library();
extern void libsetlis();
extern void read_library();
extern void  getint();

extern void *malloc();
extern void *calloc();
extern void *realloc();
extern void *free();

extern char *strccchr();

valtype usrtos();
valtype usrpop();
void usrpush();
void releasestack();
int newstack();
void gext_tree();
void getxdef();
void flush_rebuff();
void flush_code();
char *split_long();
int machash();
int newstack();
int stricmp();
void dopas();
void open_block();
void close_block();
void exit();
void gextdef();
void external_reference();
void initolo();
void write_version();
#endif

