/*
 * objmad.h
 *
 *  Created on: 23 okt. 2014
 *      Author: gerald
 */

#ifndef OBJMAD_H_
#define OBJMAD_H_

#include "defines.h"
#include "types.h"

char *split_long(unsigned long p);
void open_block(int k);
void close_block(void);
void external_reference(valtype idtf, int type);
void gextdef(void);
void gext_tree(struct symbol *j);
void flush_rebuff(void);
void flush_code(void);

#endif /* OBJMAD_H_ */
