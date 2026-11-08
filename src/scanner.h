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
	SINGLELINE_COMMENT,
	MULTILINE_COMMENT_START,
	SIMPLE_MULTILINE_COMMENT_END,
	MULTILINE_COMMENT,
	MULTILINE_COMMENT_END,
	MULTILINE_COMMENT_TAIL,

	AT,

	//řetězce
	STRING_OPEN,
	STRING_CHAR,
	STRING_ESCAPE_SEQUENCE,
	MULTILINE_STRING_OPEN,
	MULTILINE_STRING,
	MULTILINE_STRING_CLOSING,
	MULTILINE_STRING_END
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


bool can_be_in_IDs_keywords(int ch);


bool is_keyword(char* word);


int start_state(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr tkn_str);


int ampersand(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr);


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


int multiline_string_open(int ch);


int multiline_string(int ch, dynamic_string_ptr tkn_str);


int multiline_string_closing(int ch);


int multiline_string_end(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr tkn_str);

#endif