/**
* @file parser.h
* @author Jonáš Vávra (x273960)
*
* @brief Imlementace syntaxtického analyzátoru
*/

#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
 * @brief Výčet tokenů
 */
typedef enum tokenType {

    KEYWORD,
    ID,
    LIST_PROPERTY, 
    SYMBOL, 
    OPERATOR, 
    STRING,
    INT,
    DOUBLE,
    UNIT, // void

} tokenType;


/**
 * @brief Výčet datových typů
 */
typedef enum dataType {

    INT,
    DOUBLE,
    STRING,
    UNIT,
    BOOL,
    LIST,

} dataType;

/**
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
    AND,
    OR,
    UMINUS,
    UPLUS,
    NOT,
    IF,
    WHILE,
    FOR,
    IDENT,
    LIST,
    DOT_ACCESS,
    BLOCK, // pro posloupnost příkazů
    ASSIGN,
    VAR_DEF, // TODO: bool kde budu rozohodovat jestli se jedná od VAL(true) nebo VAR(false)
    FUN_DEF,
    FUN_CALL,
    LITERAL_INT,
    LITERAL_DOUBLE,
    LITERAL_STRING,
    LITERAL_BOOL,
    LITERAL_UNIT,
    RETURN,    

} NodeType;

typedef struct AST_Node AST_Node;

struct AST_Node {

    NodeType type;
    union {
        // část pro binární operace
        struct {

            AST_Node *left;
            AST_Node *right;

        } biData;
        
        // část pro unární operace 
        struct {

            AST_Node *single;

        }uData;

        // část pro if podmínku 
        struct {

            AST_Node  *condition;
            AST_Node  *then_part;
            AST_Node  *else_part; 

        } ifData;
        
        //část pro data proměnné
        struct {

            bool isvar;
            char *var_name;
            tokenType *Datatype;
            AST_Node *expresion;

        } varData;

        // část pro data funkce
        struct {

            char *fun_name;
            head param_list;
            dataType return_type;      
            AST_Node *body;

        } funData;



    } nodeData;

};

typedef struct SToken {

    tokenType;
    nodePtr ptr;

} token;

typedef struct SNode {

    nodePtr next;
    void *value;

} node, *nodePtr;

typedef struct SHead {

    nodePtr first;
} head; 



#endif // PARSER_H
