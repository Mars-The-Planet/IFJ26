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

//#include "parser.h"


/**
 * @brief Výčet typů uzlů abstraktního syntaktického stromu (AST)
 */
typedef enum ASTNodeType {
    ADD,
    SUB,
    MUL,
    DIV,
    LESS,
    EQUALS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUEAL,
    AND,
    OR,
    UMINUS,
    UPLUS,
    NOT,
    IF,
    WHILE,
    FOR,
    IDENT,
    LIST,
    DOT_ACCESS,
    BLOCK,          // pro posloupnost příkazů
    ASSIGN,
    VAR_DEF,        // TODO: bool kde budu rozhodovat jestli se jedná od VAL(true) nebo VAR(false)
    FUN_DEF,
    FUN_CALL,
    LITERAL_INT,
    LITERAL_DOUBLE,
    LITERAL_STRING,
    LITERAL_BOOL,
    LITERAL_UNIT,
    RETURN,
} NodeType;

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
 * @brief Vložení typu unit (nil) kde je potřeba
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
void get_operands(Node *node);

/**
 * @brief Generování kódu pro operátor menší než (<)
 * @param node Ukazatel na uzel AST
 */
void less(Node *node);

/**
 * @brief Generování kódu pro operátor větší než (>)
 * @param node Ukazatel na uzel AST
 */
void greater(Node *node);

/**
 * @brief Generování kódu pro operátor rovnosti (==)
 * @param node Ukazatel na uzel AST
 */
void equals(Node *node);

/**
 * @brief Generování kódu pro operátor menší nebo rovno (<=)
 * @param node Ukazatel na uzel AST
 */
void less_equal(Node *node);

/**
 * @brief Generování kódu pro operátor větší nebo rovno (>=)
 * @param node Ukazatel na uzel AST
 */
void greater_equal(Node *node);

/**
 * @brief Generování kódu pro operátor nerovnosti (!=)
 * @param node Ukazatel na uzel AST
 */
void not_equal(Node *node);

/**
 * @brief Logické negování (NOT)
 * @param node Ukazatel na uzel AST
 */
void logical_not(Node *node);

/**
 * @brief Struktura pro logické AND se zkratkovým vyhodnocováním
 * @param node Ukazatel na uzel AST
 */
void logical_and(Node *node);

/**
 * @brief Struktura pro logické OR se zkratkovým vyhodnocováním
 * @param node Ukazatel na uzel AST
 */
void logical_or(Node *node);

/**
 * @brief Generátor pro unární plus
 * @param node Ukazatel na uzel AST
 */
void unar_plus(Node *node);

/**
 * @brief Generátor pro unární mínus
 * @param node Ukazatel na uzel AST
 */
void unar_minus(Node *node);

/**
 * @brief Generátor pro sčítání (nebo řetězení stringů)
 * @param node Ukazatel na uzel AST
 */
void add(Node *node);

/**
 * @brief Generátor pro násobení
 * @param node Ukazatel na uzel AST
 */
void mul(Node *node);

/**
 * @brief Generátor pro děllení (float / integer dělení podle typů)
 * @param node Ukazatel na uzel AST
 */
void div(Node *node);

/**
 * @brief Hlavní dispatcher – generátor kódu pro jednotlivé uzly AST
 * @param node Ukazatel na uzel AST
 */
void generate_node(Node *node);

#endif // GENERATOR_H
