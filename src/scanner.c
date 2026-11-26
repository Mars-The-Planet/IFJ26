/**
 * IFJ26 - Implementace překladače imperativního jazyka IFJ26
 * @file scanner.c
 * @author Matěj Růžička (xruzicm00)
 *
 * @brief Implementace lexikálního analyzátoru
 * @date 2.11.2026
 */

#include "scanner.h"

/**
 * @brief Počítadlo odsazení
 */
int indentTotalCount = 0;

/**
 * @brief Počítadlo odsazení na aktuálním řádku
 */
int indentLineCount = 0;

/**
 * @brief Počítadlo vnořených několikařádkových komentářů
 */
int nestedCommentsCount = 0;

/**
 * @brief Sleduje zda poslední token byl EOL pro výpočet odsazení
 */
bool lastTokenWasEOL = false;

/**
 * @brief Délka seznamu klíčových slov
 */
int const keywords_len = 18;

/**
 * @brief Seznam klíčových slov
 */
const char *keywords[] = {"def", "do",     "Double",  "else", "if",   "import",
                          "Int", "return", "String",  "then", "Unit", "val",
                          "var", "while",  "Boolean", "List", "for",  "@main"};

/**
 * @brief návratová hodnota interní chyby neovlivnitelnou vstupním programem
 */
int const internalError = 99;

/**
 * @brief Hlavní funkce LA, která čte z stdin a postupně vrací tokeny a odsazení
 * z něj
 * @param tkn_ptr ukazatel na token který bude využit na předání nasledujícího
 * tokenu
 * @param indent_stack_ptr ukazatal na zásobnik určující odsazení aktuálního
 * řádku
 * @return vrátí 0 pokud se token vytvoří úspěšně, 1 pokud se skončí v chybném
 * ERROR stavu, a nebo 99 pokud nastane interní chyba
 */
int next_token(tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
  scannerState state = START_STATE;
  clear_token(tkn_ptr);

  dynamic_string tkn_str_struct;
  dynamic_string_ptr tkn_str = &tkn_str_struct;

  if (init_dynamic_string(tkn_str, 8) == 1) {
    return internalError;
  }

  int ch;
  while (1) {
    ch = getchar();
    // ch = getc(stdin);

    switch (state) {

    case START_STATE:
      state = start_state(ch, tkn_ptr, indent_stack_ptr, tkn_str);
      break;

    // v některém ze stavů došlo k selhání alokace paměti
    case INTERNAL_ERROR_STATE:
      return internalError;

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
      state = equals_sign(ch, tkn_ptr, indent_stack_ptr);
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

    case SIMPLE_MULTILINE_COMMENT_START_NESTED_STATE:
      state = simple_multiline_comment_start_nested(ch);
      break;

    case MULTILINE_COMMENT_STATE:
      state = multiline_comment(ch);
      break;

    case MULTILINE_COMMENT_NESTED_STATE:
      state = multiline_comment_nested(ch);
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

    case EMPTY_STRING_STATE:
      state = empty_string(ch, tkn_ptr, indent_stack_ptr, tkn_str);
      break;

    case STRING_CHAR_STATE:
      state = string_char(ch, tkn_ptr, indent_stack_ptr, tkn_str);
      break;

    case STRING_ESCAPE_SEQUENCE_STATE:
      state = string_escape_sequence(ch, tkn_str);
      break;

    case MULTILINE_STRING_STATE:
      state = multiline_string(ch, tkn_str);
      break;

    case MULTILINE_STRING_CLOSING_STATE:
      state = multiline_string_closing(ch, tkn_str);
      break;

    case MULTILINE_STRING_DONE_STATE:
      state = mutliline_string_done(ch, tkn_ptr, indent_stack_ptr, tkn_str);
      break;

    case MULTILINE_STRING_END_STATE:
      state = multiline_string_end(ch, tkn_str);
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
      state = radix_point(ch, tkn_str);
      break;

    case DOT_STATE:
      state = dot(ch, tkn_ptr, indent_stack_ptr, tkn_str);
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

    case INT_UNDERSCORE_STATE:
      state = int_underscore(ch, tkn_str);
      break;

    case ZERO_UNDERSCORE_STATE:
      state = zero_underscore(ch, tkn_str);
      break;

    case EXPONENT_UNDERSCORE_STATE:
      state = exponent_underscore(ch, tkn_str);
      break;

    case RADIX_UNDERSCORE_STATE:
      state = radix_underscore(ch, tkn_str);
      break;

    default:
      fprintf(stderr,
              "[Chyba] Neznamy stav, stav: %d, znak char: %c, znak ascii: %d\n",
              state, ch, ch);
      return 1;
    }

    // token byl úspěšně vytvořen
    if (state == DONE_STATE) {
      tokenType type = tkn_ptr->type;
      // nafukovací řetězec nebyl využit je potřeba ho uvolnit již tady
      if (!(type == KEYWORD_TYPE || type == ID_TYPE || type == STRING_TYPE ||
            type == INT_TYPE || type == DOUBLE_TYPE)) {
        destroy_dynamic_string(tkn_str);
      }
      return 0;
    } else if (state == ERROR_STATE) {
      destroy_dynamic_string(tkn_str);
      return 1;
    }
  }
}

/**
 * @brief Sčítá členy zásobníku, měl by být volán pouze nad zásobníkem naplňeným
 * inty
 * @param stack_ptr ukazatel na zásobník jehož členové mají být posčítáni
 * @return hodnota součtu všech prvků v zásobníku
 */
int sum_stack(stackPtr stack_ptr) {
  int sum = 0;

  if (stack_ptr == NULL) {
    return sum;
  }

  s_nodePtr current_node = stack_ptr->top;

  while (current_node != NULL) {
    sum += (int)(intptr_t)current_node->value;
    current_node = current_node->next;
  }

  return sum;
}

/**
 * @brief Funkce tvořící navratové hodnoty LA, vytvoří token a nastaví odsazení
 * @param tkn_ptr token do kterého nastavíme příslušný tokenType a řetězec
 * reprezentují jeho hodnotu
 * @param indent_stack_ptr zásobník do kterého je uloženo aktualní odsazení
 * které se také vypočítá v této funkci
 * @param state stav který nastavíme tokenu
 * @param value řetězec držící hodnotu kterou uložíme do tokenu
 */
void create_token(tokenPtr tkn_ptr, stackPtr indent_stack_ptr, tokenType state,
                  char *value) {
  if (lastTokenWasEOL && 0 != strcmp(value, "EOL")) {
    // snažíme se klesnout na aktualní odsazení
    if (indentLineCount < sum_stack(indent_stack_ptr)) {
      while (sum_stack(indent_stack_ptr) > indentLineCount) {
        stack_pop(indent_stack_ptr);
      }
    }

    // dorovnání v případě že jsme odsadili dál nebo klesli příliš hluboko
    if (indentLineCount > sum_stack(indent_stack_ptr)) {
      stack_push(
          indent_stack_ptr,
          (void *)(uintptr_t)(indentLineCount - sum_stack(indent_stack_ptr)));
    }
  }

  if (0 == strcmp(value, "EOL")) {
    lastTokenWasEOL = true;
  } else {
    lastTokenWasEOL = false;
  }
  indentLineCount = 0;

  tkn_ptr->type = state;
  tkn_ptr->value = value;
}

/**
 * @brief Uvolní paměť drženou pro řetězec v tokenu v případě že jsme využívali
 * nafukovací řetězec
 * @param tkn_ptr token ze kterého získame ukazatel na místo v paměti s řetězcem
 */
void clear_token(tokenPtr tkn_ptr) {
  tokenType type = tkn_ptr->type;
  if (type == KEYWORD_TYPE || type == ID_TYPE || type == STRING_TYPE ||
      type == INT_TYPE || type == DOUBLE_TYPE) {
    free(tkn_ptr->value);
  }
}

/**
 * @brief Funkce zjišťující zda znak může být součástí id (alfanumerické znaky a
 * podržítko)
 * @param ch znak který kontrolujeme
 * @return true pokud může, false pokud ne
 */
bool can_be_in_IDs(int ch) {
  if (isalnum(ch) || ch == '_') {
    return true;
  }

  return false;
}

/**
 * @brief Funkce zjišťující zda řetězec je součástí pole řetězců
 * @param word řetězec jehož přítomnost kontrolujeme
 * @param list pole řetězců ve kterém word hledáme
 * @param len délka pole řetězců jenž prohledáváme
 * @return true pokud word najdeme, false pokud ne
 */
bool is_word_in_list(char *word, const char **list, int len) {
  for (int i = 0; i < len; i++) {
    if (strcmp(word, list[i]) == 0) {
      return true;
    }
  }

  return false;
}

// ============================== FUNKCE STAVŮ ==============================

/**
 * @brief FUnkce počátečního stavu KA. LA.
 * @param ch načtený znak se kterým vstupjeme do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @param tkn_str nafukovací řetězec do nějž potencíalně budeme ukládat hodnotu
 * tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int start_state(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr,
                dynamic_string_ptr tkn_str) {
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
    return DOT_STATE;

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
    if (insert_last(tkn_str, ch) == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return AT_STATE;

  case '"':
    return STRING_OPEN_STATE;

  case '_':
    if (insert_last(tkn_str, ch) == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return UNDERSCORE_STATE;

  // tohle ne každý překladač vezme
  case 'A' ... 'Z':
    if (insert_last(tkn_str, ch) == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return IDENTIFIER_STATE;

  case 'a' ... 'z':
    if (insert_last(tkn_str, ch) == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return IDENTIFIER_STATE;

  case '1' ... '9':
    if (insert_last(tkn_str, ch) == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return NON_ZERO_INTEGER_STATE;

  case '0':
    if (insert_last(tkn_str, ch) == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return INT_ZERO_STATE;

  // konzumace bílých znaků
  // vertical tab, form feed, carriage return
  case '\v' ... '\r':
    return START_STATE;
  // tab
  case '\t':
    return START_STATE;

  default:
    fprintf(stderr,
            "[Chyba] Znak \'%c\', Decimal: %d, neni podporovany vstupni znak\n",
            ch, ch);
    return ERROR_STATE;
  }
}

/**
 * @brief Funkce rozhodující do kterého stavu KA se půjde po načtení jednoho &
 * tento stav očekává další & jinak vrací error stav
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int ampersand(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
  switch (ch) {
  case '&':
    create_token(tkn_ptr, indent_stack_ptr, OPERATOR_TYPE, "&&");
    return DONE_STATE;

  default:
    fprintf(stderr, "[Chyba] Po jednom znaku \'&\' muze nasledovat pouze dalsi "
                    "znak \'&\'\n");
    return ERROR_STATE;
  }
}

/**
 * @brief Funkce rozhodující do kterého stavu KA se půjde po načtení jednoho |
 * tento stav očekává další | jinak vrací error stav
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int bar(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
  switch (ch) {
  case '|':
    create_token(tkn_ptr, indent_stack_ptr, OPERATOR_TYPE, "||");
    return DONE_STATE;

  default:
    fprintf(stderr, "[Chyba] Po jednom znaku \'|\' muze nasledovat pouze dalsi "
                    "znak \'|\'\n");
    return ERROR_STATE;
  }
}

/**
 * @brief Funkce rozhodující do kterého stavu KA se půjde po načtení jednoho !
 * z tohoto stavu můžeme získat operátory != a nebo !
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int exclamation_mark(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
  switch (ch) {

  case '=':
    create_token(tkn_ptr, indent_stack_ptr, OPERATOR_TYPE, "!=");
    return DONE_STATE;

  default:
    ungetc(ch, stdin);

    create_token(tkn_ptr, indent_stack_ptr, OPERATOR_TYPE, "!");
    return DONE_STATE;
  }
}

/**
 * @brief Funkce rozhodující do kterého stavu KA se půjde po načtení jednoho =
 * z tohoto stavu můžeme získat operátory == a nebo =
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int equals_sign(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
  switch (ch) {

  case '=':
    create_token(tkn_ptr, indent_stack_ptr, OPERATOR_TYPE, "==");
    return DONE_STATE;

  default:
    ungetc(ch, stdin);

    create_token(tkn_ptr, indent_stack_ptr, SYMBOL_TYPE, "=");
    return DONE_STATE;
  }
}

/**
 * @brief Funkce rozhodující do kterého stavu KA se půjde po načtení jednoho >
 * z tohoto stavu můžeme získat operátory >= a nebo >
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int greater_than(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
  switch (ch) {

  case '=':
    create_token(tkn_ptr, indent_stack_ptr, OPERATOR_TYPE, ">=");
    return DONE_STATE;

  default:
    ungetc(ch, stdin);

    create_token(tkn_ptr, indent_stack_ptr, OPERATOR_TYPE, ">");
    return DONE_STATE;
  }
}

/**
 * @brief Funkce rozhodující do kterého stavu KA se půjde po načtení jednoho
 * znaku < z tohoto stavu můžeme nakonec dostat operátory: <=, <- a nebo prostě
 * <
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int less_than(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
  switch (ch) {

  case '=':
    create_token(tkn_ptr, indent_stack_ptr, OPERATOR_TYPE, "<=");
    return DONE_STATE;

  case '-':
    create_token(tkn_ptr, indent_stack_ptr, OPERATOR_TYPE, "<-");
    return DONE_STATE;

  default:
    ungetc(ch, stdin);

    create_token(tkn_ptr, indent_stack_ptr, OPERATOR_TYPE, "<");
    return DONE_STATE;
  }
}

/**
 * @brief Funkce rozhodující do kterého stavu KA se půjde po načtení jednoho
 * znaku / z tohoto stavu můžeme nakonec dostat operátor / a nebo přejít do
 * začátku komentáře
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int slash(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
  switch (ch) {

  case '/':
    return SINGLELINE_COMMENT_STATE;

  case '*':
    nestedCommentsCount++;
    return MULTILINE_COMMENT_START_STATE;

  default:
    ungetc(ch, stdin);

    create_token(tkn_ptr, indent_stack_ptr, OPERATOR_TYPE, "/");
    return DONE_STATE;
  }
}

/**
 * @brief Funkce reprezentující stav konzumace obsahu jednořátkového komentáře
 * obsah konzumujeme, LF ukončí řádek s komentářem
 * jednořátkový komentář nakonec vracíme jako symbol EOL
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int singleline_comment(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr) {
  if (ch == '\n') {
    create_token(tkn_ptr, indent_stack_ptr, SYMBOL_TYPE, "EOL");
    return DONE_STATE;
  } else if (ch == EOF) {
    create_token(tkn_ptr, indent_stack_ptr, SYMBOL_TYPE, "EOF");
    return DONE_STATE;
  }

  return SINGLELINE_COMMENT_STATE;
}

/**
 * @brief Funkce reprezentující stav začátku několikařádkového komentáře
 * '*' přejdeme do stavu ukočnování jednoduchého několikařádkového komentáře
 * LF značí že se bude jednat o několikařádkový komentář, který bude doopravdy
 * na několik řádků obsah komentáře konzumujeme
 * @param ch načtený znak do automatu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int multiline_comment_start(int ch) {
  if (ch == '/') {
    return SIMPLE_MULTILINE_COMMENT_START_NESTED_STATE;
  }
  if (ch == '*') {
    return SIMPLE_MULTILINE_COMMENT_END_STATE;
  } else if (ch == '\n') {
    return MULTILINE_COMMENT_STATE;
  }
  if (ch == EOF) {
    fprintf(stderr, "[Chyba] EOF pred ukoncenim nekolikaradkoveho komentare\n");
    return ERROR_STATE;
  }

  return MULTILINE_COMMENT_START_STATE;
}

/**
 * @brief Funkce reprezentující stav začátku několikařádkového komentáře kde
 * znak '\' značí začátek vnořeného komentáře LF značí že se bude jednat o
 * několikařádkový komentář, který bude doopravdy na několik řádků obsah
 * komentáře konzumujeme
 * @param ch načtený znak do automatu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int simple_multiline_comment_start_nested(int ch) {
  if (ch == '*') {
    nestedCommentsCount++;
    return MULTILINE_COMMENT_START_STATE;
  } else if (ch == '\n') {
    return MULTILINE_COMMENT_STATE;
  }
  if (ch == EOF) {
    fprintf(stderr, "[Chyba] EOF pred ukoncenim nekolikaradkoveho komentare\n");
    return ERROR_STATE;
  }

  return MULTILINE_COMMENT_START_STATE;
}

/**
 * @brief Funkce reprezentující stav KA připraveného ukončit jednoduchý
 * několikařádkový komentář / značí konec jednoduchého několikařádkového
 * komentáře - scanner to bere jako whitespace LF značí že se bude jednat o
 * několikařádkový komentář, který bude doopravdy na několik řádků obsah
 * komentáře konzumujeme
 * '*' vracíme se zpátky do stavu obsahu jednoduchého několikařádkového
 * komentáře, '*' je součástí komentáře
 * @param ch načtený znak do automatu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int simple_multiline_comment_end(int ch) {
  switch (ch) {

  case '/':
    nestedCommentsCount--;
    if (nestedCommentsCount > 0) {
      return MULTILINE_COMMENT_START_STATE;
    }
    return START_STATE;

  case '*':
    return SIMPLE_MULTILINE_COMMENT_END_STATE;

  case '\n':
    return MULTILINE_COMMENT_STATE;

  default:
    return MULTILINE_COMMENT_START_STATE;
  }
}

/**
 * @brief Funkce reprezentující stav obsha několikařádkového komentáře
 * '*' nás připraví na případné ukočnení komentáře
 * obsah komentáře konzumujeme
 * @param ch načtený znak do automatu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int multiline_comment(int ch) {
  if (ch == '*') {
    return MULTILINE_COMMENT_END_STATE;
  }
  if (ch == '/') {
    return MULTILINE_COMMENT_NESTED_STATE;
  }

  return MULTILINE_COMMENT_STATE;
}

/**
 * @brief Funkce reprezentující stav obsha několikařádkového komentáře s
 * možností vzniku zanořeného komentáře
 * * nás připraví na případné ukočnení komentáře
 * LF a tisknutelné znaky konzumujeme
 * @param ch načtený znak do automatu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int multiline_comment_nested(int ch) {
  if (ch == '*') {
    nestedCommentsCount++;
    return MULTILINE_COMMENT_END_STATE;
  }

  return MULTILINE_COMMENT_STATE;
}

/**
 * @brief Funkce reprezentující stav těsně před ukončením několikařádkového
 * komentáře kontrolující zda * je součástí komentáře a nebo jej ukočnuje
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int multiline_comment_end(int ch) {
  if (ch == '/') {
    nestedCommentsCount--;
    if (nestedCommentsCount > 0) {
      return MULTILINE_COMMENT_STATE;
    }
    return MULTILINE_COMMENT_TAIL_STATE;
  } else if (ch == '*') {
    return MULTILINE_COMMENT_END_STATE;
  }

  return MULTILINE_COMMENT_STATE;
}

/**
 * @brief Funkce reprezentující stav ukončení několikařádkového komentáře
 * kontruljeme že za takovýmto komentářem není další token
 * a čím je řádek ukončen
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int multiline_comment_tail(int ch, tokenPtr tkn_ptr,
                           stackPtr indent_stack_ptr) {
  switch (ch) {

  // kontrolujeme zda za komentarem neni dalsi token
  case ' ':
    return MULTILINE_COMMENT_TAIL_STATE;

  case '\n':
    create_token(tkn_ptr, indent_stack_ptr, SYMBOL_TYPE, "EOL");
    return DONE_STATE;

  case EOF:
    ungetc(ch, stdin);
    create_token(tkn_ptr, indent_stack_ptr, SYMBOL_TYPE, "EOF");
    return DONE_STATE;

  default:
    fprintf(stderr,
            "[Chyba] Po nekolika radkovem blokovem komentari mohou nasledovat "
            "pouze mezery ukoncene LF nebo EOL\n");
    return ERROR_STATE;
  }
}

/**
 * @brief Funkce reprezentující stav začátku anotace
 * využívá se pouze pro označní main funkce s @main
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int at(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr,
       dynamic_string_ptr tkn_str) {
  if (can_be_in_IDs(ch)) {
    if (insert_last(tkn_str, ch) == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return AT_STATE;
  } else {
    ungetc(ch, stdin);

    if (is_word_in_list(tkn_str->val_ptr, keywords, keywords_len)) {
      create_token(tkn_ptr, indent_stack_ptr, KEYWORD_TYPE, tkn_str->val_ptr);
      return DONE_STATE;
    }
  }

  fprintf(stderr, "[Chyba] Jedina podporovana anotace je @main\n");
  return ERROR_STATE;
}

/**
 * @brief Funkce reprezentující stav začátku řetězce
 * zjišťujeme zda se bude jednat o jednoduchý řetězec a nebo prazdný (to také
 * může znamenat začátek více řádkového řetězce)
 * @param ch načtený znak do automatu
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int string_open(int ch, dynamic_string_ptr tkn_str) {
  if (ch == '"') {
    return EMPTY_STRING_STATE;
  } else if (ch == '\\') {
    return STRING_ESCAPE_SEQUENCE_STATE;
  } else if (isprint(ch)) {
    if (insert_last(tkn_str, ch) == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return STRING_CHAR_STATE;
  }

  fprintf(stderr,
          "[Chyba] Retezce mohou obsahovat pouze tisknutelne znaky, uvozovky a "
          "escape "
          "sekvence (\\\\, \\\", \\n, \\r a \\t), Znak \'%c\', Decimal: %d, "
          "neni podporovany znak\n",
          ch, ch);
  return ERROR_STATE;
}

/**
 * @brief Funkce reprezentující stav prázdného řetězce
 * zjišťujeme zda se vskutku jedná a prázdný řetězec nebo o začátek
 * několikařádkového
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int empty_string(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr,
                 dynamic_string_ptr tkn_str) {
  if (ch == '"') {
    return MULTILINE_STRING_STATE;
  }

  ungetc(ch, stdin);
  create_token(tkn_ptr, indent_stack_ptr, STRING_TYPE, tkn_str->val_ptr);
  return DONE_STATE;
}

/**
 * @brief Funkce reprezentující stav obsahu jedoduchého řetězce
 * \ značí začátek escape sekvence, " značí konec řetězce
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int string_char(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr,
                dynamic_string_ptr tkn_str) {
  if (ch == '\\') {
    return STRING_ESCAPE_SEQUENCE_STATE;
  }
  if (ch == '"') {
    create_token(tkn_ptr, indent_stack_ptr, STRING_TYPE, tkn_str->val_ptr);
    return DONE_STATE;
  } else if (isprint(ch)) {
    if (insert_last(tkn_str, ch) == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return STRING_CHAR_STATE;
  }

  fprintf(
      stderr,
      "[Chyba] Retezcove literaly mohou obsahovat pouze tisknutelne znaky a "
      "escape "
      "sekvence (\\\\, \\\", \\n, \\r a \\t), Znak \'%c\', Decimal: %d, "
      "neni podporovany znak\n",
      ch, ch);
  return ERROR_STATE;
}

/**
 * @brief Funkce reprezentující stav escape sekvence v jedoduchém řetězci
 * platné escape sekvence jsou: \\, \", \n, \r a \t
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int string_escape_sequence(int ch, dynamic_string_ptr tkn_str) {
  switch (ch) {

  case '\\':
    if (insert_last(tkn_str, '\\') == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return STRING_CHAR_STATE;

  case '"':
    if (insert_last(tkn_str, '\"') == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return STRING_CHAR_STATE;

  case 'n':
    if (insert_last(tkn_str, '\n') == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return STRING_CHAR_STATE;

  case 'r':
    if (insert_last(tkn_str, '\r') == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return STRING_CHAR_STATE;

  case 't':
    if (insert_last(tkn_str, '\t') == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return STRING_CHAR_STATE;

  default:
    fprintf(stderr,
            "[Chyba] Jedine podporovane escape sekvence jsou: \\\\, \\\", \\n, "
            "\\r a "
            "\\t. Znak \'%c\', Decimal: %d, neni podporovany znak\n",
            ch, ch);
    return ERROR_STATE;
  }
}

/**
 * @brief Funkce reprezentující obsah víceřádkového řetězce
 * " by mohli znamenat jeho ukončení, ostatní znaky jednoduše vkládame do
 * hodnoty tokenu
 * @param ch načtený znak do automatu
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int multiline_string(int ch, dynamic_string_ptr tkn_str) {
  if (ch == '"') {
    return MULTILINE_STRING_CLOSING_STATE;
  }
  if (ch == EOF) {
    fprintf(stderr,
            "[Chyba] EOF pred ukocnenim viceradkoveho retezcoveho literalu\n");
    return ERROR_STATE;
  }

  if (insert_last(tkn_str, ch) == 1) {
    destroy_dynamic_string(tkn_str);
    return INTERNAL_ERROR_STATE;
  }
  return MULTILINE_STRING_STATE;
}

/**
 * @brief Funkce reprezentující stav prvních potenciálně ukončovacích "
 * několikařádkového komentáře v případě že se ukáže že " nejsou ukončovací
 * budou do řetězce vloženy
 * @param ch načtený znak do automatu
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int multiline_string_closing(int ch, dynamic_string_ptr tkn_str) {
  if (ch == '"') {
    return MULTILINE_STRING_END_STATE;
  }
  if (ch == EOF) {
    fprintf(stderr,
            "[Chyba] EOF pred ukocnenim viceradkoveho retezcoveho literalu\n");
    return ERROR_STATE;
  }

  // ukázalo se že úvozovky jsou součástí řetězce
  if (insert_last(tkn_str, '"') == 0 && insert_last(tkn_str, ch) == 0) {
    return MULTILINE_STRING_STATE;
  }
  destroy_dynamic_string(tkn_str);
  return INTERNAL_ERROR_STATE;
}

/**
 * @brief Funkce reprezentující stav druhých potenciálně ukončovacích "
 * několikařádkového komentáře v případě že se ukáže že " nejsou ukončovací
 * budou do řetězce vloženy
 * @param ch načtený znak do automatu
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int multiline_string_end(int ch, dynamic_string_ptr tkn_str) {
  if (ch == '"') {
    return MULTILINE_STRING_DONE_STATE;
  }
  if (ch == EOF) {
    fprintf(stderr,
            "[Chyba] EOF pred ukocnenim viceradkoveho retezcoveho literalu\n");
    return ERROR_STATE;
  }

  // ukázalo se že úvozovky jsou součástí řetězce
  if (insert_last(tkn_str, '"') == 0 && insert_last(tkn_str, '"') == 0 &&
      insert_last(tkn_str, ch) == 0) {
    return MULTILINE_STRING_STATE;
  }
  destroy_dynamic_string(tkn_str);
  return INTERNAL_ERROR_STATE;
}

/**
 * @brief Funkce reprezentující stav třetích potenciálně ukončovacích "
 * několikařádkového komentáře v případě že se ukáže že " nejsou ukončovací
 * budou do řetězce vloženy po třech " už nemůže následovat jiný znak
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int mutliline_string_done(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr,
                          dynamic_string_ptr tkn_str) {
  if (ch == '"') {
    if (insert_last(tkn_str, '"') == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return MULTILINE_STRING_DONE_STATE;
  }

  ungetc(ch, stdin);
  create_token(tkn_ptr, indent_stack_ptr, STRING_TYPE, tkn_str->val_ptr);
  return DONE_STATE;
}

/**
 * @brief Funkce reprezentující stav jednoho podržítka
 * jedno samostatné podřžítko nemůže tvořit ID samo o sobě ale může být jeho
 * začátkem
 * @param ch načtený znak do automatu
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int underscore(int ch, dynamic_string_ptr tkn_str) {
  if (can_be_in_IDs(ch)) {
    if (insert_last(tkn_str, ch) == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return IDENTIFIER_STATE;
  }

  fprintf(stderr, "[Chyba] Samotne podrzitko \'_\' nemuze byt pouzito jako "
                  "identifikator\n");
  return ERROR_STATE;
}

/**
 * @brief Funkce reprezentující stav ID ve kterém čteme celý jeho název
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int identifier(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr,
               dynamic_string_ptr tkn_str) {
  if (can_be_in_IDs(ch)) {
    if (insert_last(tkn_str, ch) == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return IDENTIFIER_STATE;
  } else {
    ungetc(ch, stdin);

    if (is_word_in_list(tkn_str->val_ptr, keywords, keywords_len)) {
      create_token(tkn_ptr, indent_stack_ptr, KEYWORD_TYPE, tkn_str->val_ptr);
      return DONE_STATE;
    }

    create_token(tkn_ptr, indent_stack_ptr, ID_TYPE, tkn_str->val_ptr);
    return DONE_STATE;
  }
}

/**
 * @brief Funkce reprezentující stav ve kterém čteme celou hodnotu nenulového
 * celočíselného literálu
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int non_zero_integer(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr,
                     dynamic_string_ptr tkn_str) {
  if (isdigit(ch)) {
    if (insert_last(tkn_str, ch) == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return NON_ZERO_INTEGER_STATE;
  }
  // podržítko se konzumuje
  else if (ch == '_') {
    return INT_UNDERSCORE_STATE;
  } else if (ch == '.') {
    return RADIX_POINT_STATE;
  } else if (ch == 'e' || ch == 'E') {
    if (insert_last(tkn_str, ch) == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return EXPONENT_STATE;
  }

  ungetc(ch, stdin);
  create_token(tkn_ptr, indent_stack_ptr, INT_TYPE, tkn_str->val_ptr);
  return DONE_STATE;
}

/**
 * @brief Funkce reprezentující stav konzumující _ mezi číslicemi nenulového
 * celočíselného literálu
 * @param ch načtený znak do automatu
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int int_underscore(int ch, dynamic_string_ptr tkn_str) {
  if (ch == '_') {
    return INT_UNDERSCORE_STATE;
  }
  if (isdigit(ch)) {
    if (insert_last(tkn_str, ch) == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return NON_ZERO_INTEGER_STATE;
  }

  fprintf(stderr, "[Chyba] Zadne cislo nesmi byt ukonceno podrzitkem \'_\'\n");
  return ERROR_STATE;
}

/**
 * @brief Funkce reprezentující stav celočíselného literálu 0
 * zjišťujeme zda se nejdná o začátek desetinného literálu
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int int_zero(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr,
             dynamic_string_ptr tkn_str) {
  if (isdigit(ch)) {
    if (insert_last(tkn_str, ch) == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return NON_ZERO_STATE;
  }
  // podržítko se konzumuje
  else if (ch == '_') {
    return ZERO_UNDERSCORE_STATE;
  } else if (ch == '.') {
    return RADIX_POINT_STATE;
  } else if (ch == 'e' || ch == 'E') {
    if (insert_last(tkn_str, ch) == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return EXPONENT_STATE;
  }

  ungetc(ch, stdin);
  create_token(tkn_ptr, indent_stack_ptr, INT_TYPE, tkn_str->val_ptr);
  return DONE_STATE;
}

/**
 * @brief Funkce reprezentující stav tečky
 * zjišťujeme zda se nejdná o symbol . a nebo začátek desetinného literálu
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int dot(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr,
        dynamic_string_ptr tkn_str) {
  if (isdigit(ch)) {
    // vloží se tečka až zde aby se nekomplikovalo uvolňování paměti
    if (insert_last(tkn_str, '.') == 0 && insert_last(tkn_str, ch) == 0) {
      return DOUBLE_SIMPLE_STATE;
    }

    destroy_dynamic_string(tkn_str);
    return INTERNAL_ERROR_STATE;
  }

  ungetc(ch, stdin);
  create_token(tkn_ptr, indent_stack_ptr, SYMBOL_TYPE, ".");
  return DONE_STATE;
}

/**
 * @brief Funkce reprezentující stav desetinné čárky desetinného literálu
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int radix_point(int ch, dynamic_string_ptr tkn_str) {
  if (isdigit(ch)) {
    // vloží se tečka až zde aby se nekomplikovalo uvolňování paměti
    if (insert_last(tkn_str, '.') == 0 && insert_last(tkn_str, ch) == 0) {
      return DOUBLE_SIMPLE_STATE;
    }

    destroy_dynamic_string(tkn_str);
    return INTERNAL_ERROR_STATE;
  }

  fprintf(stderr,
          "[Chyba] za desetinnou carkou musi byt alespon jedna cifra\n");
  return ERROR_STATE;
}

/**
 * @brief Funkce reprezentující stav desetinného literálu bez exponentu či
 * ukončujícího D/d
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int double_simple(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr,
                  dynamic_string_ptr tkn_str) {
  if (isdigit(ch)) {
    if (insert_last(tkn_str, ch) == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return DOUBLE_SIMPLE_STATE;
  }
  // podržítko se konzumuje
  else if (ch == '_') {
    return RADIX_UNDERSCORE_STATE;
  } else if (ch == 'e' || ch == 'E') {
    if (insert_last(tkn_str, ch) == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return EXPONENT_STATE;
  }
  // nepovinné d/D na konci double se ignoruje
  else if (!(ch == 'd' || ch == 'D')) {
    ungetc(ch, stdin);
  }

  create_token(tkn_ptr, indent_stack_ptr, DOUBLE_TYPE, tkn_str->val_ptr);
  return DONE_STATE;
}

/**
 * @brief Funkce reprezentující stav konzumující _ mezi číslicemi jednoduchého
 * desetinného literálu bez exponentu či ukončujícího D/d
 * @param ch načtený znak do automatu
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int radix_underscore(int ch, dynamic_string_ptr tkn_str) {
  if (ch == '_') {
    return RADIX_UNDERSCORE_STATE;
  }
  if (isdigit(ch)) {
    if (insert_last(tkn_str, ch) == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return DOUBLE_SIMPLE_STATE;
  }

  fprintf(stderr, "[Chyba] Zadne cislo nesmi byt ukonceno podrzitkem \'_\'\n");
  return ERROR_STATE;
}

/**
 * @brief Funkce reprezentující stav kdy je načtené číslo začínající nulou ale
 * není ještě jasné zda se z něj stane desetinný literál
 * @param ch načtený znak do automatu
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int non_zero(int ch, dynamic_string_ptr tkn_str) {
  if (isdigit(ch)) {
    if (insert_last(tkn_str, ch) == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return NON_ZERO_STATE;
  }
  // podržítko se konzumuje
  else if (ch == '_') {
    return ZERO_UNDERSCORE_STATE;
  } else if (ch == 'e' || ch == 'E') {
    if (insert_last(tkn_str, ch) == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return EXPONENT_STATE;
  } else if (ch == '.') {
    return RADIX_POINT_STATE;
  }

  fprintf(stderr, "[Chyba] Cislo zacinajici nulou muze byt nasledovano dalsimi "
                  "cislicemi, pote teckou \'.\' a nebo e ci E znaci exponent, "
                  "a muze mit zaroven cifry rozdelene podrzitkem \'_\'\n");
  return ERROR_STATE;
}

/**
 * @brief Funkce reprezentující stav konzumující _ mezi číslicemi čísla
 * začínajících nulou
 * @param ch načtený znak do automatu
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int zero_underscore(int ch, dynamic_string_ptr tkn_str) {
  if (ch == '_') {
    return ZERO_UNDERSCORE_STATE;
  }
  if (isdigit(ch)) {
    if (insert_last(tkn_str, ch) == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return NON_ZERO_STATE;
  }

  fprintf(stderr, "[Chyba] Zadne cislo nesmi byt ukonceno podrzitkem \'_\'\n");
  return ERROR_STATE;
}

/**
 * @brief Funkce reprezentující stav po přečtení E/e v desetinném literálu
 * může po něm následovat +/-
 * @param ch načtený znak do automatu
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int exponent(int ch, dynamic_string_ptr tkn_str) {
  if (isdigit(ch)) {
    if (insert_last(tkn_str, ch) == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return DOUBLE_EXPONENT_STATE;
  } else if (ch == '-' || ch == '+') {
    if (insert_last(tkn_str, ch) == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return EXPONENT_SIGN_STATE;
  }

  fprintf(
      stderr,
      "[Chyba] Exponent desetinneho literalu muze zacinat znakem plus \'+\' ci "
      "minus \'-\' a jinak se sklada z alespon jedne cislice\n");
  return ERROR_STATE;
}

/**
 * @brief Funkce reprezentující stav určující znaménko exponentu
 * @param ch načtený znak do automatu
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int exponent_sign(int ch, dynamic_string_ptr tkn_str) {
  if (isdigit(ch)) {
    if (insert_last(tkn_str, ch) == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return DOUBLE_EXPONENT_STATE;
  }

  fprintf(stderr,
          "[Chyba] Exponent desetinneho literalu muze po znaku plus \'+\' ci "
          "minus \'-\' musi obsahovat alespon jednu cislici\n");
  return ERROR_STATE;
}

/**
 * @brief Funkce reprezentující stav desetinného literálu s exponentem
 * sleduje zda nekončí na D/d a čte všechny číslice jeho exponentu
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int double_exponent(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr,
                    dynamic_string_ptr tkn_str) {
  if (isdigit(ch)) {
    if (insert_last(tkn_str, ch) == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return DOUBLE_EXPONENT_STATE;
  }
  // podržítko se konzumuje
  else if (ch == '_') {
    return EXPONENT_UNDERSCORE_STATE;
  }
  // nepovinné d/D na konci double se ignoruje
  else if (!(ch == 'd' || ch == 'D')) {
    ungetc(ch, stdin);
  }

  create_token(tkn_ptr, indent_stack_ptr, DOUBLE_TYPE, tkn_str->val_ptr);
  return DONE_STATE;
}

/**
 * @brief Funkce reprezentující stav konzumující _ mezi číslicemi jednoduchého
 * desetinného literálu s exponentem
 * @param ch načtený znak do automatu
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int exponent_underscore(int ch, dynamic_string_ptr tkn_str) {
  if (ch == '_') {
    return EXPONENT_UNDERSCORE_STATE;
  }
  if (isdigit(ch)) {
    if (insert_last(tkn_str, ch) == 1) {
      destroy_dynamic_string(tkn_str);
      return INTERNAL_ERROR_STATE;
    }
    return DOUBLE_EXPONENT_STATE;
  }

  fprintf(stderr, "[Chyba] Zadne cislo nesmi byt ukonceno podrzitkem \'_\'\n");
  return ERROR_STATE;
}