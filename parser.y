%{
#define YYPARSER

#include "globals.h"
#define YYSTYPE Node

Node savedTree;
int savedLine;

int yyerror(char * message);

//funções que ajustam a variavel escopo de cada nó
void fix_scope_aux(Node t, char *scope){
	if(t != NULL){

		t->scope = (char *) malloc (strlen(scope) * sizeof(char));
		strcpy(t->scope, scope);

		fix_scope_aux(t->child[0], scope);
		fix_scope_aux(t->child[1], scope);
		fix_scope_aux(t->child[2], scope);
		fix_scope_aux(t->sibling, scope);
	}
}

void fix_scope(Node t, char *scope){
	if(t != NULL){
		fix_scope_aux(t->child[0], scope);
		fix_scope_aux(t->child[1], scope);
		fix_scope_aux(t->child[2], scope);
		fix_scope_aux(t->sibling, scope);
	}
}

%}

%start programa
%token IF ELSE INT RETURN VOID WHILE 
%token ID NUM
%token ADD SUB MUL DIV ASSIG
%token SEMICOL COMMA OPARENT CPARENT OBRACE CBRACE OBRACKET CBRACKET MORE MOREEQ LESSEQ LESS EQUAL DIFF
%token ERROR
%%

programa : 	declaracao_lista	{savedTree = $1;}
	;

declaracao_lista : 	declaracao_lista declaracao 
						{Node t = $1;
                        if(t != NULL){
                            while(t->sibling != NULL) t = t->sibling;
                            t->sibling = $2;
                            $$ = $1;
                        }
                        else $$ = $2;}
	|	declaracao		{$$ = $1;}
	;

declaracao	:	var_declaracao 	{$$ = $1;}
	|	fun_declaracao			{$$ = $1; fix_scope($1->child[0], $1->child[0]->attr.name);}
	;

var_declaracao	:	tipo_especificador identifier_var SEMICOL
						{ $1->child[0] = $2;
						 $$ = $1;}
	|	tipo_especificador identifier_vec OBRACKET NUM CBRACKET SEMICOL
			{$4 = newExpNode(indexK); $4->attr.val = num;
			 $2->child[0] = $4;
			 $1->child[0] = $2;
			 $$ = $1;}
	;

tipo_especificador	:	INT	{$$ = newTypeNode(intK);}
	|	VOID				{$$ = newTypeNode(voidK);}
	;

fun_declaracao	:	tipo_especificador identifier_fun OPARENT params CPARENT composto_decl
						{$2->child[0] = $4;
						 $2->child[1] = $6;
						 $1->child[0] = $2;
						 $$ = $1;}
	;

identifier_fun 	:	ID	{$$ = newExpNode(id_funK); $$->attr.name = id;};
identifier_vec	:	ID 	{$$ = newExpNode(id_vecK); $$->attr.name = id;};
identifier_var	:	ID	{$$ = newExpNode(id_varK); $$->attr.name = id;};

params	:	param_lista	{$$ = $1;}
	|	VOID			{$1 = newTypeNode(voidK); $$ = $1;}
	;
			
param_lista :	param_lista COMMA param 	
					{Node t = $1;
                     if(t != NULL){
                        while(t->sibling != NULL)t = t->sibling;
                        t->sibling = $3;
                        $$ = $1;
                     }
                     else $$ = $3;}
	|	param		{$$ = $1;}
	;

param	:	tipo_especificador identifier_var
				{$1->child[0] = $2;
				 $$ = $1;}
	|	tipo_especificador identifier_vec OBRACKET CBRACKET
				{$1->child[0] = $2;
				 $$ = $1;}
	;

//voltar a fazer desta parte;
composto_decl	:	OBRACE local_declaracoes statement_lista CBRACE
					{Node t = $2;
                     if(t != NULL){
                        while(t->sibling != NULL)t = t->sibling;
                        t->sibling = $3;
                        $$ = $2;
                     }
                     else $$ = $3;}
	|	OBRACE local_declaracoes CBRACE 	{$$ = $2;}
	|	OBRACE statement_lista CBRACE		{$$ = $2;}
	|	OBRACE CBRACE
	;

local_declaracoes	:	local_declaracoes var_declaracao	
							{Node t = $1;
							if(t != NULL){
								while(t->sibling != NULL)t = t->sibling;
								t->sibling = $2;
								$$ = $1;
							}
							else $$ = $2;}
	|	var_declaracao {$$ = $1; savedLine = count_line;}
	;

statement_lista	:	statement_lista statement
							{Node t = $1;
							if(t != NULL){
								while(t->sibling != NULL)t = t->sibling;
								t->sibling = $2;
								$$ = $1;
							}
							else $$ = $2;}
	|	statement 	{$$ = $1;}
	;

statement :	expressao_decl	{$$ = $1;}
	|	composto_decl		{$$ = $1;}
	|	selecao_decl		{$$ = $1;}
	|	iteracao_decl		{$$ = $1;}
	|	retorno_decl		{$$ = $1;}
	;

expressao_decl	:	expressao SEMICOL {$$ = $1;}
	|	SEMICOL				
	;

selecao_decl	:	IF OPARENT expressao CPARENT statement
						{$1 = newStmtNode(ifK); 
						 $1->child[0] = $3;
						 $1->child[1] = $5;
						 $$ = $1;
						 savedLine = count_line;}
	|	IF OPARENT expressao CPARENT statement ELSE statement
			{$1 = newStmtNode(ifK); 
			 $1->child[0] = $3;
			 $1->child[1] = $5;
			 $1->child[2] = $7;
			 $$ = $1;
			 savedLine = count_line;}
	;

iteracao_decl	:	WHILE OPARENT expressao CPARENT statement 
						{$1 = newStmtNode(whileK); $1->lin = savedLine;
						 $1->child[0] = $3;
						 $1->child[1] = $5;
						 $$ = $1;}
	;

retorno_decl	:	RETURN SEMICOL	{$1 = newStmtNode(returnK); $$ = $1;}
	|	RETURN expressao SEMICOL	{$1 = newStmtNode(returnK); $1->child[0] = $2;
									 $$ = $1;}
	;

expressao	:	var ASSIG expressao	{$2 = newStmtNode(assignK);
									 $2->child[0] = $1;
									 $2->child[1] = $3;
									 $$ = $2;
									 savedLine = count_line;}
	|	simples_expressao			{$$ = $1; savedLine = count_line;}
	;

var 	:	identifier_var	{$$ = $1;}
	|	identifier_vec OBRACKET expressao CBRACKET	{$1->child[0] = $3;
										 $$ = $1;}
	;

simples_expressao	:	soma_expressao relacional soma_expressao
							{$2->child[0] = $1;
							 $2->child[1] = $3;
							 $$ = $2;}
	|	soma_expressao		{$$ = $1;}
	;

relacional	:	LESSEQ	{$$ = newExpNode(opK); $$->attr.op = LESSEQ;}
	|	LESS			{$$ = newExpNode(opK); $$->attr.op = LESS;}
	|	MORE 			{$$ = newExpNode(opK); $$->attr.op = MORE;}
	|	MOREEQ			{$$ = newExpNode(opK); $$->attr.op = MOREEQ;}
	|	EQUAL			{$$ = newExpNode(opK); $$->attr.op = EQUAL;}
	|	DIFF			{$$ = newExpNode(opK); $$->attr.op = DIFF;}
	;

soma_expressao	:	soma_expressao soma termo	{$2->child[0] = $1;
												 $2->child[1] = $3;
												 $$ = $2;}
	|	termo									{$$ = $1;}
	;

soma 	:	ADD 	{$$ = newExpNode(opK); $$->attr.op = ADD;}
	|	SUB 		{$$ = newExpNode(opK); $$->attr.op = SUB;}
	;

termo	:	termo mult fator 	{$2->child[0] = $1;
								 $2->child[1] = $3;
								 $$ = $2;}
	|	fator					{$$ = $1;}
	;

mult	:	MUL 	{$$ = newExpNode(opK); $$->attr.op = MUL;}
	| 	DIV			{$$ = newExpNode(opK); $$->attr.op = DIV;}
	;

fator	:	OPARENT expressao CPARENT 	{$$ = $2;}
	| 	var								{$$ = $1;}
	|	ativacao						{$$ = $1;}
	|	NUM								{$1 = newExpNode(constK); $1->attr.val = num;
										 $$ = $1;}
	;

ativacao	:	identifier_fun OPARENT args CPARENT	{$1->child[0] = $3;
										 $$ = $1;}
	|	identifier_fun OPARENT CPARENT {$$ = $1;}
	;

args	:	arg_lista	{$$ = $1;}
	;

arg_lista	:	arg_lista COMMA expressao
							{Node t = $1;
							if(t != NULL){
								while(t->sibling != NULL)t = t->sibling;
								t->sibling = $3;
								$$ = $1;
							}
							else $$ = $3;}
	|	expressao			{$$ = $1;}
	;
%%

//printar mensagem de erro quando acontecer erro sintatico
int yyerror(char * message)
{ printf("ERRO SINTATICO: ");
  printToken_inline(yychar);
  printf(" LINHA: %d\n", count_line);
  return 0;
}

//função que inicia a análise sintática
Node parser(){
	printf("Inicializando Compilação...\n");
	if (yyparse() == 0){
		return savedTree;
	} 
	else {
		return NULL;
	}
}
