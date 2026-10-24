/**
* @file avl_tree.c
 * @author Radek Večeřa (xvecerr01)
 *
 * @brief Hlavička kódu pro implementaci AVL stromu pro tabulku symbolů.
 * @date 24.10.2026
 */

#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Struktura listu stromu
 */
typedef struct avl_node {
    char *key;
    void *value;
    struct avl_node *left;
    struct avl_node *right;
    unsigned int height;
} node, *nodePtr;


// ===========================================================================
// PROTOTYPY FUNKCÍ
// ===========================================================================


/**
 * @brief Konstruktor listů pro AVL strom
 * Vytvoří a inicializuje nový uzel (node) stromu.
 *
 * @param key klíč pro přístup k hodnotě (must be allocated).
 * @param value hodnota uložená pod klíčem.
 * @return Pointer na vytvořený uzel, nebo NULL v případě chyby alokace.
 */
nodePtr avl_node_ctor(char *key, void *value);


/**
 * @brief Destruktor AVL stromu
 * @param root Kořen stromu
 */
void avl_node_dtor(nodePtr root);


/**
 * @brief Funkce vrátí výšku listu (uzlu)
 *
 * @param root Kořen stromu/uzlu. Může být NULL.
 * @return Vrátí výšku. V případě NULL vrátí 0.
 */
unsigned int avl_node_height(nodePtr root);


/**
 * @brief Funkce pro vkládání/přepisování prvků stromu
 * Implementuje rekurzivní vložení. (Poznámka: Implementace rotací chybí.)
 *
 * @param root Kořen stromu do kterého je vkládán nový prvek. Může být NULL.
 * @param key klíč pro přístup k hodnotě.
 * @param value hodnota uložená pod klíčem.
 * @return Pointer na vrchol aktualizovaného stromu (root).
 */
nodePtr avl_insert(nodePtr root, char *key, void *value);


/**
 * @brief Funkce pro vyhledání uzlu podle klíče
 *
 * @param root Kořen stromu, kde je vyhledávání zahájeno.
 * @param key Klíč, který se hledá.
 * @return Pointer na nalezený uzel, nebo NULL pokud klíč není přítomen.
 */
nodePtr avl_get_node(nodePtr root, char *key);


/**
 * @brief Funkce vrátí větší hodnotu z dvou nezáporných celých čísel.
 *
 * @param a První hodnota.
 * @param b Druhá hodnota.
 * @return Vrátí větší hodnotu.
 */
unsigned int max(unsigned int a, unsigned int b);

#endif // AVL_TREE_H
