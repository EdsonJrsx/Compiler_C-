#include "globals.h"
#include "semtree.h"
#include "parser.tab.h"

//função de criacao de no da arvore do tipo declaracao
Node newStmtNode(StmtKind kind)
{ Node t = (Node) malloc(sizeof(treeNode));
  int i;
  if (t==NULL)
    printf("Out of memory0 error at line %d\n", count_line);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = stmtK;
    t->kind.stmt = kind;
    t->lin = count_line;
    t->scope = "globals";
  }
  return t;
}

//funcao de criacao de no da arvore do tipo expressao
Node newExpNode(ExpKind kind)
{ Node t = (Node) malloc(sizeof(treeNode));
  int i;
  if (t==NULL)
    printf("Out of memory error at line %d\n", count_line);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = expK;
    t->kind.exp = kind;
    t->lin = count_line;
    t->scope = "globals";
  }
  return t;
}

//funcao de criacao de no da arvore do tipo tipo
Node newTypeNode(TypeKind kind)
{ Node t = (Node) malloc(sizeof(treeNode));
  int i;
  if (t==NULL)
    printf("Out of memory error at line %d\n", count_line);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = typeK;
    t->kind.exp = kind;
    t->lin = count_line;
    t->scope = "globals";
  }
  return t;
}

//funcoes auxiliares para a impressao da arvore
int indentno = 0;
#define INDENT indentno+=2
#define UNINDENT indentno-=2

void printSpaces(void)
{ int i;
  for (i=0;i<indentno;i++)
    printf(" ");
}

void printToken( int token )
{ switch (token)
  { case ASSIG: printf("=\n"); break;
    case LESS: printf("<\n"); break;
    case EQUAL: printf("==\n"); break;
    case LESSEQ: printf("<=\n"); break;
    case MORE: printf(">\n"); break;
    case MOREEQ: printf(">=\n"); break;
    case DIFF: printf("!=\n"); break;
    case OPARENT: printf("(\n"); break;
    case CPARENT: printf(")\n"); break;
    case OBRACE: printf("{\n"); break;
    case CBRACE: printf("}\n"); break;
    case OBRACKET: printf("[\n"); break;
    case CBRACKET: printf("]\n"); break;
    case SEMICOL: printf(";\n"); break;
    case COMMA: printf(",\n"); break;
    case ADD: printf("+\n"); break;
    case SUB: printf("-\n"); break;
    case MUL: printf("*\n"); break;
    case DIV: printf("/\n"); break;
    case ELSE: printf("else\n"); break;
    case IF: printf("if\n"); break;
    case INT: printf("int \n"); break;
    case RETURN: printf("return \n"); break;
    case VOID: printf("void \n"); break;
    case WHILE: printf("while \n"); break;
    case ERROR: printf("Error\n"); break;
    case ID: printf("ID\n"); break;
    case NUM: printf("NUM\n"); break;
    default:
      printf("Token Desconhecido: %d\n",token);
  }
}

void printToken_inline( int token )
{ switch (token)
  { case ASSIG: printf("="); break;
    case LESS: printf("<"); break;
    case EQUAL: printf("=="); break;
    case LESSEQ: printf("<="); break;
    case MORE: printf(">"); break;
    case MOREEQ: printf(">="); break;
    case DIFF: printf("!="); break;
    case OPARENT: printf("("); break;
    case CPARENT: printf(")"); break;
    case OBRACE: printf("{"); break;
    case CBRACE: printf("}"); break;
    case OBRACKET: printf("["); break;
    case CBRACKET: printf("]"); break;
    case SEMICOL: printf(";"); break;
    case COMMA: printf(","); break;
    case ADD: printf("+"); break;
    case SUB: printf("-"); break;
    case MUL: printf("*"); break;
    case DIV: printf("/"); break;
    case ELSE: printf("else"); break;
    case IF: printf("if"); break;
    case INT: printf("int"); break;
    case RETURN: printf("return"); break;
    case VOID: printf("void"); break;
    case WHILE: printf("while"); break;
    case ERROR: printf("Error"); break;
    case ID: printf("ID"); break;
    case NUM: printf("NUM"); break;
    default:
      printf("Token Desconhecido: %d",token);
  }
}

void printT(Node tree)
{ int i;
  INDENT;
  while (tree != NULL) {
    printSpaces();
    if (tree->nodekind==stmtK)
    { switch (tree->kind.stmt) {
        case ifK:
          printf("If\n");
          break;
        case whileK:
          printf("While\n");
          break;
        case assignK:
          printf("linha: %d; = \n", tree->lin);
          break;
        case returnK:
          printf("Return; linha: %d\n", tree->lin);
          break;
        default:
          printf("Desconhecido stmtNode tipo\n");
          break;
      }
    }
    else{
      if (tree->nodekind==expK){
        switch (tree->kind.exp) {
          case opK:
            printf("linha %d; Op: ", tree->lin);
            printToken(tree->attr.op);
            break;
          case constK:
            printf("linha: %d; Const: %d\n",tree->lin, tree->attr.val);
            break;
          case indexK:
            printf("linha: %d; Indice: %d\n",tree->lin, tree->attr.val);
            break;
          case id_varK:
            printf("Var ID: %s; escopo: %s; linha: %d\n",tree->attr.name, tree->scope, tree->lin);
            break;
          case id_vecK:
            printf("Vet ID: %s; escopo: %s; linha: %d\n",tree->attr.name, tree->scope, tree->lin);
            break;
          case id_funK:
            printf("Fun ID: %s; escopo: %s; linha: %d\n",tree->attr.name, tree->scope, tree->lin);
            break;
          default:
            printf("Desconhecido ExpNode tipo\n");
            break;
        }
      }
      else{
        if(tree->nodekind==typeK){ 
          switch (tree->kind.type){
            case intK:
              printf("Int; linha: %d\n", tree->lin);
            break;
            case voidK:
              printf("void; linha: %d\n", tree->lin);
            break;
            default:
              printf("Desconhecido TypeNode tipo");
            break;
          }
        }
        else printf("Desconhecido tipo de no \n");
      }
    }
    for (i=0;i<MAXCHILDREN;i++)
         printT(tree->child[i]);
    tree = tree->sibling;
  }
  UNINDENT;
}

//funcao que percoree a arvore em pre ordem e imprime as nos
void printTree(Node tree){ 
  printf("------------------------------- ARVORE SINTATICA -------------------------------\n");
  printT(tree);
  printf("\n");
}
