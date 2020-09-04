/****************************************************/
/* Arquivo: symtab.h                                */
/* Tabela de símbolos                               */
/*                                                  */
/* Nomes: Edson Junior Fontes Carvalho              */
/*        Lucas Fernando Alvarenga e Silva          */
/****************************************************/

#ifndef _SYMTAB_H_
#define _SYMTAB_H_

// Lista de nº linhas onde um elementa está no código
typedef struct LineListRec *LineList; 
typedef struct LineListRec{ 
    int nLinha;
    struct LineListRec *prox; 
}AuxLineList;

// Estrutura de elemento na tabela Hash
typedef struct BucketListRec *BucketList; 
typedef struct BucketListRec{
    char* name;
    char* scope;
    char* type;
    char* typeData; 
    int memloc; 
    LineList lines; 
    struct BucketListRec *prox;
 }AuxBucketList;

// Inserção de um bucket na tabela de símbolos
void stabInsert(char *name, char *scope, char *type, char *typeData, int memloc, int nLinha);

// Busca um elemento na tabela de símbolos retornando (-1->elemento não entrado / 1->elemento encontrado)
int stabSearch(char *name, char *scope);

// Busca um elemento na tabela de símbolos retornando tipo dele caso encontrado
char* stabSearch_type(char *name, char*scope);

// Imprime a tabela de símbolos
void printSymbolTab();

#endif
