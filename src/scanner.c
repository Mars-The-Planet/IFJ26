/**
 * @file scanner.c
 * @author Matěj Růžička (xruzicm00)
 *
 * @brief Implementace lexikálního analyzátoru
 * @todo Dynamicky dlouhe řetězce
 * @todo Zásobník na sledování odsazení
 */

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

		//printf("scanner: &d, %c \n", ch, ch);

		switch (state) {

			case START:
				state = start_state(ch, tkn_ptr);
				break;

			case AMPERSAND:
				state = ampersand(ch, tkn_ptr);
				break;

			case BAR:
				state = bar(ch, tkn_ptr);
				break;

			case EXCLAMATION_MARK:
				state = exclamation_mark(ch, tkn_ptr);
				break;

			case EQUALS:
				state = equals(ch, tkn_ptr);
				break;

			case GREATER_THAN:
				state = greater_than(ch, tkn_ptr);
				break;

			case LESS_THAN:
				state = less_than(ch, tkn_ptr);
				break;

			case SLASH:
				state = slash(ch, tkn_ptr);
				break;

			// komentáře
			case SINGLE_LINE_COMMENT:
				state = single_line_comment(ch, tkn_ptr);
				break;

			case MULTI_LINE_COMMENT_START:
				state = multi_line_comment_start(ch);
				break;

			case SIMPLE_MULTI_LINE_COMMENT_END:
				state = simple_multi_line_comment_end(ch);
				break;

			case MULTI_LINE_COMMENT:
				state = multi_line_comment(ch);
				break;

			case MULTI_LINE_COMMENT_END:
				state = mutli_line_comment_end(ch);
				break;

			case MULTI_LINE_COMMENT_TAIL:
				state = multi_line_comment_tail(ch, tkn_ptr);
				break;
			
			default:
				fprintf(stderr, "Neznamy stav, stav: %d, token: %c, ascii: %d\n", state, ch, ch);
				return 1;
		}

		// token byl úspěšně vytvořen
		if (state == DONE) {
			return 0;
		}
		else if (state == ERROR) {
			fprintf(stderr, "Neznamy token, stav: %d, token: %c, ascii: %d\n", state, ch, ch);
			return 1;
		}
	}
}


int start_state(int ch, token *tkn_ptr) {
	switch (ch) {

		case ' ':
			// mezera se zkonzumuje
			return START;

		// koncové stavy bez hloubky
		case '\n':
			tkn_ptr->type = SYMBOL;
			strcpy(tkn_ptr->symbol, "EOL");
			return DONE;

		case ';':
			tkn_ptr->type = SYMBOL;
			strcpy(tkn_ptr->symbol, ";");
			return DONE;

		case '+':
			tkn_ptr->type = OPERATOR;
			strcpy(tkn_ptr->symbol, "+");
			return DONE;

		case '*':
			tkn_ptr->type = OPERATOR;
			strcpy(tkn_ptr->symbol, "*");
			return DONE;

		case '[':
			tkn_ptr->type = SYMBOL;
			strcpy(tkn_ptr->symbol, "[");
			return DONE;

		case ']':
			tkn_ptr->type = SYMBOL;
			strcpy(tkn_ptr->symbol, "]");
			return DONE;

		case '.':
			tkn_ptr->type = SYMBOL;
			strcpy(tkn_ptr->symbol, ".");
			return DONE;

		case ',':
			tkn_ptr->type = SYMBOL;
			strcpy(tkn_ptr->symbol, ",");
			return DONE;

		case EOF:
			tkn_ptr->type = SYMBOL;
			strcpy(tkn_ptr->symbol, "EOF");
			return DONE;

		case ')':
			tkn_ptr->type = SYMBOL;
			strcpy(tkn_ptr->symbol, ")");
			return DONE;

		case '(':
			tkn_ptr->type = SYMBOL;
			strcpy(tkn_ptr->symbol, "(");
			return DONE;

		// cesty s více uzly ale pouze jedním koncovým stavem
		case '&':
			return AMPERSAND;

		case '|':
			return BAR;

		// cesty s více koncovými stavy
		case '!':
			return EXCLAMATION_MARK;

		case '=':
			return EQUALS;

		case '>':
			return GREATER_THAN;

		case '<':
			return LESS_THAN;

		case '/':
			return SLASH;


		case '\r':
			// carriage feed se ignoruje
			// mozna by to tady melo vratit error ale kdyz to tady i docasne je tak to zlehcuje testovani
			// lze se tomu vyvarovat ve vscode i na windows kdyz se v pravem dolnim rohu end of line sequence prepne CRLF na LF
			return START;

		default:
			return ERROR;
	}
}


int ampersand(int ch, token *tkn_ptr) {
	switch (ch) {
		case '&':
			tkn_ptr->type = OPERATOR;
			strcpy(tkn_ptr->symbol, "&&");
			return DONE;			
		
		default:
			return ERROR;
	}
}


int bar(int ch, token *tkn_ptr) {
	switch (ch) {
		case '|':
			tkn_ptr->type = OPERATOR;
			strcpy(tkn_ptr->symbol, "||");
			return DONE;			
		
		default:
			return ERROR;
	}
}


int exclamation_mark(int ch, token *tkn_ptr) {
	switch (ch) {

		case '=':
			tkn_ptr->type = OPERATOR;
			strcpy(tkn_ptr->symbol, "!=");
			return DONE;

		default:
			ungetc(ch, target_file);

			tkn_ptr->type = OPERATOR;
			strcpy(tkn_ptr->symbol, "!");
			return DONE;
	}
}


int equals(int ch, token *tkn_ptr) {
	switch (ch) {

		case '=':
			tkn_ptr->type = OPERATOR;
			strcpy(tkn_ptr->symbol, "==");
			return DONE;

		default:
			ungetc(ch, target_file);

			tkn_ptr->type = OPERATOR;
			strcpy(tkn_ptr->symbol, "=");
			return DONE;
	}
}


int greater_than(int ch, token *tkn_ptr) {
	switch (ch) {

		case '=':
			tkn_ptr->type = OPERATOR;
			strcpy(tkn_ptr->symbol, ">=");
			return DONE;

		default:
			ungetc(ch, target_file);

			tkn_ptr->type = OPERATOR;
			strcpy(tkn_ptr->symbol, ">");
			return DONE;
	}
}


int less_than(int ch, token *tkn_ptr) {
	switch (ch) {

		case '=':
			tkn_ptr->type = OPERATOR;
			strcpy(tkn_ptr->symbol, "<=");
			return DONE;

		case '-':
			tkn_ptr->type = OPERATOR;
			strcpy(tkn_ptr->symbol, "<-");
			return DONE;

		default:
			ungetc(ch, target_file);

			tkn_ptr->type = OPERATOR;
			strcpy(tkn_ptr->symbol, "<");
			return DONE;
	}
}


int slash(int ch, token *tkn_ptr) {
	switch (ch) {

		case '/':
			return SINGLE_LINE_COMMENT;

		case '*':
			return MULTI_LINE_COMMENT_START;

		default:
			ungetc(ch, target_file);

			tkn_ptr->type = OPERATOR;
			strcpy(tkn_ptr->symbol, "/");
			return DONE;
	}
}


int single_line_comment(int ch, token *tkn_ptr) {
	if (ch == '\n') {
		tkn_ptr->type = SYMBOL;
		strcpy(tkn_ptr->symbol, "EOL");
		return DONE;
	}
	else if (can_be_in_comments(ch)) {
		return SINGLE_LINE_COMMENT;
	}
	else {
		fprintf(stderr, "Radkovy komentar muze obsahovat pouze znaky a je ukoncen novym radkem\n");
		return ERROR;
	}
}


int multi_line_comment_start(int ch) {
	if (ch == '*') {
		return SIMPLE_MULTI_LINE_COMMENT_END;
	}
	else if (ch == '\n') {
		return MULTI_LINE_COMMENT;
	}
	else if (can_be_in_comments(ch)) {
		return MULTI_LINE_COMMENT_START;
	}
	else {
		fprintf(stderr, "Blokovy komentar muze obsahovat pouze znaky a novy radek\n");
		return ERROR;
	}
}


int simple_multi_line_comment_end(int ch) {
	switch (ch) {

		case '/':
			return START;

		default:
			fprintf(stderr, "Blokovy komentar musi byt ukonecen */ \n");
			return ERROR;
	}	
}


int multi_line_comment(int ch) {
	if (ch == '*') {
		return MULTI_LINE_COMMENT_END;
	}
	else if (ch == '\n' || can_be_in_comments(ch)) {
		return MULTI_LINE_COMMENT;
	}
	else {
		fprintf(stderr, "Blokovy komentar muze obsahovat pouze znaky a novy radek\n");
		return ERROR;
	}	
}


int mutli_line_comment_end(int ch) {
	switch (ch) {

		case '/':
			return MULTI_LINE_COMMENT_TAIL;

		default:
			fprintf(stderr, "Blokovy komentar musi byt ukonecen */ \n");
			return ERROR;
	}		
}


int multi_line_comment_tail(int ch, token *tkn_ptr) {
	switch (ch) {

		case ' ':
			return MULTI_LINE_COMMENT_TAIL;

		case '\n':
			tkn_ptr->type = SYMBOL;
			strcpy(tkn_ptr->symbol, "EOL");
			return DONE;

		default:
			fprintf(stderr, "Po nekolika radkovem blokovem komentari mohou nasledovat pouze bile znaky a znak noveho radku \n");
			return ERROR;
	}
}


bool can_be_in_comments(int ch) {
	if (ch >= 32 && ch <= 125) {
		return true;
	}

	return false;
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