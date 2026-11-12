/**
 * @file IFJ26/src/generator.h, projekt ifj
 * @author xmolikj00 
 * @brief hlavičkový soubor pro generátor kódu pro projekt IFJ26
 * @date 31.10.2026
 */

#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "parser.h"
//#include "parser.h"


/**
 * @brief Funkce pro int z ifj26 do ifjcode26
 * @param value Hodnota původního celého čísla v ifj26 
 */
void literal_int(int value);

/**
 * @brief Pro převod float hodnoty
 * @param value Hodnota desetinného čísla
 */
void literal_double(double value);

/**
 * @brief Převod typu boolean
 * @param value Původní výraz před převodem (jako int)
 */
void literal_bool(int value);

/**
 * @brief v podstatě nic nedělá
 */
void literal_unit(void);

/**
 * @brief Typová konverze z int na float na zásobníku
 */
void int_to_float(void);

/**
 * @brief Pomocná funkce pro generování kódu pro operandy a konverze typů
 * @param node Ukazatel na uzel AST
 */
void get_operands(AST_Node *node);

/**
 * @brief Generování kódu pro operátor menší než (<)
 * @param node Ukazatel na uzel AST
 */
void less(AST_Node *node);

/**
 * @brief Generování kódu pro operátor větší než (>)
 * @param node Ukazatel na uzel AST
 */
void greater(AST_Node *node);

/**
 * @brief Generování kódu pro operátor rovnosti (==)
 * @param node Ukazatel na uzel AST
 */
void equals(AST_Node *node);

/**
 * @brief Generování kódu pro operátor menší nebo rovno (<=)
 * @param node Ukazatel na uzel AST
 */
void less_equal(AST_Node *node);

/**
 * @brief Generování kódu pro operátor větší nebo rovno (>=)
 * @param node Ukazatel na uzel AST
 */
void greater_equal(AST_Node *node);

/**
 * @brief Generování kódu pro operátor nerovnosti (!=)
 * @param node Ukazatel na uzel AST
 */
void not_equal(AST_Node *node);

/**
 * @brief Logické negování (NOT)
 * @param node Ukazatel na uzel AST
 */
void logical_not(AST_Node *node);

/**
 * @brief Struktura pro logické AND se zkratkovým vyhodnocováním
 * @param node Ukazatel na uzel AST
 */
void logical_and(AST_Node *node);

/**
 * @brief Struktura pro logické OR se zkratkovým vyhodnocováním
 * @param node Ukazatel na uzel AST
 */
void logical_or(AST_Node *node);

/**
 * @brief Generátor pro unární plus
 * @param node Ukazatel na uzel AST
 */
void unar_plus(AST_Node *node);

/**
 * @brief Generátor pro unární mínus
 * @param node Ukazatel na uzel AST
 */
void unar_minus(AST_Node *node);

/**
 * @brief Generátor pro sčítání (nebo řetězení stringů)
 * @param node Ukazatel na uzel AST
 */
void add(AST_Node *node);

/**
 * @brief funkce pro odečítání
 * @param node ukazatel na uzel
 */
void sub(AST_Node *node);

/**
 * @brief Generátor pro násobení
 * @param node Ukazatel na uzel AST
 */
void mul(AST_Node *node);

/**
 * @brief Generátor pro děllení (float / integer dělení podle typů)
 * @param node Ukazatel na uzel AST
 */
void divide(AST_Node *node);

/**
 * @brief Hlavní dispatcher – generátor kódu pro jednotlivé uzly AST
 * @param node Ukazatel na uzel AST
 */
void generate_node(AST_Node *node);

#endif // GENERATOR_H
