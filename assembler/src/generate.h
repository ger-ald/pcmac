/*
 * generate.h
 *
 *  Created on: 23 okt. 2014
 *      Author: gerald
 */

#ifndef GENERATE_H_
#define GENERATE_H_

void generate_byte(int byte);
void generate_word(int word);
void generate_dword(unsigned long dword);
void note_reloc(int k);

#endif /* GENERATE_H_ */
