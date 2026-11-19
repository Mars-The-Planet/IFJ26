/**
 * IFJ26 - Implementace překladače imperativního jazyka IFJ26
 * @file main.c
 * @author Radek Večeřa (xvecerr01)
 *
 * @brief Implementace spustitelného souboru pro překladač
 * @date 18.11.2026
 */

#include "main.h"


int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, USAGE_ERR);
    return 1;
  }
  FILE *file = fopen(argv[1], "r");
  if (file == NULL) {
    fprintf(stderr, FILE_NOT_FOUND_ERR, argv[1]);
    return 1;
  }
  stack_indent = stack_ctor();
  symbol_table = symbol_table_ctor();
  used_symbols = stack_ctor();
  has_main = 0; // nesmí mít parametry a výsledkem je unit
  youre_main_node = false;
  main_node = NULL;

  setup_builtins();

  if (set_target_file("test.txt") != 0)
    exit(99);

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

  printf("přeloženo\n");

  stack_dtor(stack_indent);
  symbol_table_dtor(symbol_table);
  stack_dtor(used_symbols);
}