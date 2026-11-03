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


int next_token(token *tkn_ptr/*, stack *indent_stack_ptr*/) {
	scannerState state = START;

	int ch;
	while (1) {
		ch = getc(target_file);

		switch (state) {

			case START:
				state = start_state(ch, tkn_ptr);
				break;
			
			case OPEN_BRACKET:
				state = open_bracket(ch, tkn_ptr);
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


int start_state(int ch, token *tkn_ptr) {
	switch (ch) {

		case ' ':
			// mezera se zkonzumuje
			return START;
			break;

		// koncové stavy bez hloubky
		case '\n':
			tkn_ptr->type = SYMBOL;
			strcpy(tkn_ptr->symbol, "EOL");
			return DONE;
			break;

		case ';':
			tkn_ptr->type = SYMBOL;
			strcpy(tkn_ptr->symbol, ";");
			return DONE;
			break;

		case '+':
			tkn_ptr->type = OPERATOR;
			strcpy(tkn_ptr->symbol, "+");
			return DONE;
			break;

		case '*':
			tkn_ptr->type = OPERATOR;
			strcpy(tkn_ptr->symbol, "*");
			return DONE;
			break;

		case '[':
			tkn_ptr->type = SYMBOL;
			strcpy(tkn_ptr->symbol, "[");
			return DONE;
			break;

		case ']':
			tkn_ptr->type = SYMBOL;
			strcpy(tkn_ptr->symbol, "]");
			return DONE;
			break;

		case '.':
			tkn_ptr->type = SYMBOL;
			strcpy(tkn_ptr->symbol, ".");
			return DONE;
			break;

		case ',':
			tkn_ptr->type = SYMBOL;
			strcpy(tkn_ptr->symbol, ",");
			return DONE;
			break;

		case EOF:
			tkn_ptr->type = SYMBOL;
			strcpy(tkn_ptr->symbol, "EOF");
			return DONE;
			break;

		case ')':
			tkn_ptr->type = SYMBOL;
			strcpy(tkn_ptr->symbol, ")");
			return DONE;
			break;

		// hrany s více koncovými stavy
		case '(':
			return OPEN_BRACKET;
			break;



		case '\r':
			// carriage feed se ignoruje
			// mozna by to tady melo vratit error ale kdyz to tady i docasne je tak to zlehcuje testovani
			return START;
			break;

		default:
			return ERROR;
			break;
	}
}


int open_bracket(int ch, token *tkn_ptr) {
	switch (ch) {

		case ')':
			tkn_ptr->type = UNIT;
			strcpy(tkn_ptr->symbol, "()");
			return DONE;
			break;

		default:
			ungetc(ch, target_file);

			tkn_ptr->type = SYMBOL;
			strcpy(tkn_ptr->symbol, "(");
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