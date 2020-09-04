/* Nomes:	Edson Junior Fontes Carvalho 		120362
		      Lucas Fernando Alvarenga e Silva	120857
*/

%{
#include "globals.h"
#include "parser.tab.h"

int count_line = 1;
int num = 0;
char * id = NULL;
%}

%option noyywrap

DIGITO [0-9]
LETRA [a-zA-Z]
%%
"/*" {  char c;
        int flag = 1;
        do {
          c = input();
          if (c == '\n') count_line++;
          while(c == '*') 
            c = input();
          if(c == '/') flag = 0;
        } while (flag);
      }	       
"else"                      return ELSE;
"if"                        return IF;
"int"                       return INT;
"return"                    return RETURN;
"void"                      return VOID;
"while"                     return WHILE;
"+"	                        return ADD;
"-"	                        return SUB;
"*"	                        return MUL;
"/"	                        return DIV;
=		                        return ASSIG;
;		                        return SEMICOL;
"("	                        return OPARENT;
")"	                        return CPARENT;
>                           return MORE;
>=                          return MOREEQ;
"<"                         return LESS;
"<="                        return LESSEQ;
"=="                        return EQUAL;
"!="                        return DIFF;
"["                         return OBRACKET;
"]"                         return CBRACKET;
"{"                         return OBRACE;
"}"                         return CBRACE;
","                         return COMMA;
[\n]                        count_line++;
[ \t]+                      {}
{DIGITO}+                   {num = atoi(yytext); return NUM;}
{LETRA}+                		{ int tam = strlen(yytext);
                              id = (char *) malloc (tam * sizeof(char));
                              strcpy(id, yytext);
                              return ID;}	
({LETRA}|{DIGITO})+	        {printf("ERRO LEXICO: %s LINHA %d\n", yytext, count_line); return ERROR;};
%%