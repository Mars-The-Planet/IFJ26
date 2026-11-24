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
 * v cyklu, tak by se opakovala, pozn.: ještě jsem nevyřešil scope
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
#include "symtable.h"

/**list funkcí */
static AST_Node *function_list = NULL;

/**
 * @brief funkce pro určení posledního uzlu, používal jsem často tak jsem
 * udělal funkci
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
 * @brief zjistí, jestli uzel po sobě nechá na zásobníku hodnotu
 * definice proměnné, přiřazení a cykly nenechávají nikdy nic, bez ohledu na to,
 * co je v result, if bez else také ne
 */
static int leaves_value(AST_Node *node) {
  if (node == NULL) {
    return 0;
  }
  switch (node->type) {
  case VAR_DEF:
  case ASSIGN:
  case WHILE_N:
  case FOR_N:
  case LITERAL_UNIT:
    return 0;
  case IF_N:
    if (node->nodeData.ifData.else_part == NULL) {
      return 0;
    }
    return node->result != TYPE_UNIT;
  default:
    return node->result != TYPE_UNIT;
  }
}

/**
 * @brief typ i-tého parametru funkce (od nuly), hledá se v seznamu funkcí
 * @return typ parametru, nebo TYPE_UNDEF (funkce neexistuje / vestavěná)
 */
dataType get_param_type(const char *func_name, int index) {
  for (AST_Node *fun = function_list; fun; fun = fun->next) {
    if (fun->type == FUN_DEF &&
        strcmp(fun->nodeData.funData.fun_name, func_name) == 0) {

      head *params = fun->nodeData.funData.param_list;
      ListParam *param;

      if (params) {
        param = params->first;
      } else {
        param = NULL;
      }

      for (int i = 0; param && i < index; i++) {
        param = param->next;
      }
      if (param) {
        return param->type;
      } else {
        return TYPE_UNDEF;
      }
    }
  }
  return TYPE_UNDEF;
}

/**
 * @brief informace o jedné proměnné (nebo parametru funkce)
 */
typedef struct VariableInfo {
  const char *original_name; // jméno ve zdrojovém kódu
  char *unique_name;         // jméno v IFJcode26, unikátní v rámci funkce
  dataType type;             // typ proměnné
  dataType element_type;     // typ prvků, pokud je proměnná seznam
  int function_id;           // ve které funkci proměnná je
  int scope_depth;           // hloubka bloku, ve kterém je definovaná
  bool visible;              // false, jakmile blok skončí
} VariableInfo;

static VariableInfo *all_variables = NULL;
static int all_variables_count = 0;
static int all_variables_capacity = 0;

static int current_function_id = 0;
static int next_function_id = 1;
static int current_scope_depth = 0;

// stav zjišťování typů funkcí: 0 = ještě ne, 1 = právě probíhá, 2 = hotovo
static unsigned char *function_states = NULL;
static int function_count = 0;

/**
 * @brief kopie řetězce (strdup není v striktním C11 dostupný)
 */
static char *copy_string(const char *source) {
  size_t length = strlen(source) + 1;
  char *copy = malloc(length);
  if (copy == NULL) {
    fprintf(stderr, "generátor: chyba alokace paměti při copy_string!\n");
    exit(99);
  }
  memcpy(copy, source, length);
  return copy;
}

/**
 * @brief najde uživatelskou funkci podle jména
 * @return uzel FUN_DEF, nebo NULL (vestavěná funkce / neexistuje)
 */
static AST_Node *find_function(const char *name) {
  for (AST_Node *fun = function_list; fun; fun = fun->next) {
    if (fun->type == FUN_DEF &&
        strcmp(fun->nodeData.funData.fun_name, name) == 0) {
      return fun;
    }
  }
  return NULL;
}

/**
 * @brief otevře nový blok (tělo if, while, for)
 */
static void scope_open(void) { current_scope_depth++; }

/**
 * @brief zavře blok, proměnné z něj už nejsou vidět
 */
static void scope_close(void) {
  for (int i = 0; i < all_variables_count; i++) {
    if (all_variables[i].function_id == current_function_id &&
        all_variables[i].scope_depth == current_scope_depth) {
      all_variables[i].visible = false;
    }
  }
  current_scope_depth--;
}

/**
 * @brief je jméno už použité jako unikátní jméno v aktuální funkci?
 */
static bool unique_name_in_use(const char *name) {
  for (int i = 0; i < all_variables_count; i++) {
    if (all_variables[i].function_id == current_function_id &&
        strcmp(all_variables[i].unique_name, name) == 0) {
      return true;
    }
  }
  return false;
}

/**
 * @brief vytvoří jméno, které se v aktuální funkci ještě nepoužilo
 */
static char *make_unique_name(const char *original_name) {
  if (!unique_name_in_use(original_name)) {
    return copy_string(original_name);
  }

  char candidate[512];
  int suffix = 2;
  while (true) {
    int written =
        snprintf(candidate, sizeof(candidate), "%s$%d", original_name, suffix);
    (void)written;
    if (!unique_name_in_use(candidate)) {
      return copy_string(candidate);
    }
    suffix++;
  }
}

/**
 * @brief zaregistruje proměnnou v aktuálním bloku
 * @return ukazatel na záznam (platí jen do další registrace)
 */
static VariableInfo *declare_variable(const char *original_name, dataType type,
                                      dataType element_type) {
  if (all_variables_count == all_variables_capacity) {
    if (all_variables_capacity == 0) {
      all_variables_capacity = 64;
    } else {
      all_variables_capacity = all_variables_capacity * 2;
    }
    VariableInfo *bigger = realloc(
        all_variables, (size_t)all_variables_capacity * sizeof(VariableInfo));
    if (bigger == NULL) {
      fprintf(stderr, "ERROR: Memory allocation failed!\n");
      exit(99);
    }
    all_variables = bigger;
  }
  VariableInfo *info = &all_variables[all_variables_count];
  all_variables_count++;

  info->original_name = original_name;
  info->unique_name = make_unique_name(original_name);
  info->type = type;
  info->element_type = element_type;
  info->function_id = current_function_id;
  info->scope_depth = current_scope_depth;
  info->visible = true;
  return info;
}

/**
 * @brief najde proměnnou podle jména ze zdrojáku mezi viditelnými
 * (nejvnitřnější definice má přednost)
 */
static VariableInfo *find_visible_variable(const char *name) {
  for (int i = all_variables_count - 1; i >= 0; i--) {
    if (all_variables[i].visible &&
        all_variables[i].function_id == current_function_id &&
        strcmp(all_variables[i].original_name, name) == 0) {
      return &all_variables[i];
    }
  }
  return NULL;
}

/**
 * @brief najde proměnnou podle ukazatele na její unikátní jméno
 * (uzly stromu po průchodu ukazují přímo na tento řetězec)
 */
static VariableInfo *find_variable_by_unique(const char *unique_name) {
  for (int i = 0; i < all_variables_count; i++) {
    if (all_variables[i].unique_name == unique_name) {
      return &all_variables[i];
    }
  }
  return NULL;
}

/**
 * @brief výsledný typ aritmetické operace: Double, pokud je některý operand
 * Double, jinak Int
 */
static dataType unify_numeric(dataType left, dataType right) {
  if (left == TYPE_DOUBLE || right == TYPE_DOUBLE) {
    return TYPE_DOUBLE;
  }
  if (left == TYPE_UNDEF) {
    return right;
  }
  return left;
}
// opravit
static void infer_node(AST_Node *node);
static void infer_function(AST_Node *fun);

/**
 * @brief návratový typ uživatelské funkce, pokud není uvedený, dopočítá se z
 * těla funkce (při rekurzi bez uvedeného typu zůstane TYPE_UNDEF)
 */
static dataType user_function_return_type(AST_Node *fun) {
  infer_function(fun);
  return fun->nodeData.funData.return_type;
}

/**
 * @brief projde posloupnost příkazů a vrátí typ hodnoty bloku: typ posledního
 * příkazu, nebo Unit, pokud po sobě poslední příkaz hodnotu nenechává
 */
static dataType infer_block(AST_Node *first) {
  dataType block_type = TYPE_UNIT;
  for (AST_Node *node = first; node; node = node->next) {
    infer_node(node);
    if (leaves_value(node)) {
      block_type = node->result;
    } else {
      block_type = TYPE_UNIT;
    }
  }
  return block_type;
}

/**
 * @brief doplní result do uzlu a všech jeho potomků a přejmenuje proměnné
 */
static void infer_node(AST_Node *node) {
  if (node == NULL) {
    return;
  }

  switch (node->type) {
  case LITERAL_INT:
    node->result = TYPE_INT;
    break;

  case LITERAL_DOUBLE:
    node->result = TYPE_DOUBLE;
    break;

  case LITERAL_STRING:
    node->result = TYPE_STRING;
    break;

  case LITERAL_BOOL:
    node->result = TYPE_BOOL;
    break;

  case LITERAL_UNIT:
    node->result = TYPE_UNIT;
    break;

  case IDENT: {
    VariableInfo *info =
        find_visible_variable(node->nodeData.idVarData.id_name);
    if (info) {
      node->nodeData.idVarData.id_name = info->unique_name;
      node->result = info->type;
    } else {
      node->result = TYPE_UNDEF;
    }
    break;
  }

  case ADD:
    infer_node(node->nodeData.biData.left);
    infer_node(node->nodeData.biData.right);
    if (node->nodeData.biData.left->result == TYPE_STRING ||
        node->nodeData.biData.right->result == TYPE_STRING) {
      node->result = TYPE_STRING;
    } else {
      node->result = unify_numeric(node->nodeData.biData.left->result,
                                   node->nodeData.biData.right->result);
    }
    break;

  case SUB:
  case MUL:
  case DIV:
    infer_node(node->nodeData.biData.left);
    infer_node(node->nodeData.biData.right);
    node->result = unify_numeric(node->nodeData.biData.left->result,
                                 node->nodeData.biData.right->result);
    break;

  case LESS:
  case LESS_EQUAL:
  case GREATER:
  case GREATER_EQUAL:
  case EQUALS:
  case NOT_EQUAL:
  case AND_N:
  case OR_N:
    infer_node(node->nodeData.biData.left);
    infer_node(node->nodeData.biData.right);
    node->result = TYPE_BOOL;
    break;

  case NOT_N:
    infer_node(node->nodeData.uData.single);
    node->result = TYPE_BOOL;
    break;

  case UMINUS:
  case UPLUS:
    infer_node(node->nodeData.uData.single);
    node->result = node->nodeData.uData.single->result;
    break;

  case FUN_CALL: {
    for (AST_Node *argument = node->nodeData.callFunData.fun_args; argument;
         argument = argument->next) {
      infer_node(argument);
    }

    char *name = node->nodeData.callFunData.call_fun_name;

    AST_Node *fun = find_function(name);
    if (fun) {
      node->result = user_function_return_type(fun);
    } 
    }
    break;
  }

  case VAR_DEF: {
    AST_Node *expression = node->nodeData.varData.expression;
    infer_node(expression);

    dataType declared_type = node->nodeData.varData.varType;
    dataType variable_type = declared_type;
    dataType element_type = TYPE_UNDEF;

    // typ není uveden, vezme se z výrazu
    if (variable_type == TYPE_UNDEF && expression) {
      variable_type = expression->result;
    }

    VariableInfo *info = declare_variable(node->nodeData.varData.var_name,
                                          variable_type, element_type);
    node->nodeData.varData.var_name = info->unique_name;
    node->nodeData.varData.varType = variable_type;
    node->result = TYPE_UNIT;
    break;
  }

  case ASSIGN: {
    infer_node(node->nodeData.assignData.equal);
    VariableInfo *info =
        find_visible_variable(node->nodeData.assignData.var_name);
    if (info) {
      node->nodeData.assignData.var_name = info->unique_name;
    }
    node->result = TYPE_UNIT;
    break;
  }

  case RETURN: {
    AST_Node *expression = node->nodeData.returnData.expression;
    infer_node(expression);
    if (expression) {
      node->result = expression->result;
    } else {
      node->result = TYPE_UNIT;
    }
    break;
  }

  case IF_N: {
    infer_node(node->nodeData.ifData.condition);

    scope_open();
    dataType then_type = infer_block(node->nodeData.ifData.then_part);
    scope_close();

    if (node->nodeData.ifData.else_part == NULL) {
      node->result = TYPE_UNIT;
      break;
    }

    scope_open();
    dataType else_type = infer_block(node->nodeData.ifData.else_part);
    scope_close();

    if (then_type == else_type) {
      node->result = then_type;
    } else if ((then_type == TYPE_INT && else_type == TYPE_DOUBLE) ||
               (then_type == TYPE_DOUBLE && else_type == TYPE_INT)) {
      node->result = TYPE_DOUBLE;
    } else if (then_type == TYPE_UNIT || else_type == TYPE_UNIT) {
      node->result = TYPE_UNIT;
    } else {
      node->result = then_type;
    }
    break;
  }

  case WHILE_N:
    infer_node(node->nodeData.whileData.condition);
    scope_open();
    infer_block(node->nodeData.whileData.body);
    scope_close();
    node->result = TYPE_UNIT;
    break;

  default:
    break;
  }
}

/**
 * @brief projde jednu funkci (parametry, tělo) a dopočítá její návratový typ
 * pokud není uvedený. Funkce se projde nejvýše jednou, volá se i při
 * zjišťování typu volání (volaná funkce se projde dřív než volající).
 */
static void infer_function(AST_Node *fun) {
  int index = 0;
  for (AST_Node *current = function_list; current && current != fun;
       current = current->next) {
    index++;
  }
  if (index >= function_count || function_states[index] != 0) {
    return;
  }
  function_states[index] = 1;

  // rozpracovaná funkce se uloží, protože se může projít uprostřed jiné
  int saved_function_id = current_function_id;
  int saved_scope_depth = current_scope_depth;
  current_function_id = next_function_id;
  next_function_id++;
  current_scope_depth = 0;

  head *params = fun->nodeData.funData.param_list;
  if (params) {
    for (ListParam *param = params->first; param; param = param->next) {
      declare_variable(param->param_name, param->type, param->list_type);
    }
  }

  dataType body_type = infer_block(fun->nodeData.funData.body);

  if (fun->nodeData.funData.return_type == TYPE_UNDEF) {
    fun->nodeData.funData.return_type = body_type;
  }

  function_states[index] = 2;
  current_function_id = saved_function_id;
  current_scope_depth = saved_scope_depth;
}

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

/**
 * @brief string
 *
 * pužití ai, zeptal jsem se jak udělat funkci na string která bere všechny
 * znaky, přečetl jsem si jak to funguje a při druhém odevzdání doplním
 * komentáře
 *
 * @param string string
 */
void literal_string(char *string) {
  printf("PUSHS string@");
  for (unsigned char *c = (unsigned char *)string; *c; c++) {
    if (*c <= 32 || *c == 35 || *c == 92) {
      printf("\\%03d", *c);
    } else {
      putchar(*c);
    }
  }
  printf("\n");
}

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
 * jeden příklad funkce pomocí ai, původně jsem chtěl zjiťovat typ pomocí
 * makra _Generic, které jsem si dohledal na internetu když jsem nad tímto
 * problémem přemýšlel, nebyl jsem si ale úplně jistý jak funguje, tak jsem se
 * zeptal ai konkrétně Claude, zda je možné to použít v tomto případě, dostal
 * jsem odpověď že ne, jelikož makro by vzalo typ v C, nikoliv v mnou
 * požadovaných jazycích.
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
    if (node->next && leaves_value(node)) {

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
  if (last && leaves_value(last)) {
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

  generate_node(expression);

  // pokud je uzel typu UNIT
  if (!leaves_value(expression)) {
    printf("MOVE LF@%s nil@nil\n", name);
    return;
  }

  // typ proměnné se hledá v tabulce proměnných (jména jsou unikátní)
  VariableInfo *variable = find_variable_by_unique(name);
  if (variable && variable->type == TYPE_DOUBLE &&
      expression->result == TYPE_INT) {
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
    if (leaves_value(last)) {
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
 * @brief reprezentace seznamu jako pole na indexu 0 délka, dál ostatní prvky
 */
void generate_list(AST_Node *node) {
  AST_Node *list_head = node->nodeData.listData.list_var;

  int element_count = 0;
  AST_Node *current_element = list_head;
  while (current_element) {
    generate_node(current_element);
    element_count++;
    current_element = current_element->next;
  }

  printf("NEWARRAY GF@$t1 int@%d\n", element_count + 1);
  printf("ARRAYSET GF@$t1 int@0 int@%d\n", element_count);

  for (int i = element_count; i >= 1; i--) {
    printf("POPS GF@$t2\n");
    printf("ARRAYSET GF@$t1 int@%d GF@$t2\n", i);
  }

  printf("PUSHS GF@$t1\n");
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
      if (leaves_value(value)) {
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

  // návratový typ není uvedený, vezme se z posledního příkazu těla,
  // ale jen pokud po sobě nechává hodnotu (jinak je to Unit)
  if (function_return_type == TYPE_UNDEF) {
    if (leaves_value(function_return)) {
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

    if (leaves_value(function_return)) { // tělo skončilo hodnotou (např.
                                         // voláním funkce), ale funkce má
                                         // vracet Unit
      printf("POPS GF@$t1\n");           // hodnotu zahoď
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
  int label_number = get_new_label(); // vytvoří návěští
  printf("CREATEFRAME\n"              // nový dočasný rámec
         "PUSHFRAME\n"                // dočasný rámec se stane lokálním
         "DEFVAR LF@$$input\n"        // načtená hodnota
         "DEFVAR LF@$$comparator\n"   // pomocná hodnota pro porovnávání
         "READ LF@$$input int\n" // načte řádek jako int, při chybě uloží nil
         "JUMPIFEQ $readInt%d_wrong LF@$$input nil@nil\n" // pokud byl uložen
                                                          // nil=>chyba=>skočí
                                                          // na error výsledek 0
         "GT LF@$$comparator LF@$$input int@2147483647\n" // porovná hodnotu s
                                                          // velikostí intu
         "JUMPIFEQ $readInt%d_wrong LF@$$comparator bool@true\n" // je mimo
                                                                 // rozsah=>skok
                                                                 // na error
                                                                 // výsledek 0
         "LT LF@$$comparator LF@$$input int@-2147483648\n" // porovná hodnotu s
                                                           // velikostí intu
         "JUMPIFEQ $readInt%d_wrong LF@$$comparator bool@true\n" // je mimo
                                                                 // rozsah=>skok
                                                                 // na error
                                                                 // výsledek 0
         "JUMP $readInt%d_ok\n" // jestli doposud nehlásil chybu je v pořádku a
                                // skáče nakonec
         "LABEL $readInt%d_wrong\n" // zde začíná oprava cyhby
         "MOVE LF@$$input int@0\n"  // chybný vstup se podle specifikace převede
                                    // na 0
         "LABEL $readInt%d_ok\n"    // prostě label pokud je ok
         "PUSHS LF@$$input\n"       // výsledek na zásobník
         "POPFRAME\n",              // zahodí rámec a vrátí původní
         label_number, label_number, label_number, label_number, label_number,
         label_number);
}

/**
 * @brief vestavěná funkce readDouble, při chybě vrací 0.0
 */
void read_double() {
  int label_number = get_new_label(); // vytvoří návěští
  printf("CREATEFRAME\n"              // nový dočasný rámec
         "PUSHFRAME\n"                // dočasný rámec se stane lokálním
         "DEFVAR LF@$$input\n"        // načtená hodnota
         "READ LF@$$input float\n"    // opět načte řádek nebo nil pokud je
                                      // špatný vstup
         "JUMPIFNEQ $readDouble%d_end LF@$$input nil@nil\n" // není nil=>ok
                                                            // skočí na konec
         "MOVE LF@$$input float@%a\n"                       // výsledek nil
         "LABEL $readDouble%d_end\n"                        // pro ok
         "PUSHS LF@$$input\n" // výsledek na zásobník
         "POPFRAME\n",        // zahodí rámec a vrátí původní
         label_number, 0.0, label_number);
}

/**
 * @brief vestavěná funkce readLine, při EOF vrací prázdný řetězec
 */
void read_line() {
  int label_number = get_new_label();
  printf("CREATEFRAME\n"
         "PUSHFRAME\n"
         "DEFVAR LF@$$input\n" // proměnná s načtenou hodnotou
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

    if (!leaves_value(arguments)) {

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
  if (!leaves_value(arguments)) {
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

  case LIST:
    generate_list(node);
    break;

  case RETURN:
    generate_return(node);
    break;

    // todo: doplnit další uzly AST

  case FOR_N:
    // opravit
    break;

  case DOT_ACCESS:
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
void generate_program(stackPtr functions, const char *main_name) {

  // ze zásobníku se vyndají funkce a poskládají se do linked listu
  function_list = NULL;
  AST_Node *function;
  while ((function = stack_top(functions)) != NULL) {
    stack_pop(functions);
    function->next = function_list;
    function_list = function;
  }

  // dopočítá typy (result) a unikátní jména proměnných pro všechny funkce
  function_count = 0;
  for (AST_Node *fun = function_list; fun; fun = fun->next) {
    function_count++;
  }
  function_states = calloc((size_t)function_count + 1, 1);
  if (function_states == NULL) {
    fprintf(stderr, "ERROR: Memory allocation failed!\n");
    exit(99);
  }
  for (AST_Node *fun = function_list; fun; fun = fun->next) {
    infer_function(fun);
  }

  printf(".IFJcode26\n");
  printf("DEFVAR GF@$t1\n");
  printf("DEFVAR GF@$t2\n");
  printf("JUMP $program_start\n"); // přeskoč definice funkcí

  for (AST_Node *function = function_list; function;
       function = function->next) {
    generate_node(function); // uzel FUN_DEF -> generate_fun
  }

  printf("LABEL $program_start\n");
  printf("CALL $function$%s\n", main_name);
  printf("EXIT int@0\n");

  free(function_states);
  function_states = NULL;
  free(all_variables);
  all_variables = NULL;
}