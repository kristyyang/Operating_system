/* This file contains the prototypes and constants needed to use the
   routines defined in printRoutines.c
*/

#ifndef _PRINTROUTINES_H_
#define _PRINTROUTINES_H_

#include <stdio.h>

#define PRINTERROR -1
#define PRINTSUCCESS 0
typedef unsigned char byte;

int samplePrint(FILE *);
int writeFile(FILE *, long, byte *);

#endif /* PRINTROUTINES */
