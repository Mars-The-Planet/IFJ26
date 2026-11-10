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
 * @brief Výčet všech uzlů které můžou nastat
 */

typedef enum ASTNodeType {

    BINOP,
    UNOP,
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
