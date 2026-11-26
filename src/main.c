/**
 * IFJ26 - Implementace překladače imperativního jazyka IFJ26
 * @file main.c
 * @author Radek Večeřa (xvecerr01)
 *
 * @brief Implementace spustitelného souboru pro překladač
 * @date 18.11.2026
 */

#include "main.h"

void insert_func(stackPtr stack, nodePtr node) {
  if (node == NULL)
    return;
  bool skip = false;
  for (int i = 0; ignored_funcs[i]; i++) {
    if (strcmp(node->key, ignored_funcs[i]) == 0) {
      skip = true;
    }
  }
  if (!skip) {
    stack_push(stack, node->value);
  }
  insert_func(stack, node->left);
  insert_func(stack, node->right);
}


int main() {
  stack_indent = stack_ctor();
  symbol_table = symbol_table_ctor();
  used_symbols = stack_ctor();
  stackPtr global_used_symbols = stack_ctor();
  stack_push(used_symbols, global_used_symbols);
  has_main = 0; // nesmí mít parametry a výsledkem je unit
  youre_main_node = false;
  main_node = NULL;
  exit_code = 0;
  memory_allocs = stack_ctor();
  stalled = stack_ctor();
  saved_args = stack_ctor();

  setup_builtins();

  code_prolog();
  parse_fun_headers();

  while (stack_top(saved_args) != NULL) {
    AST_Node *node = stack_top(saved_args);
    insert_symbol(symbol_table, node->nodeData.varData.var_name, node);
    stack_pop(saved_args);
  }
  while (stack_top(stalled) != NULL) {
    stack_push(stack_top(used_symbols), stack_top(stalled));
    stack_pop(stalled);
  }
  if (check_pop() == 1) {
    fprintf(stderr, UNKNOWN_ID);
    special_exit(3);
  }

  if (has_main != 1) {
    fprintf(stderr, NO_MAIN, has_main);
    special_exit(3);
  }

  if (count_params(main_node->nodeData.funData.param_list->first) != 0) {
    fprintf(stderr, MAIN_HAS_PARAMS);
    special_exit(5);
  }

  if (main_node->nodeData.funData.return_type != TYPE_UNIT) {
    fprintf(stderr, MAIN_WRONG_RETURN);
    special_exit(6);
  }

  stackPtr func_stack = stack_ctor();
  avl_treePtr symtree = symbol_table->scopes->top->value;
  insert_func(func_stack, symtree->root);

  generate_program(func_stack, main_node->nodeData.funData.fun_name);

  stack_dtor(global_used_symbols);
  stack_dtor(func_stack);

  special_exit(0);
}