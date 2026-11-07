/**
 * @file scanner.c
 * @author Matěj Růžička (xruzicm00)
 *
 * @brief Implementace lexikálního analyzátoru
 * @todo Dynamicky dlouhe řetězce
 * @todo Nápomocné error zprávy
 */

#include "scanner.h"

FILE *targetFile;


int indentTotalCount = 0;


int indentLineCount = 0;


bool lastTokenWasEOL = false;


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
	
	targetFile = file;
	return 0;
}


int next_token(tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
	scannerState state = START;

	int ch;
	while (1) {
		ch = getc(targetFile);

		//printf("scanner: &d, %c \n", ch, ch);

		switch (state) {

			case START:
				state = start_state(ch, tkn_ptr, indent_stack_ptr);
				break;

			case AMPERSAND:
				state = ampersand(ch, tkn_ptr, indent_stack_ptr);
				break;

			case BAR:
				state = bar(ch, tkn_ptr, indent_stack_ptr);
				break;

			case EXCLAMATION_MARK:
				state = exclamation_mark(ch, tkn_ptr, indent_stack_ptr);
				break;

			case EQUALS:
				state = equals(ch, tkn_ptr, indent_stack_ptr);
				break;

			case GREATER_THAN:
				state = greater_than(ch, tkn_ptr, indent_stack_ptr);
				break;

			case LESS_THAN:
				state = less_than(ch, tkn_ptr, indent_stack_ptr);
				break;

			case SLASH:
				state = slash(ch, tkn_ptr, indent_stack_ptr);
				break;

			// komentáře
			case SINGLE_LINE_COMMENT:
				state = single_line_comment(ch, tkn_ptr, indent_stack_ptr);
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
				state = multi_line_comment_tail(ch, tkn_ptr, indent_stack_ptr);
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


int sum_stack(stackPtr stack_ptr) {
	int sum = 0;
	stackPtr tempStackPtr = stack_ctor();

	while (stack_ptr->top != NULL) {
		void *val_ptr = stack_top(stack_ptr);
        int val = (int)(intptr_t)val_ptr;

		sum += val;

		stack_push(tempStackPtr, val_ptr);
        stack_pop(stack_ptr);
	}

	while (tempStackPtr->top != NULL) {
        void *val_ptr = stack_top(tempStackPtr);
        stack_push(stack_ptr, val_ptr);
        stack_pop(tempStackPtr);
    }

	stack_dtor(tempStackPtr);
	// mozna by melo byt i 
	// free(tempStackPtr);

	return sum;
}


int create_token(tokenPtr tkn_ptr, stackPtr indent_stack_ptr, scannerState state, char* value) {
	if (lastTokenWasEOL && 0 != strcmp(value, "EOL")) {
		// snažíme se klesnout na aktualní odsazení
		if (indentLineCount < sum_stack(indent_stack_ptr)) {
			while (sum_stack(indent_stack_ptr) > indentLineCount) {
				stack_pop(indent_stack_ptr);
			}
		}

		// dorovnání v případě že jsme odsadili dál nebo klesli příliš hluboko
		if (indentLineCount > sum_stack(indent_stack_ptr)) {
			stack_push(indent_stack_ptr, (void *)(uintptr_t)(indentLineCount - sum_stack(indent_stack_ptr)));
		}
	}

	if (0 == strcmp(value, "EOL")) {
		lastTokenWasEOL = true;
	}
	else {
		lastTokenWasEOL = false;
	}
	indentLineCount = 0;

	tkn_ptr->type = state;
	tkn_ptr->value = value;

	return 0;
}


int start_state(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
	switch (ch) {

		case ' ':
			// mezera se využije pouze pro vypočítání odsazení
			if (lastTokenWasEOL) {
				indentLineCount++;
			}
			return START;

		// koncové stavy bez hloubky
		case '\n':
			create_token(tkn_ptr, indent_stack_ptr, SYMBOL, "EOL");
			return DONE;

		case ';':
			create_token(tkn_ptr, indent_stack_ptr, SYMBOL, ";");
			return DONE;

		case '+':
			create_token(tkn_ptr, indent_stack_ptr, OPERATOR, "+");
			return DONE;

		case '*':
			create_token(tkn_ptr, indent_stack_ptr, OPERATOR, "*");
			return DONE;

		case '[':
			create_token(tkn_ptr, indent_stack_ptr, SYMBOL, "[");
			return DONE;

		case ']':
			create_token(tkn_ptr, indent_stack_ptr, SYMBOL, "]");
			return DONE;

		case '.':
			create_token(tkn_ptr, indent_stack_ptr, SYMBOL, ".");
			return DONE;

		case ',':
			create_token(tkn_ptr, indent_stack_ptr, SYMBOL, ",");
			return DONE;

		case EOF:
			create_token(tkn_ptr, indent_stack_ptr, SYMBOL, "EOF");
			return DONE;

		case ')':
			create_token(tkn_ptr, indent_stack_ptr, SYMBOL, ")");
			return DONE;

		case '(':
			create_token(tkn_ptr, indent_stack_ptr, SYMBOL, "(");
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


int ampersand(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
	switch (ch) {
		case '&':
			create_token(tkn_ptr, indent_stack_ptr, OPERATOR, "&&");
			return DONE;			
		
		default:
			return ERROR;
	}
}


int bar(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
	switch (ch) {
		case '|':
			create_token(tkn_ptr, indent_stack_ptr, OPERATOR, "||");
			return DONE;			
		
		default:
			return ERROR;
	}
}


int exclamation_mark(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
	switch (ch) {

		case '=':
			create_token(tkn_ptr, indent_stack_ptr, OPERATOR, "!=");
			return DONE;

		default:
			ungetc(ch, targetFile);

			create_token(tkn_ptr, indent_stack_ptr, OPERATOR, "!");
			return DONE;
	}
}


int equals(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
	switch (ch) {

		case '=':
			create_token(tkn_ptr, indent_stack_ptr, OPERATOR, "==");
			return DONE;

		default:
			ungetc(ch, targetFile);

			create_token(tkn_ptr, indent_stack_ptr, OPERATOR, "=");
			return DONE;
	}
}


int greater_than(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
	switch (ch) {

		case '=':
			create_token(tkn_ptr, indent_stack_ptr, OPERATOR, ">=");
			return DONE;

		default:
			ungetc(ch, targetFile);

			create_token(tkn_ptr, indent_stack_ptr, OPERATOR, ">");
			return DONE;
	}
}


int less_than(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
	switch (ch) {

		case '=':
			create_token(tkn_ptr, indent_stack_ptr, OPERATOR, "<=");
			return DONE;

		case '-':
			create_token(tkn_ptr, indent_stack_ptr, OPERATOR, "<-");
			return DONE;

		default:
			ungetc(ch, targetFile);

			create_token(tkn_ptr, indent_stack_ptr, OPERATOR, "<");
			return DONE;
	}
}


int slash(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
	switch (ch) {

		case '/':
			return SINGLE_LINE_COMMENT;

		case '*':
			return MULTI_LINE_COMMENT_START;

		default:
			ungetc(ch, targetFile);

			create_token(tkn_ptr, indent_stack_ptr, OPERATOR, "/");
			return DONE;
	}
}


int single_line_comment(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
	if (ch == '\n') {
		create_token(tkn_ptr, indent_stack_ptr, SYMBOL, "EOL");
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


int multi_line_comment_tail(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
	switch (ch) {

		case ' ':
			return MULTI_LINE_COMMENT_TAIL;

		case '\n':
			create_token(tkn_ptr, indent_stack_ptr, SYMBOL, "EOL");
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
	tokenPtr tkn;
	stackPtr stack_indent = stack_ctor();
	int i = 0;
	while (i == 0) {
		i = next_token(tkn, stack_indent);
		printf("TOKEN: type: %d, symbol: %s, stack sum: %d\n", tkn->type, tkn->value, sum_stack(stack_indent));
		if (strcmp(tkn->value, "EOF") == 0)
			i = -1;
	}
}