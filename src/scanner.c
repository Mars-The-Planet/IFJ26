/**
 * @file scanner.c
 * @author Matěj Růžička (xruzicm00)
 *
 * @brief Implementace lexikálního analyzátoru
 * @todo Dynamicky dlouhe řetězce
 * @todo Nápomocné error zprávy
 * @todo vyresit tab - whitespace
 */

#include "scanner.h"

FILE *targetFile;


int indentTotalCount = 0;


int indentLineCount = 0;


bool lastTokenWasEOL = false;


int const keywords_len = 18;


const char *keywords[] = {
	"def",
	"do",
	"Double",
	"else",
	"if",
	"import",
	"Int",
	"return",
	"String",
	"then",
	"Unit",
	"val",
	"var",
	"while",
	"Boolean",
	"List",
	"for",
	"@main"
};


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
	clear_token(tkn_ptr);

	dynamic_string tkn_str_struct;
    dynamic_string_ptr tkn_str = &tkn_str_struct;
	init_dynamic_string(tkn_str, 8);

	int ch;
	while (1) {
		ch = getc(targetFile);

		//printf("scanner: %d, %c \n", ch, ch);

		switch (state) {

			case START:
				state = start_state(ch, tkn_ptr, indent_stack_ptr, tkn_str);
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
			case SINGLELINE_COMMENT:
				state = singleline_comment(ch, tkn_ptr, indent_stack_ptr);
				break;

			case MULTILINE_COMMENT_START:
				state = multiline_comment_start(ch);
				break;

			case SIMPLE_MULTILINE_COMMENT_END:
				state = simple_multiline_comment_end(ch);
				break;

			case MULTILINE_COMMENT:
				state = multiline_comment(ch);
				break;

			case MULTILINE_COMMENT_END:
				state = multiline_comment_end(ch);
				break;

			case MULTILINE_COMMENT_TAIL:
				state = multiline_comment_tail(ch, tkn_ptr, indent_stack_ptr);
				break;

			// anotace (jenom @main)
			case AT:
				state = at(ch, tkn_ptr, indent_stack_ptr, tkn_str);
				break;

			// řetězce
			case STRING_OPEN:
				state = string_open(ch, tkn_str);
				break;

			case STRING_CHAR:
				state = string_char(ch, tkn_ptr, indent_stack_ptr, tkn_str);
				break;

			case STRING_ESCAPE_SEQUENCE:
				state = string_escape_sequence(ch, tkn_str);
				break;

			case MULTILINE_STRING_OPEN:
				state = multiline_string_open(ch);
				break;
			
			case MULTILINE_STRING:
				state = multiline_string(ch, tkn_str);
				break;

			case MULTILINE_STRING_CLOSING:
				state = multiline_string_closing(ch);
				break;

			case MULTILINE_STRING_END:
				state = multiline_string_end(ch, tkn_ptr, indent_stack_ptr, tkn_str);
				break;
			
			default:
				fprintf(stderr, "Neznamy stav, stav: %d, token: %c, ascii: %d\n", state, ch, ch);
				return 1;
		}

		// token byl úspěšně vytvořen
		if (state == DONE) {
			tokenType type = tkn_ptr->type;
			// nafukovací řetězec nebyl využit je potřeba ho uvolnit již tady
			if (!(type == KEYWORD || type == ID || type == LIST_PROPERTY || type == STRING)) {
				destoy_dynamic_string(tkn_str);
			}
			return 0;
		}
		else if (state == ERROR) {
			destoy_dynamic_string(tkn_str);
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


int clear_token(tokenPtr tkn_ptr) {
	tokenType type = tkn_ptr->type;
	if (type == KEYWORD || type == ID || type == LIST_PROPERTY || type == STRING) {
		free(tkn_ptr->value);
		//destoy_dynamic_string(token_ptr->value);
	}
}


int start_state(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr tkn_str) {
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

		// cesty využívající nafukovací řetězec
		case '@':
			insert_last(tkn_str, ch);
			return AT;

		case '"':
			return STRING_OPEN;


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
			return SINGLELINE_COMMENT;

		case '*':
			return MULTILINE_COMMENT_START;

		default:
			ungetc(ch, targetFile);

			create_token(tkn_ptr, indent_stack_ptr, OPERATOR, "/");
			return DONE;
	}
}


int singleline_comment(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
	if (ch == '\n') {
		create_token(tkn_ptr, indent_stack_ptr, SYMBOL, "EOL");
		return DONE;
	}
	else if (isprint(ch)) {
		return SINGLELINE_COMMENT;
	}

	fprintf(stderr, "Radkovy komentar muze obsahovat pouze znaky a je ukoncen novym radkem\n");
	return ERROR;
}


int multiline_comment_start(int ch) {
	if (ch == '*') {
		return SIMPLE_MULTILINE_COMMENT_END;
	}
	else if (ch == '\n') {
		return MULTILINE_COMMENT;
	}
	else if (isprint(ch)) {
		return MULTILINE_COMMENT_START;
	}

	fprintf(stderr, "Blokovy komentar muze obsahovat pouze znaky a novy radek\n");
	return ERROR;
}


int simple_multiline_comment_end(int ch) {
	switch (ch) {

		case '/':
			return START;

		default:
			fprintf(stderr, "Blokovy komentar musi byt ukonecen */ \n");
			return ERROR;
	}	
}


int multiline_comment(int ch) {
	if (ch == '*') {
		return MULTILINE_COMMENT_END;
	}
	else if (ch == '\n' || isprint(ch)) {
		return MULTILINE_COMMENT;
	}
	
	fprintf(stderr, "Blokovy komentar muze obsahovat pouze znaky a novy radek\n");
	return ERROR;
}


int multiline_comment_end(int ch) {
	switch (ch) {

		case '/':
			return MULTILINE_COMMENT_TAIL;

		default:
			fprintf(stderr, "Blokovy komentar musi byt ukonecen */ \n");
			return ERROR;
	}		
}


int multiline_comment_tail(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
	switch (ch) {

		case ' ':
			return MULTILINE_COMMENT_TAIL;

		case '\n':
			create_token(tkn_ptr, indent_stack_ptr, SYMBOL, "EOL");
			return DONE;

		case EOF:
			ungetc(ch, targetFile);
			create_token(tkn_ptr, indent_stack_ptr, SYMBOL, "EOL");
			return DONE;

		default:
			fprintf(stderr, "Po nekolika radkovem blokovem komentari mohou nasledovat pouze bile znaky a znak noveho radku \n");
			return ERROR;
	}
}


int at(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr tkn_str) {
	if (can_be_in_IDs_keywords(ch)) {
		insert_last(tkn_str, ch);
		return AT;
	}
	else {
		ungetc(ch, targetFile);

		if (is_keyword(tkn_str->val_ptr)) {
			create_token(tkn_ptr, indent_stack_ptr, KEYWORD, tkn_str->val_ptr);
			return DONE;
		}
	}

	return ERROR;
}


int string_open(int ch, dynamic_string_ptr tkn_str) {
	if (ch == '"') {
		return MULTILINE_STRING_OPEN;
	}
	else if (ch == '\\') { 
		return STRING_ESCAPE_SEQUENCE;
	}
	else if (isprint(ch)) {
		insert_last(tkn_str, ch);
		return STRING_CHAR;
	}

	return ERROR;
}


int string_char(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr tkn_str) {
	if (ch == '\\') {
		return STRING_ESCAPE_SEQUENCE;
	}
	if (ch == '"') {
		create_token(tkn_ptr, indent_stack_ptr, STRING, tkn_str->val_ptr);
		return DONE;
	}
	else if (isprint(ch)) {
		insert_last(tkn_str, ch);
		return STRING_CHAR;
	}

	return ERROR;
}


int string_escape_sequence(int ch, dynamic_string_ptr tkn_str) {
	switch (ch) {

		case '\\':
			insert_last(tkn_str, '\\');
			return STRING_CHAR;

		case '"':
			insert_last(tkn_str, '\"');
			return STRING_CHAR;

		case 'n':
			insert_last(tkn_str, '\n');
			return STRING_CHAR;

		case 'r':
			insert_last(tkn_str, '\r');
			return STRING_CHAR;

		case 't':
			insert_last(tkn_str, '\t');
			return STRING_CHAR;

		default:
			return ERROR;
	}
}


int multiline_string_open(int ch) {
	if (ch == '"') {
		return MULTILINE_STRING;
	}

	return ERROR;
}


int multiline_string(int ch, dynamic_string_ptr tkn_str) {
	if (ch == '"') {
		return MULTILINE_STRING_CLOSING;
	}
	else if (isprint(ch) || ch == '\n') {
		insert_last(tkn_str, ch);
		return MULTILINE_STRING;
	}

	return ERROR;
}


int multiline_string_closing(int ch) {
	if (ch == '"') {
		return MULTILINE_STRING_END;
	}

	return ERROR;
}


int multiline_string_end(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr tkn_str) {
	if (ch == '"') {
		create_token(tkn_ptr, indent_stack_ptr, STRING, tkn_str->val_ptr);
		return DONE;
	}

	return ERROR;	
}


bool can_be_in_IDs_keywords(int ch) {
	if (isalnum(ch) || ch == '_') {
		return true;
	}

	return false;
}


// mozna z toho udelat proste generic funkci na kontrolovani pritomnosti stringu v poli stringu
bool is_keyword(char* word) {
	for (int i = 0; i < keywords_len; i++) {
		if (strcmp(word, keywords[i]) == 0) {
			return true;
		}
	}

	return false;
}


// docasny main
int main(int argc, char* argv[]) {
	printf("hello world\n");
	set_target_file("test.txt");

    token tknn;
    tknn.value = NULL;
    tokenPtr tkn = &tknn;

	stackPtr stack_indent = stack_ctor();
	int i = 0;
	while (i == 0) {
		i = next_token(tkn, stack_indent);
		printf("TOKEN: type: %d, symbol: %s, stack sum: %d, return: %d\n", tkn->type, tkn->value, sum_stack(stack_indent), i);
		if (tkn->type == SYMBOL && strcmp(tkn->value, "EOF") == 0) {
			i = -1;
		}
	}

	stack_dtor(stack_indent);

	return 0;
}