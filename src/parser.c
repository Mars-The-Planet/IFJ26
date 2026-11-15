/**
 * @file parser.c
 * @author Jonáš Vávra (x273960)
 *
 * @brief Imlementace syntaxtického analyzátoru
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "scanner.h"

token active_token;
stackPtr stack_indent;

/**
 *
 * @brief funckce zavolá a zároveň zkontroluje next_token
 */
void check_next_token() {

  if (next_token(&active_token, stack_indent) == 1) {
    exit(1);
  }
}

void check_for_eol() {

  while (active_token.type == SYMBOL_TYPE &&
         strcmp(active_token.value, "EOL") == 0) {
    check_next_token();
  }
}

dataType type_transfer() {

  if (strcmp(active_token.value, "Int") == 0)
    return TYPE_INT;
  if (strcmp(active_token.value, "Double") == 0)
    return TYPE_DOUBLE;
  if (strcmp(active_token.value, "String") == 0)
    return TYPE_STRING;
  if (strcmp(active_token.value, "Unit") == 0)
    return TYPE_UNIT;
  if (strcmp(active_token.value, "Boolean") == 0)
    return TYPE_BOOL;
  if (strcmp(active_token.value, "List") == 0)
    return TYPE_LIST;
  exit(2);
}

/**
 *
 * @brief funkce pro zpravování prologu kodu
 */
void code_prolog() {

  // případ pro newline na začátku
  check_next_token();
  check_for_eol();

  if (active_token.type == KEYWORD_TYPE) {
    if (strcmp(active_token.value, "import") != 0)
      exit(2);
  } else
    exit(2);

  check_next_token();
  if (active_token.type == ID_TYPE) {
    if (strcmp(active_token.value, "ifj26") != 0)
      exit(2);
  } else
    exit(2);

  check_next_token();
  if (active_token.type == SYMBOL_TYPE) {
    if (strcmp(active_token.value, ".") != 0)
      exit(2);
  } else
    exit(2);

  check_next_token();
  if (active_token.type == OPERATOR_TYPE) {
    if (strcmp(active_token.value, "*") != 0)
      exit(2);
  } else
    exit(2);

  return;
}

void the_parser() {

  int indent;

  while (1) {
    check_next_token();
    switch (active_token.type) {

    case KEYWORD_TYPE:
      if (strcmp(active_token.value, "def") == 0) {
        parse_fun();
        break;
      } else if (strcmp(active_token.value, "@main") == 0) {
        has_main();
      } else
        exit(2);

    case SYMBOL_TYPE:
      if (strcmp(active_token.value, "EOL") == 0)
        continue;
      else if (strcmp(active_token.value, "EOF") == 0)
        break;

    default:
      exit(2);
    }
  }

  return;
}

char precedence_table[11][11] = {
    '<', '>', '>', '>', '>', '>',
    '>', '>', '<', '>', '>', // 0. unární operátory
    '<', '>', '>', '>', '>', '>',
    '>', '>', '<', '>', '>', // 1. násobení a dělení
    '<', '<', '>', '>', '>', '>',
    '>', '>', '<', '>', '>', // 2. sčítání, odčítání, konkatenace
    '<', '<', '<', 'E', '>', '>',
    '>', '>', '<', '>', '>', // 3. relační operátory
    '<', '<', '<', '<', 'E', '>',
    '>', '>', '<', '>', '>', // 4. '==' a '!='
    '<', '<', '<', '<', '<', '>',
    '>', '>', '<', '>', '>', // 5. AND (&&)
    '<', '<', '<', '<', '<', '<',
    '>', '>', '<', '>', '>', // 6. OR (||)
    '<', '<', '<', '<', '<', '<',
    '<', '>', '<', '>', '>', // 7. to, untill, by (for cyklus)
    '<', '<', '<', '<', '<', '<',
    '<', '<', '<', '=', 'E', // 8. otevírací závorka
    '<', '>', '>', '>', '>', '>',
    '>', '>', '<', '=', '>', // 9. uzavírací závorka
    '<', '<', '<', '<', '<', '<',
    '<', '<', '<', 'E', '=' // 10. dno zásobníku
};

int get_table_index(NodeType type) {

  if (type == UMINUS || type == UPLUS || type == NOT_N)
    return 0;

  if (type == MUL || type == DIV)
    return 1;

  if (type == ADD || type == SUB || type == CON_SEZNAM)
    return 2;

  if (type == LESS || type == GREATER || type == LESS_EQUAL ||
      type == GREATER_EQUAL)
    return 3;

  if (type == EQUALS || type == NOT_EQUAL)
    return 4;

  if (type == AND_N)
    return 5;

  if (type == OR_N)
    return 6;

  if (type == TO_N || type == UNTILL_N || type == BY_N)
    return 7;

  if (type == BRACK_START)
    return 8;

  if (type == BRACK_END)
    return 9;

  if (type == STACK_END)
    return 10;

  return 11;
}

// přidání parametru abych neztratil jméno identifikátoru
AST_Node *parse_expression(AST_Node *past_node) {

  // zásobník pro uložení operátorů
  stackPtr operator_stack = stack_ctor();
  // zásobník pro uložení proměných
  stackPtr node_stack = stack_ctor();

  bool stack_switch; // stack_operator(0) nebo node_stack(1)

  stack_push(operator_stack, (void *)(uintptr_t)STACK_END);

  if (past_node != NULL) {

    stack_push(node_stack, past_node);
    stack_switch = 0;
  } else
    stack_switch = 1;

  return NULL;
}

head *parse_param() {

  head *params = malloc(sizeof(head));
  if (params == NULL) {

    fprintf(stderr, "ERROR: Memory allocation failed!");
    exit(99);
  }

  ListParam *list_end = NULL;

  params->first = NULL;

  while (1) {
    check_next_token();

    if (active_token.type == SYMBOL_TYPE) {

      if (strcmp(active_token.value, "EOL") == 0)
        continue;
      if (strcmp(active_token.value, ")") == 0)
        return params;

    } else if (active_token.type == ID_TYPE) {

      ListParam *new_param = malloc(sizeof(ListParam));
      if (new_param == NULL) {

        fprintf(stderr, "ERROR: Memory allocation failed!");
        exit(99);
      }

      new_param->next = NULL;
      new_param->param_name = strdup(active_token.value);

      if (params->first == NULL) {
        params->first = new_param;
        list_end = new_param;
      } else {
        list_end->next = new_param;
        list_end = new_param;
      }

      check_next_token();
      check_for_eol();

      if (active_token.type == SYMBOL_TYPE) {
        if (strcmp(active_token.value, ":") != 0)
          exit(2);

        check_next_token();
        check_for_eol();
        if (active_token.type != KEYWORD_TYPE)
          exit(2);
        list_end->type = type_transfer();

        if (strcmp(active_token.value, "List") == 0) {
          check_next_token();
          check_for_eol();
          if (active_token.type != SYMBOL_TYPE)
            exit(2);
          if (strcmp(active_token.value, "[") != 0)
            exit(2);

          check_next_token();
          check_for_eol();
          if (active_token.type != KEYWORD_TYPE)
            exit(2);
          list_end->list_type = type_transfer();

          check_next_token();
          check_for_eol();
          if (active_token.type != SYMBOL_TYPE)
            exit(2);
          if (strcmp(active_token.value, "]") != 0)
            exit(2);
        }
        check_next_token();
        check_for_eol();
        if (active_token.type != SYMBOL_TYPE)
          exit(2);
        if (strcmp(active_token.value, ")") == 0)
          return params;
        if (strcmp(active_token.value, ",") == 0)
          continue;

      } else
        exit(2);
    }
  }
}

void parse_fun() {

  int indent = sum_stack(stack_indent);

  AST_Node *new_fun = malloc(sizeof(AST_Node));
  if (new_fun == NULL) {
    fprintf(stderr, "ERROR: Memory allocation failed!");
    exit(99);
  }

  new_fun->type = FUN_DEF;
  new_fun->next = NULL;

  check_next_token();
  if (active_token.type == ID_TYPE) {
    new_fun->nodeData.funData.fun_name = strdup(active_token.value);
  } else
    exit(2);

  check_next_token();
  if (active_token.type == SYMBOL_TYPE) {
    if (strcmp(active_token.value, "(") != 0) {
      exit(2);
    }
  } else
    exit(2);

  new_fun->nodeData.funData.param_list = parse_param();

  check_next_token();
  if (active_token.type == SYMBOL_TYPE) {
    if (strcmp(active_token.value, ":") == 0) {

      check_next_token();
      if (active_token.type == KEYWORD_TYPE) {

        new_fun->nodeData.funData.return_type = type_transfer();

        // případ kde by vrácená hodnota byl list
        if (strcmp(active_token.value, "List") == 0) {

          check_next_token();
          if (active_token.type != SYMBOL_TYPE)
            exit(2);
          if (strcmp(active_token.value, "[") != 0)
            exit(2);

          check_next_token();
          if (active_token.type != KEYWORD_TYPE)
            exit(2);
          // Uložení hodnoty které bude mít vracený list v sobě uložené
          new_fun->nodeData.funData.return_list = type_transfer();

          check_next_token();
          if (active_token.type != SYMBOL_TYPE)
            exit(2);
          if (strcmp(active_token.value, "]") != 0)
            exit(2);
        }

        check_next_token();
        if (active_token.type == SYMBOL_TYPE) {
          if (strcmp(active_token.value, "=") == 0) {
            new_fun->nodeData.funData.body = parse_fun_body(indent);
          } else
            exit(2);
        } else
          exit(2);
      } else
        exit(2);
    } else if (strcmp(active_token.value, "=") == 0) {
      new_fun->nodeData.funData.body = parse_fun_body(indent);
    } else
      exit(2);
  } else
    exit(2);

  return;
}

AST_Node *parse_fun_body(int origin_indent) {

  AST_Node *statements = NULL;
  AST_Node *temp_statement = NULL;
  AST_Node *last_statement = NULL;

  check_next_token();
  // pokud je tam odsazení jedná se o tělo funkce, jinak je to výraz
  if (active_token.type == SYMBOL_TYPE &&
      strcmp(active_token.value, "EOL") == 0) {
    check_for_eol();
    if (active_token.type == SYMBOL_TYPE &&
        strcmp(active_token.value, "EOF") == 0)
      exit(2);
    // tady může mít stack_indent mít hodnotu 0
    if (sum_stack(stack_indent) <= origin_indent)
      exit(2);
    origin_indent = sum_stack(stack_indent);

    while (1) {
      check_for_eol();
      if (active_token.type == SYMBOL_TYPE &&
          strcmp(active_token.value, "EOF") == 0)
        return statements;
      if (sum_stack(stack_indent) < origin_indent)
        return statements;
      // odsazení se nesmí jen tak zvětšit
      if (sum_stack(stack_indent) > origin_indent)
        exit(2);

      switch (active_token.type) {

      case KEYWORD_TYPE:
        // var(1) a val(0) se rozlišuje podle bool
        if (strcmp(active_token.value, "val") == 0 ||
            strcmp(active_token.value, "var") == 0)
          temp_statement = parse_var();
        else if (strcmp(active_token.value, "while") == 0)
          temp_statement = parse_while(origin_indent);
        else if (strcmp(active_token.value, "if") == 0)
          temp_statement = parse_if(origin_indent);
        else if (strcmp(active_token.value, "for") == 0)
          temp_statement = parse_for(origin_indent);
        else if (strcmp(active_token.value, "return") == 0)
          temp_statement = parse_return();
        else
          exit(2);
        break;

      case ID_TYPE:
        // funkce která už bude rozhodovat co s id vlastně dělat
        temp_statement = parse_id();
        break;

      // Jelikož pro všechny case se bude volat stejná funkce, použíl jsem case
      // featuru propadnutí
      case INT_TYPE:
      case DOUBLE_TYPE:
      case STRING_TYPE:
      case SYMBOL_TYPE:
      case OPERATOR_TYPE:
        temp_statement = parse_expression(NULL);
        break;
      }

      if (statements == NULL) {
        statements = temp_statement;
        last_statement = temp_statement;
      } else {
        last_statement->next = temp_statement;
        last_statement = temp_statement;
      }
    }

  } else {
    if (active_token.type == KEYWORD_TYPE) {
      if (strcmp(active_token.value, "if") == 0) {
        return parse_if(origin_indent);
      }
    }

    return parse_expression(NULL);
  }
}

AST_Node *parse_var() {

  AST_Node *temp_var = malloc(sizeof(AST_Node));
  if (temp_var == NULL) {
    fprintf(stderr, "Memory allocation failed!");
    exit(99);
  }

  temp_var->type = VAR_DEF;
  temp_var->next = NULL;

  if (strcmp(active_token.value, "var") != 0)
    temp_var->nodeData.varData.isvar = 0;
  else
    temp_var->nodeData.varData.isvar = 1;

  check_next_token();
  if (active_token.type == ID_TYPE) {
    temp_var->nodeData.varData.var_name = strdup(active_token.value);
  } else
    exit(2);

  check_next_token();
  if (active_token.type == SYMBOL_TYPE) {
    if (strcmp(active_token.value, ":") == 0) {

      check_next_token();
      if (active_token.type == KEYWORD_TYPE) {
        temp_var->nodeData.varData.varType = type_transfer();

        if (strcmp(active_token.value, "List") == 0) {

          check_next_token();
          if (active_token.type != SYMBOL_TYPE)
            exit(2);
          if (strcmp(active_token.value, "[") != 0)
            exit(2);

          check_next_token();
          if (active_token.type != KEYWORD_TYPE)
            exit(2);

          check_next_token();
          temp_var->nodeData.varData.varListType = type_transfer();

          check_next_token();
          if (active_token.type != SYMBOL_TYPE)
            exit(2);
          if (strcmp(active_token.value, "]") != 0)
            exit(2);
        }

        check_next_token();
        if (active_token.type == SYMBOL_TYPE) {
          if (strcmp(active_token.value, "=") != 0)
            exit(2);
          check_next_token();
          temp_var->nodeData.varData.expression = parse_expression(NULL);

        } else
          exit(2);
      } else
        exit(2);
      // rovná se znamená že proměnná nemá ještě definovaný typ
    } else if (strcmp(active_token.value, "=") == 0) {
      temp_var->nodeData.varData.varType = TYPE_UNDEF;
      check_next_token();
      temp_var->nodeData.varData.expression = parse_expression(NULL);
    } else
      exit(2);
  } else
    exit(2);

  return temp_var;
}

AST_Node *parse_for(int origin_indent) {

  AST_Node *temp_for = malloc(sizeof(AST_Node));
  if (temp_for == NULL) {
    fprintf(stderr, "Memory allocation failed!");
    exit(99);
  }

  temp_for->type = FOR_N;
  temp_for->next = NULL;

  check_next_token();
  if (active_token.type == SYMBOL_TYPE &&
      strcmp(active_token.value, "EOL") == 0) {
    check_for_eol();

    if (sum_stack(stack_indent) <= origin_indent)
      exit(2);
    origin_indent = sum_stack(stack_indent);
  }

  if (active_token.type == ID_TYPE) {
    temp_for->nodeData.forData.var_name = strdup(active_token.value);
  } else
    exit(2);

  check_next_token();
  if (active_token.type == OPERATOR_TYPE) {
    if (strcmp(active_token.value, "<-") != 0)
      exit(2);
  } else
    exit(2);

  check_next_token();
  temp_for->nodeData.forData.condition = parse_expression(NULL);

  if (active_token.type == KEYWORD_TYPE) {
    if (strcmp(active_token.value, "do") != 0)
      exit(2);
  } else
    exit(2);

  temp_for->nodeData.forData.body = parse_fun_body(origin_indent);

  return temp_for;
}

AST_Node *parse_if(int origin_indent) {

  AST_Node *temp_if = malloc(sizeof(AST_Node));
  if (temp_if == NULL) {
    fprintf(stderr, "ERROR: Memory allocation failed!");
    exit(99);
  }

  temp_if->type = IF_N;
  temp_if->next = NULL;

  // zavoláme token pro podmínku
  check_next_token();
  temp_if->nodeData.ifData.condition = parse_expression(NULL);

  if (active_token.type == KEYWORD_TYPE) {
    if (strcmp(active_token.value, "then") != 0)
      exit(2);
  } else
    exit(2);

  temp_if->nodeData.ifData.then_part = parse_fun_body(origin_indent);

  check_for_eol();

  if (strcmp(active_token.value, "else") == 0 &&
      sum_stack(stack_indent) == origin_indent) {

    temp_if->nodeData.ifData.else_part = parse_fun_body(origin_indent);

  } else {
    temp_if->nodeData.ifData.else_part = NULL;
    return temp_if;
  }
  return temp_if;
}

AST_Node *parse_while(int origin_indent) {

  AST_Node *temp_while = malloc(sizeof(AST_Node));
  if (temp_while == NULL) {
    fprintf(stderr, "ERROR: Memory allocation failed!");
    exit(99);
  }

  temp_while->type = WHILE_N;
  temp_while->next = NULL;

  check_next_token();
  temp_while->nodeData.whileData.condition = parse_expression(NULL);

  // nevolám next_token protože počítám stím že ho zavolal funkce
  // parse_expression
  if (active_token.type == KEYWORD_TYPE) {
    if (strcmp(active_token.value, "do") != 0)
      exit(2);
  } else
    exit(2);

  temp_while->nodeData.whileData.body = parse_fun_body(origin_indent);

  return temp_while;
}

AST_Node *parse_id() {

  // uložení jména proměnné, protože ještě nevím o jaký typ výrazu se jedná
  char *temp_name = strdup(active_token.value);

  AST_Node *temp_id = malloc(sizeof(AST_Node));
  if (temp_id == NULL) {
    fprintf(stderr, "Memory allocation failed!");
    exit(99);
  }

  temp_id->next = NULL;

  check_next_token();
  // pokud tam je rovná se jedná se o přiřazení hodnoty
  if (active_token.type == SYMBOL_TYPE &&
      strcmp(active_token.value, "=") == 0) {

    temp_id->type = ASSIGN;

    temp_id->nodeData.assignData.var_name = temp_name;

    check_next_token();
    temp_id->nodeData.assignData.equal = parse_expression(NULL);

    return temp_id;
  }

  temp_id->type = IDENT;
  temp_id->nodeData.idVarData.id_name = temp_name;
  return parse_expression(temp_id);
}

AST_Node *parse_return() {

  AST_Node *temp_return = malloc(sizeof(AST_Node));
  if (temp_return == NULL) {
    fprintf(stderr, "Memory allocation failed!");
    exit(99);
  }

  temp_return->type = RETURN;
  temp_return->next = NULL;

  check_next_token();

  if (active_token.type == SYMBOL_TYPE) {
    if (strcmp(active_token.value, "EOL") == 0 ||
        strcmp(active_token.value, "EOF") == 0 ||
        strcmp(active_token.value, ";") == 0) {

      temp_return->nodeData.returnData.expression = NULL;

      return temp_return;
    }
  }

  temp_return->nodeData.returnData.expression = parse_expression(NULL);

  return temp_return;
}

// TODO: dodělat funkci na kontrolu existence funkce main
void has_main() {}

// dočasný main
int main() {

  stack_indent = stack_ctor();

  if (set_target_file("test.txt") != 0)
    exit(99);

  code_prolog();

  stack_dtor(stack_indent);
  return 0;
}
