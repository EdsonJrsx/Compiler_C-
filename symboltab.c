#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symboltab.h"

#define SIZE 211                    // Tamanho da Tabela Hash
#define SHIFT 4                     // Deslocamento Hash

static BucketList hashTable[SIZE];

// Define a posição em que o bucket será alocado na HashTable
static int stabIndex(char *name, char *scope){

    int i = 0, aux = 0;

    while(name[i] != '\0'){ // Verifica pelo nome
        aux = ((aux << SHIFT) + name[i]) % SIZE;
        i++;
    }
    i=0;
    while (scope[i] != '\0'){ // Verifica pelo escopo
        aux = ((aux << SHIFT) + scope[i]) % SIZE;
        i++;
    }
    //printf("Pos: %d\n", aux);
    return aux;
}

// Inserção de um bucket na tabela de símbolos
void stabInsert(char *name, char *scope, char *type, char *typeData, int memloc, int nLinha){   
    
    int h = stabIndex(name, scope);
    BucketList bucketNode = hashTable[h];

    // Verifica se o bucket já existe
    while ((bucketNode != NULL) && (strcmp(name,bucketNode->name) != 0) && (strcmp(scope,bucketNode->scope) != 0))   
        bucketNode = bucketNode->prox;

    if (bucketNode == NULL) { // Bucket não existe, insere na tabela
        bucketNode = (BucketList) malloc(sizeof(AuxBucketList));
        
        bucketNode->name = name;
        bucketNode->scope = scope;
        bucketNode->type = type;
        bucketNode->typeData = typeData;
        bucketNode->memloc = memloc;
        bucketNode->lines = NULL;

        if (nLinha != -1){
            bucketNode->lines = (LineList) malloc(sizeof(AuxLineList));
            bucketNode->lines->nLinha = nLinha;
            bucketNode->lines->prox = NULL;
        }
        
        bucketNode->prox = hashTable[h];
        hashTable[h] = bucketNode;
    }
    else { // Bucket existe, acrescenta a linha no bucket respectivo
        LineList lineNode = bucketNode->lines;
        
        if(lineNode == NULL){
            bucketNode->lines = (LineList) malloc(sizeof(AuxLineList));
            bucketNode->lines->nLinha = nLinha;
            bucketNode->lines->prox = NULL;
        }
        else{
            while (lineNode->prox != NULL) 
            lineNode = lineNode->prox;
        
            lineNode->prox = (LineList) malloc(sizeof(struct LineListRec));
            lineNode->prox->nLinha = nLinha;
            lineNode->prox->prox = NULL;
        }
    }
} 

// Busca um elemento na tabela de símbolos retornando (-1->elemento não entrado / 1->elemento encontrado)
int stabSearch(char *name, char*scope){

    int h = stabIndex(name, scope);
    BucketList bucketNode = hashTable[h];
    
    while ((bucketNode != NULL) && ((strcmp(name,bucketNode->name) != 0) || (strcmp(scope,bucketNode->scope) != 0)))  //Acha um espaço vazio na memória
       bucketNode = bucketNode->prox;

    if (bucketNode == NULL)
        return -1;
    else
        return 1;
}

// Busca um elemento na tabela de símbolos retornando tipo dele caso encontrado
char* stabSearch_type(char *name, char *scope){

    int h = stabIndex(name, scope);
    BucketList bucketNode = hashTable[h];

    while ((bucketNode != NULL) && ((strcmp(name,bucketNode->name) != 0) || (strcmp(scope,bucketNode->scope) != 0)))  //Acha um espaço vazio na memória
        bucketNode = bucketNode->prox;
    
    if (bucketNode == NULL) 
        return "null";
    else
        return bucketNode->typeData;
}

// Imprime a tabela de Símbolos
void printSymbolTab(){

    int i;
    printf("------------------------------ TABELA DE SIMBOLOS ------------------------------\n");
    printf("   Scope         Name        Type      TypeData            Line Numbers         \n");
    printf("------------ ------------ ---------- ------------ ------------------------------\n");

    for (i=0;i<SIZE;++i){
        if (hashTable[i] != NULL){
            BucketList bucketNode = hashTable[i];
            while (bucketNode != NULL){
                LineList lineNode = bucketNode->lines;

                printf("%-12s ", bucketNode->scope);
                printf("%-12s ", bucketNode->name);
                printf("%-10s ", bucketNode->type);
                printf("%-12s ", bucketNode->typeData);
                while (lineNode != NULL){
                    printf("%2d ",lineNode->nLinha);
                    lineNode = lineNode->prox;
                }
                printf("\n");
                bucketNode = bucketNode->prox;
            }
        }
    }
}
