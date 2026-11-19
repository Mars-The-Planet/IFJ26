/**
 * IFJ26 - Implementace překladače imperativního jazyka IFJ26
 * @file scanner.h
 * @author Matěj Růžička (xruzicm00)
 *
 * @brief Hlavičkový soubor lexikálního analyzátoru
 * @date 13.11.2026
 */

#ifndef SCANNER_H
#define SCANNER_H

#include "dynamic_string.h"
#include "stack.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Výčet typů tokenů
 */
typedef enum ETokenType {
  KEYWORD_TYPE,
  ID_TYPE,
  SYMBOL_TYPE,
  OPERATOR_TYPE,
  STRING_TYPE,
  INT_TYPE,
  DOUBLE_TYPE,
} tokenType;

/**
 * @brief Struktura mezistavů konečného automatu
 */
typedef enum EScannerState {
  DONE_STATE,  // extra stav značící dokončení čtení
  ERROR_STATE, // extra stav značící že načtení neočekávaného znaku
  INTERNAL_ERROR_STATE, // extra stav značící interní chybu neovlivnitelnou vstupním programem

  START_STATE,

  BAR_STATE,
  AMPERSAND_STATE,

  LESS_THAN_STATE,
  GREATER_THAN_STATE,
  EQUALS_STATE,
  EXCLAMATION_MARK_STATE,

  SLASH_STATE,
  SINGLELINE_COMMENT_STATE,
  MULTILINE_COMMENT_START_STATE,
  SIMPLE_MULTILINE_COMMENT_END_STATE,
  MULTILINE_COMMENT_STATE,
  MULTILINE_COMMENT_END_STATE,
  MULTILINE_COMMENT_TAIL_STATE,

  AT_STATE,

  // řetězce
  STRING_OPEN_STATE,
  STRING_CHAR_STATE,
  STRING_ESCAPE_SEQUENCE_STATE,
  EMPTY_STRING_STATE,
  MULTILINE_STRING_STATE,
  MULTILINE_STRING_CLOSING_STATE,
  MULTILINE_STRING_END_STATE,
  MULTILINE_STRING_DONE_STATE,

  // IDs
  UNDERSCORE_STATE,
  IDENTIFIER_STATE,

  // číselné literály
  NON_ZERO_INTEGER_STATE,
  INT_ZERO_STATE,

  NON_ZERO_STATE,
  RADIX_POINT_STATE,
  EXPONENT_STATE,
  EXPONENT_SIGN_STATE,
  DOUBLE_SIMPLE_STATE,
  DOUBLE_EXPONENT_STATE,

  INT_UNDERSCORE_STATE,
  ZERO_UNDERSCORE_STATE,
  EXPONENT_UNDERSCORE_STATE,
  RADIX_UNDERSCORE_STATE
} scannerState;

/**
 * @brief Struktura tokenu vystupujího z lexikálního analyzátoru
 */
typedef struct SToken {
  tokenType type;
  char *value;
} token, *tokenPtr;

/**
 * @brief Nastaví soubor ze kterého čte LA.
 * @param file_name název souboru který chceme nastavit
 * @return vrátí 0 pokud nastavení proběhlo úspěšně, nebo 1 pokud neúspěšně
 */
int set_target_file(char *file_name);

/**
 * @brief Hlavní funkce LA, která čte z cílového souboru a postupně vrací tokeny
 * a odsazení z něj
 * @param tkn_ptr ukazatel na token který bude využit na předání nasledujícího
 * tokenu
 * @param indent_stack_ptr ukazatal na zásobnik určující odsazení aktuálního
 * řádku
 * @return vrátí 0 pokud se token vytvoří úspěšně, 1 pokud se skončí v chybném
 * ERROR stavu, a nebo 99 pokud nastane interní chyba
 */
int next_token(tokenPtr tkn_ptr, stackPtr indent_stack_ptr);

/**
 * @brief Sčítá členy zásobníku, měl by být volán pouze nad zásobníkem naplňeným
 * inty
 * @param stack_ptr ukazatel na zásobník jehož členové mají být posčítáni
 * @return hodnota součtu všech prvků v zásobníku
 */
int sum_stack(stackPtr stack_ptr);

/**
 * @brief Funkce tvořící navratové hodnoty LA., vytvoří token a nastaví odsazení
 * @param tkn_ptr token do kterého nastavíme příslušný tokenType a řetězec
 * reprezentují jeho hodnotu
 * @param indent_stack_ptr zásobník do kterého je uloženo aktualní odsazení
 * které se také vypočítá v této funkci
 * @param state stav který nastavíme tokenu
 * @param value řetězec držící hodnotu kterou uložíme do tokenu
 */
void create_token(tokenPtr tkn_ptr, stackPtr indent_stack_ptr, tokenType state,
                  char *value);

/**
 * @brief Uvolní paměť drženou pro řetězec v tokenu v případě že jsme využívali
 * nafukovací řetězec
 * @param tkn_ptr token ze kterého získame ukazatel na místo v paměti s řetězcem
 */
void clear_token(tokenPtr tkn_ptr);

/**
 * @brief Funkce zjišťující zda znak může být součástí id (alfanumerické znaky a
 * podržítko)
 * @param ch znak který kontrolujeme
 * @return true pokud může, false pokud ne
 */
bool can_be_in_IDs(int ch);

/**
 * @brief Funkce zjišťující zda řetězec je součástí pole řetězců
 * @param word řetězec jehož přítomnost kontrolujeme
 * @param list pole řetězců ve kterém word hledáme
 * @param len délka pole řetězců jenž prohledáváme
 * @return true pokud word najdeme, false pokud ne
 */
bool is_word_in_list(char *word, const char **list, int len);

/**
 * @brief Funkce počátečního stavu KA. LA.
 * @param ch načtený znak se kterým vstupjeme do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @param tkn_str nafukovací řetězec do nějž potencíalně budeme ukládat hodnotu
 * tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int start_state(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr,
                dynamic_string_ptr tkn_str);

/**
 * @brief Funkce rozhodující do kterého stavu KA se půjde po načtení jednoho &
 * tento stav očekává další & jinak vrací error stav
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int ampersand(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr);

/**
 * @brief Funkce rozhodující do kterého stavu KA se půjde po načtení jednoho |
 * tento stav očekává další | jinak vrací error stav
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int bar(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr);

/**
 * @brief Funkce rozhodující do kterého stavu KA se půjde po načtení jednoho !
 * z tohoto stavu můžeme získat operátory != a nebo !
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int exclamation_mark(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr);

/**
 * @brief Funkce rozhodující do kterého stavu KA se půjde po načtení jednoho =
 * z tohoto stavu můžeme získat operátory == a nebo =
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int equals_sign(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr);

/**
 * @brief Funkce rozhodující do kterého stavu KA se půjde po načtení jednoho >
 * z tohoto stavu můžeme získat operátory >= a nebo >
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int greater_than(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr);

/**
 * @brief Funkce rozhodující do kterého stavu KA se půjde po načtení jednoho
 * znaku < z tohoto stavu můžeme nakonec dostat operátory: <=, <- a nebo prostě
 * <
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int less_than(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr);

/**
 * @brief Funkce rozhodující do kterého stavu KA se půjde po načtení jednoho
 * znaku / z tohoto stavu můžeme nakonec dostat operátor / a nebo přejít do
 * začátku komentáře
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int slash(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr);

/**
 * @brief Funkce reprezentující stav konzumace obsahu jednořátkového komentáře
 * tisknutelný znak konzumujeme, LF ukončí řádek s komentářem
 * jednořátkový komentář nakonec vracíme jako symbol EOL
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int singleline_comment(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr);

/**
 * @brief Funkce reprezentující stav začátku několikařádkového komentáře
 * * přejdeme do stavu ukočnování jednoduchého několikařádkového komentáře
 * LF značí že se bude jednat o několikařádkový komentář, který bude doopravdy
 * na několik řádků tisknutelný znak konzumujeme
 * @param ch načtený znak do automatu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int multiline_comment_start(int ch);

/**
 * @brief Funkce reprezentující stav KA připraveného ukončit jednoduchý
 * několikařádkový komentář / značí konec jednoduchého několikařádkového
 * komentáře - scanner to bere jako whitespace LF značí že se bude jednat o
 * několikařádkový komentář, který bude doopravdy na několik řádků tisknutelný
 * znak konzumujeme
 * * vracíme se zpátky do stavu obsahu jednoduchého několikařádkového komentáře,
 * * je součástí komentáře
 * @param ch načtený znak do automatu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int simple_multiline_comment_end(int ch);

/**
 * @brief Funkce reprezentující stav obsha několikařádkového komentáře
 * * nás připraví na případné ukočnení komentáře
 * LF a tisknutelné znaky konzumujeme
 * @param ch načtený znak do automatu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int multiline_comment(int ch);

/**
 * @brief Funkce reprezentující stav těsně před ukončením několikařádkového
 * komentáře kontrolujeme zda * je součástí komentáře a nebo jej ukočnuje
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int multiline_comment_end(int ch);

/**
 * @brief Funkce reprezentující stav ukončení několikařádkového komentáře
 * kontruljeme že za takovýmto komentářem není další token
 * a čím je řádek ukončen
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int multiline_comment_tail(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr);

/**
 * @brief Funkce reprezentující stav začátku anotace
 * využívá se pouze pro označní main funkce s @main
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int at(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr,
       dynamic_string_ptr tkn_str);

/**
 * @brief Funkce reprezentující stav začátku řetězce
 * zjišťujeme zda se bude jednat o jednoduchý řetězec a nebo prazdný (to také
 * může znamenat začátek více řádkového řetězce)
 * @param ch načtený znak do automatu
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int string_open(int ch, dynamic_string_ptr tkn_str);

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
                dynamic_string_ptr tkn_str);

/**
 * @brief Funkce reprezentující stav escape sekvence v jedoduchém řetězci
 * platné escape sekvence jsou: \\, \", \n, \r a \t
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int string_escape_sequence(int ch, dynamic_string_ptr tkn_str);

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
                 dynamic_string_ptr tkn_str);

/**
 * @brief Funkce reprezentující obsah víceřádkového řetězce
 * " by mohli znamenat jeho ukončení, ostatní tisknutelné znaky jednoduše
 * vkládame do hodnoty tokenu
 * @param ch načtený znak do automatu
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int multiline_string(int ch, dynamic_string_ptr tkn_str);

/**
 * @brief Funkce reprezentující stav prvních potenciálně ukončovacích "
 * několikařádkového komentáře v případě že se ukáže že " nejsou ukončovací
 * budou do řetězce vloženy
 * @param ch načtený znak do automatu
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int multiline_string_closing(int ch, dynamic_string_ptr tkn_str);

/**
 * @brief Funkce reprezentující stav druhých potenciálně ukončovacích "
 * několikařádkového komentáře v případě že se ukáže že " nejsou ukončovací
 * budou do řetězce vloženy
 * @param ch načtený znak do automatu
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int multiline_string_end(int ch, dynamic_string_ptr tkn_str);

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
                          dynamic_string_ptr tkn_str);

/**
 * @brief Funkce reprezentující stav jednoho podržítka
 * jedno samostatné podřžítko nemůže tvořit ID samo o sobě ale může být jeho
 * začátkem
 * @param ch načtený znak do automatu
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int underscore(int ch, dynamic_string_ptr tkn_str);

/**
 * @brief Funkce reprezentující stav ID ve kterém čteme celý jeho název
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int identifier(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr,
               dynamic_string_ptr tkn_str);

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
                     dynamic_string_ptr tkn_str);

/**
 * @brief Funkce reprezentující stav konzumující _ mezi číslicemi nenulového
 * celočíselného literálu
 * @param ch načtený znak do automatu
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int int_underscore(int ch, dynamic_string_ptr tkn_str);

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
             dynamic_string_ptr tkn_str);

/**
 * @brief Funkce reprezentující stav tečky
 * zjišťujeme zda se nejdná o symbol . a nebo začátek desetinného literálu
 * @param ch načtený znak do automatu
 * @param tkn_ptr token který vytváříme a budeme předávat
 * @param indent_stack_ptr zásobník který vytváříme a budeme předávat
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int radix_point(int ch, tokenPtr tkn_ptr, stackPtr indent_stack_ptr,
                dynamic_string_ptr tkn_str);

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
                  dynamic_string_ptr tkn_str);

/**
 * @brief Funkce reprezentující stav konzumující _ mezi číslicemi jednoduchého
 * desetinného literálu bez exponentu či ukončujícího D/d
 * @param ch načtený znak do automatu
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int radix_underscore(int ch, dynamic_string_ptr tkn_str);

/**
 * @brief Funkce reprezentující stav kdy je načtené číslo začínající nulou ale
 * není ještě jasné zda se z něj stane desetinný literál
 * @param ch načtený znak do automatu
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int non_zero(int ch, dynamic_string_ptr tkn_str);

/**
 * @brief Funkce reprezentující stav konzumující _ mezi číslicemi čísla
 * začínajících nulou
 * @param ch načtený znak do automatu
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int zero_underscore(int ch, dynamic_string_ptr tkn_str);

/**
 * @brief Funkce reprezentující stav po přečtení E/e v desetinném literálu
 * může po něm následovat +/-
 * @param ch načtený znak do automatu
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int exponent(int ch, dynamic_string_ptr tkn_str);

/**
 * @brief Funkce reprezentující stav určující znaménko exponentu
 * @param ch načtený znak do automatu
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int exponent_sign(int ch, dynamic_string_ptr tkn_str);

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
                    dynamic_string_ptr tkn_str);

/**
 * @brief Funkce reprezentující stav konzumující _ mezi číslicemi jednoduchého
 * desetinného literálu s exponentem
 * @param ch načtený znak do automatu
 * @param tkn_str nafukovací řetězec do nějž ukládáme hodnotu tokenu
 * @return vrací int reprezntující stav KA. LA. scannerState
 */
int exponent_underscore(int ch, dynamic_string_ptr tkn_str);

#endif // SCANNER
