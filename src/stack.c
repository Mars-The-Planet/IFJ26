/**
 * @file stack.c
 * @author Radek Večeřa (xvecerr01)
 *
 * @brief Implementace zásobníku pomocí listů
 * @date 31.10.2026
 */

#include <stdio.h>
#include <stdlib.h>


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


/**
 * @brief Konstruktor listů pro zásobník
 * @param value hodnota uložená v listu.
 * @return Vrátí pointer na vytvořený list, nebo NULL
 */
s_nodePtr s_node_ctor(void *value) {
    s_nodePtr newNode = malloc(sizeof(s_node));
    if (newNode!=NULL) {
        newNode->value = value;
        newNode->next = nullptr;
    }
    return newNode;
}


/**
 * @brief Destruktor listu
 * @param current_node ničený list
 */
void s_node_dtor(s_nodePtr current_node) {
    if (current_node!=NULL) {
        s_node_dtor(current_node->next);
        free(current_node);
    }
}


/**
 * @brief Konstruktor zásobníku
 * @return Vrátí pointer na vytvořený zásobník, nebo NULL
 */
stackPtr stack_ctor() {
    stackPtr newStack = malloc(sizeof(stack));
    if (newStack!=NULL) {
        newStack->top = nullptr;
    }
    return newStack;
}


/**
 * @brief Destruktor zásobníku
 * @param st ničený zásobník
 */
void stack_dtor(stackPtr st) {
    s_node_dtor(st->top);
}


/**
 * @brief Vloží prvek na začátek
 * @param st ukazatel na strukturu zásobníku
 * @param value hodnota, která se uloží do listu.
 * @return Vrátí 0 při úspěchu, 1 při selhání
 */
int stack_push(stackPtr st, void *value) {
    s_nodePtr oldNode = st->top;
    st->top = s_node_ctor(value);
    if (st->top==NULL)
        return 1;
    st->top->next = oldNode;
    return 0;
}


/**
 * @brief Procedura pro odstranění prvního prvku zásobníku
 * @param st ukazatel na strukturu zásobníku
 */
void stack_pop(stackPtr st) {
    s_nodePtr oldNode = st->top;
    st->top = oldNode->next;
    free(oldNode);
}


/**
 * @brief Procedura pro přečtení hodnoty v prvku na vrcholu zásobníku
 * @param st ukazatel na strukturu zásobníku
 * @return Vrátí ukazatel na hodnotu v prvku na vrcholu zásobníku
 */
void *stack_top(stackPtr st) {
    s_nodePtr selNode = st->top;
    if (st->top==NULL)
        return NULL;
    return selNode->value;
}
