/**
 * IFJ26 - Implementace překladače imperativního jazyka IFJ26
 * @file dynamic_string.c
 * @author Matěj Růžička (xruzicm00)
 *
 * @brief Implementace nafukovacího řetězce s vkládáním pouze na konec
 * @date 7.11.2026
 */

#include "dynamic_string.h"

/**
 * @brief Inicializuje nafukovací řetězec
 * @param string_ptr ukazatel na nafukovací řetězec
 * @param len velikost nafukovacího řetězce
 * @return vrátí 0 pokud nastavení proběhlo úspěšně, nebo 1 pokud neúspěšně
 */
int init_dynamic_string(dynamic_string_ptr string_ptr, int len) {

  if (len <= 0) {
    fprintf(stderr, "ERROR: Velikost nafukovaciho retezce musi byt kladna\n");
    return 1;
  }

  string_ptr->max_size = len;
  string_ptr->size = 0;

  // calloc nastaví všude \0
  string_ptr->val_ptr = calloc(len, sizeof(char));

  if (string_ptr->val_ptr == NULL) {
    fprintf(stderr, "ERROR: Selhala alkoace noveho nafukovaciho retezce\n");
    return 1;
  }

  return 0;
}

/**
 * @brief Vloží znak za poslední prvek
 * @param string_ptr ukazatel na nafukovací řetězec
 * @param ch znak který má být vložen
 * @return vrátí 0 pokud vložení proběhlo úspěšně, nebo 1 pokud neúspěšně
 */
int insert_last(dynamic_string_ptr string_ptr, int ch) {

  // kontrola zda int ch je znakem
  if (ch < 0 || ch > 255) {
    fprintf(stderr,
            "ERROR: Prvek ktery ma byt vlozen neni znak, int ktery mel byt "
            "vlozen: %d\n",
            ch);
    return 1;
  }
  char c = (char)ch;

  // nafukování řetězce je-li potřeba
  if (string_ptr->size >= string_ptr->max_size - 1) {
    int new_max_size = string_ptr->max_size * 2;
    char *temp = realloc(string_ptr->val_ptr, new_max_size * sizeof(char));

    if (temp == NULL) {
      fprintf(stderr, "ERROR: Selhalo rozsireni dynamickeho retezce\n");
      return 1;
    }

    string_ptr->max_size = new_max_size;
    string_ptr->val_ptr = temp;
  }

  string_ptr->val_ptr[string_ptr->size] = c;
  string_ptr->val_ptr[string_ptr->size + 1] = '\0';

  string_ptr->size++;
  return 0;
}

/**
 * @brief Uvolní paměť nafukovacího řetězece
 * @param string_ptr ukazatel na nafukovací řetězec
 */
void destroy_dynamic_string(dynamic_string_ptr string_ptr) {
  if (string_ptr->val_ptr == NULL) {
    return;
  }

  free(string_ptr->val_ptr);
  string_ptr->val_ptr = NULL;
  string_ptr->max_size = 0;
  string_ptr->size = 0;
}
