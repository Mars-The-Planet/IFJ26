/**
 * @file scanner.h
 * @author Matěj Růžička (xruzicm00)
 *
 * @brief Implementace lexikálního analyzátoru
 * @todo Zbytek stavů fsm
 */

#ifndef SCANNER_H
#define SCANNER_H

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
	UNIT, 
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

	OPEN_BRACKET,
	LESS_THAN,
	GREATER_THAN,
	EQUALS,
	EXCLAMATION_MARK
} scannerState;


// docasna node
typedef struct SNode {
	struct SNode *next;
	void *value;
} node, *nodePtr;


typedef struct SToken {
	tokenType type;
	//nodePtr ptr;
	char symbol[4];
} token;


FILE *target_file;


/**
 * @brief Nastaví soubor ze kterého čte LA
 * @param file_name název souboru který chceme nastavit
 * @return vrátí 0 pokud nastavení proběhlo úspěšně, nebo 1 pokud neúspěšně
 */
int set_target_file(char *file_name);


int next_token(token *token_ptr/*, stack *indent_stack_ptr*/);


int start_state(int ch, token *token_ptr);


int ampersand(int ch, token *tkn_ptr);


int bar(int ch, token *tkn_ptr);


int open_bracket(int ch, token *token_ptr);


int exclamation_mark(int ch, token *tkn_ptr);


int equals(int ch, token *tkn_ptr);


int greater_than(int ch, token *tkn_ptr);


int less_than(int ch, token *tkn_ptr);

#endif