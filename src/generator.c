/**
 * @file IFJ26/src/generator.c, projekt ifj
 * @author xmolikj00 
 * 
 * @brief generátor kódu pro projekt IFJ26
 * použití AI - Ai jsem použil pro "debatu" o implementaci některých funkcí, promty vypadali tak,
 * že jsem poslal funkci co jsem napsal a nebyl jsem si u ní jistý zda je správně, popsal jsem svůj způsob uvažování
 * a pokus o implementaci a jak by to mělo fungovat, více u jednotlivých funkcí.
 * @date 31.10.2026
 */

//#include avl_tree.h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


static int actual_label_number = 0;

/**
 * @brief funkce pro různé jumpy, 
 */
static int get_new_label(void){
  return actual_label_number++;
}


/**
 * @brief struktura od x273960 z parseru
 */
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
 * @brief pomocná funkce pro generátor kódu pro relace, vygeneruje kód pro oba operandy
 *  a pokud je potřeba provede konverzi z int na float
 * 
 * jeden příklad funkce pomocí ai, původně jsem chtěl zjiťovat typ pomocí makra _Generic, které jsem si 
 * dohledal na internetu když jsem nad tímto problémem přemýšlel, nebyl jsem si ale úplně jistý jak funguje,
 * tak jsem se zeptal ai konkrétně Claude, zda je možné to použít v tomto případě, dostal jsem odpověď že ne,
 * jelikož makro by vzalo typ v C, nikoliv v mnou požadovaných jazycích.
 * 
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
*   @brief pro operátor less
*/
void less(Node *node){
  generate_operands(node);
  printf("LTS\n");
}

/**
 * @brief pro operátor greater
 */
void greater(Node *node){
  generate_operands(node);
  printf("GTS\n");
}

/**
 * @brief pro operátor equals
 */
void equals(Node *node){
  generate_operands(node);
  printf("EQTS\n");
}

/**
 * @brief pro operátor less_equal
 */

void less_equal(Node *node){
  generate_operands(node);
  printf("GTS\nNOTS\n");
}

/**
 * @brief pro operátor greater_equal
 */
void greater_equal(Node *node){
  generate_operands(node);
  printf("LTS\nNOTS\n");
}

/**
 * @brief pro operátor not_equal
 */
void not_equal(Node *node){
  generate_operands(node);
  printf("EQTS\nNOTS\n");
}

/**
 * @brief logické not
 * @param node ukazatel na uzel ass
 */
void logical_not(Node *node){
    generate_node(node->single);
    printf("NOTS\n");
}
/**
 * @brief struktura pro logické and, používá zkratkové vyhodnocování
 * @param node ukazatel na uzel
 */
void logical_and(Node *node){

  int label_number = get_new_label();

  generate(node->left);
  printf("PUSHS bool@false\n");
  printf("JUMPIFEQS $ß_and%d_false\n",label_number);

  generate(node->right);
  printf("PUSHS bool@false\n");
  printf("JUMPIFEQS $ß_and%d_false\n",label_number);

  printf("PUSHS bool@true\n");
  printf("JUMP $ß_and%d_end\n",label_number);

  printf("LABEL $ß_and%d_false\n",label_number);
  printf("PUSHS bool@false\n");
  printf("LABEL $ß_and%d_end\n",label_number);

}

/**
 * @brief struktura pro logické or, používá zkratkové vyhodnocování
 * @param node ukazatel na uzel
 */
void logical_or(){
  int label_number = get_new_label();

  generate(node->left);
  printf("PUSHS bool@true\n");
  printf("JUMPIFEQS $ß_or%d_true\n",label_number);

  generate(node->right);
  printf("PUSHS bool@true\n");
  printf("JUMPIFEQS $ß_or%d_true\n",label_number);

  printf("PUSHS bool@false\n");
  printf("JUMP $ß_or%d_end\n",label_number);

  printf("LABEL $ß_or%d_true\n",label_number);
  printf("PUSHS bool@false\n");
  printf("LABEL $ß_or%d_end\n",label_number);
}

/**
 * @brief struktura pro inarní plus
 * @param node ukazatel na uzelk ass
 */
void unar_plus(Node *node){
  generate_node(node->single);
}

/**
 * @brief generátor kódu pro jednotlivé uzly AST
 *  používá switch a prokaždý uzel podle jeho typu zavolá funkci která vygeneruje kód pro daný uzel
 * @param node ukazatel na uzel AST
 */
generate_node(Node *node){
  
  switch (node->nodetype){
    case LITERAL_INT:
      literal_int(node->value);
    break;
    case LITERAL_DOUBLE:
      literal_double(node->value);
    break;
    case LITERAL_STRING:
      literal_sting(node->value);
    break;
    case LITERAL_BOOL:
      literal_bool(node->value);
    break;
    case LITERAL_UNIT:
      literal_unit();
    break;
    case LESS:
      less(node);
    break;
    case GREATER:
      greater(node);
    break;
    case EQUALS:
      equals(node);
    break;
    case LESS_EQUAL:
      less_equal(node);
      break;
    case GREATER_EQUAL:
      greater_equal(node);
      break;
    case NOT_EQUAL:
      not_equal(node)
      break;
    case NOT_N:
      logical_not(node);
      break;
    case AND_N:
      logical_and(node);
      break;
    case OR_N:
      logical_or(node);
      break;
    case UPLUS:
      printf("Today is Saturday");
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



 
  case UMINUS:
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






