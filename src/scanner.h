/**
 * @file scanner.h
 * @author Matěj Růžička (xruzicm00)
 *
 * @brief Implementace lexikálního analyzátoru
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

/**
 * @brief Struktura typů tokenu
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
	MULTI_LINE_COMMENT_TAIL
} scannerState;


typedef struct SToken {
	tokenType type;
	char *value;
} token;


/**
 * @brief Nastaví soubor ze kterého čte LA
 * @param file_name název souboru který chceme nastavit
 * @return vrátí 0 pokud nastavení proběhlo úspěšně, nebo 1 pokud neúspěšně
 */
int set_target_file(char *file_name);


int next_token(token *tkn_ptr, stackPtr indent_stack_ptr);


int sum_stack(stackPtr stack_ptr);


int create_token(token *tkn_ptr, stackPtr indent_stack_ptr, scannerState state, char* value);


// pozor akceptuje i hvezdicku '*'
bool can_be_in_comments(int ch);


int start_state(int ch, token *tkn_ptr, stackPtr indent_stack_ptr);


int ampersand(int ch, token *tkn_ptr, stackPtr indent_stack_ptr);


int bar(int ch, token *tkn_ptr, stackPtr indent_stack_ptr);


int exclamation_mark(int ch, token *tkn_ptr, stackPtr indent_stack_ptr);


int equals(int ch, token *tkn_ptr, stackPtr indent_stack_ptr);


int greater_than(int ch, token *tkn_ptr, stackPtr indent_stack_ptr);


int less_than(int ch, token *tkn_ptr, stackPtr indent_stack_ptr);


int slash(int ch, token *tkn_ptr, stackPtr indent_stack_ptr);


int single_line_comment(int ch, token *tkn_ptr, stackPtr indent_stack_ptr);


int multi_line_comment_start(int ch);


int simple_multi_line_comment_end(int ch);


int multi_line_comment(int ch);


int mutli_line_comment_end(int ch);


int multi_line_comment_tail(int ch, token *tkn_ptr, stackPtr indent_stack_ptr);

#endif