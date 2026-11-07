/**
* @file stack.h
 * @brief Hlavička kódu pro implementaci zásobníku pomocí listů
 * @author Radek Večeřa (xvecerr01)
 * @date 31.10.2026
 */

#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Struktura listu zásobníku
 */
typedef struct stack_node {
    void *value;
    struct stack_node *next;
} s_node, *s_nodePtr;


/**
 * @brief Struktura zásobníku
 */
typedef struct stack {
    s_nodePtr top;
} stack, *stackPtr;



// ===========================================================================
// PROTOTYPY FUNKCÍ
// ===========================================================================


/**
 * @brief Konstruktor listů pro zásobníku
 * @param value hodnota uložená v listu.
 * @return Vrátí pointer na vytvořený list, nebo NULL
 */
s_nodePtr s_node_ctor(void *value);


/**
 * @brief Destruktor listu
 * @param current_node ničený list
 */
void s_node_dtor(s_nodePtr current_node);


/**
 * @brief Konstruktor zásobníku
 * @return Vrátí pointer na vytvořený zásobník, nebo NULL
 */
stackPtr stack_ctor();


/**
 * @brief Destruktor zásobníku
 * @param st ničený zásobník
 */
void stack_dtor(stackPtr st);


/**
 * @brief Vloží prvek na začátek
 * @param st ukazatel na strukturu zásobníku
 * @param value hodnota, která se uloží do listu.
 * @return Vrátí 0 při úspěchu, 1 při selhání
 */
int stack_push(stackPtr st, void *value);


/**
 * @brief Procedura pro odstranění prvního prvku zásobníku
 * @param st ukazatel na strukturu zásobníku
 */
void stack_pop(stackPtr st);


/**
 * @brief Procedura pro přečtení hodnoty v prvku na vrcholu zásobníku
 * @param st ukazatel na strukturu zásobníku
 * @return Vrátí ukazatel na hodnotu v prvku na vrcholu zásobníku
 */
void *stack_top(stackPtr st);

#endif // STACK_H
