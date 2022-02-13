/*
 * express.h
 *
 *  Created on: 23 okt. 2014
 *      Author: gerald
 */

#ifndef EXPRESS_H_
#define EXPRESS_H_

#include "defines.h"

int expression(char *s, valtype *val, int *err, int *reloc);


int isIDchar(char c);
int isIDalpha(char c);
void getsymbol(void);


#endif /* EXPRESS_H_ */
