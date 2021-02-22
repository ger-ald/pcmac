/*
 * stable.h
 *
 *  Created on: 23 okt. 2014
 *      Author: gerald
 */

#ifndef STABLE_H_
#define STABLE_H_

int hashpjw(char *s);
int machash(char *s, int k);
struct symbol *search_in_the_table(char *s);
void listsymbols(void);
void list_tree(struct symbol *j);
void write_header(void);
void write_tree(struct symbol *j);

#endif /* STABLE_H_ */
