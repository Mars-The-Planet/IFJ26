/**
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
#include <math.h>
#include <stdint.h>
#include "stack.h"
#include "dynamic_string.h"

/**
 * @brief Výčet typů tokenů
 */
typedef enum ETokenType {
	KEYWORD,
	ID,
	LIST_PROPERTY,
	SYMBOL,
	OPERATOR,
	STRING,
	INT,
	DOUBLE,
} tokenType;

/**
 * @brief Struktura mezistavů konečného automatu
 */
typedef enum EScannerState {
	DONE,
	ERROR,

	START,
	
	BAR,
	AMPERSAND,

	LESS_THAN,
	GREATER_THAN,
	EQUALS,
	EXCLAMATION_MARK,

	SLASH,
	SINGLE_LINE_COMMENT,
	MULTI_LINE_COMMENT_START,
	SIMPLE_MULTI_LINE_COMMENT_END,
	MULTI_LINE_COMMENT,
	MULTI_LINE_COMMENT_END,
	MULTI_LINE_COMMENT_TAIL,

	AT
} scannerState;


typedef struct SToken {
	tokenType type;
	char *value;
} token, *tokenPtr;


/**
 * @brief Nastaví soubor ze kterého čte LA
 * @param file_name název souboru který chceme nastavit
 * @return vrátí 0 pokud nastavení proběhlo úspěšně, nebo 1 pokud neúspěšně
 */
int set_target_file(char *file_name);


int next_token(tokenPtr tkn_ptr, stackPtr indent_stack_ptr);


int sum_stack(stackPtr stack_ptr);


int clear_token(tokenPtr tkn_ptr);


int create_token(tokenPtr tkn_ptr, stackPtr indent_stack_ptr, scannerState state, char* value);


// pozor akceptuje i hvezdicku '*'
bool can_be_in_comments(int ch);


bool can_be_in_IDs_keywords(int ch);


bool can_be_in_IDs_keywords(int ch);


bool is_keyword(char* word);


int start_state(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr token_str);


int ampersand(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr);


int bar(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr);


int exclamation_mark(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr);


int equals(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr);


int greater_than(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr);


int less_than(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr);


int slash(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr);


int single_line_comment(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr);


int multi_line_comment_start(int ch);


int simple_multi_line_comment_end(int ch);


int multi_line_comment(int ch);


int mutli_line_comment_end(int ch);


int multi_line_comment_tail(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr);


int at(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr token_str);

#endif