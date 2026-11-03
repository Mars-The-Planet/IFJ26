/**
 * @file scanner.c
 * @author Matěj Růžička (xruzicm00)
 *
 * @brief Implementace lexikálního analyzátoru
 * @todo Dynamicky dlouhe řetězce
 * @todo Zásobník na sledování odsazení
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "scanner.h"

/**
 * @brief Nastaví soubor ze kterého čte LA
 * @param file_name název souboru který chceme nastavit
 * @return vrátí 0 pokud nastavení proběhlo úspěšně, nebo 1 pokud neúspěšně
 */
int set_target_file(char *file_name) {
	FILE *file = fopen(file_name, "r");

	if (file == NULL) {
		fprintf(stderr, "Nebylo mozne otevrit soubor\n");
		return 1;
	}
	
	target_file = file;
	return 0;
}


int next_token(token *token_ptr/*, stack *indent_stack_ptr*/) {
	scannerState state = START;

	int ch;
	while (1) {
		ch = getc(target_file);

		switch (state) {

			case START:
				state = start_state(ch, token_ptr);
				break;
			
			case OPEN_BRACKET:
				state = open_bracket(ch, token_ptr);
				break;
			
			default:
				fprintf(stderr, "Neznamy stav %c, ascii: %d\n", ch, ch);
				return 1;
				break;
		}

		// token byl úspěšně vytvořen
		if (state == DONE) {
			return 0;
		}
		else if (state == ERROR) {
			fprintf(stderr, "Neznamy znak %c, ascii: %d\n", ch, ch);
			return 1;
		}
	}
}


int start_state(int ch, token *token_ptr) {
	switch (ch) {

		case ' ':
			// mezera se zkonzumuje
			return START;
			break;

		// koncové stavy bez hloubky
		case '\n':
			token_ptr->type = SYMBOL;
			strcpy(token_ptr->symbol, "EOL");
			return DONE;
			break;

		case ';':
			token_ptr->type = SYMBOL;
			strcpy(token_ptr->symbol, ";");
			return DONE;
			break;

		case '+':
			token_ptr->type = OPERATOR;
			strcpy(token_ptr->symbol, "+");
			return DONE;
			break;

		case '*':
			token_ptr->type = OPERATOR;
			strcpy(token_ptr->symbol, "*");
			return DONE;
			break;

		case '[':
			token_ptr->type = SYMBOL;
			strcpy(token_ptr->symbol, "[");
			return DONE;
			break;

		case ']':
			token_ptr->type = SYMBOL;
			strcpy(token_ptr->symbol, "]");
			return DONE;
			break;

		case '.':
			token_ptr->type = SYMBOL;
			strcpy(token_ptr->symbol, ".");
			return DONE;
			break;

		case ',':
			token_ptr->type = SYMBOL;
			strcpy(token_ptr->symbol, ",");
			return DONE;
			break;

		case EOF:
			token_ptr->type = SYMBOL;
			strcpy(token_ptr->symbol, "EOF");
			return DONE;
			break;

		case ')':
			token_ptr->type = SYMBOL;
			strcpy(token_ptr->symbol, ")");
			return DONE;
			break;

		// cesty s více koncovými stavy
		case '(':
			return OPEN_BRACKET;
			break;



		case '\r':
			// carriage feed se ignoruje
			return START;
			break;

		default:
			return ERROR;
			break;
	}
}


int open_bracket(int ch, token *token_ptr) {
	switch (ch) {

		case ')':
			token_ptr->type = UNIT;
			strcpy(token_ptr->symbol, "()");
			return DONE;
			break;

		default:
			ungetc(ch, target_file);

			token_ptr->type = SYMBOL;
			strcpy(token_ptr->symbol, "(");
			return DONE;
			break;
	}
}


// docasny main
int main(int argc, char* argv[]) {
	printf("hello world\n");
	set_target_file("test.txt");
	token *tkn;
	int i = next_token(tkn);
	while (i == 0) {
		printf("type: %d, symbol: %s \n", tkn->type, tkn->symbol);
		i = next_token(tkn);
		if (strcmp(tkn->symbol, "EOF") == 0)
			i = -1;
	}
}