/**
 * IFJ26 - Implementace překladače imperativního jazyka IFJ26
 * @file scanner.h
 * @author Matěj Růžička (xruzicm00)
 *
 * @brief Hlavičkový soubor lexikálního analyzátoru
 * @todo Zbytek stavů fsm
 */

#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "stack.h"
#include "dynamic_string.h"

/**
 * @brief Výčet typů tokenů
 */
typedef enum ETokenType {
	KEYWORD_TYPE,
	ID_TYPE,
	LIST_PROPERTY_TYPE,
	SYMBOL_TYPE,
	OPERATOR_TYPE,
	STRING_TYPE,
	INT_TYPE,
	DOUBLE_TYPE,
} tokenType;


/**
 * @brief Struktura mezistavů konečného automatu
 */
typedef enum EScannerState {
	DONE_STATE,		// extra stav značící dokončení čtení
	ERROR_STATE,	// extra stav značící že načtení neočekávaného znaku

	START_STATE,
	
	BAR_STATE,
	AMPERSAND_STATE,

	LESS_THAN_STATE,
	GREATER_THAN_STATE,
	EQUALS_STATE,
	EXCLAMATION_MARK_STATE,

	SLASH_STATE,
	SINGLELINE_COMMENT_STATE,
	MULTILINE_COMMENT_START_STATE,
	SIMPLE_MULTILINE_COMMENT_END_STATE,
	MULTILINE_COMMENT_STATE,
	MULTILINE_COMMENT_END_STATE,
	MULTILINE_COMMENT_TAIL_STATE,

	AT_STATE,

	// řetězce
	STRING_OPEN_STATE,
	STRING_CHAR_STATE,
	STRING_ESCAPE_SEQUENCE_STATE,
	EMPTY_STRING_STATE,
	MULTILINE_STRING_STATE,
	MULTILINE_STRING_CLOSING_STATE,
	MULTILINE_STRING_END_STATE,
	MULTILINE_STRING_DONE_STATE,

	// IDs
	UNDERSCORE_STATE,
	IDENTIFIER_STATE,

	// číselné literály
	NON_ZERO_INTEGER_STATE,
	INT_ZERO_STATE,

	NON_ZERO_STATE,
	RADIX_POINT_STATE,
	EXPONENT_STATE,
	EXPONENT_SIGN_STATE,
	DOUBLE_SIMPLE_STATE,
	DOUBLE_EXPONENT_STATE
} scannerState;


/** 
 * @brief Struktura tokenu vystupujího z lexikálního analyzátoru
*/
typedef struct SToken {
	tokenType type;
	char *value;
} token, *tokenPtr;


/**
 * @brief Nastaví soubor ze kterého čte LA.
 * @param file_name název souboru který chceme nastavit
 * @return vrátí 0 pokud nastavení proběhlo úspěšně, nebo 1 pokud neúspěšně
 */
int set_target_file(char *file_name);


/** 
 * @brief Hlavní funkce LA., která čte z cílového souboru a postupně vrací tokeny a odsazení z něj
 * @param tkn_ptr ukazatel na token který bude využit na předání nasledujícího tokenu
 * @param indent_stack_ptr ukazatal na zásobnik určující odsazení aktuálního řádku
*/
int next_token(tokenPtr tkn_ptr, stackPtr indent_stack_ptr);


/**
 * @brief Sčítá členy zásobníku, měl by být volán pouze nad zásobníkem naplňeným inty
 * @param stack_ptr ukazatel na zásobník jehož členové mají být posčítáni 
 * @return hodnota součtu všech prvků v zásobníku
 */
int sum_stack(stackPtr stack_ptr);


/**
 * @brief Funkce tvořící navratové hodnoty LA., vytvoří token a nastaví odsazení
 * @param tkn_ptr token do kterého nastavíme příslušný tokenType a řetězec reprezentují jeho hodnotu
 * @param indent_stack_ptr zásobník do kterého je uloženo aktualní odsazení které se také vypočítá v této funkci
 * @param state stav který nastavíme tokenu
 * @param value řetězec držící hodnotu kterou uložíme do tokenu
 */
void create_token(tokenPtr tkn_ptr, stackPtr indent_stack_ptr, tokenType state, char *value);


/**
 * @brief Uvolní paměť drženou pro řetězec v tokenu v případě že jsme využívali nafukovací řetězec
 * @param tkn_ptr token ze kterého získame ukazatel na místo v paměti s řetězcem
 */
void clear_token(tokenPtr tkn_ptr);


/**
 * @brief Funkce zjišťující zda znak může být součástí id (alfanumerické znaky a podržítko)
 * @param ch znak který kontrolujeme
 * @return true pokud může, false pokud ne
 */
bool can_be_in_IDs(int ch);


/**
 * @brief Funkce zjišťující zda řetězec je součástí pole řetězců
 * @param word řetězec jehož přítomnost kontrolujeme
 * @param list pole řetězců ve kterém word hledáme
 * @param len délka pole řetězců jenž prohledáváme
 * @return true pokud word najdeme, false pokud ne
 */
bool is_word_in_list(char *word, const char **list, int len);


/**
 * @brief Funkce počátečního stavu KA. LA.
 * @param ch načtený znak se kterým vstupjeme do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @param tkn_str nafukovací řetězec do nějž potencíalně budeme ukládat hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState 
 */
int start_state(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr tkn_str);


/**
 * @brief Funkce rozhodující do kterého stavu KA se půjde po načtení jednoho &
 * tento stav očekává další & jinak vrací error stav
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @return vrací int reprezntující stav KA. LA. scannerState 
 */
int ampersand(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr);


/**
 * @brief Funkce rozhodující do kterého stavu KA se půjde po načtení jednoho |
 * tento stav očekává další | jinak vrací error stav
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @return vrací int reprezntující stav KA. LA. scannerState 
 */
int bar(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr);


int exclamation_mark(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr);


int equals(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr);


int greater_than(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr);


int less_than(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr);


int slash(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr);


int singleline_comment(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr);


int multiline_comment_start(int ch);


int simple_multiline_comment_end(int ch);


int multiline_comment(int ch);


int multiline_comment_end(int ch);


int multiline_comment_tail(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr);


int at(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr tkn_str);


int string_open(int ch, dynamic_string_ptr tkn_str);


int string_char(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr tkn_str);


int string_escape_sequence(int ch, dynamic_string_ptr tkn_str);


int empty_string(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr tkn_str);


int multiline_string(int ch, dynamic_string_ptr tkn_str);


int multiline_string_closing(int ch, dynamic_string_ptr tkn_str);


int multiline_string_end(int ch, dynamic_string_ptr tkn_str);


int mutliline_string_done(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr tkn_str);


int underscore(int ch, dynamic_string_ptr tkn_str);


int identifier(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr tkn_str);


int non_zero_integer(int  ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr tkn_str);


int int_zero(int  ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr tkn_str);


int radix_point(int  ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr tkn_str);


int double_simple(int  ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr tkn_str);


int non_zero(int ch, dynamic_string_ptr tkn_str);


int exponent(int ch, dynamic_string_ptr tkn_str);


int exponent_sign(int ch, dynamic_string_ptr tkn_str);


int double_exponent(int  ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr tkn_str);

#endif