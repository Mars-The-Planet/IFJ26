/**
 * @file generator.c
 * @author xmolikj00
 * 
 * @brief adt zásobník
 * @date 31.10.2026
 */

//#include avl_tree.h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef enum ASTNodeType {

    ADD,
    SUB,
    MUL,
    DIV,
    LESS,
    EQUALS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUEAL,
    AND,
    OR,
    UMINUS,
    UPLUS,
    NOT,
    IF,
    WHILE,
    FOR,
    IDENT,
    LIST,
    DOT_ACCESS,
    BLOCK, // pro posloupnost příkazů
    ASSIGN,
    VAR_DEF, // TODO: bool kde budu rozohodovat jestli se jedná od VAL(true) nebo VAR(false)
    FUN_DEF,
    FUN_CALL,
    LITERAL_INT,
    LITERAL_DOUBLE,
    LITERAL_STRING,
    LITERAL_BOOL,
    LITERAL_UNIT,
    RETURN,    

} NodeType;


/**
 * @brief funkce pro int z ifj26 do ifjcode26
 * @param value hodnota původního celého čísla v ifj26 
*/
void literal_int(int value){
    printf("PUSHS int@%d\n", value);
}

/**
 * @brief pro převod float
 * @param value hodnota desetiného čísla
*/
void literal_double(double value){
    printf("PUSHS float@%a\n", value);
}

//opravit
/*
void literal_string(char* string){
  printf("PUSHS string@")
  ;
}
*/

/**
 * @brief převod typu boolean 
 * @param value původní výraz před převodem, ovšem jako int jelikož se to lépe předává
*/
void literal_bool(int value){
  if (value){
    printf("PUSHS bool@true\n");
  }
  else printf("PUSHS bool@false\n")
}

/**
 * @brief vložení typu unit kde potřeba
 */
void literal_unit(void){
    printf("PUSHS nil@nil\n");
}

/**
 * @brief typová konverze z int na float
 */
void int_to_float(void){
  printf("INT2FLOATS\n");
}

/**
*   @brief pro operátor less
*/
void less(void){
  printf("LTS\n");
}

/**
 * @brief pro operátor greater
 */
void greater(void){
  printf("GTS\n");
}

/**
 * @brief pro operátor equals
 */
void equals(void){
  printf("EQTS\n");
}

/**
 * @brief pro operátor less_equal
 */

void less_equal(){
  printf("GTS\nNOTS\n");
}

/**
 * @brief pro operátor greater_equal
 */
void greater_equal(void){
  printf("LTS\nNOTS\n");
}

/**
 * @brief pro operátor not_equal
 */
void not_equal(void){
  printf("EQTS\nNOTS\n");
}

/**
 * @brief pomocná funkce pro generátor kódu pro relace, vygeneruje kód pro oba operandy
 *  a pokud je potřeba provede konverzi z int na float
 * @param node ukazatel na uzel AST
 */
void get_operands(Node *node){
  generate_node(node->left)

  if(node->left->type == TYPE_INT && node->left->type == TYPE_FLOAT){
    int_to_float();
  }

  generate_node(node->right)

  if(node->left->type == TYPE_INT && node->left->type == TYPE_FLOAT){
    int_to_float();
  }
}
/**
 * @brief pomocná funkce pro generátor kódu pro relace, vybere relační operátor o který se jedná a zavolá příslušnou funkci
 * @param node ukazatel na uzel AST
 */
void relations(Node *node)
  generate_operands(node);

  switch (node->nodeType)
  {
  case: LESS:
    less();
    break;
  case GREATER:
    greater();
    break;
  case EQUALS:
    equals();
    break;
  case LESS_EQUAL:
    less_equal();
    break;
  case GREATER_EQUAL:
    greater_equal();
    break;
  case NOT_EQUAL:
    not_equal();
    break;
  }

/**
 * @brief generátor kódu pro jednotlivé uzly AST
 *  používá switch a prokaždý uzel podle jeho typu zavolá funkci která vygeneruje kód pro daný uzel
 * @param node ukazatel na uzel AST
 */
generate_node(Node *node){
  
  switch (node.nodetype){
    case LITERAL_INT:
      literal_int(node.value);
    break;
    case LITERAL_DOUBLE:
      literal_double(node.value);
    break;
    case LITERAL_STRING:
      literal_sting(node.value);
    break;
    case LITERAL_BOOL:
      literal_bool(value);
    break;
    case LITERAL_UNIT:
      literal_unit();
    break;
    case LESS:
      operands(node)
    break;
    case GREATER:
      operands(node);
    break;
    case EQUALS:
      operands(node);
    break;
    case LESS_EQUAL:
      operands(node);
      break;
    case GREATER_EQUAL:
      operands(node);
      break;
    case NOT_EQUAL:
      operands(node)
      break;

      //todo: doplnit další uzly AST

    case ADD:
      printf("Today is Saturday");
    break;
    case SUB:
      printf("Today is Saturday");
    break;
    case MUL:
      printf("Today is Saturday");
    break;
    case DIV:
      printf("Today is Saturday");
    break;



  case AND_N:
    printf("Today is Saturday");
    break;
   case OR_N:
    printf("Today is Saturday");
    break;
  case UMINUS:
    printf("Today is Saturday");
    break;
  case UPLUS:
    printf("Today is Saturday");
    break;
  case NOT_N:
    printf("Today is Saturday");
    break;
  case IF_N:
    printf("Today is Saturday");
    break;
   case WHILE_N:
    printf("Today is Saturday");
    break;
  case FOR_N:
    printf("Today is Saturday");
    break;
  case IDENT:
    printf("Today is Saturday");
    break;
  case LIST:
    printf("Today is Saturday");
    break;
  case DOT_ACCES:
    printf("Today is Saturday");
    break;
   case BLOCK:
    printf("Today is Saturday");
    break;
  case ASSIGN:
    printf("Today is Saturday");
    break;
  case VAR_DEF:
    printf("Today is Saturday");
    break;
  case FUN_DEF:
    printf("Today is Saturday");
    break;
  case FUN_CALL:
    printf("Today is Saturday");
    break;



  case RETURN:
    printf("Today is Saturday");
    break;
}
}






