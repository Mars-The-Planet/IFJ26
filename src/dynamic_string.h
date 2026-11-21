/**
 * IFJ26 - Implementace překladače imperativního jazyka IFJ26
 * @file dynamic_string.h
 * @author Matěj Růžička (xruzicm00)
 *
 * @brief Hlavičkový soubor nafukovacího řetězce s vkládáním pouze na konec
 * @date 13.11.2026
 */

#ifndef DYNAMIC_STRING_H
#define DYNAMIC_STRING_H

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Struktura nafukovacího řetězce
 */
typedef struct EDynamicString {
  char *val_ptr;
  int size;
  int max_size;
} dynamic_string, *dynamic_string_ptr;

/**
 * @brief Inicializuje nafukovací řetězec
 * @param string_ptr ukazatel na nafukovací řetězec
 * @param len velikost nafukovacího řetězce
 * @return vrátí 0 pokud nastavení proběhlo úspěšně, nebo 1 pokud neúspěšně
 */
int init_dynamic_string(dynamic_string_ptr string_ptr, int len);

/**
 * @brief Vloží znak za poslední prvek
 * @param string_ptr ukazatel na nafukovací řetězec
 * @param ch znak který má být vložen
 * @return vrátí 0 pokud vložení proběhlo úspěšně, nebo 1 pokud neúspěšně
 */
int insert_last(dynamic_string_ptr string_ptr, int ch);

/**
 * @brief Uvolní paměť nafukovacího řetězece
 * @param string_ptr ukazatel na nafukovací řetězec
 */
void destroy_dynamic_string(dynamic_string_ptr string_ptr);

#endif // DYNAMIC_STRING
