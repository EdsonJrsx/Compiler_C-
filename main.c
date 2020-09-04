#include "globals.h"
#include "semtree.h"
#include "analyze.h"
#include "intercod.h"

int error = 0;

int main (int argc, char *argv[]){
    //declaração e atribuição dos apontadores dos arquivos fonte e arquivo objeto
    FILE *source = NULL;
    FILE *target = NULL;
    switch (argc){
    case 2:
        source = fopen(argv[1], "r");
        target = fopen("exec.bin", "w");
        yyin = source;       
    break;
    case 3:
        source = fopen(argv[1], "r");
        target = fopen(argv[2], "w");
        yyin = source; 
    break;
    default:
        printf("Quantidade de parametros errado! \n");
        printf("[Arquivo-Fonte] [Arquivo-Objeto]");
        return 0;
    }

    //ponteiro do tipo No da arvore
    Node semanticTree;
    //recebe a raiz da arvore sintatica construida pelo analsador sintatico.
    semanticTree = parser();

    if(semanticTree == NULL)
        printf("\nCompilacao terminada com erros.\n");
    else{
        //imprime arvore sintatica
        printTree(semanticTree);
        
        //constroi a tabela de simbolos e realiza a analise semantica
        buildSymtab(semanticTree, 1);

        biuldIntermediateCode(semanticTree);

        if(error == 1)  printf("\nCompilacao terminada com erros.\n");
        else            printf("\nCompilacao terminada com sucesso!\n");
    }
    
    return 0;
}