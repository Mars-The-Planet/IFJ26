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

scoped_symtablePtr symbol_table;
token active_token;
stackPtr stack_indent;

stackPtr used_symbols;

/**
 *
 * @brief funckce zavolá a zároveň zkontroluje next_token
 */
void check_next_token() {

  if (next_token(&active_token, stack_indent) == 1) {
    fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
    exit(1);
  }
}

void check_for_eol() {

  while (active_token.type == SYMBOL_TYPE &&
         strcmp(active_token.value, "EOL") == 0) {
    check_next_token();
  }
}

void check_stack_pop(stackPtr stack) {
  if (stack_top(stack) != NULL) {
    stack_pop(stack);
    return;
  } else {

    fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
    exit(2);
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
  {
      /*
  fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
     exit(3);
  */
  };
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
    if (strcmp(active_token.value, "import") != 0) {
      fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
      exit(2);
    };
  } else {
    fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
    exit(2);
  };

  check_next_token();
  if (active_token.type == ID_TYPE) {
    if (strcmp(active_token.value, "ifj26") != 0) {
      fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
      exit(2);
    };
  } else {
    fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
    exit(2);
  };

  check_next_token();
  if (active_token.type == SYMBOL_TYPE) {
    if (strcmp(active_token.value, ".") != 0) {
      fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
      exit(2);
    };
  } else {
    fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
    exit(2);
  };

  check_next_token();
  if (active_token.type == OPERATOR_TYPE) {
    if (strcmp(active_token.value, "*") != 0) {
      fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
      exit(2);
    };
  } else {
    fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
    exit(2);
  };

  return;
}

void the_parser() {

  int indent;

  check_next_token();

  while (1) {

    switch (active_token.type) {

    case KEYWORD_TYPE:
      if (strcmp(active_token.value, "def") == 0) {
        parse_fun();
        break;
      } else if (strcmp(active_token.value, "@main") == 0) {
        check_next_token();

        if (active_token.type == KEYWORD_TYPE &&
            strcmp(active_token.value, "def") == 0) {
          parse_fun();
          break;

        } else {
          fprintf(stderr, "Špatně definovaný main %d\n", __LINE__);
          exit(2);
        }
      } else {
        fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
        exit(2);
      };

    case SYMBOL_TYPE:
      if (strcmp(active_token.value, "EOL") == 0) {
        check_next_token();
        continue;

      } else if (strcmp(active_token.value, "EOF") == 0)
        return;

    default: {
      fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
      exit(2);
    };
    }
  }

  return;
}

AST_Node *allocate_expression(AST_Node *expression) {

  expression = malloc(sizeof(AST_Node));
  if (expression == NULL) {
    fprintf(stderr, "ERROR: Memory allocation failed!");
    exit(99);
  }

  return expression;
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
    '<', '>', '<', '>', '>', // 7. to, until, by (for cyklus)
    '<', '<', '<', '<', '<', '<',
    '<', '<', '<', '=', 'E', // 8. otevírací závorka
    '<', '>', '>', '>', '>', '>',
    '>', '>', '<', '=', '>', // 9. uzavírací závorka
    '<', '<', '<', '<', '<', '<',
    '<', '<', '<', 'E', 'E' // 10. dno zásobníku
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

  if (type == TO_N || type == UNTIL_N || type == BY_N)
    return 7;

  if (type == BRACK_START)
    return 8;

  if (type == BRACK_END)
    return 9;

  if (type == STACK_END)
    return 10;

  return 11;
}

// přidání parametru abych neztratil jméno identifikátoru z funkce parse_id
AST_Node *parse_expression(AST_Node *past_node) {

  // zásobník pro uložení operátorů
  stackPtr operator_stack = stack_ctor();
  // zásobník pro uložení proměných
  stackPtr node_stack = stack_ctor();

  AST_Node *temp_expression = NULL;
  AST_Node *expression = NULL;
  AST_Node *stack_nodes = NULL;

  NodeType operator_type;
  NodeType stack_operators;

  char prec_table_result;

  int index_horizontal;
  int index_vertical;

  bool stack_switch; // stack_operator(0) nebo node_stack(1)

  stack_push(operator_stack, (void *)(uintptr_t)STACK_END);

  if (past_node != NULL) {

    stack_push(node_stack, past_node);
    stack_switch = 0;
  } else
    stack_switch = 1;

  while (1) {

    temp_expression = allocate_expression(temp_expression);
    // režim pro čtení nodes
    if (stack_switch == 1) {

      if (active_token.type == INT_TYPE) {
        temp_expression->type = LITERAL_INT;
        temp_expression->nodeData.elemData.valueType.int_value =
            atoi(active_token.value);

        stack_switch = 0;
        stack_push(node_stack, temp_expression);
        check_next_token();

      } else if (active_token.type == DOUBLE_TYPE) {
        temp_expression->type = LITERAL_DOUBLE;
        temp_expression->nodeData.elemData.valueType.double_value =
            atof(active_token.value);

        stack_switch = 0;
        stack_push(node_stack, temp_expression);
        check_next_token();

      } else if (active_token.type == STRING_TYPE) {
        temp_expression->type = LITERAL_STRING;
        temp_expression->nodeData.elemData.valueType.string_value =
            strdup(active_token.value);

        stack_switch = 0;
        stack_push(node_stack, temp_expression);
        check_next_token();

      } else if (active_token.type == ID_TYPE) {
        if (strcmp(active_token.value, "true") == 0) {
          temp_expression->type = LITERAL_BOOL;
          temp_expression->nodeData.elemData.valueType.bool_value = 1;

          stack_switch = 0;
          stack_push(node_stack, temp_expression);
          check_next_token();

        } else if (strcmp(active_token.value, "false") == 0) {
          temp_expression->type = LITERAL_BOOL;
          temp_expression->nodeData.elemData.valueType.bool_value = 0;

          stack_switch = 0;
          stack_push(node_stack, temp_expression);
          check_next_token();

        } else {
          temp_expression->type = IDENT;
          temp_expression->nodeData.idVarData.id_name =
              strdup(active_token.value);

          stack_switch = 0;
          stack_push(node_stack, temp_expression);
          check_next_token();

          // stack_push(stack_top(used_symbols), temp_expression);
          if (get_symbol(symbol_table,
                         temp_expression->nodeData.idVarData.id_name) == NULL) {
            stack_push(stack_top(used_symbols), temp_expression);
          }
        }
      } else if (active_token.type == KEYWORD_TYPE) {

        if (strcmp(active_token.value, "if") == 0) {
          temp_expression = parse_if(sum_stack(stack_indent));

          stack_switch = 0;
          stack_push(node_stack, temp_expression);
          // jako jediný případ nemusí volat další token protože je token
          // načtěný z volání funkce

        } else if (strcmp(active_token.value, "List") == 0) {
          temp_expression = parse_list();

          stack_switch = 0;
          stack_push(node_stack, temp_expression);
          check_next_token();

        } else {
          fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
          exit(2);
        };

      } else if (active_token.type == OPERATOR_TYPE) {
        if (strcmp(active_token.value, "-") == 0) {
          stack_push(operator_stack, (void *)(uintptr_t)UMINUS);

          check_next_token();

        } else if (strcmp(active_token.value, "+") == 0) {
          stack_push(operator_stack, (void *)(uintptr_t)UPLUS);

          check_next_token();

        } else if (strcmp(active_token.value, "!") == 0) {
          stack_push(operator_stack, (void *)(uintptr_t)NOT_N);

          check_next_token();

        } else {
          fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
          exit(2);
        };
      } else if (active_token.type == SYMBOL_TYPE &&
                 strcmp(active_token.value, "(") == 0) {

        check_next_token();
        // pokud je závorka prázdná nastavím ho na UNIT
        if (active_token.type == SYMBOL_TYPE &&
            strcmp(active_token.value, ")") == 0) {
          temp_expression->type = LITERAL_UNIT;

          stack_switch = 0;
          stack_push(node_stack, temp_expression);
          check_next_token();

        } else
          stack_push(operator_stack, (void *)(uintptr_t)BRACK_START);
      } else {

        fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
        exit(2);
      }
      // Režim pro čtení operátorů
    } else if (stack_switch == 0) {
      if (active_token.type == SYMBOL_TYPE) {
        if (strcmp(active_token.value, "(") == 0) {

          stack_nodes = (AST_Node *)stack_top(node_stack);
          check_stack_pop(node_stack);

          if (stack_nodes->type == IDENT) {
            temp_expression->type = FUN_CALL;
            temp_expression->nodeData.callFunData.call_fun_name =
                stack_nodes->nodeData.idVarData.id_name;

            temp_expression->nodeData.callFunData.fun_args = exp_in_brackets();
            stack_switch = 1;

            stack_push(stack_top(used_symbols), temp_expression);

            return temp_expression;
          }
        } else if (strcmp(active_token.value, ".") == 0) {

          temp_expression->type = DOT_ACCESS;

          check_next_token();
          if (active_token.type == ID_TYPE) {
            temp_expression->nodeData.accessDotData.before_dot =
                (AST_Node *)stack_top(node_stack);
            check_stack_pop(node_stack);

            temp_expression->nodeData.accessDotData.name_after_dot =
                strdup(active_token.value);

            check_next_token();
            if (active_token.type == SYMBOL_TYPE) {
              if (strcmp(active_token.value, "(") == 0) {

                temp_expression->nodeData.accessDotData.dot_args =
                    exp_in_brackets();

              } else if (strcmp(active_token.value, "EOL") == 0) {
                temp_expression->nodeData.accessDotData.dot_args = NULL;

                stack_push(node_stack, temp_expression);
                stack_switch = 0;

              } else {
                stack_push(node_stack, temp_expression);
                stack_switch = 0;
              }
            }
          }

        } else if (strcmp(active_token.value, ")") == 0) {
          operator_type = BRACK_END;
        } else {
          operator_type = STACK_END;
        }

        // načítání pro precedenční tabulku
      } else if (active_token.type == OPERATOR_TYPE) {
        if (strcmp(active_token.value, "+") == 0) {

          operator_type = ADD;
          index_horizontal = get_table_index(operator_type);

        } else if (strcmp(active_token.value, "-") == 0) {

          operator_type = SUB;
          index_horizontal = get_table_index(operator_type);

        } else if (strcmp(active_token.value, "*") == 0) {

          operator_type = MUL;
          index_horizontal = get_table_index(operator_type);

        } else if (strcmp(active_token.value, "/") == 0) {

          operator_type = DIV;
          index_horizontal = get_table_index(operator_type);

        } else if (strcmp(active_token.value, "<") == 0) {

          operator_type = LESS;
          index_horizontal = get_table_index(operator_type);

        } else if (strcmp(active_token.value, ">") == 0) {

          operator_type = GREATER;
          index_horizontal = get_table_index(operator_type);

        } else if (strcmp(active_token.value, "<=") == 0) {

          operator_type = LESS_EQUAL;
          index_horizontal = get_table_index(operator_type);

        } else if (strcmp(active_token.value, ">=") == 0) {

          operator_type = GREATER_EQUAL;
          index_horizontal = get_table_index(operator_type);

        } else if (strcmp(active_token.value, "==") == 0) {

          operator_type = EQUALS;
          index_horizontal = get_table_index(operator_type);

        } else if (strcmp(active_token.value, "!=") == 0) {

          operator_type = NOT_EQUAL;
          index_horizontal = get_table_index(operator_type);

        } else if (strcmp(active_token.value, "&&") == 0) {

          operator_type = AND_N;
          index_horizontal = get_table_index(operator_type);

        } else if (strcmp(active_token.value, "||") == 0) {

          operator_type = OR_N;
          index_horizontal = get_table_index(operator_type);

        } else
          operator_type = STACK_END;

      } else if (active_token.type == ID_TYPE) {

        if (strcmp(active_token.value, "to") == 0) {

          operator_type = TO_N;
          index_horizontal = get_table_index(operator_type);
        } else if (strcmp(active_token.value, "until") == 0) {

          operator_type = UNTIL_N;
          index_horizontal = get_table_index(operator_type);
        } else if (strcmp(active_token.value, "by") == 0) {

          operator_type = BY_N;
          index_horizontal = get_table_index(operator_type);

        } else
          operator_type = STACK_END;

      } else
        operator_type = STACK_END;

      index_horizontal = get_table_index(operator_type);

      stack_operators = (NodeType)(uintptr_t)stack_top(operator_stack);
      index_vertical = get_table_index(stack_operators);

      prec_table_result = precedence_table[index_vertical][index_horizontal];

      switch (prec_table_result) {

      case '<':

        stack_push(operator_stack, (void *)(uintptr_t)operator_type);
        check_next_token();

        stack_switch = 1;

        break;

      case '>':

        check_stack_pop(operator_stack);

        temp_expression->type = stack_operators;

        if (get_table_index(stack_operators) == 0) {

          temp_expression->nodeData.uData.single =
              (AST_Node *)stack_top(node_stack);
          check_stack_pop(node_stack);
        } else {
          temp_expression->nodeData.biData.right =
              (AST_Node *)stack_top(node_stack);
          check_stack_pop(node_stack);

          temp_expression->nodeData.biData.left =
              (AST_Node *)stack_top(node_stack);
          check_stack_pop(node_stack);
        }

        stack_push(node_stack, temp_expression);

        break;

      case '=':

        check_stack_pop(operator_stack);

        check_next_token();

        break;

      case 'E':

        if (stack_operators == STACK_END) {

          expression = (AST_Node *)stack_top(node_stack);

          if (temp_expression != NULL)
            free(temp_expression);

          return expression;

        } else {

          fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
          exit(2);
        }
      }
    } else {
      fprintf(stderr, "ERROR: Memory allocation failed!");
      exit(99);
    }
  }

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
        if (strcmp(active_token.value, ":") != 0) {
          fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
          exit(2);
        };

        check_next_token();
        check_for_eol();
        if (active_token.type != KEYWORD_TYPE) {
          fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
          exit(2);
        };
        list_end->type = type_transfer();

        if (strcmp(active_token.value, "List") == 0) {
          check_next_token();
          check_for_eol();
          if (active_token.type != SYMBOL_TYPE) {
            fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
            exit(2);
          };
          if (strcmp(active_token.value, "[") != 0) {
            fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
            exit(2);
          };

          check_next_token();
          check_for_eol();
          if (active_token.type != KEYWORD_TYPE) {
            fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
            exit(2);
          };
          list_end->list_type = type_transfer();

          check_next_token();
          check_for_eol();
          if (active_token.type != SYMBOL_TYPE) {
            fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
            exit(2);
          };
          if (strcmp(active_token.value, "]") != 0) {
            fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
            exit(2);
          };
        }
        check_next_token();
        check_for_eol();
        if (active_token.type != SYMBOL_TYPE) {
          fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
          exit(2);
        };
        if (strcmp(active_token.value, ")") == 0)
          return params;
        if (strcmp(active_token.value, ",") == 0)
          continue;

      } else {
        fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
        exit(2);
      };
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
  } else {
    fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
    exit(2);
  };

  insert_symbol(symbol_table, new_fun->nodeData.funData.fun_name, new_fun);
  scope_enter(symbol_table);
  stack_push(used_symbols, stack_ctor());

  check_next_token();
  if (active_token.type == SYMBOL_TYPE) {
    if (strcmp(active_token.value, "(") != 0) {
      {
        fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
        exit(2);
      };
    }
  } else {
    fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
    exit(2);
  };

  new_fun->nodeData.funData.param_list = parse_param();
  ListParam *current_param = new_fun->nodeData.funData.param_list->first;
  while (current_param != NULL) {
    insert_symbol(symbol_table, current_param->param_name, current_param);
    current_param = current_param->next;
  }

  check_next_token();
  if (active_token.type == SYMBOL_TYPE) {
    if (strcmp(active_token.value, ":") == 0) {

      check_next_token();
      if (active_token.type == KEYWORD_TYPE) {

        new_fun->nodeData.funData.return_type = type_transfer();

        // případ kde by vrácená hodnota byl list
        if (strcmp(active_token.value, "List") == 0) {

          check_next_token();
          if (active_token.type != SYMBOL_TYPE) {
            fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
            exit(2);
          };
          if (strcmp(active_token.value, "[") != 0) {
            fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
            exit(2);
          };

          check_next_token();
          if (active_token.type != KEYWORD_TYPE) {
            fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
            exit(2);
          };
          // Uložení hodnoty které bude mít vracený list v sobě uložené
          new_fun->nodeData.funData.return_list = type_transfer();

          check_next_token();
          if (active_token.type != SYMBOL_TYPE) {
            fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
            exit(2);
          };
          if (strcmp(active_token.value, "]") != 0) {
            fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
            exit(2);
          };
        }

        check_next_token();
        if (active_token.type == SYMBOL_TYPE) {
          if (strcmp(active_token.value, "=") == 0) {
            new_fun->nodeData.funData.body = parse_fun_body(indent);
          } else {
            fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
            exit(2);
          };
        } else {
          fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
          exit(2);
        };
      } else {
        fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
        exit(2);
      };
    } else if (strcmp(active_token.value, "=") == 0) {
      new_fun->nodeData.funData.body = parse_fun_body(indent);
    } else {
      fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
      exit(2);
    };
  } else {
    fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
    exit(2);
  };

  if (check_pop() == 1) {
    fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
    exit(3);
  }

  scope_leave(symbol_table);
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
        strcmp(active_token.value, "EOF") == 0) {
      fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
      exit(2);
    };
    // tady může mít stack_indent mít hodnotu 0
    if (sum_stack(stack_indent) <= origin_indent) {
      fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
      exit(2);
    };
    origin_indent = sum_stack(stack_indent);

    while (1) {
      check_for_eol();
      if (active_token.type == SYMBOL_TYPE &&
          strcmp(active_token.value, "EOF") == 0)
        return statements;
      if (sum_stack(stack_indent) < origin_indent)
        return statements;
      // odsazení se nesmí jen tak zvětšit
      if (sum_stack(stack_indent) > origin_indent) {
        fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
        exit(2);
      };

      switch (active_token.type) {

      case KEYWORD_TYPE:
        // var(1) a val(0) se rozlišuje podle bool
        if (strcmp(active_token.value, "val") == 0 ||
            strcmp(active_token.value, "var") == 0) {
          temp_statement = parse_var();
          insert_symbol(symbol_table, temp_statement->nodeData.varData.var_name,
                        temp_statement);
        } else if (strcmp(active_token.value, "while") == 0)
          temp_statement = parse_while(origin_indent);
        else if (strcmp(active_token.value, "if") == 0)
          temp_statement = parse_if(origin_indent);
        else if (strcmp(active_token.value, "for") == 0)
          temp_statement = parse_for(origin_indent);
        else if (strcmp(active_token.value, "return") == 0)
          temp_statement = parse_return();
        else {
          fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
          exit(2);
        };
        break;

      case ID_TYPE:
        // funkce která už bude rozhodovat co s id vlastně dělat
        // update_symbol(symbol_table, strdup(active_token.value),
        //       temp_statement); // tohle rozbije symbol table, co se tu
        //       vlastně vůbec děje???
        temp_statement = parse_id();
        break;

      // Jelikož pro všechny case se bude volat stejná funkce, použíl jsem
      // case featuru propadnutí
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
  } else {
    fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
    exit(2);
  };

  check_next_token();
  if (active_token.type == SYMBOL_TYPE) {
    if (strcmp(active_token.value, ":") == 0) {

      check_next_token();
      if (active_token.type == KEYWORD_TYPE) {
        temp_var->nodeData.varData.varType = type_transfer();

        if (strcmp(active_token.value, "List") == 0) {

          check_next_token();
          if (active_token.type != SYMBOL_TYPE) {
            fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
            exit(2);
          };
          if (strcmp(active_token.value, "[") != 0) {
            fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
            exit(2);
          };

          check_next_token();
          if (active_token.type != KEYWORD_TYPE) {
            fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
            exit(2);
          };

          check_next_token();
          temp_var->nodeData.varData.varListType = type_transfer();

          check_next_token();
          if (active_token.type != SYMBOL_TYPE) {
            fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
            exit(2);
          };
          if (strcmp(active_token.value, "]") != 0) {
            fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
            exit(2);
          };
        }

        check_next_token();
        if (active_token.type == SYMBOL_TYPE) {
          if (strcmp(active_token.value, "=") != 0) {
            fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
            exit(2);
          };
          check_next_token();
          temp_var->nodeData.varData.expression = parse_expression(NULL);

        } else {
          fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
          exit(2);
        };
      } else {
        fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
        exit(2);
      };
      // rovná se znamená že proměnná nemá ještě definovaný typ
    } else if (strcmp(active_token.value, "=") == 0) {
      temp_var->nodeData.varData.varType = TYPE_UNDEF;
      check_next_token();
      temp_var->nodeData.varData.expression = parse_expression(NULL);
    } else {
      fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
      exit(2);
    };
  } else {
    fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
    exit(2);
  };

  return temp_var;
}

AST_Node *parse_for(int origin_indent) {

  scope_enter(symbol_table);
  stack_push(used_symbols, stack_ctor());

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

    if (sum_stack(stack_indent) <= origin_indent) {
      fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
      exit(2);
    };
    origin_indent = sum_stack(stack_indent);
  }

  if (active_token.type == ID_TYPE) {
    temp_for->nodeData.forData.var_name = strdup(active_token.value);
  } else {
    fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
    exit(2);
  };

  check_next_token();
  if (active_token.type == OPERATOR_TYPE) {
    if (strcmp(active_token.value, "<-") != 0) {
      fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
      exit(2);
    };
  } else {
    fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
    exit(2);
  };

  check_next_token();
  temp_for->nodeData.forData.condition = parse_expression(NULL);

  if (active_token.type == KEYWORD_TYPE) {
    if (strcmp(active_token.value, "do") != 0) {
      fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
      exit(2);
    };
  } else {
    fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
    exit(2);
  };

  temp_for->nodeData.forData.body = parse_fun_body(origin_indent);

  if (check_pop() == 1) {
    fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
    exit(3);
  }
  scope_leave(symbol_table);

  return temp_for;
}

AST_Node *parse_if(int origin_indent) {

  scope_enter(symbol_table);
  stack_push(used_symbols, stack_ctor());

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
    if (strcmp(active_token.value, "then") != 0) {
      fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
      exit(2);
    };
  } else {
    fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
    exit(2);
  };

  temp_if->nodeData.ifData.then_part = parse_fun_body(origin_indent);

  check_for_eol();

  if (strcmp(active_token.value, "else") == 0 &&
      sum_stack(stack_indent) == origin_indent) {

    temp_if->nodeData.ifData.else_part = parse_fun_body(origin_indent);

  } else {
    temp_if->nodeData.ifData.else_part = NULL;
    if (check_pop() == 1) {
      fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
      exit(3);
    }
    scope_leave(symbol_table);
    return temp_if;
  }
  if (check_pop() == 1) {
    fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
    exit(3);
  }
  scope_leave(symbol_table);
  return temp_if;
}

AST_Node *parse_while(int origin_indent) {

  scope_enter(symbol_table);
  stack_push(used_symbols, stack_ctor());

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
    if (strcmp(active_token.value, "do") != 0) {
      fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
      exit(2);
    };
  } else {
    fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
    exit(2);
  };

  temp_while->nodeData.whileData.body = parse_fun_body(origin_indent);

  if (check_pop() == 1) {
    fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
    exit(3);
  }
  scope_leave(symbol_table);
  return temp_while;
}

AST_Node *exp_in_brackets() {

  AST_Node *temp_expression_list = NULL;
  AST_Node *expression_list = NULL;
  AST_Node *expression_list_end = NULL;

  check_next_token();
  if (active_token.type == SYMBOL_TYPE &&
      strcmp(active_token.value, ")") == 0) {
    check_next_token();
    return expression_list;
  }

  while (1) {

    temp_expression_list = parse_expression(NULL);

    if (expression_list == NULL) {
      expression_list = temp_expression_list;
      expression_list_end = temp_expression_list;
    } else {
      expression_list_end->next = temp_expression_list;
      expression_list_end = temp_expression_list;
    }
    expression_list_end->next =
        NULL; // když to není NULL, těžko se zjistí, kdy je konec --Radek

    if (active_token.type == SYMBOL_TYPE) {
      if (strcmp(active_token.value, ",") == 0) {
        check_next_token();
        continue;
      } else if (strcmp(active_token.value, ")") == 0) {

        check_next_token();
        return expression_list;

      } else {
        fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
        exit(2);
      };
    } else {
      fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
      exit(2);
    };
  }

  return expression_list;
}

AST_Node *parse_list() {

  AST_Node *temp_list = malloc(sizeof(AST_Node));
  if (temp_list == NULL) {

    fprintf(stderr, "Memory allocation failed!");
    exit(99);
  }

  check_next_token();

  if (active_token.type == SYMBOL_TYPE) {
    if (strcmp(active_token.value, "(") == 0) {
      temp_list->nodeData.listData.list_var = exp_in_brackets();

      temp_list->type = LIST;

    } else if (strcmp(active_token.value, ".") == 0) {
      check_next_token();
      if (active_token.type == ID_TYPE &&
          strcmp(active_token.value, "range") == 0) {
        check_next_token();
        if (active_token.type == SYMBOL_TYPE &&
            strcmp(active_token.value, "(") == 0) {
          temp_list->nodeData.listData.list_var = exp_in_brackets();

        } else {
          fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
          exit(2);
        };

        temp_list->type = DOT_ACCESS;

      } else {
        fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
        exit(2);
      };
    } else {
      fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
      exit(2);
    };
  }

  return temp_list;
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

int count_args(AST_Node *node) {
  int count = 0;
  while (node != NULL) {
    if (node->type == IDENT) {
      count++;
    } else if (node->type == LITERAL_INT) {
      count++;
    } else if (node->type == LITERAL_DOUBLE) {
      count++;
    } else if (node->type == LITERAL_STRING) {
      count++;
    } else if (node->type == LITERAL_BOOL) {
      count++;
    } else if (node->type == LITERAL_UNIT) {
      count++;
    } else if (node->type == FUN_CALL) {
      count++;
    }
    node = node->next;
  }
  return count;
}

int count_params(ListParam *list) {
  int count = 0;
  ListParam *current = list;
  while (current != NULL) {
    count++;
    current = current->next;
  }
  return count;
}

int check_pop() {
  stackPtr used_top = stack_top(used_symbols);
  int failed = 0;
  while (stack_top(used_top) != NULL) {
    AST_Node *temp = stack_top(used_top);
    if (temp->type == IDENT) {
      printf("ident fail\n");
      failed = 1;
    } else if (temp->type == FUN_CALL) {
      char *temp_name = temp->nodeData.callFunData.call_fun_name;
      AST_Node *found_node = get_symbol(symbol_table, temp_name);
      if (found_node == NULL) {
        printf("fun fail\n");
        failed = 1;
      } else {
        if (count_args(temp->nodeData.callFunData.fun_args) !=
            count_params(found_node->nodeData.funData.param_list->first)) {
          exit(5);
        }
      }
    } else {
      // printf("Somehow stored a value that isn't IDENT or FUN_CALL. Type is
      // [%d]\n.", temp->type);
    }

    stack_pop(used_top);
  }
  stack_pop(used_symbols);
  return failed;
}

void setup_globals() {
  {
    AST_Node *global_func = allocate_expression(NULL);
    ListParam *new_param = malloc(sizeof(ListParam));
    head *params = malloc(sizeof(head));
    global_func->nodeData.funData.param_list = params;
    global_func->nodeData.funData.param_list->first = new_param;
    new_param->next = NULL;
    new_param->type = TYPE_ANY;
    insert_symbol(symbol_table, "print", global_func);
  }

  {
    AST_Node *global_func = allocate_expression(NULL);
    head *params = malloc(sizeof(head));
    global_func->nodeData.funData.param_list = params;
    global_func->nodeData.funData.param_list->first = NULL;
    insert_symbol(symbol_table, "readLine", global_func);
  }

  {
    AST_Node *global_func = allocate_expression(NULL);
    head *params = malloc(sizeof(head));
    global_func->nodeData.funData.param_list = params;
    global_func->nodeData.funData.param_list->first = NULL;
    insert_symbol(symbol_table, "readInt", global_func);
  }

  {
    AST_Node *global_func = allocate_expression(NULL);
    head *params = malloc(sizeof(head));
    global_func->nodeData.funData.param_list = params;
    global_func->nodeData.funData.param_list->first = NULL;
    insert_symbol(symbol_table, "readDouble", global_func);
  }

  {
    AST_Node *global_func = allocate_expression(NULL);
    ListParam *new_param = malloc(sizeof(ListParam));
    head *params = malloc(sizeof(head));
    global_func->nodeData.funData.param_list = params;
    global_func->nodeData.funData.param_list->first = new_param;
    new_param->next = NULL;
    new_param->type = TYPE_ANY;
    insert_symbol(symbol_table, "toStr", global_func);
  }

  {
    AST_Node *global_func = allocate_expression(NULL);
    ListParam *new_param = malloc(sizeof(ListParam));
    head *params = malloc(sizeof(head));
    global_func->nodeData.funData.param_list = params;
    global_func->nodeData.funData.param_list->first = new_param;
    new_param->next = NULL;
    new_param->type = TYPE_DOUBLE;
    insert_symbol(symbol_table, "toInt", global_func);
  }

  {
    AST_Node *global_func = allocate_expression(NULL);
    ListParam *new_param = malloc(sizeof(ListParam));
    head *params = malloc(sizeof(head));
    global_func->nodeData.funData.param_list = params;
    global_func->nodeData.funData.param_list->first = new_param;
    new_param->next = NULL;
    new_param->type = TYPE_STRING;
    insert_symbol(symbol_table, "strLength", global_func);
  }

  {
    AST_Node *global_func = allocate_expression(NULL);
    ListParam *new_param1 = malloc(sizeof(ListParam));
    ListParam *new_param2 = malloc(sizeof(ListParam));
    ListParam *new_param3 = malloc(sizeof(ListParam));
    head *params = malloc(sizeof(head));
    global_func->nodeData.funData.param_list = params;
    global_func->nodeData.funData.param_list->first = new_param1;
    new_param1->next = new_param2;
    new_param2->next = new_param3;
    new_param3->next = NULL;
    new_param1->type = TYPE_STRING;
    new_param2->type = TYPE_INT;
    new_param3->type = TYPE_INT;
    insert_symbol(symbol_table, "subStr", global_func);
  }

  {
    AST_Node *global_func = allocate_expression(NULL);
    ListParam *new_param1 = malloc(sizeof(ListParam));
    ListParam *new_param2 = malloc(sizeof(ListParam));
    head *params = malloc(sizeof(head));
    global_func->nodeData.funData.param_list = params;
    global_func->nodeData.funData.param_list->first = new_param1;
    new_param1->next = new_param2;
    new_param2->next = NULL;
    new_param1->type = TYPE_STRING;
    new_param2->type = TYPE_INT;
    insert_symbol(symbol_table, "ord", global_func);
  }

  {
    AST_Node *global_func = allocate_expression(NULL);
    ListParam *new_param = malloc(sizeof(ListParam));
    head *params = malloc(sizeof(head));
    global_func->nodeData.funData.param_list = params;
    global_func->nodeData.funData.param_list->first = new_param;
    new_param->next = NULL;
    new_param->type = TYPE_INT;
    insert_symbol(symbol_table, "chr", global_func);
  }
}

// dočasný main
int main() {

  stack_indent = stack_ctor();
  symbol_table = symbol_table_ctor();
  used_symbols = stack_ctor();

  setup_globals();

  if (set_target_file("test.txt") != 0)
    exit(99);

  code_prolog();
  the_parser();

  printf("přeloženo\n");

  stack_dtor(stack_indent);
  symbol_table_dtor(symbol_table);
  return 0;
}
