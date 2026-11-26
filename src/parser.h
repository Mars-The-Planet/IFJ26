/**
 * IFJ26 - Implementace překladače imperativního jazyka IFJ26
 * @file parser.h
 * @author Jonáš Vávra (x273960)
 * @author Radek Večeřa (281687)
 *
 * @brief Imlementace syntaktického analyzátoru
 * @date 25.11.2026
 */

#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"
#include "stack.h"
#include "symtable.h"

#define ERROR "[Chyba] "
#define MALLOC_FAIL ERROR "Selhalo alokování paměti.\n"
#define ASSIGN_TYPE_FAIL                                                       \
  ERROR "Proměnné '%s' byla přiřazena hodnota se špatným typem.\n"
#define TYPE_CONVERSION_FAIL ERROR "Chyba typové kompatibility.\n"
#define BUILTIN_REDEF_ATTEMPT                                                  \
  ERROR "Pokus o nepovolenou redefinici vestavěné funkce.\n"
#define REDEF_ATTEMPT                                                          \
  ERROR "Pokus o nepovolenou redefinici proměnné nebo funkce.\n"
#define CONST_REDEF_ATTEMPT ERROR "Pokud o nepovolenou redefinici konstanty.\n"
#define UNKNOWN_ID ERROR "Pokus o použití neznámé proměnné nebo funkce.\n"
#define INVALID_ARG_TYPE                                                       \
  ERROR "Funkce '%s' je volána s nesprávným typem argumentů.\n"
#define INVALID_ARG_AMOUNT                                                     \
  ERROR "Funkce '%s' je volána s nesprávným počtem argumentů.\n"
#define RETURN_TYPE_FAIL ERROR"Funkce vrátila špatný typ.\n"

/**
 *
 * @brief Výčet datových typů
 */
typedef enum dataType {
  TYPE_UNASSIGNED,
  TYPE_INT,
  TYPE_DOUBLE,
  TYPE_STRING,
  TYPE_UNIT,
  TYPE_BOOL,
  TYPE_LIST,
  TYPE_UNDEF,
  TYPE_ANY,

} dataType;

/**
 *
 * @brief Výčet všech uzlů pro abstraktní syntaktický strom
 */
typedef enum ASTNodeType {

  ADD = 1,
  SUB,
  MUL,
  DIV,
  LESS,
  LESS_EQUAL,
  GREATER,
  GREATER_EQUAL,
  EQUALS,
  NOT_EQUAL,
  AND_N,
  OR_N,
  UMINUS,
  UPLUS,
  NOT_N,
  CON_SEZNAM,
  TO_N,
  UNTIL_N,
  BY_N,
  BRACK_START,
  BRACK_END,
  STACK_END,
  IF_N,
  WHILE_N,
  FOR_N,
  IDENT,
  LIST,
  DOT_ACCESS,
  ASSIGN,
  VAR_DEF,
  FUN_DEF,
  FUN_CALL,
  LITERAL_INT,
  LITERAL_DOUBLE,
  LITERAL_STRING,
  LITERAL_BOOL,
  LITERAL_UNIT,
  RETURN,

} NodeType;

/**
 *
 * @brief struktura spojového seznamu
 */
typedef struct ListParam {

  dataType type;
  dataType list_type;
  char *param_name;
  struct ListParam *next;

} ListParam;

/**
 *
 * @brief struktura pro zaznačení začátku spojového seznamu
 */
typedef struct SHead {

  ListParam *first;
} head;

/**
 *
 * @brief struktura abstraktního derivačního stromu
 *
 */
typedef struct AST_Node AST_Node;
struct AST_Node {

  NodeType type;
  AST_Node *next;
  dataType result;
  union {
    // scénář pro binární operace
    struct {

      AST_Node *left;
      AST_Node *right;

    } biData;

    // scénář pro unární operace
    struct {

      AST_Node *single;

    } uData;

    // scénář pro if podmínku
    struct {

      AST_Node *condition;
      AST_Node *then_part;
      AST_Node *else_part;

    } ifData;

    // scénář pro proměnou
    struct {

      bool isvar;
      char *var_name;
      dataType varType;
      dataType varListType;
      AST_Node *expression;

    } varData;

    // scénář pro data funkce
    struct {

      char *fun_name;
      head *param_list;
      dataType return_type;
      dataType return_list;
      AST_Node *body;

    } funData;

    struct {

      AST_Node *condition;
      AST_Node *body;

    } whileData;

    struct {

      char *var_name;
      AST_Node *condition;
      AST_Node *if_condition;
      AST_Node *body;

    } forData;

    // scénář pro volání funkce
    struct {

      char *call_fun_name;
      AST_Node *fun_args; // AST_Node má být spíš ukazatel na linked list

    } callFunData;

    struct {

      char *name_after_dot;
      AST_Node *before_dot;
      AST_Node *dot_args;

    } accessDotData;

    struct {

      char *var_name;
      AST_Node *equal;

    } assignData;

    struct {

      AST_Node *expression;

    } returnData;

    struct {

      AST_Node *list_var;

    } listData;

    // scénář pro poslední část AST stromu
    struct {

      union {

        int int_value;
        double double_value;
        bool bool_value;
        char *string_value;

      } valueType;

    } elemData;

    // scénář když se používá proměnná mimo deklaraci
    struct {

      char *id_name;

    } idVarData;
  } nodeData;
};

AST_Node *allocate_expression(AST_Node *expression);

/**
 *
 * @brief Mapování chyb v kódu
 *
 * @param errmsg Zpráva popisující důvod chyby
 * @param line Řádek na kterém se chyba stala
 *
 * @note Funkce nic nevrací - vždy končí proces voláním exit(2)
 *
 * */
void syntax_error(char *errsmg, int line);

/**
 *
 * @brief Zavolá a rovnou zkontroluje next_token
 *
 *
 * @warning Při selhání alokace okamžitě ukončí program exit(99)
 * @warning Při špatně načteném tokenu okamžitě ukončí program exit(1)
 */
void check_next_token();

/**
 *
 * @brief Prochází prázdné řádky
 */
void check_for_eol();

/**
 *
 * @brief Kontroluje vršek zásobníku
 *
 * @param stack Ukazatel na zásobník co se má zkontrolovat
 *
 * @warning Pokud na vrcholu zásboníku nic není okamžitě ukončí program exit(2)
 */
void check_stack_pop(stackPtr stack);

/**
 *
 * @brief Nastavuje základní datové typy pro uzel
 *
 *
 * @return Základní datový typ
 * @warning Při špatném datovém typu okamžitě ukončí program exit(3)
 */
dataType type_transfer();

/**
 *
 * @brief Zpracovává prolog funkce
 *
 * @warning Pokud je prolog špatně zapsaná okamžitě ukončí program exit(2)
 */
void code_prolog();

/**
 *
 * @brief Zpracovává hlavičky funkcí a volá zpracování jejich argumentů
 *
 * @warning Pokud je hlavička špatně zapsaná okamžitě ukončí program exit(2)
 */
void parse_fun_headers();

/**
 *
 * @brief Alokuje paměť pro výraz
 *
 * @param expression Ukazatel na alokovaný výraz
 *
 * @return Alokovaný výraz
 */

AST_Node *allocate_expression(AST_Node *expression);

/**
 *
 * @brief Nastaví prioritu operátoru, čím menší číslo tím větší priorita
 *
 * @return Prioritu operátoru
 *
 * @warning Pokud neznámý operátor okamžitě ukončí program exit(2)
 */
int get_table_index(NodeType type);

/**
 *
 * @brief Volá funcki na zpracování parametrů funkce nebo těla funkce,
 * zpracovává co funcke vrací
 *
 *
 * @warning Při špatném zápisu návratových hodnot funkce okamžite ukončí průběch
 * programu exit(2)
 */
void parse_fun();



/**
 * @brief Funkce pro počítání argumentů při volání funkcí
 * @param node První argument
 * @return Vrátí počet argumentů ve funkci
 */
int count_args(AST_Node *node);


/**
 * @brief Funkce pro kontrolu existence symbolů
 * @return Vrátí 1 pokud hledaný symbol je identifikátor, značí soft-fail
 * @warning Pokud je hledaná funkce chybně volaná může volat exit(4), nebo exit(5)
 */
int check_pop();


/**
 * @brief Funkce pro porovnání typů argumentů a parametrů
 * @param param Porovnávaný parametr
 * @param arg Porovnávaný argument
 * @return Vrátí TRUE pokud jsou typy shodné nebo mezi nimi existuje implicitní
 * konverze
 */
bool compare_param_arg(ListParam *param, AST_Node *arg);

/**
 * @brief Funkce vsune do tabulky symbolů funkce definované jazykem
 */
void setup_builtins();


/**
 * @brief Funkce zkontroluje, zda se do tabulky symbolů nevkládá již existující funkce
 * @param symbol_name Jméno vkládané funkce
 * @warning může zavolat exit(4) pokud nastane pokus o redefinici
 */
void allow_symbol_insert(char *symbol_name);

/**
 * @brief Funkce pro zjištění, zda je povolné uložit hodnotu do proměnné
 * @param var Uzel typu IDENT
 * @param expr Uzel, který se ukládá do var
 * @warning Skončí s exit(6) u některých nekompatibilních typů
 */
void check_assign_types(AST_Node *var, AST_Node *expr);


/**
 * @brief Funkce pro získání datového typu u komplikovanějších operací
 * @param expr Uzel s operací
 * @param allow_string Určuje jestli je operace povolená nad řetězcem
 * @return Datový typ operace
 * @warning Skončí s exit(6) u nekompatibilních typů
 */
dataType get_operation_type(AST_Node *expr, bool allow_string);


/**
 * @brief Funkce pro určení kompatibility mezi typy
 * @param a datový typ TYPE
 * @param expr Uzel s porovnávaným typem
 * @param imp_double_int Povoluje implicitní konverzi z INT na DOUBLE
 * @return Vrátí TRUE pokud je operace povolená
 * @warning Skončí s exit(6) u některých nekompatibilních typů
 */
bool type_compatible(dataType a, AST_Node *expr, bool imp_double_int);


/**
 * @brief Funkce pro interní převod mezi datovými typy
 * @param expr Uzel s datovým typem LITERAL
 * @return Datový typ TYPE
 * @warning Skončí s exit(6) u nekompatibilních typů
 */
dataType literal_to_type(AST_Node *expr);

/**
 *
 * @brief Zpracuje výraz pomocí precedenční syntaktické analýzy
 *
 * @param past_node Volitelný již načtený uzel identifikátoru (z parse id)
 *
 * @return Ukazatel uzel zpracovanéhou výrazu
 *
 * @warning Při syntaktické chybě ve výrazu okamžite ukončí program exit(2)
 */
AST_Node *parse_expression(AST_Node *past_node);

/**
 *
 * @brief Zpracovává parametry funcke jako spojový seznam
 *
 *
 * @return Ukazatel na začátek spojového seznamu
 *
 *
 * @warning Jakmile nastane jakákoliv chyba zápisu argumentů okamžitě ukončí
 * program exit(2)
 */
head *parse_param();

/**
 *
 * @brief Zpracuje hodnoty v závorce u listů nebo volání funkce jako spojový
 * seznam
 *
 * @return Ukazatel na začátek spojového seznamu
 *
 * @warning Pokud jsou výrazy uvnitř funkce špatně zapsané okamžitě ukončí
 * průběch programu exit(2)
 */
AST_Node *exp_in_brackets();

/**
 *
 * @brief Zpracovává tělo funcke
 *
 * @param origin_indent Odsazení těla funkce
 *
 * @warning Pokud je v tělo funkce špatně zapsané okamžitě pozastaví průběch
 * programu exit(2)
 */
AST_Node *parse_fun_body(int origin_indent);

/**
 *
 * @brief Zpracovává proměnné, statické i dynamické
 *
 * @return  Ukazatel na uzel zpracované proměnné
 *
 * @warning Při špatně napsané proměnné okamžitě zastaví průběh programu exit(2)
 */
AST_Node *parse_var();

/**
 *
 * @brief Zpacuje while cyklus
 *
 * @param origin_indent Odsazení cyklu
 *
 * @return Ukazatel na uzel zpracovaného cyklu
 *
 * @warning Pokud je cyklus špatně napsaný okamžitě ukončí průběch programu
 * exit(2)
 */
AST_Node *parse_while(int origing_indent);

/**
 *
 * @brief Zpracuje if podmínku
 *
 * @param origin_indent odsazení podmínky
 *
 * @return Ukazatel na uzel se zpracovanou if podmínkou
 *
 * @warning Pokud je podmínka špatně napsaná okamžitě ukončí průběh programu
 * exit(2)
 */
AST_Node *parse_if(int origin_indent);

/**
 *
 * @brief Zpracuje for loop
 *
 * @param origin_indent odsazení cyklu
 *
 * @return Ukazatel na uzel zpracovaného for cyklu
 *
 * @warning Pokud je cyklus špatně napsaný okamžitě ukončý průběh programu
 * exit(2)
 */
AST_Node *parse_for(int origin_indent);

/**
 *
 * @brief Zpracuje list hodnot
 *
 * @return Ukazatel na uzel zpracovaného listu
 *
 * @warning Pokud je list špatně zapsaný okamžitě ukončý průběch programu
 * exit(2)
 */
AST_Node *parse_list();

/**
 *
 * @brief Zpracuje identifikátor, ten můžu být ještě nedefinovaný
 *
 * @return Ukazatel na uzel zpracovaného identifikátoru
 *
 * @warning Pokud je identifikátor špatně definovaný okamžitě ukončí průběch
 * programu exit(2)
 */
AST_Node *parse_id();

/**
 *
 * @brief Zpracuje return
 *
 * @return Ukazatel na uzel na zpracovaný return
 *
 * @warning Pokud je return špatně napsaný okamžitě ukončí průběch programu
 * exit(2)
 */
AST_Node *parse_return();


/**
 * @brief Funkce pro interní převod mezi datovými typy
 * @param data_type Datový typ TYPE
 * @return Datový typ LITERAL
 */
NodeType type_to_literal(dataType data_type);


/**
 * @brief Funkce pro počítání parametrů definice při volání funkcí
 * @param list Seznam parametrů
 * @return Vrátí počet parametrů funkce
 */
int count_params(ListParam *list);


/**
 * @brief Speciálně upravený malloc, který interně volá calloc a ukládá
 * alokované místo do zásobníku
 * @param size Velikost požadovaného místa
 * @return Vrátí ukazatel na alokované místo
 * @warning Skončí s exit(99) pokud selže alokace
 */
void *special_malloc(size_t size);


/**
 * @brief Speciálně upravený strdup, alokované místo ukládá do zásobníku
 * @param str Řetězec, který bude duplikován
 * @return Vrátí řetězec
 * @warning Skončí s exit(99) pokud selže alokace
 */
char *special_strdup(char *str);


/**
 * @brief Speciálně upravený exit, uklízí alokované místo v zásobníku
 * @param code Číslo, s kterým volá exit()
 */
void special_exit(int code);


/**
 * @brief Funkce pro získání typu u proměnné
 * @param var Uzel typu IDENT
 * @return Vrátí TYPE uzlu var
 * @warning Skončí s exit(6) u některých nekompatibilních typů
 */
dataType getVarType(AST_Node *var);

extern int has_main;

extern bool youre_main_node;

extern AST_Node *main_node;

extern scoped_symtablePtr symbol_table;
extern token active_token;
extern stackPtr stack_indent;

extern stackPtr used_symbols;

extern int exit_code;

extern stackPtr memory_allocs;

extern stackPtr stalled;
extern stackPtr saved_args;

extern dataType fun_ret_type;

#endif // PARSER_H
