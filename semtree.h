#ifndef SEMTREE_H
#define SEMTREE_H

#define MAXCHILDREN 3

//estrutura do no da arvore
typedef enum {stmtK, expK, typeK} NodeKind;
typedef enum {ifK, whileK, assignK, returnK} StmtKind;
typedef enum {opK, constK, indexK, id_varK, id_vecK, id_funK} ExpKind;
typedef enum {voidK, intK} TypeKind;

typedef struct tNode *Node; 


typedef struct tNode {
    Node child[MAXCHILDREN];
    Node sibling;
    
    NodeKind nodekind;
    union { StmtKind stmt; ExpKind exp; TypeKind type; } kind;
    union { int op; int val; char *name; } attr;

    int lin;
    char * scope;
}treeNode;

//funcoes pertinentes a criacao da arvore sintatica
Node newStmtNode(StmtKind);
Node newExpNode(ExpKind);
Node newTypeNode(TypeKind);
void printTree(Node);
void printToken(int);
void printToken_inline(int);

#endif