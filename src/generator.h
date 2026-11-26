/**
 * IFJ26 - Implementace překladače imperativního jazyka IFJ26
 * @file IFJ26/src/generator.h, projekt ifj
 * @author xmolikj00
 * @brief hlavičkový soubor pro generátor kódu pro projekt IFJ26
 * @date 31.10.2026
 */

#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

/**
 * @brief pomocná funkce pro určení posledního prvku seznamu
 * @param node ukazatel na uzel ast
 */
AST_Node *last_node(AST_Node *node);

/**
 * @brief zjistění parametru funkce
 */
dataType get_param_type(const char *func_name, int index);

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
 * @brief funkce pro stringy
 * @param string string
 */
void literal_string(char *string);

/**
 * @brief v podstatě nic nedělá
 */
void literal_unit(void);

/**
 * @brief načtení identifikátoru
 */
void ident(AST_Node *node);

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
void generate_equals(AST_Node *node);

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
 * @brief vygeneruje vnitřní blok, například tělo cyklu pro while
 * @param node Ukazatel na uzel AST
 */
void generate_block(AST_Node *node);

/**
 * @brief vygeneruje blok pro while
 * @param node Ukazatel na uzel AST
 */
void generate_while(AST_Node *node);

/**
 * @brief pro přiřazení proměnné
 * @param node ukazatel na uzel ast
 */
void assign(AST_Node *node);

/**
 * @brief pro definování proměnné
 * @param node ukazatel na uzel ast
 */
void var_def(AST_Node *node);

/**
 * @brief funcke pro generování větve pro if
 * @param block ukazatel na blok ve funkci
 * @param return_type požadovaný návratový typ funkce
 */
void generate_branch(AST_Node *block, dataType return_type);

/**
 * @brief pro uzel if, počítá s rozšířením control a uchovává podle toho
 * návratové typy
 * @param node ukazatel na uzel ast
 */
void generate_if(AST_Node *node);

/**
 * @brief reprezentace seznamu jako pole na indexu 0 délka, dál ostatní prvky
 */
void generate_list(AST_Node *node);

/**
 * @brief pro generování return
 * @param node ukazatel na uzel ast
 * @param function_return_type není přímo parametr ale využívá globální
 * proměnou, která se nastavuje právě kvůli této funkci
 */
void generate_return(AST_Node *node);

/**
 * @brief sbírá definici proměnných, jelikož mohou být definované jen jednou,
 * nemůžou být uvnitř cyklů, proměnné se definují hned při začátku funkce
 *
 * @param node ukazatel na uzel ast
 */
void collect_vars(AST_Node *node);

/**
 * @brief funkce pro vybírání parametrů ze zásobníku
 * používá rekurzi aby byl pravý parametr nejhlouběji a levý nejvíce navrchu
 * @param params ukazatel na hlavičku listu parametrů funkce
 */
void pop_params(ListParam *params);

/**
 * @brief funkce pro vytvoření spouštění funkcí
 *
 * @param node ukazatel na uzel ast
 */
void generate_fun(AST_Node *node);

/**
 * @brief vestavěná funkce readInt
 */
void read_int(void);

/**
 * @brief vestavěná funkce readDouble
 */
void read_double(void);

/**
 * @brief vestavěná funkce readLine
 */
void read_line(void);

/**
 * @brief vestavěná funkce readBoolean
 */
void read_boolean(void);

/**
 * @brief vestavěná funkce ord
 */
void ord(void);

/**
 * @brief vestavěná funkce subStr
 */
void sub_str(void);

/**
 * @brief vestavěná funkce toInt
 */
void to_int(AST_Node *arguments);

/**
 * @brief 
 */

/**
 * @brief Volání uživatelské funkce
 */
void call_user_function(AST_Node *node, char *name, AST_Node *arguments);

/**
 * @brief Vestavěná funkce print
 */
void print(AST_Node *arguments);

/**
 * @brief Vestavěná funkce chr
 */
void chr(AST_Node *arguments);

/**
 * @brief Vestavěná funkce strLength
 */
void str_length(AST_Node *arguments);

/**
 * @brief Převod na řetězec vestavěná funkce
 */
void to_string(dataType type, AST_Node *arguments);

/**
 * @brief Volání funkcí (rozcestník)
 */
void call_fun(AST_Node *node);

/**
 * @brief funkce získá parametry
 */
dataType get_param_type(const char *func_name, int index);
/**
 * @brief Hlavní dispatcher – generátor kódu pro jednotlivé uzly AST
 * @param node Ukazatel na uzel AST
 */
void generate_node(AST_Node *node);

/**
 * @brief hlavní funkce, spouští celý program
 * @param functions linked list který dodá parser
 * @param main_name jméno mainu
 */
void generate_program(stack *functions, const char *main_name);

#endif // GENERATOR_H
