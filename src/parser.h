/**
 * @file parser.h
 * @author Jonáš Vávra (x273960)
 * @author Radek Večeřa (281687)
 *
 * @brief Imlementace syntaktického analyzátoru
 */

#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"
#include "stack.h"
#include "symtable.h"

/**
 *
 * @brief Výčet datových typů
 */
typedef enum dataType {

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
 * @brief Výčet všech uzlů které můžou nastat
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
  CON_SEZNAM, // funkce pro konkatenanci seznamů
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
 * @param isvar bool rozeznávající proměnou (1) a konstantu (0)
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

void code_prolog();

void the_parser();

void parse_fun();

AST_Node *parse_return();

int count_args(AST_Node *node);

int check_pop();

bool compare_param_arg(ListParam *param, AST_Node *arg);

void setup_builtins();

void allow_symbol_insert(char *symbol_name);

void check_assign_types(AST_Node *var, AST_Node *expr);

dataType get_operation_type(AST_Node *expr, bool allow_string);

bool type_compatible(dataType a, AST_Node *expr, bool imp_double_int);

dataType literal_to_type(AST_Node *expr);

AST_Node *parse_expression(AST_Node *past_node);

head *parse_param();

AST_Node *exp_in_brackets();

AST_Node *parse_fun_body(int origin_indent);

AST_Node *parse_var();

AST_Node *parse_while(int origing_indent);

AST_Node *parse_if(int origin_indent);

AST_Node *parse_for(int origin_indent);

AST_Node *parse_list();

AST_Node *parse_id();

AST_Node *parse_return();

int has_main;

bool youre_main_node;

AST_Node *main_node;

NodeType type_to_literal(dataType data_type);

#endif // PARSER_H
