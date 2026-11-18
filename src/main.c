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
}