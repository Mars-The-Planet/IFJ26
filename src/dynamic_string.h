/**
 * IFJ26 - Implementace překladače imperativního jazyka IFJ26
 * @file dynamic_string.h
 * @author Matěj Růžička (xruzicm00)
 *
 * @brief Hlavičkový soubor nafukovacího řetězce s vkládáním pouze na konec
 */

#ifndef DYNAMIC_STRING_H
#define DYNAMIC_STRING_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/**
 * @brief Struktura nafukovacího řetězce
 */
typedef struct EDynamicString {
    char *val_ptr;
    int size;
    int max_size;
} dynamic_string, *dynamic_string_ptr;


/**
 * @brief inicializuje nafukovacího řetězce
 * @param string_ptr ukazatel na nafukovacího řetězce
 * @param len velikost nafukovacího řetězce
 * @return vrátí 0 pokud nastavení proběhlo úspěšně, nebo 1 pokud neúspěšně
 */
int init_dynamic_string(dynamic_string_ptr string_ptr, int len);


/**
 * @brief vloží znak na poslední prvek
 * @param string_ptr ukazatel na nafukovací řetězce
 * @param ch znak který má být vložen 
 * @return vrátí 0 pokud nastavení proběhlo úspěšně, nebo 1 pokud neúspěšně
 */
int insert_last(dynamic_string_ptr string_ptr, int ch);


/**
 * @brief uvolní nafukovací řetězec
 * @param string_ptr ukazatel na nafukovací řetězce
 */
void destoy_dynamic_string(dynamic_string_ptr string_ptr);

#endif