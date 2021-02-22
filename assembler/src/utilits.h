/*
 * utilits.h
 *
 *  Created on: 23 okt. 2014
 *      Author: gerald
 */

#ifndef UTILITS_H_
#define UTILITS_H_

#include "defines.h"

int cut_comment(char *s);
int check_directive(char *s, char *l);
void split_the_line(char *l, char *s, char *t);
void extend_line(char *line, char **setstrings);
void build_up_a_var_list(int k);
void build_up_a_set(void);
void check_sharps(char *s, int limit);
char *strccchr(char *s, char c1, char c2);
char *compress(char *s);
int newstack(void);
void releasestack(int k);
void usrpush(valtype v, int s);
valtype usrpop(int s);
valtype usrtos(int s);

#endif /* UTILITS_H_ */
