/**
 * IFJ26 - Implementace překladače imperativního jazyka IFJ26
 * @file scanner.c
 * @author Matěj Růžička (xruzicm00)
 *
 * @brief Implementace lexikálního analyzátoru
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


int const list_properties_len = 10;


const char *list_properties[] = {
	"drop",
	"take",
	"slice",
	"head",
	"tail",
	"toString",
	"range",
	"toList"
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
	scannerState state = START_STATE;
	clear_token(tkn_ptr);

	dynamic_string tkn_str_struct;
    dynamic_string_ptr tkn_str = &tkn_str_struct;
	init_dynamic_string(tkn_str, 8);

	int ch;
	while (1) {
		ch = getc(targetFile);

		switch (state) {

			case START_STATE:
				state = start_state(ch, tkn_ptr, indent_stack_ptr, tkn_str);
				break;

			case AMPERSAND_STATE:
				state = ampersand(ch, tkn_ptr, indent_stack_ptr);
				break;

			case BAR_STATE:
				state = bar(ch, tkn_ptr, indent_stack_ptr);
				break;

			case EXCLAMATION_MARK_STATE:
				state = exclamation_mark(ch, tkn_ptr, indent_stack_ptr);
				break;

			case EQUALS_STATE:
				state = equals(ch, tkn_ptr, indent_stack_ptr);
				break;

			case GREATER_THAN_STATE:
				state = greater_than(ch, tkn_ptr, indent_stack_ptr);
				break;

			case LESS_THAN_STATE:
				state = less_than(ch, tkn_ptr, indent_stack_ptr);
				break;

			case SLASH_STATE:
				state = slash(ch, tkn_ptr, indent_stack_ptr);
				break;

			// komentáře
			case SINGLELINE_COMMENT_STATE:
				state = singleline_comment(ch, tkn_ptr, indent_stack_ptr);
				break;

			case MULTILINE_COMMENT_START_STATE:
				state = multiline_comment_start(ch);
				break;

			case SIMPLE_MULTILINE_COMMENT_END_STATE:
				state = simple_multiline_comment_end(ch);
				break;

			case MULTILINE_COMMENT_STATE:
				state = multiline_comment(ch);
				break;

			case MULTILINE_COMMENT_END_STATE:
				state = multiline_comment_end(ch);
				break;

			case MULTILINE_COMMENT_TAIL_STATE:
				state = multiline_comment_tail(ch, tkn_ptr, indent_stack_ptr);
				break;

			// anotace (jenom @main)
			case AT_STATE:
				state = at(ch, tkn_ptr, indent_stack_ptr, tkn_str);
				break;

			// řetězce
			case STRING_OPEN_STATE:
				state = string_open(ch, tkn_str);
				break;

			case STRING_CHAR_STATE:
				state = string_char(ch, tkn_ptr, indent_stack_ptr, tkn_str);
				break;

			case STRING_ESCAPE_SEQUENCE_STATE:
				state = string_escape_sequence(ch, tkn_str);
				break;

			case STRING_DONE_STATE:
				state = string_done(ch, tkn_ptr, indent_stack_ptr, tkn_str);
				break;
			
			case MULTILINE_STRING_STATE:
				state = multiline_string(ch, tkn_str);
				break;

			case MULTILINE_STRING_CLOSING_STATE:
				state = multiline_string_closing(ch);
				break;

			case MULTILINE_STRING_END_STATE:
				state = multiline_string_end(ch, tkn_ptr, indent_stack_ptr, tkn_str);
				break;

			case UNDERSCORE_STATE:
				state = underscore(ch, tkn_str);
				break;

			case IDENTIFIER_STATE:
				state = identifier(ch, tkn_ptr, indent_stack_ptr, tkn_str);
				break;

			case NON_ZERO_INTEGER_STATE:
				state = non_zero_integer(ch, tkn_ptr, indent_stack_ptr, tkn_str);
				break;

			case INT_ZERO_STATE:
				state = int_zero(ch, tkn_ptr, indent_stack_ptr, tkn_str);
				break;

			case RADIX_POINT_STATE:
				state = radix_point(ch, tkn_ptr, indent_stack_ptr, tkn_str);
				break;
			
			case DOUBLE_SIMPLE_STATE:
				state = double_simple(ch, tkn_ptr, indent_stack_ptr, tkn_str);
				break;

			case NON_ZERO_STATE:
				state = non_zero(ch, tkn_str);
				break;

			case EXPONENT_STATE:
				state = exponent(ch, tkn_str);
				break;

			case EXPONENT_SIGN_STATE:
				state = exponent_sign(ch, tkn_str);
				break;

			case DOUBLE_EXPONENT_STATE:
				state = double_exponent(ch, tkn_ptr, indent_stack_ptr, tkn_str);
				break;
			
				
			default:
				fprintf(stderr, "Neznamy stav, stav: %d, token: %c, ascii: %d\n", state, ch, ch);
				return 1;
		}

		// token byl úspěšně vytvořen
		if (state == DONE_STATE) {
			tokenType type = tkn_ptr->type;
			// nafukovací řetězec nebyl využit je potřeba ho uvolnit již tady
			if (!(type == KEYWORD_TYPE || type == ID_TYPE || type == LIST_PROPERTY_TYPE || type == STRING_TYPE || type == INT_TYPE || type == DOUBLE_TYPE)) {
				destoy_dynamic_string(tkn_str);
			}
			return 0;
		}
		else if (state == ERROR_STATE) {
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
	if (type == KEYWORD_TYPE || type == ID_TYPE || type == LIST_PROPERTY_TYPE || type == STRING_TYPE || type == INT_TYPE || type == DOUBLE_TYPE) {
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
			return START_STATE;

		// koncové stavy bez hloubky
		case '\n':
			create_token(tkn_ptr, indent_stack_ptr, SYMBOL_TYPE, "EOL");
			return DONE_STATE;

		case ';':
			create_token(tkn_ptr, indent_stack_ptr, SYMBOL_TYPE, ";");
			return DONE_STATE;

		case '+':
			create_token(tkn_ptr, indent_stack_ptr, OPERATOR_TYPE, "+");
			return DONE_STATE;

		case '-':
			create_token(tkn_ptr, indent_stack_ptr, OPERATOR_TYPE, "-");
			return DONE_STATE;

		case '*':
			create_token(tkn_ptr, indent_stack_ptr, OPERATOR_TYPE, "*");
			return DONE_STATE;

		case '[':
			create_token(tkn_ptr, indent_stack_ptr, SYMBOL_TYPE, "[");
			return DONE_STATE;

		case ']':
			create_token(tkn_ptr, indent_stack_ptr, SYMBOL_TYPE, "]");
			return DONE_STATE;

		case '.':
			return RADIX_POINT_STATE;

		case ',':
			create_token(tkn_ptr, indent_stack_ptr, SYMBOL_TYPE, ",");
			return DONE_STATE;

		case EOF:
			create_token(tkn_ptr, indent_stack_ptr, SYMBOL_TYPE, "EOF");
			return DONE_STATE;

		case ')':
			create_token(tkn_ptr, indent_stack_ptr, SYMBOL_TYPE, ")");
			return DONE_STATE;

		case '(':
			create_token(tkn_ptr, indent_stack_ptr, SYMBOL_TYPE, "(");
			return DONE_STATE;

		case ':':
			create_token(tkn_ptr, indent_stack_ptr, SYMBOL_TYPE, ":");
			return DONE_STATE;

		// cesty s více uzly ale pouze jedním koncovým stavem
		case '&':
			return AMPERSAND_STATE;

		case '|':
			return BAR_STATE;

		// cesty s více koncovými stavy
		case '!':
			return EXCLAMATION_MARK_STATE;

		case '=':
			return EQUALS_STATE;

		case '>':
			return GREATER_THAN_STATE;

		case '<':
			return LESS_THAN_STATE;

		case '/':
			return SLASH_STATE;

		// cesty využívající nafukovací řetězec
		case '@':
			insert_last(tkn_str, ch);
			return AT_STATE;

		case '"':
			return STRING_OPEN_STATE;

		case '_':
			insert_last(tkn_str, ch);
			return UNDERSCORE_STATE;

		// tohle ne každý překladač vezme
		case 65 ... 90:
			insert_last(tkn_str, ch);
			return IDENTIFIER_STATE;

		case 97 ... 122:
			insert_last(tkn_str, ch);
			return IDENTIFIER_STATE;

		case 49 ... 57:
			insert_last(tkn_str, ch);
			return NON_ZERO_INTEGER_STATE;

		case '0':
			insert_last(tkn_str, ch);
			return INT_ZERO_STATE;


		case '\r':
			// carriage feed se ignoruje
			// mozna by to tady melo vratit error ale kdyz to tady i docasne je tak to zlehcuje testovani
			// lze se tomu vyvarovat ve vscode i na windows kdyz se v pravem dolnim rohu end of line sequence prepne CRLF na LF
			return START_STATE;

		default:
			return ERROR_STATE;
	}
}


int ampersand(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
	switch (ch) {
		case '&':
			create_token(tkn_ptr, indent_stack_ptr, OPERATOR_TYPE, "&&");
			return DONE_STATE;			
		
		default:
			return ERROR_STATE;
	}
}


int bar(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
	switch (ch) {
		case '|':
			create_token(tkn_ptr, indent_stack_ptr, OPERATOR_TYPE, "||");
			return DONE_STATE;			
		
		default:
			return ERROR_STATE;
	}
}


int exclamation_mark(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
	switch (ch) {

		case '=':
			create_token(tkn_ptr, indent_stack_ptr, OPERATOR_TYPE, "!=");
			return DONE_STATE;

		default:
			ungetc(ch, targetFile);

			create_token(tkn_ptr, indent_stack_ptr, OPERATOR_TYPE, "!");
			return DONE_STATE;
	}
}


int equals(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
	switch (ch) {

		case '=':
			create_token(tkn_ptr, indent_stack_ptr, OPERATOR_TYPE, "==");
			return DONE_STATE;

		default:
			ungetc(ch, targetFile);

			create_token(tkn_ptr, indent_stack_ptr, OPERATOR_TYPE, "=");
			return DONE_STATE;
	}
}


int greater_than(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
	switch (ch) {

		case '=':
			create_token(tkn_ptr, indent_stack_ptr, OPERATOR_TYPE, ">=");
			return DONE_STATE;

		default:
			ungetc(ch, targetFile);

			create_token(tkn_ptr, indent_stack_ptr, OPERATOR_TYPE, ">");
			return DONE_STATE;
	}
}


int less_than(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
	switch (ch) {

		case '=':
			create_token(tkn_ptr, indent_stack_ptr, OPERATOR_TYPE, "<=");
			return DONE_STATE;

		case '-':
			create_token(tkn_ptr, indent_stack_ptr, OPERATOR_TYPE, "<-");
			return DONE_STATE;

		default:
			ungetc(ch, targetFile);

			create_token(tkn_ptr, indent_stack_ptr, OPERATOR_TYPE, "<");
			return DONE_STATE;
	}
}


int slash(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
	switch (ch) {

		case '/':
			return SINGLELINE_COMMENT_STATE;

		case '*':
			return MULTILINE_COMMENT_START_STATE;

		default:
			ungetc(ch, targetFile);

			create_token(tkn_ptr, indent_stack_ptr, OPERATOR_TYPE, "/");
			return DONE_STATE;
	}
}


int singleline_comment(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
	if (ch == '\n') {
		create_token(tkn_ptr, indent_stack_ptr, SYMBOL_TYPE, "EOL");
		return DONE_STATE;
	}
	else if (isprint(ch)) {
		return SINGLELINE_COMMENT_STATE;
	}

	fprintf(stderr, "Radkovy komentar muze obsahovat pouze znaky a je ukoncen novym radkem\n");
	return ERROR_STATE;
}


int multiline_comment_start(int ch) {
	if (ch == '*') {
		return SIMPLE_MULTILINE_COMMENT_END_STATE;
	}
	else if (ch == '\n') {
		return MULTILINE_COMMENT_STATE;
	}
	else if (isprint(ch)) {
		return MULTILINE_COMMENT_START_STATE;
	}

	fprintf(stderr, "Blokovy komentar muze obsahovat pouze znaky a novy radek\n");
	return ERROR_STATE;
}


int simple_multiline_comment_end(int ch) {
	switch (ch) {

		case '/':
			return START_STATE;

		default:
			fprintf(stderr, "Blokovy komentar musi byt ukonecen */ \n");
			return ERROR_STATE;
	}	
}


int multiline_comment(int ch) {
	if (ch == '*') {
		return MULTILINE_COMMENT_END_STATE;
	}
	else if (ch == '\n' || isprint(ch)) {
		return MULTILINE_COMMENT_STATE;
	}
	
	fprintf(stderr, "Blokovy komentar muze obsahovat pouze znaky a novy radek\n");
	return ERROR_STATE;
}


int multiline_comment_end(int ch) {
	switch (ch) {

		case '/':
			return MULTILINE_COMMENT_TAIL_STATE;

		default:
			fprintf(stderr, "Blokovy komentar musi byt ukonecen */ \n");
			return ERROR_STATE;
	}		
}


int multiline_comment_tail(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
	switch (ch) {

		// kontrolujeme zda za komentarem neni dalsi token
		case ' ':
			return MULTILINE_COMMENT_TAIL_STATE;

		case '\n':
			create_token(tkn_ptr, indent_stack_ptr, SYMBOL_TYPE, "EOL");
			return DONE_STATE;

		case EOF:
			ungetc(ch, targetFile);
			create_token(tkn_ptr, indent_stack_ptr, SYMBOL_TYPE, "EOL");
			return DONE_STATE;

		default:
			fprintf(stderr, "Po nekolika radkovem blokovem komentari mohou nasledovat pouze bile znaky a znak noveho radku \n");
			return ERROR_STATE;
	}
}


int at(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr tkn_str) {
	if (can_be_in_IDs_keywords(ch)) {
		insert_last(tkn_str, ch);
		return AT_STATE;
	}
	else {
		ungetc(ch, targetFile);

		if (is_word_in_list(tkn_str->val_ptr, keywords, keywords_len)) {
			create_token(tkn_ptr, indent_stack_ptr, KEYWORD_TYPE, tkn_str->val_ptr);
			return DONE_STATE;
		}
	}

	return ERROR_STATE;
}


int string_open(int ch, dynamic_string_ptr tkn_str) {
	if (ch == '"') {
		return STRING_DONE_STATE;
	}
	else if (ch == '\\') { 
		return STRING_ESCAPE_SEQUENCE_STATE;
	}
	else if (isprint(ch)) {
		insert_last(tkn_str, ch);
		return STRING_CHAR_STATE;
	}

	return ERROR_STATE;
}


int string_char(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr tkn_str) {
	if (ch == '\\') {
		return STRING_ESCAPE_SEQUENCE_STATE;
	}
	if (ch == '"') {
		create_token(tkn_ptr, indent_stack_ptr, STRING_TYPE, tkn_str->val_ptr);
		return DONE_STATE;
	}
	else if (isprint(ch)) {
		insert_last(tkn_str, ch);
		return STRING_CHAR_STATE;
	}

	return ERROR_STATE;
}


int string_escape_sequence(int ch, dynamic_string_ptr tkn_str) {
	switch (ch) {

		case '\\':
			insert_last(tkn_str, '\\');
			return STRING_CHAR_STATE;

		case '"':
			insert_last(tkn_str, '\"');
			return STRING_CHAR_STATE;

		case 'n':
			insert_last(tkn_str, '\n');
			return STRING_CHAR_STATE;

		case 'r':
			insert_last(tkn_str, '\r');
			return STRING_CHAR_STATE;

		case 't':
			insert_last(tkn_str, '\t');
			return STRING_CHAR_STATE;

		default:
			return ERROR_STATE;
	}
}


int string_done(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr tkn_str) {
	if (ch == '"') {
		return MULTILINE_STRING_STATE;
	}

	create_token(tkn_ptr, indent_stack_ptr, STRING_TYPE, tkn_str->val_ptr);
	return DONE_STATE;
}


int multiline_string(int ch, dynamic_string_ptr tkn_str) {
	if (ch == '"') {
		return MULTILINE_STRING_CLOSING_STATE;
	}
	else if (isprint(ch) || ch == '\n') {
		insert_last(tkn_str, ch);
		return MULTILINE_STRING_STATE;
	}

	return ERROR_STATE;
}


int multiline_string_closing(int ch) {
	if (ch == '"') {
		return MULTILINE_STRING_END_STATE;
	}

	return ERROR_STATE;
}


int multiline_string_end(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr tkn_str) {
	if (ch == '"') {
		create_token(tkn_ptr, indent_stack_ptr, STRING_TYPE, tkn_str->val_ptr);
		return DONE_STATE;
	}

	return ERROR_STATE;	
}


int underscore(int ch, dynamic_string_ptr tkn_str) {
	if (can_be_in_IDs_keywords(ch)) {
		insert_last(tkn_str, ch);
		return ID_TYPE;
	}

	return ERROR_STATE;
}


int identifier(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr tkn_str) {
	if (can_be_in_IDs_keywords(ch)) {
		insert_last(tkn_str, ch);
		return IDENTIFIER_STATE;
	}
	else {
		ungetc(ch, targetFile);

		if (is_word_in_list(tkn_str->val_ptr, keywords, keywords_len)) {
			create_token(tkn_ptr, indent_stack_ptr, KEYWORD_TYPE, tkn_str->val_ptr);
			return DONE_STATE;
		}

		create_token(tkn_ptr, indent_stack_ptr, ID_TYPE, tkn_str->val_ptr);
		return DONE_STATE;
	}
}


int non_zero_integer(int  ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr tkn_str) {
	if (isdigit(ch)) {
		insert_last(tkn_str, ch);
		return NON_ZERO_INTEGER_STATE;
	}
	// podržítko se konzumuje
	else if (ch == '_') {
		return NON_ZERO_INTEGER_STATE;
	}
	else if (ch == '.') {
		return RADIX_POINT_STATE;
	}
	else if (ch == 'e' || ch == 'E') {
		insert_last(tkn_str, ch);
		return EXPONENT_STATE;
	}

	ungetc(ch, targetFile);
	create_token(tkn_ptr, indent_stack_ptr, INT_TYPE, tkn_str->val_ptr);
	return DONE_STATE;
}


int int_zero(int  ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr tkn_str) {
	if (isdigit(ch)) {
		insert_last(tkn_str, ch);
		return NON_ZERO_STATE;
	}
	// podržítko se konzumuje
	else if (ch == '_') {
		return NON_ZERO_STATE;
	}
	else if (ch == '.') {
		return RADIX_POINT_STATE;
	}
	else if (ch == 'e' || ch == 'E') {
		insert_last(tkn_str, ch);
		return EXPONENT_STATE;
	}

	ungetc(ch, targetFile);
	create_token(tkn_ptr, indent_stack_ptr, INT_TYPE, tkn_str->val_ptr);
	return DONE_STATE;
}


int radix_point(int  ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr tkn_str) {
	if (isdigit(ch)) {
		// vloží se tečka až zde aby se nekomplikovalo uvolňování paměti
		insert_last(tkn_str, '.');
		insert_last(tkn_str, ch);
		return DOUBLE_SIMPLE_STATE;
	}

	ungetc(ch, targetFile);
	create_token(tkn_ptr, indent_stack_ptr, SYMBOL_TYPE, ".");
	return DONE_STATE;
}


int double_simple(int  ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr tkn_str) {
	if (isdigit(ch)) {
		insert_last(tkn_str, ch);
		return DOUBLE_SIMPLE_STATE;
	}
	// podržítko se konzumuje
	else if (ch == '_') {
		return DOUBLE_SIMPLE_STATE;
	}
	else if (ch == 'e' || ch == 'E') {
		insert_last(tkn_str, ch);
		return EXPONENT_STATE;
	}
	// nepovinné d/D na konci double se ignoruje
	else if (!(ch == 'd' || ch == 'D')) {
		ungetc(ch, targetFile);
	}
	
	create_token(tkn_ptr, indent_stack_ptr, DOUBLE_TYPE, tkn_str->val_ptr);
	return DONE_STATE;
}


int non_zero(int ch, dynamic_string_ptr tkn_str) {
	if (isdigit(ch)) {
		insert_last(tkn_str, ch);
		return NON_ZERO_STATE;
	}
	// podržítko se konzumuje
	else if (ch == '_') {
		return NON_ZERO_STATE;
	}
	else if (ch == 'e' || ch == 'E') {
		insert_last(tkn_str, ch);
		return EXPONENT_STATE;
	}
	else if (ch == '.') {
		return RADIX_POINT_STATE;
	}

	return ERROR_STATE;
}


int exponent(int ch, dynamic_string_ptr tkn_str) {
	if (isdigit(ch)) {
		insert_last(tkn_str, ch);
		return DOUBLE_EXPONENT_STATE;
	}
	else if (ch == '-' || ch == '+') {
		insert_last(tkn_str, ch);
		return EXPONENT_SIGN_STATE;
	}

	return ERROR_STATE;
}


int exponent_sign(int ch, dynamic_string_ptr tkn_str) {
	if (isdigit(ch)) {
		insert_last(tkn_str, ch);
		return DOUBLE_EXPONENT_STATE;
	}

	return ERROR_STATE;
}


int double_exponent(int  ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr, dynamic_string_ptr tkn_str) {
	if (isdigit(ch)) {
		insert_last(tkn_str, ch);
		return DOUBLE_EXPONENT_STATE;
	}
	// podržítko se konzumuje
	else if (ch == '_') {
		return DOUBLE_EXPONENT_STATE;
	}
	// nepovinné d/D na konci double se ignoruje
	else if (!(ch == 'd' || ch == 'D')) {
		ungetc(ch, targetFile);
	}

	create_token(tkn_ptr, indent_stack_ptr, DOUBLE_TYPE, tkn_str->val_ptr);
	return DONE_STATE;
}


bool can_be_in_IDs_keywords(int ch) {
	if (isalnum(ch) || ch == '_') {
		return true;
	}

	return false;
}


bool is_word_in_list(char* word, const char **list, int len) {
	for (int i = 0; i < len; i++) {
		if (strcmp(word, list[i]) == 0) {
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
		if (tkn->type == SYMBOL_TYPE && strcmp(tkn->value, "EOF") == 0) {
			i = -1;
		}
	}

	stack_dtor(stack_indent);

	return 0;
}