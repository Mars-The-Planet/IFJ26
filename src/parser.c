/**
 * IFJ26 - Implementace překladače imperativního jazyka IFJ26
 * @file parser.c
 * @author Jonáš Vávra (x273960)
 * @author Radek Večeřa (281687)
 *
 * @brief Imlementace syntaxtického analyzátoru
 * @date 25.11.2026
 */

#include "parser.h"

/**
 *
 * @brief Mapování chyb v kódu
 *
 * @param errmsg Zpráva popisující důvod chyby
 * @param line Řádek na kterém se chyba stala
 *
 * @note Funkce nic nevrací - vždy končí proces voláním exit(2)
 *
 * */
void syntax_error(char *errmsg, int line) {

  fprintf(stderr, "ERROR: Syntaxe (radek %d): %s | token: '%s'\n", line, errmsg,
          active_token.value, active_token.type);
  special_exit(2);
}

/**
 *
 * @brief Zavolá a rovnou zkontroluje next_token
 *
 *
 * @warning Při selhání alokace okamžitě ukončí program exit(99)
 * @warning Při špatně načteném tokenu okamžitě ukončí program exit(1)
 */
void check_next_token() {

  int err = next_token(&active_token, stack_indent);

  if (err != 0) {

    if (err == 99) {

      fprintf(stderr, "ERROR: Chyba alokace pameti!");
      special_exit(99);
    } else {

      fprintf(stderr, "Chyba scanneru %d\n", __LINE__);
      special_exit(1);
    }
  }
}

/**
 *
 * @brief Prochází prázdné řádky
 */

void check_for_eol() {

  while (active_token.type == SYMBOL_TYPE &&
         strcmp(active_token.value, "EOL") == 0) {
    check_next_token();
  }
}

/**
 *
 * @brief Kontroluje vršek zásobníku
 *
 * @param stack Ukazatel na zásobník co se má zkontrolovat
 *
 * @warning Pokud na vrcholu zásboníku nic není okamžitě ukončí program exit(2)
 */
void check_stack_pop(stackPtr stack) {
  if (stack_top(stack) != NULL) {
    stack_pop(stack);
    return;
  } else {

    syntax_error("Popnuti prazdneho zasobniku", __LINE__);
  }
}

/**
 *
 * @brief Nastavuje základní datové typy pro uzel
 *
 *
 * @return Základní datový typ
 * @warning Při špatném datovém typu okamžitě ukončí program exit(3)
 */
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

  fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
  special_exit(3);
}

/**
 *
 * @brief Zpracovává prolog funkce
 *
 * @warning Pokud je prolog špatně zapsaná okamžitě ukončí program exit(2)
 */
void code_prolog() {

  // případ pro newline na začátku
  check_next_token();
  check_for_eol();

  if (active_token.type == KEYWORD_TYPE) {
    if (strcmp(active_token.value, "import") != 0) {

      syntax_error("Spatná hodnota tokenu, ocekavano 'import'", __LINE__);
    };
  } else {

    syntax_error("Spatny typ tokenu, ocekavano KEYWORD", __LINE__);
  };

  check_next_token();
  if (active_token.type == ID_TYPE) {
    if (strcmp(active_token.value, "ifj26") != 0) {

      syntax_error("Špatná hodnota tokenu, očekávano 'ifj26'", __LINE__);
    };
  } else {
    syntax_error("Spatny typ tokenu, ocekavano ID", __LINE__);
  };

  check_next_token();
  if (active_token.type == SYMBOL_TYPE) {
    if (strcmp(active_token.value, ".") != 0) {
      syntax_error("Špatná hodnota tokenu, očekávano '.'", __LINE__);
    };
  } else {
    syntax_error("Spatny typ tokenu, ocekavano SYMBOL", __LINE__);
  };

  check_next_token();
  if (active_token.type == OPERATOR_TYPE) {
    if (strcmp(active_token.value, "*") != 0) {
      syntax_error("Špatná hodnota tokenu, očekávano '*'", __LINE__);
    };
  } else {
    syntax_error("Spatny typ tokenu, ocekavano OPERATOR", __LINE__);
  };
}

/**
 *
 * @brief Zpracovává hlavičky funkcí a volá zpracování jejich argumentů
 *
 * @warning Pokud je hlavička špatně zapsaná okamžitě ukončí program exit(2)
 */

void parse_fun_headers() {

  check_next_token();

  while (1) {

    switch (active_token.type) {

    case KEYWORD_TYPE:
      if (strcmp(active_token.value, "def") == 0) {
        parse_fun();
        break;
      } else if (strcmp(active_token.value, "@main") == 0) {
        has_main++;
        youre_main_node = true;
        check_next_token();
        check_for_eol();

        if (active_token.type == KEYWORD_TYPE &&
            strcmp(active_token.value, "def") == 0) {
          parse_fun();
          break;

        } else {
          syntax_error("Spatny typ tokenu, ocekavan KEYWORD", __LINE__);
        }
      } else {
        fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
        syntax_error("Spatna hodnota tokenu, ocekavano '@main'", __LINE__);
      };

    case SYMBOL_TYPE:
      if (strcmp(active_token.value, "EOL") == 0) {
        check_next_token();
        continue;

      } else if (strcmp(active_token.value, "EOF") == 0)
        return;

    default: {
      syntax_error("Žádná hlavička funkce", __LINE__);
    };
    }
  }
}

/**
 *
 * @brief Alokuje paměť pro výraz
 *
 * @param expression Ukazatel na alokovaný výraz
 *
 * @return Alokovaný výraz
 */
AST_Node *allocate_expression(AST_Node *expression) {

  expression = special_malloc(sizeof(AST_Node));
  expression->next = NULL; // PROSÍM

  return expression;
}

/**
 *
 * @brief Precedenční tabulka pro syntaktickou analýzu výrazů
 */

char precedence_table[11][11] = {
    // 0. unární operátory
    {'<', '>', '>', '>', '>', '>', '>', '>', '<', '>', '>'},
    // 1. násobení a dělení
    {'<', '>', '>', '>', '>', '>', '>', '>', '<', '>', '>'},
    // 2. sčítání, odčítání, konkatenace
    {'<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '>'},
    // 3. relační operátory
    {'<', '<', '<', 'E', '>', '>', '>', '>', '<', '>', '>'},
    // 4. '==' a '!='
    {'<', '<', '<', '<', 'E', '>', '>', '>', '<', '>', '>'},
    // 5. AND (&&)
    {'<', '<', '<', '<', '<', '>', '>', '>', '<', '>', '>'},
    // 6. OR (||)
    {'<', '<', '<', '<', '<', '<', '>', '>', '<', '>', '>'},
    // 7. to, until, by (for cyklus)
    {'<', '<', '<', '<', '<', '<', '<', '>', '<', '>', '>'},
    // 8. otevírací závorka
    {'<', '<', '<', '<', '<', '<', '<', '<', '<', '=', 'E'},
    // 9. uzavírací závorka
    {'<', '>', '>', '>', '>', '>', '>', '>', '<', '=', '>'},
    // 10. dno zásobníku
    {'<', '<', '<', '<', '<', '<', '<', '<', '<', 'E', 'E'}};

/**
 *
 * @brief Nastaví prioritu operátoru, čím menší číslo tím větší priorita
 *
 * @return Prioritu operátoru
 *
 * @warning Pokud neznámý operátor okamžitě ukončí program exit(2)
 */

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

  syntax_error("Neznámý operátor", __LINE__);
}

/**
 *
 * @brief Zpracuje výraz pomocí precedenční syntaktické analýzy
 *
 * @param past_node Volitelný již načtený uzel identifikátoru (z parse id)
 *
 * @return Ukazatel uzel zpracovanéhou výrazu
 *
 * @warning Při syntaktické chybě ve výrazu okamžite ukončí program exit(2)
 */
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

    // režim pro čtení nodes
    if (stack_switch == 1) {

      if (active_token.type == INT_TYPE) {

        temp_expression = allocate_expression(temp_expression);

        temp_expression->type = LITERAL_INT;
        temp_expression->nodeData.elemData.valueType.int_value =
            atoi(active_token.value);

        stack_switch = 0;
        stack_push(node_stack, temp_expression);
        check_next_token();

      } else if (active_token.type == DOUBLE_TYPE) {

        temp_expression = allocate_expression(temp_expression);

        temp_expression->type = LITERAL_DOUBLE;
        temp_expression->nodeData.elemData.valueType.double_value =
            atof(active_token.value);

        stack_switch = 0;
        stack_push(node_stack, temp_expression);
        check_next_token();

      } else if (active_token.type == STRING_TYPE) {

        temp_expression = allocate_expression(temp_expression);

        temp_expression->type = LITERAL_STRING;
        temp_expression->nodeData.elemData.valueType.string_value =
            special_strdup(active_token.value);

        stack_switch = 0;
        stack_push(node_stack, temp_expression);
        check_next_token();

      } else if (active_token.type == ID_TYPE) {

        temp_expression = allocate_expression(temp_expression);

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
              special_strdup(active_token.value);

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

        } else {
          syntax_error("Špatná hondota tokenu, očekáváno 'if' nebo 'List'",
                       __LINE__);
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
          syntax_error("Špatná hodnota tokenu, očekáván unární operátor",
                       __LINE__);
        };
      } else if (active_token.type == SYMBOL_TYPE &&
                 strcmp(active_token.value, "(") == 0) {

        check_next_token();
        // pokud je závorka prázdná nastavím ho na UNIT
        if (active_token.type == SYMBOL_TYPE &&
            strcmp(active_token.value, ")") == 0) {

          temp_expression = allocate_expression(temp_expression);

          temp_expression->type = LITERAL_UNIT;

          stack_switch = 0;
          stack_push(node_stack, temp_expression);
          check_next_token();

        } else
          stack_push(operator_stack, (void *)(uintptr_t)BRACK_START);
      } else {

        syntax_error("Špatná hodnota tokenu, očekáváno '('", __LINE__);
      }
      // Režim pro čtení operátorů
    } else if (stack_switch == 0) {
      if (active_token.type == SYMBOL_TYPE) {
        if (strcmp(active_token.value, "(") == 0) {

          stack_nodes = (AST_Node *)stack_top(node_stack);
          check_stack_pop(node_stack);

          if (stack_nodes->type == IDENT) {
            if (temp_expression == NULL)
              temp_expression = allocate_expression(temp_expression);

            temp_expression->type = FUN_CALL;
            temp_expression->nodeData.callFunData.call_fun_name =
                stack_nodes->nodeData.idVarData.id_name;

            temp_expression->nodeData.callFunData.fun_args = exp_in_brackets();
            stack_switch = 0;

            stack_push(stack_top(used_symbols), temp_expression);

            stack_push(node_stack, temp_expression);
            continue;
          }
        } else if (strcmp(active_token.value, ".") == 0) {

          temp_expression = allocate_expression(temp_expression);

          temp_expression->type = DOT_ACCESS;

          check_next_token();
          if (active_token.type == ID_TYPE) {
            temp_expression->nodeData.accessDotData.before_dot =
                (AST_Node *)stack_top(node_stack);
            check_stack_pop(node_stack);

            temp_expression->nodeData.accessDotData.name_after_dot =
                special_strdup(active_token.value);

            check_next_token();
            if (active_token.type == SYMBOL_TYPE &&
                strcmp(active_token.value, "(") == 0) {
              temp_expression->nodeData.accessDotData.dot_args =
                  exp_in_brackets();
            } else
              temp_expression->nodeData.accessDotData.dot_args = NULL;

            stack_push(node_stack, temp_expression);
            stack_switch = 0;
          }
          continue;

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

        } else if (strcmp(active_token.value, "++") == 0) {

          operator_type = CON_SEZNAM;
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

        temp_expression = allocate_expression(temp_expression);

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

          stack_dtor(operator_stack);
          stack_dtor(node_stack);

          return expression;

        } else {

          syntax_error("Očekáván konec zásobníku", __LINE__);
        }
      }
    }
  }
}

/**
 *
 * @brief Zpracovává parametry funcke jako spojový seznam
 *
 *
 * @return Ukazatel na začátek spojového seznamu
 *
 *
 * @warning Jakmile nastane jakákoliv chyba zápisu argumentů okamžitě ukončí
 * program exit(2)
 */

head *parse_param() {

  head *params = special_malloc(sizeof(head));

  ListParam *list_end = NULL;

  params->first = NULL;

  // existuje čárka
  bool after_comma = false;
  // je za čárkou EOL
  bool had_eol = false;

  while (1) {
    check_next_token();

    if (active_token.type == SYMBOL_TYPE) {

      if (strcmp(active_token.value, "EOL") == 0) {
        had_eol = true;
        continue;
      }
      if (strcmp(active_token.value, ")") == 0) {
        if (after_comma && !had_eol) {
          syntax_error("Špatná hodnota tokenu, očekáváno 'EOL'", __LINE__);
        }

        return params;
      }
    } else if (active_token.type == ID_TYPE) {

      after_comma = false;
      had_eol = false;

      ListParam *new_param = special_malloc(sizeof(ListParam));

      new_param->next = NULL;
      new_param->param_name = special_strdup(active_token.value);

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

          syntax_error("Spatna hodnota tokenu, ocekavano ':'", __LINE__);
        };

        check_next_token();
        check_for_eol();
        if (active_token.type != KEYWORD_TYPE) {

          syntax_error("Spatny typ tokenu, ocekavano KEYWORD", __LINE__);
        };
        list_end->type = type_transfer();

        if (strcmp(active_token.value, "List") == 0) {
          check_next_token();
          check_for_eol();
          if (active_token.type != SYMBOL_TYPE) {

            syntax_error("Spatny typ tokenu, ocekavano SYMBOL", __LINE__);
          };
          if (strcmp(active_token.value, "[") != 0) {

            syntax_error("Spatna hodnota tokenu, ocekavano '['", __LINE__);
          };

          check_next_token();
          check_for_eol();
          if (active_token.type != KEYWORD_TYPE) {

            syntax_error("Spatny typ tokenu, ocekavano KEYWORD", __LINE__);
          };
          list_end->list_type = type_transfer();

          check_next_token();
          check_for_eol();
          if (active_token.type != SYMBOL_TYPE) {
            syntax_error("Spatny typ tokenu, ocekavano SYMBOL", __LINE__);
          };
          if (strcmp(active_token.value, "]") != 0) {
            syntax_error("Spatna hodnota tokenu, ocekavano ']'", __LINE__);
          };
        }
        check_next_token();
        check_for_eol();
        if (active_token.type != SYMBOL_TYPE) {
          syntax_error("Spatny typ tokenu, ocekavano SYMBOL", __LINE__);
        };
        if (strcmp(active_token.value, ")") == 0)
          return params;
        if (strcmp(active_token.value, ",") == 0) {
          after_comma = true;
          had_eol = false;
          continue;
        }

        syntax_error("Špatná hodnota tokenu, očekáváno ',' nebo ')'", __LINE__);

      } else {
        syntax_error("Spatny typ tokenu, ocekavano SYMBOL", __LINE__);
      };
    }
  }
}

/**
 *
 * @brief Volá funcki na zpracování parametrů funkce nebo těla funkce,
 * zpracovává co funcke vrací
 *
 *
 * @warning Při špatném zápisu návratových hodnot funkce okamžite ukončí průběch
 * programu exit(2)
 */

void parse_fun() {

  int indent = sum_stack(stack_indent);

  AST_Node *new_fun = special_malloc(sizeof(AST_Node));

  new_fun->type = FUN_DEF;
  new_fun->next = NULL;

  if (youre_main_node) {
    youre_main_node = false;
    main_node = new_fun;
  }

  check_next_token();
  if (active_token.type == ID_TYPE) {
    new_fun->nodeData.funData.fun_name = special_strdup(active_token.value);
  } else {
    syntax_error("Spatny typ tokenu, ocekavano ID", __LINE__);
  };

  // může být redefinovaný print, pokud není v global scope???
  char *fun_name = new_fun->nodeData.funData.fun_name;
  allow_symbol_insert(fun_name);
  insert_symbol(symbol_table, fun_name, new_fun);
  scope_enter(symbol_table);
  stack_push(used_symbols, stack_ctor());

  check_next_token();
  if (active_token.type == SYMBOL_TYPE) {
    if (strcmp(active_token.value, "(") != 0) {
      {
        syntax_error("Spatna hodnota tokenu, ocekavano '('", __LINE__);
      };
    }
  } else {
    syntax_error("Spatny typ tokenu, ocekavano SYMBOL", __LINE__);
  };

  new_fun->nodeData.funData.param_list = parse_param();

  ListParam *current_param = new_fun->nodeData.funData.param_list->first;

  while (current_param != NULL) {

    allow_symbol_insert(current_param->param_name);

    AST_Node *param_node = allocate_expression(NULL);

    param_node->type = type_to_literal(current_param->type);

    insert_symbol(symbol_table, current_param->param_name, param_node);

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

            syntax_error("Spatny typ tokenu, ocekavano SYMBOL", __LINE__);
          };
          if (strcmp(active_token.value, "[") != 0) {

            syntax_error("Spatna hodnota tokenu, ocekavano '[' ", __LINE__);
          };

          check_next_token();
          if (active_token.type != KEYWORD_TYPE) {

            syntax_error("Spatny typ tokenu, ocekavano KEYWORD", __LINE__);
          };
          // Uložení hodnoty které bude mít vracený list v sobě uložené
          new_fun->nodeData.funData.return_list = type_transfer();

          check_next_token();
          if (active_token.type != SYMBOL_TYPE) {

            syntax_error("Spatny typ tokenu, ocekavano SYMBOL", __LINE__);
          };
          if (strcmp(active_token.value, "]") != 0) {

            syntax_error("Spatna hodnota tokenu, ocekavano ']'", __LINE__);
          };
        }

        check_next_token();
        if (active_token.type == SYMBOL_TYPE) {
          if (strcmp(active_token.value, "=") == 0) {
            new_fun->nodeData.funData.body = parse_fun_body(indent);
          } else {

            syntax_error("Spatna hodnota tokenu, ocekavano '='", __LINE__);
          };
        } else {

          syntax_error("Spatny typ tokenu, ocekavano SYMBOL", __LINE__);
        };
      } else {
        syntax_error("Spatny typ tokenu, ocekavano KEYWORD", __LINE__);
      };
    } else if (strcmp(active_token.value, "=") == 0) {
      new_fun->nodeData.funData.body = parse_fun_body(indent);
    } else {

      syntax_error("Spatna hodnota tokenu, ocekavano '='", __LINE__);
    };
  } else {
    syntax_error("Spatny typ tokenu, ocekavano SYMBOL", __LINE__);
  };

  if (check_pop() == 1) {
    fprintf(stderr, UNKNOWN_ID);
    special_exit(3);
  }

  scope_leave(symbol_table);
  return;
}

/**
 *
 * @brief Zpracovává tělo funcke
 *
 * @param origin_indent Odsazení těla funkce
 *
 * @warning Pokud je v tělo funkce špatně zapsané okamžitě pozastaví průběch
 * programu exit(2)
 */
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

      syntax_error("Nečekená konec souboru", __LINE__);
    };
    // stack_indent můžu mít hodnotu 0
    if (sum_stack(stack_indent) <= origin_indent) {

      syntax_error("Chyba odsazení", __LINE__);
    };
    origin_indent = sum_stack(stack_indent);

    while (1) {

      // jakoby vylepšení check for eol který přeskakuje aji středníky
      while (active_token.type == SYMBOL_TYPE &&
             (strcmp(active_token.value, "EOL") == 0 ||
              strcmp(active_token.value, ";") == 0)) {

        check_next_token();
      }

      if (active_token.type == SYMBOL_TYPE &&
          strcmp(active_token.value, "EOF") == 0)
        return statements;
      if (sum_stack(stack_indent) < origin_indent)
        return statements;
      // odsazení se nesmí jen tak zvětšit
      if (sum_stack(stack_indent) > origin_indent) {

        syntax_error("Chyba odsazení", __LINE__);
      };

      switch (active_token.type) {

      case KEYWORD_TYPE:

        // var(1) a val(0) se rozlišuje podle bool
        if (strcmp(active_token.value, "val") == 0 ||
            strcmp(active_token.value, "var") == 0) {

          temp_statement = parse_var();

          char *var_name = temp_statement->nodeData.varData.var_name;

          allow_symbol_insert(var_name);
          insert_symbol(symbol_table, var_name, temp_statement);
          check_assign_types(temp_statement,
                             temp_statement->nodeData.varData.expression);

        } else if (strcmp(active_token.value, "while") == 0)
          temp_statement = parse_while(origin_indent);

        else if (strcmp(active_token.value, "if") == 0)
          temp_statement = parse_if(origin_indent);

        else if (strcmp(active_token.value, "for") == 0)
          temp_statement = parse_for(origin_indent);

        else if (strcmp(active_token.value, "return") == 0)
          temp_statement = parse_return();

        else {
          syntax_error("Spatna hodnota tokenu, ocekavano 'while', 'if', 'for' "
                       "nebo 'return'",
                       __LINE__);
        };
        break;

      case ID_TYPE:
        // funkce která už bude rozhodovat co s id vlastně dělat
        // update_symbol(symbol_table, special_strdup(active_token.value),
        //       temp_statement); // tohle rozbije symbol table, co se tu
        //       vlastně vůbec děje???
        temp_statement = parse_id();
        if (temp_statement->type == IDENT &&
            get_symbol(symbol_table,
                       temp_statement->nodeData.idVarData.id_name) == NULL) {
          fprintf(stderr, UNKNOWN_ID);
          special_exit(3);
        }
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

/**
 *
 * @brief Zpracovává proměnné, statické i dynamické
 *
 * @return  Ukazatel na uzel zpracované proměnné
 *
 * @warning Při špatně napsané proměnné okamžitě zastaví průběh programu exit(2)
 */

AST_Node *parse_var() {

  AST_Node *temp_var = special_malloc(sizeof(AST_Node));

  temp_var->type = VAR_DEF;
  temp_var->next = NULL;

  if (strcmp(active_token.value, "var") != 0)
    temp_var->nodeData.varData.isvar = 0;
  else
    temp_var->nodeData.varData.isvar = 1;

  check_next_token();
  if (active_token.type == ID_TYPE) {
    temp_var->nodeData.varData.var_name = special_strdup(active_token.value);
  } else {
    syntax_error("Spatny typ tokenu, ocekavano ID", __LINE__);
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

            syntax_error("Spatny typ tokenu, ocekavano SYMBOL", __LINE__);
          };
          if (strcmp(active_token.value, "[") != 0) {

            syntax_error("Spatna hodnota tokenu, ocekavano '['", __LINE__);
          };

          check_next_token();
          if (active_token.type != KEYWORD_TYPE) {

            syntax_error("Spatny typ tokenu, ocekavano KEYWORD", __LINE__);
          };

          temp_var->nodeData.varData.varListType = type_transfer();

          check_next_token();
          if (active_token.type != SYMBOL_TYPE) {

            syntax_error("Spatny typ tokenu, ocekavano SYMBOL", __LINE__);
          };
          if (strcmp(active_token.value, "]") != 0) {

            syntax_error("Spatna hodnota tokenu, ocekavano ']'", __LINE__);
          };
        }

        check_next_token();
        if (active_token.type == SYMBOL_TYPE) {
          if (strcmp(active_token.value, "=") != 0) {

            syntax_error("Spatna hodnota tokenu, ocekavano '='", __LINE__);
          };
          check_next_token();
          temp_var->nodeData.varData.expression = parse_expression(NULL);

        } else {
          syntax_error("Spatny typ tokenu, ocekavano SYMBOL", __LINE__);
        };
      } else {

        syntax_error("Spatny typ tokenu, ocekavano KEYWORD", __LINE__);
      };
      // rovná se znamená že proměnná nemá ještě definovaný typ
    } else if (strcmp(active_token.value, "=") == 0) {
      temp_var->nodeData.varData.varType = TYPE_UNDEF;
      check_next_token();
      temp_var->nodeData.varData.expression = parse_expression(NULL);
      temp_var->nodeData.varData.varType =
          literal_to_type(temp_var->nodeData.varData.expression);
    } else {

      syntax_error("Spatna hodnota tokenu, ocekavano '='", __LINE__);
    };
  } else {

    syntax_error("Spatny typ tokenu, ocekavano SYMBOL", __LINE__);
  };

  return temp_var;
}

/**
 *
 * @brief Zpracuje for loop
 *
 * @param origin_indent odsazení cyklu
 *
 * @return Ukazatel na uzel zpracovaného for cyklu
 *
 * @warning Pokud je cyklus špatně napsaný okamžitě ukončý průběh programu
 * exit(2)
 */

AST_Node *parse_for(int origin_indent) {

  scope_enter(symbol_table);
  stack_push(used_symbols, stack_ctor());

  AST_Node *temp_for = special_malloc(sizeof(AST_Node));

  temp_for->type = FOR_N;
  temp_for->next = NULL;

  int base_indent;
  bool is_indent = false;

  check_next_token();
  if (active_token.type == SYMBOL_TYPE &&
      strcmp(active_token.value, "EOL") == 0) {
    is_indent = true;
    check_for_eol();

    if (sum_stack(stack_indent) <= origin_indent) {

      syntax_error("Chyba odsazení", __LINE__);
    };

    base_indent = sum_stack(stack_indent);
  }

  if (active_token.type == ID_TYPE) {
    temp_for->nodeData.forData.var_name = special_strdup(active_token.value);
    char *var_name = temp_for->nodeData.forData.var_name;
    allow_symbol_insert(var_name);
    insert_symbol(symbol_table, var_name, temp_for);
  } else {
    syntax_error("Spatny typ tokenu, ocekavano ID", __LINE__);
  };

  check_next_token();
  if (active_token.type == OPERATOR_TYPE) {
    if (strcmp(active_token.value, "<-") != 0) {
      syntax_error("Spatna hodnota tokenu, ocekavano '<-'", __LINE__);
    };
  } else {
    syntax_error("Spatny typ tokenu, ocekavano OPERATOR", __LINE__);
  };

  check_next_token();
  temp_for->nodeData.forData.condition = parse_expression(NULL);
  literal_to_type(temp_for->nodeData.forData.condition);

  // pokud má odsazení
  if (is_indent == true) {
    check_for_eol();
    // zkontroluj jestli je to if a jestli je správné odsazení
    while (active_token.type == KEYWORD_TYPE &&
           strcmp(active_token.value, "if") == 0 &&
           sum_stack(stack_indent) == base_indent) {
      check_next_token();

      temp_for->nodeData.forData.if_condition = parse_expression(NULL);
      literal_to_type(temp_for->nodeData.forData.if_condition);

      check_for_eol();
    }
  }

  // odsazení pro do musí být stejné jako pro for
  if (sum_stack(stack_indent) != origin_indent) {

    syntax_error("Chyba odsazení", __LINE__);
  }

  if (active_token.type == KEYWORD_TYPE) {
    if (strcmp(active_token.value, "do") != 0) {
      fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
      syntax_error("Spatna hodnota tokenu, ocekavano 'do'", __LINE__);
    };
  } else {
    syntax_error("Spatny typ tokenu, ocekavano KEYWORD", __LINE__);
  };

  temp_for->nodeData.forData.body = parse_fun_body(origin_indent);

  if (check_pop() == 1) {
    fprintf(stderr, UNKNOWN_ID);
    special_exit(3);
  }
  scope_leave(symbol_table);

  return temp_for;
}

/**
 *
 * @brief Zpracuje if podmínku
 *
 * @param origin_indent odsazení podmínky
 *
 * @return Ukazatel na uzel se zpracovanou if podmínkou
 *
 * @warning Pokud je podmínka špatně napsaná okamžitě ukončí průběh programu
 * exit(2)
 */

AST_Node *parse_if(int origin_indent) {

  scope_enter(symbol_table);
  stack_push(used_symbols, stack_ctor());

  AST_Node *temp_if = special_malloc(sizeof(AST_Node));

  temp_if->type = IF_N;
  temp_if->next = NULL;

  // zavoláme token pro podmínku
  check_next_token();
  temp_if->nodeData.ifData.condition = parse_expression(NULL);
  literal_to_type(temp_if->nodeData.ifData.condition);

  if (active_token.type == KEYWORD_TYPE) {
    if (strcmp(active_token.value, "then") != 0) {

      syntax_error("Spatna hodnota tokenu, ocekavano 'then'", __LINE__);
    };
  } else {

    syntax_error("Spatny typ tokenu, ocekavano KEYWORD", __LINE__);
  };

  temp_if->nodeData.ifData.then_part = parse_fun_body(origin_indent);

  check_for_eol();

  if (strcmp(active_token.value, "else") == 0 &&
      sum_stack(stack_indent) == origin_indent) {

    temp_if->nodeData.ifData.else_part = parse_fun_body(origin_indent);

  } else {
    temp_if->nodeData.ifData.else_part = NULL;
    if (check_pop() == 1) {
      fprintf(stderr, UNKNOWN_ID);
      special_exit(3);
    }
    scope_leave(symbol_table);
    return temp_if;
  }
  if (check_pop() == 1) {
    fprintf(stderr, UNKNOWN_ID);
    special_exit(3);
  }
  scope_leave(symbol_table);
  return temp_if;
}

/**
 *
 * @brief Zpacuje while cyklus
 *
 * @param origin_indent Odsazení cyklu
 *
 * @return Ukazatel na uzel zpracovaného cyklu
 *
 * @warning Pokud je cyklus špatně napsaný okamžitě ukončí průběch programu
 * exit(2)
 */

AST_Node *parse_while(int origin_indent) {

  scope_enter(symbol_table);
  stack_push(used_symbols, stack_ctor());

  AST_Node *temp_while = special_malloc(sizeof(AST_Node));

  temp_while->type = WHILE_N;
  temp_while->next = NULL;

  check_next_token();
  temp_while->nodeData.whileData.condition = parse_expression(NULL);
  literal_to_type(temp_while->nodeData.whileData.condition);

  // nevolám next_token protože počítám stím že ho zavolal funkce
  // parse_expression
  if (active_token.type == KEYWORD_TYPE) {
    if (strcmp(active_token.value, "do") != 0) {
      fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
      syntax_error("Spatna hodnota tokenu, ocekavano 'do'", __LINE__);
    };
  } else {
    syntax_error("Spatny typ tokenu, ocekavano KEYWORD", __LINE__);
  };

  temp_while->nodeData.whileData.body = parse_fun_body(origin_indent);

  if (check_pop() == 1) {
    fprintf(stderr, UNKNOWN_ID);
    special_exit(3);
  }
  scope_leave(symbol_table);
  return temp_while;
}

/**
 *
 * @brief Zpracuje hodnoty v závorce u listů nebo volání funkce jako spojový
 * seznam
 *
 * @return Ukazatel na začátek spojového seznamu
 *
 * @warning Pokud jsou výrazy uvnitř funkce špatně zapsané okamžitě ukončí
 * průběch programu exit(2)
 */

AST_Node *exp_in_brackets() {

  AST_Node *temp_expression_list = NULL;
  AST_Node *expression_list = NULL;
  AST_Node *expression_list_end = NULL;

  check_next_token();
  check_for_eol();
  if (active_token.type == SYMBOL_TYPE &&
      strcmp(active_token.value, ")") == 0) {
    check_next_token();
    return expression_list;
  }

  while (1) {

    temp_expression_list = parse_expression(NULL);
    check_for_eol();

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

        bool had_eol = false;
        if (active_token.type == SYMBOL_TYPE &&
            strcmp(active_token.value, "EOL") == 0) {

          had_eol = true;
        }
        check_for_eol();

        if (active_token.type == SYMBOL_TYPE &&
            strcmp(active_token.value, ")") == 0) {

          if (!had_eol) {
            syntax_error("Špatná hodnota tokenu, očekáváno 'EOL'", __LINE__);
          }

          check_next_token();
          return expression_list;
        }

        continue;
      } else if (strcmp(active_token.value, ")") == 0) {
        check_next_token();
        return expression_list;

      } else {
        syntax_error("Spatna hodnota tokenu, ocekavano ')'", __LINE__);
      };
    } else {
      syntax_error("Spatny typ tokenu, ocekavano SYMBOL", __LINE__);
    };
  }
}

/**
 *
 * @brief Zpracuje list hodnot
 *
 * @return Ukazatel na uzel zpracovaného listu
 *
 * @warning Pokud je list špatně zapsaný okamžitě ukončý průběch programu
 * exit(2)
 */
AST_Node *parse_list() {

  AST_Node *temp_list = special_malloc(sizeof(AST_Node));

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
          syntax_error("Spatna hodnota tokenu, ocekavano '('", __LINE__);
        };

        temp_list->type = DOT_ACCESS;

      } else {
        syntax_error("Spatna hodnota tokenu, ocekavano 'range'", __LINE__);
      };
    } else {
      syntax_error("Spatna hodnota tokenu, ocekavano '.'", __LINE__);
      ;
    };
  }

  return temp_list;
}

/**
 *
 * @brief Zpracuje identifikátor, ten můžu být ještě nedefinovaný
 *
 * @return Ukazatel na uzel zpracovaného identifikátoru
 *
 * @warning Pokud je identifikátor špatně definovaný okamžitě ukončí průběch
 * programu exit(2)
 */
AST_Node *parse_id() {

  // uložení jména proměnné, protože ještě nevím o jaký typ výrazu se jedná
  char *temp_name = special_strdup(active_token.value);

  AST_Node *temp_id = special_malloc(sizeof(AST_Node));

  temp_id->next = NULL;

  check_next_token();
  // pokud tam je rovná se jedná se o přiřazení hodnoty
  if (active_token.type == SYMBOL_TYPE &&
      strcmp(active_token.value, "=") == 0) {

    temp_id->type = ASSIGN;

    temp_id->nodeData.assignData.var_name = temp_name;

    AST_Node *stored = get_symbol(symbol_table, temp_name);
    if (!stored->nodeData.varData.isvar) { // pokus o redefinici VAL (konstanty)
      fprintf(stderr, CONST_REDEF_ATTEMPT);
      special_exit(7);
    }

    check_next_token();
    temp_id->nodeData.assignData.equal = parse_expression(NULL);
    AST_Node *var =
        get_symbol(symbol_table, temp_id->nodeData.assignData.var_name);
    check_assign_types(var, temp_id->nodeData.assignData.equal);

    return temp_id;
  }

  temp_id->type = IDENT;
  temp_id->nodeData.idVarData.id_name = temp_name;
  return parse_expression(temp_id);
}

/**
 *
 * @brief Zpracuje return
 *
 * @return Ukazatel na uzel na zpracovaný return
 *
 * @warning Pokud je return špatně napsaný okamžitě ukončí průběch programu
 * exit(2)
 */
AST_Node *parse_return() {

  AST_Node *temp_return = special_malloc(sizeof(AST_Node));

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
    } else if (node->type == ADD) {
      count++;
    } else if (node->type == ADD) {
      count++;
    } else if (node->type == SUB) {
      count++;
    } else if (node->type == MUL) {
      count++;
    } else if (node->type == DIV) {
      count++;
    } else if (node->type == LESS) {
      count++;
    } else if (node->type == LESS_EQUAL) {
      count++;
    } else if (node->type == GREATER) {
      count++;
    } else if (node->type == GREATER_EQUAL) {
      count++;
    } else if (node->type == EQUALS) {
      count++;
    } else if (node->type == NOT_EQUAL) {
      count++;
    } else if (node->type == UMINUS) {
      count++;
    } else if (node->type == UPLUS) {
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

bool compare_param_arg(ListParam *param, AST_Node *arg) {
  if (arg->type == IDENT) {
    arg = get_symbol(symbol_table,
                     arg->nodeData.assignData.var_name); // fails for params
  }
  if (arg == NULL) {
    fprintf(stderr, UNKNOWN_ID);
    special_exit(3);
  }
  return type_compatible(param->type, arg, false);
}

int check_pop() {
  stackPtr used_top = stack_top(used_symbols);
  int failed = 0;
  while (stack_top(used_top) != NULL) {
    AST_Node *temp = stack_top(used_top);
    if (temp->type == IDENT) {
      failed = 1;
    } else if (temp->type == FUN_CALL) {
      char *temp_name = temp->nodeData.callFunData.call_fun_name;
      AST_Node *found_node = get_symbol(symbol_table, temp_name);
      if (found_node == NULL) {
        // failed = 1;
        stack_push(stalled, temp);
        AST_Node *cur_arg = temp->nodeData.callFunData.fun_args;
        while (cur_arg != NULL) {
          if (cur_arg->type == IDENT) {
            stack_push(saved_args,
                       get_symbol(symbol_table,
                                  cur_arg->nodeData.assignData.var_name));
          }
          cur_arg = cur_arg->next;
        }
      } else {
        if (found_node->type != FUN_DEF) {
          fprintf(stderr, REDEF_ATTEMPT);
          special_exit(4);
        }
        if (count_args(temp->nodeData.callFunData.fun_args) !=
            count_params(found_node->nodeData.funData.param_list->first)) {
          fprintf(stderr, INVALID_ARG_AMOUNT,
                  found_node->nodeData.funData.fun_name);
          special_exit(5);
        }
        temp->result = found_node->result;
        ListParam *current_param =
            found_node->nodeData.funData.param_list->first;
        AST_Node *current_arg = temp->nodeData.callFunData.fun_args;
        while (current_param != NULL) {
          if (!compare_param_arg(current_param, current_arg)) {
            fprintf(stderr, INVALID_ARG_TYPE,
                    found_node->nodeData.funData.fun_name);
            special_exit(5);
          }
          current_param = current_param->next;
          current_arg = current_arg->next;
        }
      }
    } else {
      // printf("Somehow stored a value that isn't IDENT or FUN_CALL. Type is
      // [%d]\n.", temp->type);
    }

    stack_pop(used_top);
  }
  stack_pop(used_symbols);
  if (stack_top(stalled) != NULL && used_symbols->top == NULL) {
    fprintf(stderr, UNKNOWN_ID);
    special_exit(3);
  }
  // while (stack_top(stalled) != NULL) {
  //   stack_push(stack_top(used_symbols), stack_top(stalled));
  //   stack_pop(stalled);
  // }
  return failed;
}

void setup_builtins() {
  {
    AST_Node *global_func = allocate_expression(NULL);
    ListParam *new_param = special_malloc(sizeof(ListParam));
    head *params = special_malloc(sizeof(head));
    global_func->nodeData.funData.param_list = params;
    global_func->nodeData.funData.param_list->first = new_param;
    global_func->nodeData.funData.return_type = TYPE_UNIT;
    global_func->result = TYPE_UNIT;
    global_func->type = FUN_DEF;
    new_param->next = NULL;
    new_param->type = TYPE_ANY;
    insert_symbol(symbol_table, "print", global_func);
  }

  {
    AST_Node *global_func = allocate_expression(NULL);
    head *params = special_malloc(sizeof(head));
    global_func->nodeData.funData.param_list = params;
    global_func->nodeData.funData.param_list->first = NULL;
    global_func->nodeData.funData.return_type = TYPE_STRING;
    global_func->result = TYPE_STRING;
    global_func->type = FUN_DEF;
    insert_symbol(symbol_table, "readLine", global_func);
  }

  {
    AST_Node *global_func = allocate_expression(NULL);
    head *params = special_malloc(sizeof(head));
    global_func->nodeData.funData.param_list = params;
    global_func->nodeData.funData.param_list->first = NULL;
    global_func->nodeData.funData.return_type = TYPE_INT;
    global_func->result = TYPE_INT;
    global_func->type = FUN_DEF;
    insert_symbol(symbol_table, "readInt", global_func);
  }

  {
    AST_Node *global_func = allocate_expression(NULL);
    head *params = special_malloc(sizeof(head));
    global_func->nodeData.funData.param_list = params;
    global_func->nodeData.funData.param_list->first = NULL;
    global_func->nodeData.funData.return_type = TYPE_DOUBLE;
    global_func->result = TYPE_DOUBLE;
    global_func->type = FUN_DEF;
    insert_symbol(symbol_table, "readDouble", global_func);
  }

  {
    AST_Node *global_func = allocate_expression(NULL);
    ListParam *new_param = special_malloc(sizeof(ListParam));
    head *params = special_malloc(sizeof(head));
    global_func->nodeData.funData.param_list = params;
    global_func->nodeData.funData.param_list->first = new_param;
    global_func->nodeData.funData.return_type = TYPE_STRING;
    global_func->result = TYPE_STRING;
    global_func->type = FUN_DEF;
    new_param->next = NULL;
    new_param->type = TYPE_ANY;
    insert_symbol(symbol_table, "toStr", global_func);
  }

  {
    AST_Node *global_func = allocate_expression(NULL);
    ListParam *new_param = special_malloc(sizeof(ListParam));
    head *params = special_malloc(sizeof(head));
    global_func->nodeData.funData.param_list = params;
    global_func->nodeData.funData.param_list->first = new_param;
    global_func->nodeData.funData.return_type = TYPE_INT;
    global_func->result = TYPE_INT;
    global_func->type = FUN_DEF;
    new_param->next = NULL;
    new_param->type = TYPE_DOUBLE;
    insert_symbol(symbol_table, "toInt", global_func);
  }

  {
    AST_Node *global_func = allocate_expression(NULL);
    ListParam *new_param = special_malloc(sizeof(ListParam));
    head *params = special_malloc(sizeof(head));
    global_func->nodeData.funData.param_list = params;
    global_func->nodeData.funData.param_list->first = new_param;
    global_func->nodeData.funData.return_type = TYPE_INT;
    global_func->result = TYPE_INT;
    global_func->type = FUN_DEF;
    new_param->next = NULL;
    new_param->type = TYPE_STRING;
    insert_symbol(symbol_table, "strLength", global_func);
  }

  {
    AST_Node *global_func = allocate_expression(NULL);
    ListParam *new_param1 = special_malloc(sizeof(ListParam));
    ListParam *new_param2 = special_malloc(sizeof(ListParam));
    ListParam *new_param3 = special_malloc(sizeof(ListParam));
    head *params = special_malloc(sizeof(head));
    global_func->nodeData.funData.param_list = params;
    global_func->nodeData.funData.param_list->first = new_param1;
    global_func->nodeData.funData.return_type = TYPE_STRING;
    global_func->result = TYPE_STRING;
    global_func->type = FUN_DEF;
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
    ListParam *new_param1 = special_malloc(sizeof(ListParam));
    ListParam *new_param2 = special_malloc(sizeof(ListParam));
    head *params = special_malloc(sizeof(head));
    global_func->nodeData.funData.param_list = params;
    global_func->nodeData.funData.param_list->first = new_param1;
    global_func->nodeData.funData.return_type = TYPE_INT;
    global_func->result = TYPE_INT;
    global_func->type = FUN_DEF;
    new_param1->next = new_param2;
    new_param2->next = NULL;
    new_param1->type = TYPE_STRING;
    new_param2->type = TYPE_INT;
    insert_symbol(symbol_table, "ord", global_func);
  }

  {
    AST_Node *global_func = allocate_expression(NULL);
    ListParam *new_param = special_malloc(sizeof(ListParam));
    head *params = special_malloc(sizeof(head));
    global_func->nodeData.funData.param_list = params;
    global_func->nodeData.funData.param_list->first = new_param;
    global_func->nodeData.funData.return_type = TYPE_STRING;
    global_func->result = TYPE_STRING;
    global_func->type = FUN_DEF;
    new_param->next = NULL;
    new_param->type = TYPE_INT;
    insert_symbol(symbol_table, "chr", global_func);
  }
}

void allow_symbol_insert(char *symbol_name) {
  if (tree_get(symbol_table->scopes->top->value, symbol_name) != NULL) {
    fprintf(stderr,
            REDEF_ATTEMPT); // nepovolená redefinice funkce nebo proměnné
    special_exit(4);
  }
  char *builtins[] = {"print", "readLine",  "readInt", "readDouble", "toStr",
                      "toInt", "strLength", "subStr",  "ord",        "chr"};
  for (int i = 0; i < 10; i++) {
    if (strcmp(builtins[i], symbol_name) == 0) {
      fprintf(stderr,
              BUILTIN_REDEF_ATTEMPT); // nepovolená redefinice vestavěné funkce
      special_exit(4);
    }
  }
}

dataType get_operation_type(AST_Node *expr, bool allow_string) {
  dataType right = literal_to_type(expr->nodeData.biData.right);
  if (expr->type == UMINUS || expr->type == UPLUS) {
    if (expr->nodeData.biData.left->type == LITERAL_STRING) {
      fprintf(stderr, TYPE_CONVERSION_FAIL);
      special_exit(6);
    }
  }
  if (type_compatible(right, expr->nodeData.biData.left, true)) {
    if (right == TYPE_STRING && !allow_string) {
      fprintf(stderr, TYPE_CONVERSION_FAIL);
      special_exit(6);
    }
    return right;
  } else {
    fprintf(stderr, TYPE_CONVERSION_FAIL);
    special_exit(6);
  }
}

dataType literal_to_type(AST_Node *expr) {
  if (expr == NULL)
    return TYPE_UNDEF;
  NodeType node_type = expr->type;
  switch (node_type) {
  case LITERAL_INT:
    return TYPE_INT;
  case LITERAL_DOUBLE:
    return TYPE_DOUBLE;
  case LITERAL_STRING:
    return TYPE_STRING;
  case LITERAL_BOOL:
    return TYPE_BOOL;
  case LITERAL_UNIT:
    return TYPE_UNIT;
  case VAR_DEF:
    return expr->nodeData.varData.varType;
  case IDENT: {
    AST_Node *ident =
        get_symbol(symbol_table, expr->nodeData.assignData.var_name);
    return literal_to_type(ident);
  }
  case FUN_CALL: {
    AST_Node *func_def =
        get_symbol(symbol_table, expr->nodeData.funData.fun_name);
    return func_def->nodeData.funData.return_type;
  }
  case ADD:
    return get_operation_type(expr, true);
  case SUB:
    return get_operation_type(expr, false);
  case MUL:
    return get_operation_type(expr, false);
  case DIV:
    return get_operation_type(expr, false);
  case LESS:
    return get_operation_type(expr, true);
  case LESS_EQUAL:
    return get_operation_type(expr, true);
  case GREATER:
    return get_operation_type(expr, true);
  case GREATER_EQUAL:
    return get_operation_type(expr, true);
  case EQUALS:
    return get_operation_type(expr, true);
  case NOT_EQUAL:
    return get_operation_type(expr, true);
  case UMINUS:
    return get_operation_type(expr, false);
  case UPLUS:
    return get_operation_type(expr, false);
  default:
    return TYPE_UNDEF;
  }
}

NodeType type_to_literal(dataType data_type) {
  switch (data_type) {
  case TYPE_INT:
    return LITERAL_INT;
  case TYPE_DOUBLE:
    return LITERAL_DOUBLE;
  case TYPE_STRING:
    return LITERAL_STRING;
  case TYPE_BOOL:
    return LITERAL_BOOL;
  case TYPE_UNIT:
    return LITERAL_UNIT;
  default:
    return LITERAL_STRING;
  }
}

bool type_compatible(dataType a, AST_Node *expr, bool imp_double_int) {
  NodeType nodeType = expr->type;
  dataType b;
  if (nodeType == IDENT) {
    AST_Node *ident =
        get_symbol(symbol_table, expr->nodeData.assignData.var_name);
    b = literal_to_type(ident);
  } else if (nodeType == FUN_CALL) {
    AST_Node *b_node =
        get_symbol(symbol_table, expr->nodeData.callFunData.call_fun_name);
    b = b_node->nodeData.funData.return_type;
  } else {
    b = literal_to_type(expr);
  }
  if (a == b)
    return true;
  if ((imp_double_int && a == TYPE_INT && b == TYPE_DOUBLE) ||
      (a == TYPE_DOUBLE && b == TYPE_INT))
    return true;
  if (a == TYPE_UNDEF)
    return true;
  if (b == TYPE_UNDEF)
    return true;
  if (a == TYPE_ANY)
    return true;
  return false;
}

dataType getVarType(AST_Node *var) {
  if (var->nodeData.varData.varType == TYPE_UNDEF) {
    return literal_to_type(var->nodeData.varData.expression);
  } else
    return var->nodeData.varData.varType;
}

void check_assign_types(AST_Node *var, AST_Node *expr) {
  AST_Node *current = expr;
  while (current != NULL) {
    if (!type_compatible(getVarType(var), current, false)) {
      fprintf(stderr, ASSIGN_TYPE_FAIL, var->nodeData.varData.var_name);
      special_exit(6);
    }
    current = current->next;
  }
}

void *special_malloc(size_t size) {
  void *ptr = calloc(1, size);
  if (ptr == NULL) {
    fprintf(stderr, MALLOC_FAIL);
    special_exit(99);
  }
  stack_push(memory_allocs, ptr);
  return ptr;
}

char *special_strdup(char *str) {
  char *new_str = strdup(str);
  if (new_str == NULL) {
    fprintf(stderr, MALLOC_FAIL);
    special_exit(99);
  }
  stack_push(memory_allocs, new_str);
  return new_str;
}

void special_exit(int code) {
  exit_code = code;
  while (stack_top(memory_allocs) != NULL) {
    free(stack_top(memory_allocs));
    stack_pop(memory_allocs);
  }
  stack_dtor(stack_indent);
  symbol_table_dtor(symbol_table);
  stack_dtor(used_symbols);
  stack_dtor(memory_allocs);
  exit(code);
}
