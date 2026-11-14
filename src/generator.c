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

//návratová hodnota funkce, inicializuje se na unit a volání funkce ji vždy přepíše a pak "vynuluje"
static dataType function_return_type = TYPE_UNIT;

/**
 * @brief funkce pro různé jumpy, 
 */
static int get_new_label(void){
  return actual_label_number++;
}



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

void ident(AST_Node *node){
  if (node->result != TYPE_UNIT){
    printf("PUSHS LF@%s\n", node->nodeData.idVarData.id_name);
  }
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
 * @brief generátor bloků, například uvnitř cyklů atd..
 * 
 * kvůli této částispecifikace:
 * 
 * "Není-li proveden return, funkce vrátí výsledek svého těla. Výsledkem jednořádkového těla
 * je hodnota jeho výrazu či volání; výsledkem bloku zakončeného výrazem či voláním je hod-
 * nota tohoto posledního příkazu. Blok zakončený definicí proměnné, přiřazením nebo cyklem
 * má výsledek () (typu Unit). Hodnoty z těla cyklu se nepřenášejí do výsledku nadřazeného
 * bloku."
 * 
 * výsledek zahazuje voláním POPS do dočasné proměnné
 * @param node ukazatel na uzel stromu
 */

void generate_block(AST_Node *node){
    while (node){
        generate_node(node);
        if (node->next && node->result != TYPE_UNIT){
          
            printf("POPS GF@$t1\n");
        }
    node = node->next;
    }
    
}

/**
 * @brief pro funkci while
 * 
 * @param node ukazatel na uzel ast 
 */
void generate_while(AST_Node *node){
    int label_number = get_new_label();
    
    printf("LABEL $while%d_start\n",label_number);
    generate_node(node->nodeData.whileData.condition);    //
    printf("PUSHS bool@false\n");
    printf("JUMPIFEQS $while%d_end\n", label_number);
    
    //vygeneruje blok příkazů uvnitř cyklu
    generate_block(node->nodeData.whileData.body);

    //zjistí typ posledního příkazu v bloku, pokud není unit, tak ho zahodí, 
    //jelikož while si nepotřebuje uchovávat hodnotu z těla cyklu
    AST_Node *last = node->nodeData.while.body;
    while(last->next){
      last = last.next;
    }
    
    //
    if (last->result != TYPE_UNIT){
      printf("POPS GF@$t1\n");
    }

    printf("JUMP $while%d_start\n", label_number);
    printf("LABEL $while%d_end\n", label_number);
}

/**
 * @brief funkce pro definicici proměnných
 * @param node ukazatel na uzel ast 
 */
void var_def(AST_Node *node){

    //pro zkrácení dlouhých názvů
    dataType var_type = node->NodeData.varData.var_name;
    AST_Node expression = node->nodeData.varData.expression;
    char* name = node->nodeData.varData.var_name;

    //pokud není typ proměnné deklarován vezme si ho z přiřazované hodnoty 
    if (var_type == TYPE_UNDEF){
        var_type = expression->result;
    }
  
    generate_node(expression);
    
    //pokud je uzel typu UNIT
    if (var_type == TYPE_UNIT){
        printf("MOVE LF@%s nil@nil\n",name);
        return;
    }

    //datová konverze
    if (var_type == TYPE_DOUBLE && expression->result == TYPE_INT){
        int_to_float();
    }
    printf("POPS LF@%s\n", name);
}

/**
 * @brief funkce pro definicici proměnných
 * @param node ukazatel na uzel ast 
 */
void assign(AST_Node *node){
    //pro zkrácení názvů
    char *name = node->nodeData.assignData.name;
    AST_Node expression = node->nodeData.assignData.equal;
    
    dataType = expression->result;
    
    generate_node(expression);

    //pokud je uzel typu UNIT
    if (var_type == TYPE_UNIT){
     printf("MOVE LF@%s nil@nil\n", name);
     return;
   }

   if (var_type == TYPE_DOUBLE && expr->result == TYPE_INT){
     int_to_float();
   }

   printf("POPS LF@%s\n", name);
}

/**
 * @brief unární minus, násobí -1, využívá dříve definované funkce
 * @param node ukazatel na uzel ast
*/
void unar_minus(AST_Node *node){
  generate_node(node->nodeData.uData.single);
  if(node->nodeData.uData.single->result == TYPE_DOUBLE){
    literal_double(-1.0);
  }
    else{
    literal_int(-1);
  }
  mul();
}

/**
 * @brief funcke pro generování větve pro if
 * @param block ukazatel na blok ve funkci
 * @param return_type požadovaný návratový typ funkce
 */
void generate_branch(AST_Node *block, dataType return_type){
    generate_block(block);

    AST_Node *last = block;
    while(last->next){
        last = last->next;
    }


    if (return_type == TYPE_UNIT){
      if (last->result != TYPE_UNIT){
        printf("POPS GF@$t1\n");
      }
    }
    else if (return_type == TYPE_DOUBLE && return_type->result == TYPE_INT){
      int_to_float();
    }
}
/**
 * @brief pro uzel if, počítá s rozšířením control a uchovává podle toho návratové typy
 * @param node ukazatel na uzel ast
 */
void generate_if(AST_Node *node){

    int label_number = get_new_label();

    dataType return_type = node->result;
    if(node->nodeData.ifData.else_part == NULL){
        return_type = TYPE_UNIT;
    }

    generate_node(node->nodeData.ifData.condition);
    printf("PUSHS bool@false\n");
    printf("JUMPIFEQS $if%d_else_part\n", label_number);

    generate_branch(node->nodeData.ifData.then_part,return_type);
    if (node->nodeData.ifData.else_part){
      printf("JMP $if%d_end\n", label_number);
      printf("LABEL $if%d_else_part\n", label_number);
      generate_branch(node->nodeData.ifData.else_part,return_type);
      printf("LABEL $if%d_end\n", label_number);
    }
    
    else {
      printf("LABEL $if%d_else\n", label_number);
    }
}

/**
 * @brief pro generování return
 * @param node ukazatel na uzel ast
 * @param function_return_type není přímo parametr ale využívá globální proměnou, 
 * která se nastavuje právě kvůli této funkci
 */
void generate_return(AST_Node *node){
  AST_Node *value = node->nodeData.returnData.expression;
  
  //pokud má nějaku hodnotu vygeneruje pro ni uzel
  generate_node(value);
  
  //pokud funkce nemá vracet hodnotu a nějaká ji přece jen vyjde, 
  //tak ji zahodí do pomocné proměnné
  if (function_return_type == TYPE_UNIT){
    if (value->result != TYPE_UNIT){
      printf("POPS GF@t1\n");
    }
  }
    else if (function_return_type == TYPE_DOUBLE && function_return_type->result == TYPE_INT){
      int_to_float();
    }


    if (function_return_type == TYPE_UNIT){
      printf("PUSHS nil@nil\n");
    }

  printf("POPFRAME\n");
  printf("RETURN\n");
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
      unar_plus(node);
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

    case WHILE_N:
      generate_while(node);
      break;

   case ASSIGN:
        assign(node);
        break;
    case VAR_DEF:
        var_def(node);
        break;

    case IDENT:
      ident(node);
      break;
    case UMINUS:
      unar_minus(node);
      break;
    case IF_N:
      generate_if(node);
      break;
    case RETURN:
      generate_return(node);
      break;

//todo: doplnit další uzly AST


 




  case FOR_N:
    printf("Today is Saturday");
    break;

  case LIST:
    printf("Today is Saturday");
    break;

  case DOT_ACCESS:
    printf("Today is Saturday");
    break;


  case FUN_DEF:
    printf("Today is Saturday");
    break;

  case FUN_CALL:
    printf("Today is Saturday");
    break;



  default:
  break;
}
}






