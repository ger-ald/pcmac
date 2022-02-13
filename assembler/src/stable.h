/*
 * stable.h
 *
 *  Created on: 23 okt. 2014
 *      Author: gerald
 */

#ifndef STABLE_H_
#define STABLE_H_

int machash(char *s, int k);
struct symbol *search_in_the_table(char *s);
void listsymbols(void);
void write_header(void);

#endif /* STABLE_H_ */
