/*
 * getline.h
 *
 *  Created on: 23 okt. 2014
 *      Author: gerald
 */

#ifndef GETLINE_H_
#define GETLINE_H_

//#include "defines.h"
#include "types.h"
//#include "vars.h"
//#include "tfunc.h"

int get_line(void);
void push_macro(struct macro *mcr, char **sets_lexeme);
void pop_macro(void);

#endif /* GETLINE_H_ */
