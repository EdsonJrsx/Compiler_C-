#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semtree.h"

//declara a função de análise sintática
extern Node parser(void);

//declara os ponteiros para os arquivos fonte e arquivo objeto
extern FILE* source;
extern FILE* target;
extern FILE* yyin;

//declara a função de análise léxica e as variaveis globais alimentadas pela análise léxica.
extern int yylex(void);
extern int count_line;
extern char* id;
extern int num;
extern int error;
#endif