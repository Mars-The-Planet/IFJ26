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
 * @brief funkce pro urření posledního uzlu
 * @param node ukazatel na uzel ast
 */
AST_Node last_node(AST_Node *node) {
  while (node->next) {
    n = node->next;
  }
  return node;
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

  if (node->nodeData.biData.left->result == TYPE_DOUBLE || node->nodeData.biData.right->result == TYPE_DOUBLE){
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
    AST_Node *last = node->nodeData.whileData.body;
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
    dataType var_type = node->nodeData.varData.varType;
    AST_Node *expression = node->nodeData.varData.expression;
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
    char *name = node->nodeData.var_name;
    AST_Node *expression = node->nodeData.assignData.equal;
    
    dataType var_type= expression->result;
    
    generate_node(expression);

    //pokud je uzel typu UNIT
    if (var_type == TYPE_UNIT){
     printf("MOVE LF@%s nil@nil\n", name);
     return;
   }

   if (var_type == TYPE_DOUBLE && expression->result == TYPE_INT){
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
  printf("MULS\n");
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
    else if (return_type == TYPE_DOUBLE && last->result == TYPE_INT){
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
      printf("JUMP $if%d_end\n", label_number);
      printf("LABEL $if%d_else_part\n", label_number);
      generate_branch(node->nodeData.ifData.else_part,return_type);
      printf("LABEL $if%d_end\n", label_number);
    }
    
    else {
      printf("LABEL $if%d_else_part\n", label_number);
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
  if(value){
    generate_node(value);
  }
  //pokud funkce nemá vracet hodnotu a nějaká ji přece jen vyjde, 
  //tak ji zahodí do pomocné proměnné
  if (function_return_type == TYPE_UNIT){
    if (value->result != TYPE_UNIT){
      printf("POPS GF@t1\n");
    }
  }
    else if (function_return_type == TYPE_DOUBLE && value->result == TYPE_INT){
      int_to_float();
    }


    if (function_return_type == TYPE_UNIT){
      printf("PUSHS nil@nil\n");
    }

  printf("POPFRAME\n");
  printf("RETURN\n");
}


/**
 * @brief sbírá definici proměnných, jelikož mohou být definované jen jednou, nemůžou být uvnitř cyklů,
 *  proměnné se definují hned při začátku funkce
 * 
 * @param node ukazatel na uzel ast
 */
void collect_vars(AST_Node *node){
  while (node)
  {
    switch (node->type)
    {
    case VAR_DEF:
      printf("DEFVAR LF@%s\n",node->nodeData.varData.var_name)
      break;
    case ASSIGN:
      collect_vars(n->nodeData.assignData.equal);
      break;
    case IF_N:
      collext_vars(node->nodeData.assignData.equal);
      break;
    case IF_N:
      collect_defvars(node->nodeData.ifData.condition);
      collect_defvars(node->nodeData.ifData.then_part);
      collect_defvars(node->nodeData.ifData.else_part);  
      break;
    case WHILE:
      collect_defvars(node->nodeData.whileData.condition);
      collect_defvars(node->nodeData.whileData.body);
      break;
    
    //pro binární
    case ADD: case SUB: case MUL: case DIV:
    case LESS: case GREATER: case EQUALS: 
    case LESS_EQUAL: case GREATER_EQUAL:
    case AND_N: case OR_N:
      collect_defvars(node->nodeData.biData.left);
      collect_defvars(node->nodeData.biData.right);
      break;

    //pro unární
    case UMINUS: case UPLUS: case NOT:
      collect_defvars(node->nodeData.uData.single);
      break;
    
    case RETURN:
      collect_vars(node->nodeData.returnData.expression);
      break;
    case FUN_CALL:
      collect_vars(node->nodeData.callFunData.fun_args);
      break;
    default:
      break;
    }
   node = node->next;
  }
}

/**
 * @brief funkce pro vybírání parametrů ze zásobníku
 * používá rekurzi aby byl pravý parametr nejhlouběji a levý nejvíce navrchu
 * @param params ukazatel na hlavičku listu parametrů funkce
 */
void pop_params(ListParam *params){
  if (params == NULL){
    return;
  }
  pop_params(params->next);
  printf("POPS LF@%s\n", params->param_name);
}

/**
 * @brief funkce pro vytvoření spouštění funkcí
 * 
 * @param node ukazatel na uzel ast
 */
void generate_fun(AST_node *node){

  AST_Node *body = node->nodeData.funData.body;
  function_return_type = node->nodeData.funData.return_type;


  if(function_return_type == TYPE_UNDEF){
    function_return_type = last_node(body)->result;
  }

  printf("LABEL $function\n$",node->nodeData.funData.fun_name);
  printf("CREATEFRAME\n");
  printf("PUSHFRAME\n");

  head *params = node->nodeData.funData.param_list;
  if(params){
    for (ListParams *p = params->first; p; p = p->next){
        printf("DEFVAR LF@%s\n", p->param_name);
    }
    pop_params(param_first);
  }
    //sesbírá všechny proměnné
  collect_vars(body);

  //vygeneruje tělo funkce
  generate_block(body);

  AST_Node *function_return = last_node(body);

  if(function_return_type == TYPE_UNIT){
    if(function_return->result != TYPE_UNIT){
      printf("POPS GF$t1\n");
    }
    printf("PUSHS nil@nil\n");
  }
  else if (function_return_type == TYPE_DOUBLE && last->result == TYPE_INT){
    int_to_float();
  }

  printf("POPFRAME");
  printf("RETURN");

  function_return_type = TYPE_UNIT;
}

/**
 * @brief vestavěná funkce readInt
 */
void read_int() {
  printf("LABEL $BUILDIN$readInt_start\n" // vytvoří zásobníkový rámec
         "CREATEFRAME\n"
         "PUSHFRAME\n"
         "DEFVAR LF@$$input\n"      // načtená hodnota
         "DEFVAR LF@$$comparator\n" // pomocná hodnota pro porovnání
         "READ LF@$$input int\n "   // načte vstup, pokud hodnota kterou se
                                    // snaží načíst není int, vrátí nil
         "JUMPIFEQ $BUILDIN$readInt_wrong LF@$$comparato\n" // ověří zda čtení
                                                            // prošlo
         "GT LF@comparator LF@input int@2147483647\n"       // je větší než max.
                                                            // 32bitové Int?
         "JUMPIFEQ $BUILDIN$readInt_wrong LF@c bool@true\n"
         "LT LF@comparator LF@input int@-2147483648\n" // je menší než min.
                                                       // 32bitové Int?
         "JUMPIFEQ $BUILDIN$readInt_wrong LF@comparator bool@true\n"
         "JUMP $BUILDIN$readInt_ok\n"
         "LABEL $BUILDIN$readInt_wrong\n"
         "MOVE LF@$$input INT@0\n"
         "LABEL $BUILDIN$readInt_ok\n"
         "PUSHS LF@input\n" // výsledek na zásobník
         "POPFRAME\n"
         "RETURN\n");
}
/**
 * @brief vestavěná funkce readDouble
 */
void read_double() {
  printf("LABEL $BUILDIN$readDouble_start\n"
         "CREATEFRAME\n"
         "PUSHFRAME\n"
         "DEFVAR LF@$$input\n"
         "READ LF@$$input float\n"
         "JUMPIFNEQ $BUILDIN$readDouble_end LF@$$input nil@nil\n"
         "MOVE LF@$$input float%a\n"
         "LABEL $BUILDIN$readDouble_end\n"
         "PUSHS LF@$$input\n" // výsledek na zásobník
         "POPFRAME\n"
         "RETURN\n",
         0.0);
}

/**
 * @brief vestavěná funkce readLine
 */
void read_line() {
  printf("LABEL $BUILDIN$readLine_start\n"
         "CREATEFRAME\n"
         "PUSHFRAME\n"
         "DEFVAR LF@$$input\n"
         "READ LF@$$input string\n"
         "JUMPIFNEQ $BUILDIN$readLine_end LF@$$input nil@nil\n"
         "MOVE LF@$$input string@\n"
         "LABEL $BUILDIN$readLine_end\n"
         "PUSHS LF@$$input\n"
         "POPFRAME\n"
         "RETURN\n");
}
/**
 * @brief vestavěná funkce readBoolean
 */
void read_boolean() {
  printf("LABEL $BUILDIN$readBoolean\n"
         "CREATEFRAME\n"
         "PUSHFRAME\n"
         "DEFVAR LF@v\n"
         "READ LF@v bool\n"
         "JUMPIFNEQ $BUILDIN$readBoolean$end LF@v nil@nil\n"
         "MOVE LF@v bool@false\n"
         "LABEL $BUILDIN$readBoolean$end\n"
         "PUSHS LF@v\n"
         "POPFRAME\n"
         "RETURN\n");
}

/**
 * @brief vestavěná funkce ord
 */
void ord(char *string, int index) {
  printf(
      "LABEL $b$ord\n"
      "CREATEFRAME\n"
      "PUSHFRAME\n"
      "DEFVAR LF@$$string\n"
      "DEFVAR LF@$$index\n"
      "DEFVAR LF@$$len\n"
      "DEFVAR LF@c\n"
      "DEFVAR LF@r\n"
      "POPS LF@$$index\n"
      "POPS LF@$$string\n"
      "STRLEN LF@$$len LF@$$string\n" // zjistí délku řetězce a uloží ji do len
      "LT LF@c LF@$$index int@0\n"    // index < 0 ?
      "JUMPIFEQ $BUILDIN$ord_wrong LF@c bool@true\n"
      "LT LF@c LF@$$index LF@$$len\n"
      "JUMPIFEQ $BUILDIN$ord_wrong LF@c bool@false\n"
      "STRI2INT LF@r LF@$$string LF@$$index\n" //; r = kód znaku na pozici i
      "JUMP $BUILDIN$ord$end\n"
      "LABEL $BUILDIN$ord_wrong\n"
      "MOVE LF@r int@-1\n" // mimo rozsah nebo prázdný řetězec vrátí -1
      "LABEL $BUILDIN$ord_end\n"
      "PUSHS LF@r\n"
      "POPFRAME\n"
      "RETURN\n")
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

    case FUN_DEF:
      generate_fun(node);
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

  case FUN_CALL:
    printf("Today is Saturday");
    break;



  default:
  break;
}
}

void generate_program_start(void){
  printf(".IFJcode26\n");
  printf("DEFVAR GF@$t1\n");
  printf("DEFVAR GF@$t2\n");
  printf("JUMP $program_start\n");
}

void generate_program_end(const char *main_name){
  generate_builtins_runtime();
 
  printf("LABEL $program_start\n");
  printf("CALL $function$%s\n", main_name);
  printf("EXIT int@0\n");
}

void generate_program(AST_Node *functions, const char *main_name){
 
  generate_program_start();
 
  for (AST_Node *fun = functions; fun; fun = fun->next){
    generate_node(fun);
  }
 
  generate_program_end(main_name);
}






