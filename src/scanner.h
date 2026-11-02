/**
 * @file scanner.h
 * @author Matěj Růžička (xruzicm00)
 *
 * @brief Implementace lexikálního analyzátoru
 */

#ifndef SCANNER_H
#define SCANNER_H

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


// docasna node
typedef struct SNode {
	struct SNode *next;
	void *value;
} node, *nodePtr;


typedef struct SToken {
	tokenType type;
	nodePtr ptr;
} token;

FILE *target_file;


/**
 * @brief Nastaví soubor ze kterého čte LA
 * @param file_name název souboru který chceme nastavit
 * @return vrátí 0 pokud nastavení proběhlo úspěšně, nebo 1 pokud neúspěšně
 */
int set_target_file(char *file_name);


int next_token(token *token_ptr/*, stack *indent_stack_ptr*/);

#endif