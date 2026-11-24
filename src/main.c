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
  has_main = 0; // nesmí mít parametry a výsledkem je unit
  youre_main_node = false;
  main_node = NULL;
  exit_code = 0;
  memory_allocs = stack_ctor();

  setup_builtins();

  code_prolog();
  the_parser();

  if (has_main != 1) {
    fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
    exit(3);
  }

  if (count_params(main_node->nodeData.funData.param_list->first) != 0) {
    fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
    exit(5);
  }

  if (main_node->nodeData.funData.return_type != TYPE_UNIT) {
    fprintf(stderr, "Chyba syntaxe na radku %d\n", __LINE__);
    exit(10);
  }

  stackPtr func_stack = stack_ctor();
  avl_treePtr symtree = symbol_table->scopes->top->value;
  insert_func(func_stack, symtree->root);

  generate_program(func_stack, main_node->nodeData.funData.fun_name);

  special_exit(0);
}