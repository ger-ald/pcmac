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
int isIDchar(char c);
int isIDalpha(char c);
valtype hex(int c);
valtype bin(int c);
valtype okt(int c);
valtype dec(int c);
void getsymbol(void);


#endif /* EXPRESS_H_ */
