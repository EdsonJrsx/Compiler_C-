#include "semtree.h"
#include "globals.h"
#include "symboltab.h"
#include "analyze.h"
#include <string.h>

static int location = 0;        // Location

// Função de retorno nulo
static void nullFunc(Node AcNode){ return; }

// Mostra tipo de erro
static void typeError(Node AcNode, char *message){
    printf("ERRO SEMANTICO: '%s' LINHA: %d -> %s \n",AcNode->attr.name, AcNode->lin, message);
    error = 1;
}

// Função genérica de percurso na árvore
static void searchTree(Node AcNode, void (*preProc) (Node), void (*postProc) (Node)){ 
	if (AcNode != NULL){ 
		preProc(AcNode);
    	{
	  		int i;
	        for (i=0; i < MAXCHILDREN; i++)
        		searchTree(AcNode->child[i], preProc, postProc);
    	}
    	postProc(AcNode);
    	searchTree(AcNode->sibling, preProc, postProc);
    }
}

// Verifica as condições de entrada baseada no seu tipo
static void insertNode(Node AcNode){
    
	switch (AcNode->nodekind){
        
        case expK: 
            
            switch (AcNode->kind.exp){ 
               
                // Tipo variavel 
                case id_varK:
                    // Verifica se a variável já foi declarada
                    if (stabSearch(AcNode->attr.name, AcNode->scope) == -1 && stabSearch(AcNode->attr.name, "globals") == -1)
                        typeError(AcNode, "Variavel nao declarada");
                    else{
                        if (stabSearch(AcNode->attr.name, "globals") == -1) // É global?
                            stabInsert(AcNode->attr.name, AcNode->scope, "variable", "integer", location++, AcNode->lin);
                        else //É local
                            stabInsert(AcNode->attr.name, "globals", "variable", "integer", location++, AcNode->lin);	
                    }
                break;
            
                // Tipo vetor
                case id_vecK:
                    // Verifica se o vetor já foi declarado
                    if (stabSearch(AcNode->attr.name, AcNode->scope) == -1 && stabSearch(AcNode->attr.name, "globals") == -1)
                        typeError(AcNode, "Variavel nao declarada");
                    else{ 
                        if (stabSearch(AcNode->attr.name, "globals") == -1) // É global?
                            stabInsert(AcNode->attr.name, AcNode->scope, "variable", "integer", location++, AcNode->lin);
                        else //É local
                            stabInsert(AcNode->attr.name, "globals", "variable", "integer", location++, AcNode->lin);	
                    }
                break;

                // Tipo função
                case id_funK:
                    // Verifica se a função já foi declarada
                    if (stabSearch(AcNode->attr.name, "globals") == -1){
                        typeError(AcNode, "Funcao nao declarada");
                    }
                    // Função tipo int ou void
                    else{
                        if(AcNode->kind.type == intK)
                            stabInsert(AcNode->attr.name, "globals", "function", "integer", location++, AcNode->lin);
                        else
                            stabInsert(AcNode->attr.name, "globals", "function", "void", location++, AcNode->lin);
                    }
                break;

                default:
                break;
            }
        break;

        case typeK:
            switch (AcNode->kind.type){
                
                // Tipo void
                case voidK:
                    // Verificação de declação vazia (função(void))
                    if (AcNode->child[0] != NULL){
                        // Declaração void necessita uma função
                        if (AcNode->child[0]->kind.type != id_funK){
                            typeError(AcNode->child[0], "Atribuicao de void invalida para a variavel");
                        }
                        // Verifica se existe uma função com o mesmo nome declarada
                        else{
                            if (stabSearch(AcNode->child[0]->attr.name, "globals") == -1)
                                stabInsert(AcNode->child[0]->attr.name, "globals", "function", "void", 0, -1);
                            else
                                typeError(AcNode->child[0], "Funcao ja declarada");
                        }
                    }
                break;
                
                //Tipo int
                case intK:
                    // Verifica se existe uma função com o mesmo nome declarada
                    if(AcNode->child[0]->kind.exp == id_funK){
                        if (stabSearch(AcNode->child[0]->attr.name, "globals") == -1)
                            stabInsert(AcNode->child[0]->attr.name, "globals", "function", "integer", 0, -1);
                        else
                            typeError(AcNode->child[0], "Funcao ja declarada");
                    }
                    // Verifica se existe uma variável/vetor com o mesmo nome e escopo declarada
                    else{
                        if (stabSearch(AcNode->child[0]->attr.name, AcNode->child[0]->scope) == -1 && stabSearch(AcNode->child[0]->attr.name, "globals") == -1)
                            if (stabSearch(AcNode->child[0]->attr.name, "globals") == -1)
                                stabInsert(AcNode->child[0]->attr.name, AcNode->child[0]->scope, "variable", "integer", location++, AcNode->lin);
                            else
                                stabInsert(AcNode->child[0]->attr.name, "globals", "variable", "integer", 0, -1);
                        else
                            typeError(AcNode->child[0], "Variavel ja declarada"); 
                    }
                break;
                
                default:
                break;
            }
        break;

        default:
        break;
    } 
}

// Verifica a checagem de nós após a construção da tabela
static void checkNode(Node AcNode){

    switch (AcNode->nodekind){
        case expK:
            switch (AcNode->kind.exp){ 
                // Tipo Operador
                case opK:
                    // Verifica se o primeiro operador é um retorno de função void
                    if(AcNode->child[0]->kind.exp == id_funK){  
                        if(stabSearch_type(AcNode->child[0]->attr.name, "globals") == "void"); 
                            typeError(AcNode->child[0], "Operacao com valor nao inteiro"); 
                    }
                    // Verifica se o segundo operador é um retorno de função void
                    if(AcNode->child[1] != NULL){                  
                        if(AcNode->child[1]->kind.exp == id_funK){
                            if(stabSearch_type(AcNode->child[1]->attr.name, "globals") == "void");
                                typeError(AcNode->child[1], "Operacao com valor nao inteiro");
                        }
                    }       
                break;

                // Tipo vetor
                case id_vecK:
                    // Verifica se o conteúdo do vetor é um retorno de função void
                    if(AcNode->child[0] != NULL)
                        if (AcNode->child[0]->nodekind == expK)
                            if(AcNode->child[0]->kind.exp == id_funK)
                                if(stabSearch_type(AcNode->child[0]->attr.name, "globals") == "void")
                                    typeError(AcNode->child[0], "Indice com valor nao inteiro");
                break;

                default:
                break;
            }
        break;

        case stmtK:
            switch (AcNode->kind.stmt){
                
                // Tipo condição
                case ifK:
                    // Verifica a condição do if é void
                    if (AcNode->child[0]->nodekind == expK)
                        if(AcNode->child[0]->kind.exp == id_funK)
                            if(stabSearch_type(AcNode->child[0]->attr.name, "globals") == "void")
                                typeError(AcNode->child[0], "Condicao com valor nao inteiro");  
                break;

                // Tipo de atribuição
                case assignK:
                    // Verifica se a atribuição é void
                    if (stabSearch_type(AcNode->child[0]->attr.name, "globals") == "void")
                        typeError(AcNode->child[0], "Atribuicao com valor nao inteiro");
                    // Verifica se o atribuído é void
                    if(AcNode->child[1]->kind.exp == id_funK)                        
                        if (stabSearch_type(AcNode->child[1]->attr.name, "globals") == "void")
                            typeError(AcNode->child[1], "Atribuicao com valor nao inteiro");
                    
                break;

                // Tipo retorno
                case returnK:
                    // Verifica se o retorno é void
                    if (AcNode->child[0]->nodekind == expK)
                        if(AcNode->child[0]->kind.exp == id_funK)
                            if(stabSearch_type(AcNode->child[0]->attr.name, "globals") == "void")
                                typeError(AcNode->child[0], "Retorno com valor nao inteiro");
                break;

                default:
                break;
            }
        break;

        default:
        break;
    }
}


// Constrói a tabela de símbolos
void buildSymtab(Node syntaxTree, int showSymboltab){ 
    
    // Declara as variáveis do compilador
    stabInsert("output", "globals", "function", "void", 0, 0);
    stabInsert("input", "globals", "function", "integer", 0, 0);
    
    // Verificação pré ordem da árvore síntática
    searchTree(syntaxTree, insertNode, nullFunc); 
    
    // Tem main?
    if(stabSearch("main", "globals") == -1){
        printf("ERRO SEMANTICO: 'main' não declarada\n");
        error = 1;
    }
    
    // Verificação pós ordem da árvore síntática
    searchTree(syntaxTree, nullFunc, checkNode);

    if (showSymboltab)
        printSymbolTab();
}