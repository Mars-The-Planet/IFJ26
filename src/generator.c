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

#include "generator.h"
#include "parser.h"

/**
 * @brief vrací nové unikátní číslo pro návěští (skoky)
 */

static int actual_label_number = 0;

/**
 * @brief funkce pro různé jumpy, 
 */
static int get_new_label(void){
  return actual_label_number++;
}


//doplnit až bude parser vědět jak vypadá blok
void generate_block();



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
  else printf("PUSHS bool@false\n");
}

/**
 * @brief unit nic nenechává na zásobníku
 */
void literal_unit(void){
}

void ident();

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
void get_operands(AST_Node *node){

  dataType left = node->nodeData.biData.left->result;
  dataType right = node->nodeData.biData.right->result;
  generate_node(node->nodeData.biData.left);

  if( left == TYPE_INT && right == TYPE_DOUBLE){
    int_to_float();
  }

  generate_node(node->nodeData.biData.right);

  if(right == TYPE_INT && left == TYPE_DOUBLE){
    int_to_float();
  }
}


/**
*   @brief pro operátor less
*/
void less(AST_Node *node){
  get_operands(node);
  printf("LTS\n");
}

/**
 * @brief pro operátor greater
 */
void greater(AST_Node *node){
  get_operands(node);
  printf("GTS\n");
}

/**
 * @brief pro operátor equals
 */
void equals(AST_Node *node){
  get_operands(node);
  printf("EQS\n");
}

/**
 * @brief pro operátor less_equal
 */

void less_equal(AST_Node *node){
  get_operands(node);
  printf("GTS\nNOTS\n");
}

/**
 * @brief pro operátor greater_equal
 */
void greater_equal(AST_Node *node){
  get_operands(node);
  printf("LTS\nNOTS\n");
}

/**
 * @brief pro operátor not_equal
 */
void not_equal(AST_Node *node){
  get_operands(node);
  printf("EQS\nNOTS\n");
}

/**
 * @brief logické not
 * @param node ukazatel na uzel ass
 */
void logical_not(AST_Node *node){
    generate_node(node->nodeData.uData.single);
    printf("NOTS\n");
}
/**
 * @brief struktura pro logické and, používá zkratkové vyhodnocování
 * @param node ukazatel na uzel
 */
void logical_and(AST_Node *node){

  int label_number = get_new_label();

  generate_node(node->nodeData.biData.left);
  printf("PUSHS bool@false\n");
  printf("JUMPIFEQS $_and%d_false\n",label_number);

  generate_node(node->nodeData.biData.right);
  printf("PUSHS bool@false\n");
  printf("JUMPIFEQS $_and%d_false\n",label_number);

  printf("PUSHS bool@true\n");
  printf("JUMP $_and%d_end\n",label_number);

  printf("LABEL $_and%d_false\n",label_number);
  printf("PUSHS bool@false\n");
  printf("LABEL $_and%d_end\n",label_number);

}

/**
 * @brief struktura pro logické or, používá zkratkové vyhodnocování
 * @param node ukazatel na uzel
 */
void logical_or(AST_Node *node){
  int label_number = get_new_label();

  generate_node(node->nodeData.biData.left);
  printf("PUSHS bool@true\n");
  printf("JUMPIFEQS $_or%d_true\n",label_number);

  generate_node(node->nodeData.biData.right);
  printf("PUSHS bool@true\n");
  printf("JUMPIFEQS $_or%d_true\n",label_number);

  printf("PUSHS bool@false\n");
  printf("JUMP $_or%d_end\n",label_number);

  printf("LABEL $_or%d_true\n",label_number);
  printf("PUSHS bool@true\n");
  printf("LABEL $_or%d_end\n",label_number);
}

/**
 * @brief funkce pro inarní plus
 * @param node ukazatel na uzelk ass
 */
void unar_plus(AST_Node *node){
  generate_node(node->nodeData.uData.single);
}

/**
 * @brief funkce pro sčítání a kokatenaci řetězců
 * @param node ukazatel na uzel
 */
void add(AST_Node *node){
  get_operands(node);

  //stačí jeden, sémantika zkontroluj epředtím;
  if (node->nodeData.biData.left->result == TYPE_STRING){
    printf("POPS GF@$t2\n");
    printf("POPS GF@$t1\n");

    //concat nemá zásobníkovou verzi
    printf("CONCAT GF@$t1 GF@$t1 GF@$t2\n");
    printf("PUSHS GF@$t1\n");
  }
  else{
    printf("ADDS\n");
  }
}

/**
 * @brief funkce pro odečítání
 * @param node ukazatel na uzel
 */
void sub(AST_Node *node){
  get_operands(node);
  printf("SUBS\n");
}
/**
 * @brief funkce pro násobení
 * @param node ukazatel na uzel
 */
void mul(AST_Node *node){
  get_operands(node);

  printf("MULS\n");
}

/**
 * @brief fuknce pro dělení
 * tady jsem s ai konzultoval zda převádět na float nebo ne, ai mě přesvědčilo že chování scaly odpovídá
 * nepřevádět na float a použít funkci DIVS
 * @param node ukazatel na uzel
 */
void divide(AST_Node *node){
  get_operands(node);

  if (node->nodeData.biData.left->result == TYPE_DOUBLE || node->nodeData.biData.left->result == TYPE_DOUBLE){
    printf("DIVS\n");
  }
  else
    printf("IDIVS\n");
}


/**
 * @brief generátor kódu pro jednotlivé uzly AST
 *  používá switch a prokaždý uzel podle jeho typu zavolá funkci která vygeneruje kód pro daný uzel
 * @param node ukazatel na uzel AST
 */

 
void generate_node(AST_Node *node){
  
  switch (node->type){
    case LITERAL_INT:
      literal_int(node->nodeData.elemData.valueType.int_value);
    break;
    case LITERAL_DOUBLE:
      literal_double(node->nodeData.elemData.valueType.double_value);
    break;
    case LITERAL_STRING:
      literal_string(node->nodeData.elemData.valueType.string_value);
    break;
    case LITERAL_BOOL:
      literal_bool(node->nodeData.elemData.valueType.bool_value);
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
      not_equal(node);
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
      
    case ADD:
      add(node);
      break;
    case SUB:
      sub(node);
      break;
    case MUL:
      mul(node);
      break;
    case DIV:
      divide(node);
      break;
//todo: doplnit další uzly AST


 
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
  case DOT_ACCESS:
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






