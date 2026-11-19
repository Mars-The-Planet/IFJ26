/**
 * IFJ26 - Implementace překladače imperativního jazyka IFJ26
 * @file IFJ26/src/generator.c, projekt ifj
 * @author xmolikj00
 *
 * @brief generátor kódu pro projekt IFJ26
 *
 * POUŽITÍ AI -
 * Ai jsem použil pro "debatu" o implementaci některých funkcí,
 * promty vypadali tak, že jsem poslal funkci co jsem napsal a nebyl jsem si u
 * ní jistý zda je správně, popsal jsem svůj způsob uvažování a pokus o
 * implementaci a jak by to mělo fungovat, více u jednotlivých funkcí.
 *
 * MYŠLENKY FUNGOVÁNÍ GNERÁTORU:
 * 1. prochází strom pomocí generate_node rekurzivně
 * 2. pracuje hlavně se zásobníkem, pravidlo: každý uzel nechá po sobě na
 * zásobníku právě jednu hodnotu
 * 3. vytváří vždy unikátní návěští pomocí funkce get new label, čímž brání tomu
 * aby se návěští se stejným názvem definovalao dvakrát
 * 4. DEFVAR se vypisuje naráz pomocí collect vars, jelikož kdyby byla definice
 * v cyklu tak by se opakovala, pozn.: ještě jsem nevyřešil scope
 *
 * ROZŠÍŘENÍ:
 * FUNEXP, BOOLTHEN, CONTROL, LISTS
 *
 *
 * @date 31.10.2026
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "generator.h"

/**
 * @brief funkce pro určení posledního uzlu, používal jsem často tak jsem udělal
 * funkci
 */
AST_Node *last_node(AST_Node *node) {
  if (node == NULL) {
    return NULL;
  }
  while (node->next) {
    node = node->next;
  }
  return node;
}

/**
 * @brief funkce pro urování typu parametrů (nedokončená)
 */
dataType get_param_type(const char *func_name, int index) {
  (void)func_name;
  (void)index;
  return TYPE_UNDEF;
}

/**
 * @brief vrací nové unikátní číslo pro návěští (skoky)
 */

static int actual_label_number = 0;

// návratová hodnota funkce, inicializuje se na unit a volání funkce ji vždy
// přepíše a pak "vynuluje"
static dataType function_return_type = TYPE_UNIT;

/**
 * @brief funkce pro různé jumpy,
 */
static int get_new_label(void) { return actual_label_number++; }

/**
 * @brief funkce pro int z ifj26 do ifjcode26
 * @param value hodnota původního celého čísla v ifj26
 */
void literal_int(int value) { printf("PUSHS int@%d\n", value); }

/**
 * @brief pro převod float
 * @param value hodnota desetiného čísla
 */
void literal_double(double value) { printf("PUSHS float@%a\n", value); }

// opravit

void literal_string(char *string) { printf("PUSHS string@%s", string); }

/**
 * @brief převod typu boolean
 * @param value původní výraz před převodem, ovšem jako int jelikož se to lépe
 * předává
 */
void literal_bool(int value) {
  if (value) {
    printf("PUSHS bool@true\n");
  } else
    printf("PUSHS bool@false\n");
}

/**
 * @brief unit nic nenechává na zásobníku
 */
void literal_unit(void) {}

void ident(AST_Node *node) {
  if (node->result != TYPE_UNIT) {
    printf("PUSHS LF@%s\n", node->nodeData.idVarData.id_name);
  }
}

/**
 * @brief typová konverze z int na float
 */
void int_to_float(void) { printf("INT2FLOATS\n"); }

/**
 * @brief pomocná funkce pro generátor kódu pro relace, vygeneruje kód pro oba
 * operandy a pokud je potřeba provede konverzi z int na float
 *
 * jeden příklad funkce pomocí ai, původně jsem chtěl zjiťovat typ pomocí makra
 * _Generic, které jsem si dohledal na internetu když jsem nad tímto problémem
 * přemýšlel, nebyl jsem si ale úplně jistý jak funguje, tak jsem se zeptal ai
 * konkrétně Claude, zda je možné to použít v tomto případě, dostal jsem odpověď
 * že ne, jelikož makro by vzalo typ v C, nikoliv v mnou požadovaných jazycích.
 *
 * @param node ukazatel na uzel AST
 */
void get_operands(AST_Node *node) {

  dataType left = node->nodeData.biData.left->result;
  dataType right = node->nodeData.biData.right->result;
  generate_node(node->nodeData.biData.left);

  if (left == TYPE_INT && right == TYPE_DOUBLE) {
    int_to_float();
  }

  generate_node(node->nodeData.biData.right);

  if (right == TYPE_INT && left == TYPE_DOUBLE) {
    int_to_float();
  }
}

/**
 *   @brief pro operátor less
 */
void less(AST_Node *node) {
  get_operands(node);
  printf("LTS\n");
}

/**
 * @brief pro operátor greater
 */
void greater(AST_Node *node) {
  get_operands(node);
  printf("GTS\n");
}

/**
 * @brief pro operátor equals
 */
void generate_equals(AST_Node *node) {
  get_operands(node);
  printf("EQS\n");
}

/**
 * @brief pro operátor less_equal
 */

void less_equal(AST_Node *node) {
  get_operands(node);
  printf("GTS\nNOTS\n");
}

/**
 * @brief pro operátor greater_equal
 */
void greater_equal(AST_Node *node) {
  get_operands(node);
  printf("LTS\nNOTS\n");
}

/**
 * @brief pro operátor not_equal
 */
void not_equal(AST_Node *node) {
  get_operands(node);
  printf("EQS\nNOTS\n");
}

/**
 * @brief logické not
 * @param node ukazatel na uzel ass
 */
void logical_not(AST_Node *node) {
  generate_node(node->nodeData.uData.single);
  printf("NOTS\n");
}
/**
 * @brief struktura pro logické and, používá zkratkové vyhodnocování
 * @param node ukazatel na uzel
 */
void logical_and(AST_Node *node) {

  int label_number = get_new_label();

  generate_node(node->nodeData.biData.left);
  printf("PUSHS bool@false\n");
  printf("JUMPIFEQS $_and%d_false\n", label_number);

  generate_node(node->nodeData.biData.right);
  printf("PUSHS bool@false\n");
  printf("JUMPIFEQS $_and%d_false\n", label_number);

  printf("PUSHS bool@true\n");
  printf("JUMP $_and%d_end\n", label_number);

  printf("LABEL $_and%d_false\n", label_number);
  printf("PUSHS bool@false\n");
  printf("LABEL $_and%d_end\n", label_number);
}

/**
 * @brief struktura pro logické or, používá zkratkové vyhodnocování
 * @param node ukazatel na uzel
 */
void logical_or(AST_Node *node) {
  int label_number = get_new_label();

  generate_node(node->nodeData.biData.left);
  printf("PUSHS bool@true\n");
  printf("JUMPIFEQS $_or%d_true\n", label_number);

  generate_node(node->nodeData.biData.right);
  printf("PUSHS bool@true\n");
  printf("JUMPIFEQS $_or%d_true\n", label_number);

  printf("PUSHS bool@false\n");
  printf("JUMP $_or%d_end\n", label_number);

  printf("LABEL $_or%d_true\n", label_number);
  printf("PUSHS bool@true\n");
  printf("LABEL $_or%d_end\n", label_number);
}

/**
 * @brief funkce pro inarní plus
 * @param node ukazatel na uzelk ass
 */
void unar_plus(AST_Node *node) { generate_node(node->nodeData.uData.single); }

/**
 * @brief funkce pro sčítání a kokatenaci řetězců
 * @param node ukazatel na uzel
 */
void add(AST_Node *node) {
  get_operands(node);

  // stačí jeden, sémantika zkontroluj epředtím;
  if (node->nodeData.biData.left->result == TYPE_STRING) {
    printf("POPS GF@$t2\n");
    printf("POPS GF@$t1\n");

    // concat nemá zásobníkovou verzi
    printf("CONCAT GF@$t1 GF@$t1 GF@$t2\n");
    printf("PUSHS GF@$t1\n");
  } else {
    printf("ADDS\n");
  }
}

/**
 * @brief funkce pro odečítání
 * @param node ukazatel na uzel
 */
void sub(AST_Node *node) {
  get_operands(node);
  printf("SUBS\n");
}
/**
 * @brief funkce pro násobení
 * @param node ukazatel na uzel
 */
void mul(AST_Node *node) {
  get_operands(node);

  printf("MULS\n");
}

/**
 * @brief fuknce pro dělení
 * tady jsem s ai konzultoval zda převádět na float nebo ne, ai mě přesvědčilo
 * že chování scaly odpovídá nepřevádět na float a použít funkci DIVS
 * @param node ukazatel na uzel
 */
void divide(AST_Node *node) {
  get_operands(node);

  if (node->nodeData.biData.left->result == TYPE_DOUBLE ||
      node->nodeData.biData.right->result == TYPE_DOUBLE) {
    printf("DIVS\n");
  } else
    printf("IDIVS\n");
}

/**
 * @brief generátor bloků, například uvnitř cyklů atd..
 *
 * kvůli této částispecifikace:
 *
 * "Není-li proveden return, funkce vrátí výsledek svého těla. Výsledkem
 * jednořádkového těla je hodnota jeho výrazu či volání; výsledkem bloku
 * zakončeného výrazem či voláním je hod- nota tohoto posledního příkazu. Blok
 * zakončený definicí proměnné, přiřazením nebo cyklem má výsledek () (typu
 * Unit). Hodnoty z těla cyklu se nepřenášejí do výsledku nadřazeného bloku."
 *
 * výsledek zahazuje voláním POPS do dočasné proměnné
 * @param node ukazatel na uzel stromu
 */

void generate_block(AST_Node *node) {
  while (node) {
    generate_node(node);
    if (node->next && node->result != TYPE_UNIT) {

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
void generate_while(AST_Node *node) {
  int label_number = get_new_label();

  printf("LABEL $while%d_start\n", label_number);
  generate_node(node->nodeData.whileData.condition); //
  printf("PUSHS bool@false\n");
  printf("JUMPIFEQS $while%d_end\n", label_number);

  // vygeneruje blok příkazů uvnitř cyklu
  generate_block(node->nodeData.whileData.body);

  // zjistí typ posledního příkazu v bloku, pokud není unit, tak ho zahodí,
  // jelikož while si nepotřebuje uchovávat hodnotu z těla cyklu
  AST_Node *last = last_node(node->nodeData.whileData.body);
  if (last && last->result != TYPE_UNIT) {
    printf("POPS GF@$t1\n");
  }

  printf("JUMP $while%d_start\n", label_number);
  printf("LABEL $while%d_end\n", label_number);
}

/**
 * @brief funkce pro definicici proměnných
 * @param node ukazatel na uzel ast
 */
void var_def(AST_Node *node) {

  // pro zkrácení dlouhých názvů
  dataType var_type = node->nodeData.varData.varType;
  AST_Node *expression = node->nodeData.varData.expression;
  char *name = node->nodeData.varData.var_name;

  // pokud není typ proměnné deklarován vezme si ho z přiřazované hodnoty
  if (var_type == TYPE_UNDEF) {
    var_type = expression->result;
  }

  generate_node(expression);

  // pokud je uzel typu UNIT
  if (var_type == TYPE_UNIT) {
    printf("MOVE LF@%s nil@nil\n", name);
    return;
  }

  // datová konverze
  if (var_type == TYPE_DOUBLE && expression->result == TYPE_INT) {
    int_to_float();
  }
  printf("POPS LF@%s\n", name);
}

/**
 * @brief funkce pro definicici proměnných
 * @param node ukazatel na uzel ast
 */
void assign(AST_Node *node) {
  // pro zkrácení názvů
  char *name = node->nodeData.assignData.var_name;
  AST_Node *expression = node->nodeData.assignData.equal;

  dataType var_type = expression->result;

  generate_node(expression);

  // pokud je uzel typu UNIT
  if (var_type == TYPE_UNIT) {
    printf("MOVE LF@%s nil@nil\n", name);
    return;
  }

  if (var_type == TYPE_DOUBLE && expression->result == TYPE_INT) {
    int_to_float();
  }

  printf("POPS LF@%s\n", name);
}

/**
 * @brief unární minus, násobí -1, využívá dříve definované funkce
 * @param node ukazatel na uzel ast
 */
void unar_minus(AST_Node *node) {
  generate_node(node->nodeData.uData.single);
  if (node->nodeData.uData.single->result == TYPE_DOUBLE) {
    literal_double(-1.0);
  } else {
    literal_int(-1);
  }
  printf("MULS\n");
}

/**
 * @brief funcke pro generování větve pro if
 * @param block ukazatel na blok ve funkci
 * @param return_type požadovaný návratový typ funkce
 */
void generate_branch(AST_Node *block, dataType return_type) {
  generate_block(block);

  AST_Node *last = last_node(block);

  if (last == NULL) {
    return;
  }

  if (return_type == TYPE_UNIT) {
    if (last->result != TYPE_UNIT) {
      printf("POPS GF@$t1\n");
    }
  } else if (return_type == TYPE_DOUBLE && last->result == TYPE_INT) {
    int_to_float();
  }
}
/**
 * @brief pro uzel if, počítá s rozšířením control a uchovává podle toho
 * návratové typy
 * @param node ukazatel na uzel ast
 */
void generate_if(AST_Node *node) {

  int label_number = get_new_label();

  dataType return_type = node->result;
  if (node->nodeData.ifData.else_part == NULL) {
    return_type = TYPE_UNIT;
  }

  generate_node(node->nodeData.ifData.condition);
  printf("PUSHS bool@false\n");
  printf("JUMPIFEQS $if%d_else_part\n", label_number);

  generate_branch(node->nodeData.ifData.then_part, return_type);
  if (node->nodeData.ifData.else_part) {
    printf("JUMP $if%d_end\n", label_number);
    printf("LABEL $if%d_else_part\n", label_number);
    generate_branch(node->nodeData.ifData.else_part, return_type);
    printf("LABEL $if%d_end\n", label_number);
  }

  else {
    printf("LABEL $if%d_else_part\n", label_number);
  }
}

/**
 * @brief pro generování return
 * @param node ukazatel na uzel ast
 * @param function_return_type není přímo parametr ale využívá globální
 * proměnou, která se nastavuje právě kvůli této funkci
 */
void generate_return(AST_Node *node) {
  AST_Node *value = node->nodeData.returnData.expression;

  // pokud má nějaku hodnotu vygeneruje pro ni uzel
  if (value) {
    generate_node(value);
    // pokud funkce nemá vracet hodnotu a nějaká ji přece jen vyjde,
    // tak ji zahodí do pomocné proměnné
    if (function_return_type == TYPE_UNIT) {
      if (value->result != TYPE_UNIT) {
        printf("POPS GF@$t1\n");
      }
    } else if (function_return_type == TYPE_DOUBLE &&
               value->result == TYPE_INT) {
      int_to_float();
    }
  }

  if (function_return_type == TYPE_UNIT) {
    printf("PUSHS nil@nil\n");
  }

  printf("POPFRAME\n");
  printf("RETURN\n");
}

/**
 * @brief sbírá definici proměnných, jelikož mohou být definované jen jednou,
 * nemůžou být uvnitř cyklů, proměnné se definují hned při začátku funkce
 *
 * @param node ukazatel na uzel ast
 */
void collect_vars(AST_Node *node) {
  while (node) {
    switch (node->type) {

    case VAR_DEF:
      printf("DEFVAR LF@%s\n", node->nodeData.varData.var_name);
      collect_vars(node->nodeData.varData.expression);
      break;

    case ASSIGN:
      collect_vars(node->nodeData.assignData.equal);
      break;

    case IF_N:
      collect_vars(node->nodeData.ifData.condition);
      collect_vars(node->nodeData.ifData.then_part);
      collect_vars(node->nodeData.ifData.else_part);
      break;
      // TO DO: CASE_FOR_N
    case WHILE_N:
      collect_vars(node->nodeData.whileData.condition);
      collect_vars(node->nodeData.whileData.body);
      break;

    // pro binární
    case ADD:
    case SUB:
    case MUL:
    case DIV:
    case LESS:
    case GREATER:
    case EQUALS:
    case NOT_EQUAL:
    case LESS_EQUAL:
    case GREATER_EQUAL:
    case AND_N:
    case OR_N:
      collect_vars(node->nodeData.biData.left);
      collect_vars(node->nodeData.biData.right);
      break;

    // pro unární
    case UMINUS:
    case UPLUS:
    case NOT_N:
      collect_vars(node->nodeData.uData.single);
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
 * @brief vygeneruje všechny argumenty zleva doprava
 */
void generate_arguments(AST_Node *arguments) {
  while (arguments) {
    generate_node(arguments);
    arguments = arguments->next;
  }
}

/**
 * @brief funkce pro vybírání parametrů ze zásobníku
 * používá rekurzi aby byl pravý parametr nejhlouběji a levý nejvíce navrchu
 * @param params ukazatel na hlavičku listu parametrů funkce
 */
void pop_params(ListParam *params) {
  if (params == NULL) {
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
void generate_fun(AST_Node *node) {
  AST_Node *body = node->nodeData.funData.body;
  AST_Node *function_return = last_node(body);
  function_return_type = node->nodeData.funData.return_type;

  if (function_return_type == TYPE_UNDEF) {
    if (function_return != NULL) {
      function_return_type = function_return->result;
    } else {
      function_return_type = TYPE_UNIT;
    }
  }

  printf("LABEL $function$%s\n", node->nodeData.funData.fun_name);
  printf("CREATEFRAME\n");
  printf("PUSHFRAME\n");

  head *params = node->nodeData.funData.param_list;
  if (params) {
    for (ListParam *p = params->first; p; p = p->next) {
      printf("DEFVAR LF@%s\n", p->param_name);
    }
    pop_params(params->first);
  }

  // sesbírá všechny proměnné
  collect_vars(body);

  // vygeneruje tělo funkce
  generate_block(body);

  if (function_return_type == TYPE_UNIT) {

    if (function_return &&
        function_return->result !=
            TYPE_UNIT) { // tělo skončilo hodnotou (např. voláním funkce), ale
                         // funkce má vracet Unit
      printf("POPS GF@$t1\n"); // hodnotu zahoď
    }
    printf("PUSHS nil@nil\n");
  } else if (function_return_type == TYPE_DOUBLE && function_return &&
             function_return->result == TYPE_INT) {
    int_to_float();
  }

  printf("POPFRAME\n");
  printf("RETURN\n");

  function_return_type = TYPE_UNIT;
}

/**
 * @brief vestavěná funkce readInt
 */
void read_int() {
  int label_number = get_new_label();
  printf("CREATEFRAME\n"
         "PUSHFRAME\n"
         "DEFVAR LF@$$input\n"      // načtená hodnota
         "DEFVAR LF@$$comparator\n" // pomocná hodnota pro porovnání
         "READ LF@$$input int\n"
         "JUMPIFEQ $readInt%d_wrong LF@$$input nil@nil\n"
         "GT LF@$$comparator LF@$$input int@2147483647\n"
         "JUMPIFEQ $readInt%d_wrong LF@$$comparator bool@true\n"
         "LT LF@$$comparator LF@$$input int@-2147483648\n"
         "JUMPIFEQ $readInt%d_wrong LF@$$comparator bool@true\n"
         "JUMP $readInt%d_ok\n"
         "LABEL $readInt%d_wrong\n"
         "MOVE LF@$$input int@0\n"
         "LABEL $readInt%d_ok\n"
         "PUSHS LF@$$input\n"
         "POPFRAME\n",
         label_number, label_number, label_number, label_number, label_number,
         label_number);
}

/**
 * @brief vestavěná funkce readDouble, při chybě vrací 0.0
 */
void read_double() {
  int label_number = get_new_label();
  printf("CREATEFRAME\n"
         "PUSHFRAME\n"
         "DEFVAR LF@$$input\n"
         "READ LF@$$input float\n"
         "JUMPIFNEQ $readDouble%d_end LF@$$input nil@nil\n"
         "MOVE LF@$$input float@%a\n"
         "LABEL $readDouble%d_end\n"
         "PUSHS LF@$$input\n"
         "POPFRAME\n",
         label_number, 0.0, label_number);
}

/**
 * @brief vestavěná funkce readLine, při EOF vrací prázdný řetězec
 */
void read_line() {
  int label_number = get_new_label();
  printf("CREATEFRAME\n"
         "PUSHFRAME\n"
         "DEFVAR LF@$$input\n"
         "READ LF@$$input string\n"
         "JUMPIFNEQ $readLine%d_end LF@$$input nil@nil\n"
         "MOVE LF@$$input string@\n"
         "LABEL $readLine%d_end\n"
         "PUSHS LF@$$input\n"
         "POPFRAME\n",
         label_number, label_number);
}

/**
 * @brief vestavěná funkce readBoolean, při chybě vrací false
 */
void read_boolean() {
  int label_number = get_new_label();
  printf("CREATEFRAME\n"
         "PUSHFRAME\n"
         "DEFVAR LF@$$input\n"
         "READ LF@$$input bool\n"
         "JUMPIFNEQ $readBoolean%d_end LF@$$input nil@nil\n"
         "MOVE LF@$$input bool@false\n"
         "LABEL $readBoolean%d_end\n"
         "PUSHS LF@$$input\n"
         "POPFRAME\n",
         label_number, label_number);
}

/**
 * @brief vestavěná funkce ord(string, index), argumenty už jsou na
 * zásobníku, vrací kód znaku, při indexu mimo rozsah -1
 */
void ord() {
  int label_number = get_new_label();
  printf("CREATEFRAME\n"
         "PUSHFRAME\n"
         "DEFVAR LF@$$string\n"
         "DEFVAR LF@$$index\n"
         "DEFVAR LF@$$len\n"
         "DEFVAR LF@$$comparator\n"
         "DEFVAR LF@$$result\n"
         "POPS LF@$$index\n"
         "POPS LF@$$string\n"
         "STRLEN LF@$$len LF@$$string\n"
         "LT LF@$$comparator LF@$$index int@0\n"
         "JUMPIFEQ $ord%d_wrong LF@$$comparator bool@true\n"
         "LT LF@$$comparator LF@$$index LF@$$len\n"
         "JUMPIFEQ $ord%d_wrong LF@$$comparator bool@false\n"
         "STRI2INT LF@$$result LF@$$string LF@$$index\n"
         "JUMP $ord%d_end\n"
         "LABEL $ord%d_wrong\n"
         "MOVE LF@$$result int@-1\n"
         "LABEL $ord%d_end\n"
         "PUSHS LF@$$result\n"
         "POPFRAME\n",
         label_number, label_number, label_number, label_number, label_number);
}

/**
 * @brief vestavěná funkce subStr(string, start, end), argumenty už jsou na
 * zásobníku, neplatné meze = chyba 40
 */
void sub_str() {
  int label_number = get_new_label();
  printf("CREATEFRAME\n"
         "PUSHFRAME\n"
         "DEFVAR LF@$$string\n"
         "DEFVAR LF@$$start\n"
         "DEFVAR LF@$$end\n"
         "DEFVAR LF@$$len\n"
         "DEFVAR LF@$$comparator\n"
         "DEFVAR LF@$$result\n"
         "DEFVAR LF@$$char\n"
         "POPS LF@$$end\n"
         "POPS LF@$$start\n"
         "POPS LF@$$string\n"
         "STRLEN LF@$$len LF@$$string\n"
         "LT LF@$$comparator LF@$$start int@0\n"
         "JUMPIFEQ $subStr%d_error LF@$$comparator bool@true\n"
         "LT LF@$$comparator LF@$$end int@0\n"
         "JUMPIFEQ $subStr%d_error LF@$$comparator bool@true\n"
         "GT LF@$$comparator LF@$$start LF@$$end\n"
         "JUMPIFEQ $subStr%d_error LF@$$comparator bool@true\n"
         "LT LF@$$comparator LF@$$start LF@$$len\n"
         "JUMPIFEQ $subStr%d_error LF@$$comparator bool@false\n"
         "GT LF@$$comparator LF@$$end LF@$$len\n"
         "JUMPIFEQ $subStr%d_error LF@$$comparator bool@true\n"
         "MOVE LF@$$result string@\n"
         "LABEL $subStr%d_loop\n"
         "JUMPIFEQ $subStr%d_done LF@$$start LF@$$end\n"
         "GETCHAR LF@$$char LF@$$string LF@$$start\n"
         "CONCAT LF@$$result LF@$$result LF@$$char\n"
         "ADD LF@$$start LF@$$start int@1\n"
         "JUMP $subStr%d_loop\n"
         "LABEL $subStr%d_done\n"
         "PUSHS LF@$$result\n"
         "POPFRAME\n"
         "JUMP $subStr%d_after\n"
         "LABEL $subStr%d_error\n"
         "EXIT int@40\n"
         "LABEL $subStr%d_after\n",
         label_number, label_number, label_number, label_number, label_number,
         label_number, label_number, label_number, label_number, label_number,
         label_number, label_number);
}

/**
 * @brief vestavěná funkce toint
 */
void to_int(AST_Node *arguments) {
  generate_node(arguments);
  printf("FLOAT2INTS\n");
}

/**
 * @brief pro uživatelské funkce
 */
void call_user_function(AST_Node *node, char *name, AST_Node *arguments) {
  int index = 0;
  while (arguments) {
    generate_node(arguments);
    dataType param_type = get_param_type(name, index);

    if (arguments->result == TYPE_UNIT) {

      printf("PUSHS nil@nil\n");
    } else if (param_type == TYPE_DOUBLE && arguments->result == TYPE_INT) {
      int_to_float();
    }

    arguments = arguments->next;
    index++;
  }
  printf("CALL $function$%s\n", name);

  // funkce vrací vždy jednu hodnotu, výsledek typu Unit se zahodí
  if (node->result == TYPE_UNIT) {
    printf("POPS GF@$t1\n");
  }
}

/**
 * @brief pro vestavěnou funkci print
 */
void print(AST_Node *arguments) {
  generate_node(arguments);
  if (arguments->result == TYPE_UNIT) {
    printf("WRITE string@()\n");
  } else {
    printf("POPS GF@$t1\n");
    printf("WRITE GF@$t1\n");
  }
}
/**
 * @brief vestavěná funkce chr
 */
void chr(AST_Node *arguments) {
  generate_node(arguments);
  printf("INT2CHARS\n");
}

/**
 * @brief vestavěná funkce strlen
 */
void str_length(AST_Node *arguments) {
  generate_node(arguments);
  printf("POPS GF@$t1\n");
  printf("STRLEN GF@$t1 GF@$t1\n");
  printf("PUSHS GF@$t1\n");
}

/**
 * @brief vestavěná funkce floor , argument už je na zásobníku.
 * FLOAT2INT usekává k nule, u záporných necelých čísel se musí odečíst 1
 */
void builtin_floor() {
  int label_number = get_new_label();
  printf("CREATEFRAME\n"
         "PUSHFRAME\n"
         "DEFVAR LF@$$value\n"
         "DEFVAR LF@$$truncated_int\n"
         "DEFVAR LF@$$truncated_float\n"
         "DEFVAR LF@$$comparator\n"
         "POPS LF@$$value\n"
         "FLOAT2INT LF@$$truncated_int LF@$$value\n"
         "INT2FLOAT LF@$$truncated_float LF@$$truncated_int\n"
         "GT LF@$$comparator LF@$$truncated_float LF@$$value\n"
         "JUMPIFEQ $floor%d_end LF@$$comparator bool@false\n"
         "SUB LF@$$truncated_float LF@$$truncated_float float@%a\n"
         "LABEL $floor%d_end\n"
         "PUSHS LF@$$truncated_float\n"
         "POPFRAME\n",
         label_number, 1.0, label_number);
}
/**
 * @brief vestavěná funkce toString, převede řetězec na string
 * @param type typ vstupního řetězce
 * @param arguments převáděný řetězec, neboli argument vestavěnné funkce
 * toString
 */
void to_string(dataType type, AST_Node *arguments) {
  generate_node(arguments);
  switch (type) {
  case TYPE_INT:
    printf("INT2STRS\n");
    break;

  case TYPE_DOUBLE:
    printf("FLOAT2STRS\n");
    break;

  case TYPE_STRING:
    break;

  case TYPE_BOOL: {
    int label_number = get_new_label();
    printf("PUSHS bool@true\n");
    printf("JUMPIFEQS $str%d_true\n", label_number);
    printf("PUSHS string@false\n");
    printf("JUMP $str%d_end\n", label_number);
    printf("LABEL $str%d_true\n", label_number);
    printf("PUSHS string@true\n");
    printf("LABEL $str%d_end\n", label_number);
    break;
  }

  case TYPE_UNIT:
    printf("PUSHS string@()\n");
    break;
  default:
    break;
  }
}

/**
 * @brief pro volání funkcí
 * @param node ukazatel na funkci kterou budeme volat
 */
void call_fun(AST_Node *node) {
  char *name = node->nodeData.callFunData.call_fun_name;
  AST_Node *arguments = node->nodeData.callFunData.fun_args;

  if (strcmp(name, "print") == 0) {
    print(arguments);
  } else

      if (strcmp(name, "readInt") == 0) {
    read_int();
  } else

      if (strcmp(name, "readDouble") == 0) {
    read_double();
  } else

      if (strcmp(name, "readLine") == 0) {
    read_line();
  } else

      if (strcmp(name, "readBoolean") == 0) {
    read_boolean();
  } else

      if (strcmp(name, "toInt") == 0) {
    to_int(arguments);
  } else

      if (strcmp(name, "toStr") == 0) {
    to_string(arguments->result, arguments);
  } else

      if (strcmp(name, "chr") == 0) {
    chr(arguments);
  } else

      if (strcmp(name, "strLength") == 0) {
    str_length(arguments);
  } else

      if (strcmp(name, "ord") == 0) {
    generate_arguments(arguments);
    ord();
  } else

      if (strcmp(name, "subStr") == 0) {
    generate_arguments(arguments);
    sub_str();
  } else

  {
    call_user_function(node, name, arguments);
  }
}

/**
 * @brief generátor kódu pro jednotlivé uzly AST
 *  používá switch a prokaždý uzel podle jeho typu zavolá funkci která
 * vygeneruje kód pro daný uzel
 * @param node ukazatel na uzel AST
 */

void generate_node(AST_Node *node) {
  if (node == NULL) {
    return;
  }
  switch (node->type) {
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
    generate_equals(node);
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

    // todo: doplnit další uzly AST

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
    call_fun(node);
    break;

  default:
    break;
  }
}

/**
 * @brief hlavní funkce, spouští celý program
 * @param functions linked list který dodá parser
 * @param main_name jméno mainu
 */
void generate_program(AST_Node *functions, const char *main_name) {
  printf(".IFJcode26\n");
  printf("DEFVAR GF@$t1\n"); // vytvoří první pomocnou proměnou kterou využívá
                             // několik funkcí
  printf("DEFVAR GF@$t2\n"); // vytvoří druhou pomocnou proměnnou
  printf("JUMP $program_start\n"); // přeskoč definice funkcí

  while (functions) {
    generate_node(functions); // uzel FUN_DEF -> generate_fun
    functions = functions->next;
  }

  printf("LABEL $program_start\n"); // sem skočí první JUMP
  printf("CALL $function$%s\n", main_name);
  printf("EXIT int@0\n");
}