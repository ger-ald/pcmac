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

void external_reference(valtype idtf, int type);
void gextdef(void);
void flush_rebuff(void);
void flush_code(void);

#endif /* OBJMAD_H_ */
