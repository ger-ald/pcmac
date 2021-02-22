/*
 * primitiv.h
 *
 *  Created on: 23 okt. 2014
 *      Author: gerald
 */

#ifndef PRIMITIV_H_
#define PRIMITIV_H_

void func_error(void);
void func_warning(void);
void func_fatal(void);
void func_message(void);
void func_pause(void);
void func_if(void);
void func_endif(void);
void func_else(void);
void elsi_func(void);
void ifde_func(void);
void func_stack(void);
void ifnde_func(void);
void func_while(void);
void func_wend(void);
void func_repeat(void);
void func_until(void);
void func_lib(void);
void func_octal(void);
void func_decimal(void);
void func_dw(void);
void func_dd(void);
void func_macnum(void);
void func_char(void);
void func_macarg(void);
void func_pop(void);
void func_push(void);
void func_clostak(void);
void func_list(void);
void func_ilist(void);
void func_direc(void);
void func_def_byte(void);
void do_primitive(char *ss);
void errorcheck(int err);

#endif /* PRIMITIV_H_ */
