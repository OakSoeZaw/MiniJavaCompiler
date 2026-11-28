#ifndef CODEGEN_H
#define CODEGEN_H
#include <stdio.h>
#include "ast.h"
int codegen(PROGRAM *p, FILE *out);  /* 0 ok, nonzero on error */
#endif