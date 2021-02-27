/*
 * file.h
 *
 *  Created on: 23 okt. 2014
 *      Author: gerald
 */

#ifndef FILE_H_
#define FILE_H_


//Linux, and just about every flavor of Unix for that matter, doesn't differentiate between binary and text files. ergo:
#ifndef O_TEXT
#define O_TEXT	0x4000
#endif
#ifndef O_BINARY
#define O_BINARY	0x8000
#endif


int next_line(void);
FILE *open_a_file_for_read(char *filename, int type);

#endif /* FILE_H_ */
