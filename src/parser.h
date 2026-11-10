/**
* @file parser.h
* @author Jonáš Vávra (x273960)
*
* @brief Imlementace syntaktického analyzátoru
*/

#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "scanner.h"

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

} dataType;

/**
 * 
 * @brief Výčet všech uzlů které můžou nastat
 */
typedef enum ASTNodeType {

    ADD,
    SUB,
    MUL,
    DIV,
    LESS,
    EQUALS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUEAL,
    AND_N,
    OR_N,
    UMINUS,
    UPLUS,
    NOT,
    IF_N,
    WHILE_N,
    FOR_N,
    IDENT,
    LIST,
    DOT_ACCESS,
    BLOCK, // pro posloupnost příkazů
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

        }uData;

        // scénář pro if podmínku
        struct {

            AST_Node  *condition;
            AST_Node  *then_part;
            AST_Node  *else_part; 

        } ifData;
        
        // scénář pro proměnou
        struct {

            bool isvar;
            char *var_name;
            tokenType *Datatype;
            AST_Node *expresion;

        } varData;

        // scénář pro data funkce
        struct {

            char *fun_name;
            head *param_list;
            dataType return_type;      
            AST_Node *body;

        } funData;

        // scénář pro volání funkce 
        struct {

            char *call_fun_name;
            AST_Node *fun_args;


        } callFunData;


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

#endif // PARSER_H
