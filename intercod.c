#include "globals.h"
#include "semtree.h"
#include "analyze.h"
#include "parser.tab.h"

int TREG, TLAB;
char TXT_REG[10];
FILE *arq;

void biuldIntermediateCode(Node AcNode);
static void intermediateCode(Node AcNode, int goSibling);
static void icStmt(Node AcNode);
static void icExp(Node AcNode);
static void icType(Node AcNode);
static void writeIC(char func[], char atr1[], char atr2[], char atr3[]);

static char*  concatenate(const char *s1, const char *s2){
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

static void icStmt(Node AcNode){

    switch (AcNode->kind.stmt){
        case ifK:
            intermediateCode(AcNode->child[0], 1);
            
            writeIC("IF", concatenate("$t",itoa(TREG,TXT_REG,10)), concatenate("L",itoa(TLAB,TXT_REG,10)), "-"); 
            int AuxLBL = TLAB;

            intermediateCode(AcNode->child[1], 1);

            if(AcNode->child[2] == NULL) {
                writeIC("LBL", concatenate("L",itoa(AuxLBL,TXT_REG,10)), "-", "-");
            }
            else{
                TLAB++;
                writeIC("GOTO", concatenate("L",itoa(TLAB,TXT_REG,10)), "-", "-");
                writeIC("LBL", concatenate("L",itoa(AuxLBL,TXT_REG,10)), "-", "-"); 
                AuxLBL = TLAB;    
                intermediateCode(AcNode->child[2], 1);
                writeIC("LBL", concatenate("L",itoa(AuxLBL,TXT_REG,10)), "-", "-");
            }
            break;

        case whileK:
            TLAB++;
            int Aux2LBL = TLAB;
            writeIC("LBL", concatenate("L",itoa(TLAB,TXT_REG,10)), "-", "-");
            intermediateCode(AcNode->child[0], 1);
            TLAB++;
            writeIC("WIF",  concatenate("$t",itoa(TREG,TXT_REG,10)), "GOTO", concatenate("L",itoa(TLAB,TXT_REG,10)));
            intermediateCode(AcNode->child[1], 1);
            writeIC("GOTO", concatenate("L",itoa(Aux2LBL,TXT_REG,10)), "-", "-");
            writeIC("LBL", concatenate("L",itoa(Aux2LBL+1,TXT_REG,10)), "-", "-");
            break;
        
        case assignK:
            intermediateCode(AcNode->child[0], 1);
            intermediateCode(AcNode->child[1], 1);
            writeIC("ASSIGN", concatenate("$t",itoa(TREG-1,TXT_REG,10)), concatenate("$t",itoa(TREG,TXT_REG,10)), "-");
            writeIC("STORE", AcNode->child[0]->attr.name, concatenate("$t",itoa(TREG-1,TXT_REG,10)), "-");
            break;

        case returnK:
            intermediateCode(AcNode->child[0], 1);
            writeIC("RET", concatenate("$t",itoa(TREG,TXT_REG,10)), "-", "-");
            break;
        
        default:
            break;
    }
}

static void icExp(Node AcNode){

    switch (AcNode->kind.exp){
        case opK:
            intermediateCode(AcNode->child[0], 1);
            intermediateCode(AcNode->child[1], 1);
            TREG++; 
           
            switch (AcNode->attr.op){
                case LESS:      writeIC("LESS", concatenate("$t",itoa(TREG,TXT_REG,10)), concatenate("$t",itoa(TREG-1,TXT_REG,10)), concatenate("$t",itoa(TREG-2,TXT_REG,10)));     break;
                case LESSEQ:    writeIC("LESSEQ", concatenate("$t",itoa(TREG,TXT_REG,10)), concatenate("$t",itoa(TREG-1,TXT_REG,10)), concatenate("$t",itoa(TREG-2,TXT_REG,10)));   break;
                case MORE:      writeIC("MORE", concatenate("$t",itoa(TREG,TXT_REG,10)), concatenate("$t",itoa(TREG-1,TXT_REG,10)), concatenate("$t",itoa(TREG-2,TXT_REG,10)));     break;
                case MOREEQ:    writeIC("MOREEQ", concatenate("$t",itoa(TREG,TXT_REG,10)), concatenate("$t",itoa(TREG-1,TXT_REG,10)), concatenate("$t",itoa(TREG-2,TXT_REG,10)));   break;
                case EQUAL:     writeIC("EQUAL", concatenate("$t",itoa(TREG,TXT_REG,10)), concatenate("$t",itoa(TREG-1,TXT_REG,10)), concatenate("$t",itoa(TREG-2,TXT_REG,10)));    break;
                case DIFF:      writeIC("DIFF", concatenate("$t",itoa(TREG,TXT_REG,10)), concatenate("$t",itoa(TREG-1,TXT_REG,10)), concatenate("$t",itoa(TREG-2,TXT_REG,10)));    break;
                case ADD:       writeIC("ADD", concatenate("$t",itoa(TREG,TXT_REG,10)), concatenate("$t",itoa(TREG-1,TXT_REG,10)), concatenate("$t",itoa(TREG-2,TXT_REG,10)));      break;
                case SUB:       writeIC("SUB", concatenate("$t",itoa(TREG,TXT_REG,10)), concatenate("$t",itoa(TREG-1,TXT_REG,10)), concatenate("$t",itoa(TREG-2,TXT_REG,10)));      break;
                case MUL:       writeIC("MUL", concatenate("$t",itoa(TREG,TXT_REG,10)), concatenate("$t",itoa(TREG-1,TXT_REG,10)), concatenate("$t",itoa(TREG-2,TXT_REG,10)));      break;
                case DIV:       writeIC("DIV", concatenate("$t",itoa(TREG,TXT_REG,10)), concatenate("$t",itoa(TREG-1,TXT_REG,10)), concatenate("$t",itoa(TREG-2,TXT_REG,10)));      break;
                default:                                                                                                                                             break;
            }     
            break;  

        case constK:
            TREG++;
            writeIC("LOADC", concatenate("$t",itoa(TREG,TXT_REG,10)), concatenate("",itoa(AcNode->attr.val,TXT_REG,10)), "-");
            break;

        case id_varK:
            TREG++;
            writeIC("LOAD", concatenate("$t",itoa(TREG,TXT_REG,10)), AcNode->attr.name, "-");
            break;

        case id_vecK:
            intermediateCode(AcNode->child[0], 1);
            TREG++;
            writeIC("LOADVEC", concatenate("$t",itoa(TREG,TXT_REG,10)), AcNode->attr.name, concatenate("$t",itoa(TREG-1,TXT_REG,10)));
            break;

        case id_funK:{
            int contPar = 0;
            Node lstParam = AcNode->child[0];
            
            while (lstParam != NULL){
                contPar++;
                //printf("ola: %d\n", contPar);
                intermediateCode(lstParam, 0);
                writeIC("PARAM", concatenate("$t",itoa(TREG,TXT_REG,10)), "-", "-");
                lstParam = lstParam->sibling;
            }
            TREG++;
            char CONT_REG[3]; 
            writeIC("CALL", concatenate("$t",itoa(TREG,TXT_REG,10)), AcNode->attr.name, itoa(contPar,CONT_REG,10));
            break;
        }
        default:
            break;
    }
}

static void icType(Node AcNode){

    switch (AcNode->kind.type){
        case voidK:
            writeIC("FUNC", "void", AcNode->child[0]->attr.name, "-");
            
            Node auxNode = AcNode->child[0]->child[0];

            while(auxNode != NULL){
                if(auxNode->nodekind == typeK && auxNode->kind.type == intK){
                    if(auxNode->child[0]->nodekind == expK && auxNode->child[0]->kind.exp == id_varK){
                        TREG++;
                        writeIC("LOAD", concatenate("$t",itoa(TREG,TXT_REG,10)), auxNode->child[0]->attr.name, "-");
                        writeIC("ARGVAR", auxNode->child[0]->attr.name, concatenate("$t",itoa(TREG,TXT_REG,10)), AcNode->child[0]->attr.name);
                    }
                    else{
                        TREG++;
                        writeIC("LOAD", concatenate("$t",itoa(TREG,TXT_REG,10)), auxNode->child[0]->attr.name, "-");
                        writeIC("ARGVEC", auxNode->child[0]->attr.name, concatenate("$t",itoa(TREG,TXT_REG,10)), AcNode->child[0]->attr.name);
                    }             
                }
                auxNode = auxNode->sibling;
            }
            intermediateCode(AcNode->child[0]->child[1], 1);
            writeIC("ENDFUNC",  AcNode->child[0]->attr.name, "-", "-");
            break;

        case intK:
            switch (AcNode->child[0]->kind.exp){
                case id_varK:
                    writeIC("VAR",  AcNode->child[0]->attr.name, "-", "-");
                    break;

                case id_vecK:
                    writeIC("VEC",  AcNode->child[0]->attr.name, itoa(AcNode->child[0]->child[0]->attr.val,TXT_REG,10), AcNode->child[0]->scope);
                    break;

                case id_funK:
                    writeIC("FUNC", "int", AcNode->child[0]->attr.name, "-");
                
                    Node auxNode = AcNode->child[0]->child[0];
                    
                    while(auxNode != NULL){
                        if(auxNode->nodekind == typeK && auxNode->kind.type == intK){
                            if(auxNode->child[0]->nodekind == expK && auxNode->child[0]->kind.exp == id_varK){
                                TREG++;
                                writeIC("LOAD", concatenate("$t",itoa(TREG,TXT_REG,10)), auxNode->child[0]->attr.name, "-");
                                writeIC("ARGVAR", "int", concatenate("$t",itoa(TREG,TXT_REG,10)), AcNode->child[0]->attr.name);
                            }
                            else{
                                TREG++;
                                writeIC("LOAD", concatenate("$t",itoa(TREG,TXT_REG,10)), auxNode->child[0]->attr.name, "-");
                                writeIC("ARGVEC", "int", concatenate("$t",itoa(TREG,TXT_REG,10)), AcNode->child[0]->attr.name);
                            }             
                        }
                    auxNode = auxNode->sibling;
                    }
                    intermediateCode(AcNode->child[0]->child[1], 1);
                    writeIC("ENDFUNC", AcNode->child[0]->attr.name, "-", "-");
                    break;
            
            }
            break;
        
        default:
            break;
    }
}
 
static void intermediateCode(Node AcNode, int goSibling){
    if (AcNode != NULL){
        switch (AcNode->nodekind){
            case stmtK:
                icStmt(AcNode);
                break;

            case expK:
                icExp(AcNode);
                break;
            
            case typeK:
                icType(AcNode); 
                break;

            default:
                break;
        }
        
        if(goSibling == 1) intermediateCode(AcNode->sibling, 1);
    }
}

static void writeIC(char func[], char atr1[], char atr2[], char atr3[]){
    printf("%s; %s; %s; %s\n", func, atr1, atr2, atr3);
    if(arq!=NULL){
        fputs(func, arq);
        fputs("; ", arq);
        fputs(atr1, arq);
        fputs("; ", arq);
        fputs(atr2, arq);
        fputs("; ", arq);
        fputs(atr3, arq);
        fputs("\n", arq);
    }
}

void biuldIntermediateCode(Node AcNode){
    TREG = -1;TLAB = 0;
    arq = fopen("quadruple.txt", "w");

    printf("\n----------------------------- CODIGO INTERMEDIARIO -----------------------------\n");
    intermediateCode(AcNode, 1);
    writeIC("HLT", "-", "-", "-");
    fclose(arq);
}

