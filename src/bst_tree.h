/**
 * IFJ26 - Implementace překladače imperativního jazyka IFJ26
 * @file bst_tree.h
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


/**
 * @brief Struktura AVL stromu
 */
typedef struct bst_tree {
    nodePtr root;
} avl_tree, *avl_treePtr;


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
 * @brief Funkce provede rotaci, pokud má nastat
 * @param root List stromu, na kterém se může provést rotace
 * @return Vrátí nový kořen, nebo původní nezměněný kořen
 */
nodePtr avl_node_rotate(nodePtr root);


/**
 * @brief Funkce pro vkládání/přepisování prvků stromu
 * Implementuje rekurzivní vložení. (Poznámka: Implementace rotací chybí.)
 *
 * @param root Kořen stromu do kterého je vkládán nový prvek. Může být NULL.
 * @param key klíč pro přístup k hodnotě.
 * @param value hodnota uložená pod klíčem.
 * @return Pointer na vrchol aktualizovaného stromu (root).
 */
nodePtr avl_node_insert(nodePtr root, char *key, void *value, int *failed);


/**
 * @brief Funkce pro vyhledání uzlu podle klíče
 *
 * @param root Kořen stromu, kde je vyhledávání zahájeno.
 * @param key Klíč, který se hledá.
 * @return Pointer na nalezený uzel, nebo NULL pokud klíč není přítomen.
 */
nodePtr avl_node_get(nodePtr root, char *key);


/**
 * @brief Funkce vrátí větší hodnotu z dvou nezáporných celých čísel.
 *
 * @param a První hodnota.
 * @param b Druhá hodnota.
 * @return Vrátí větší hodnotu.
 */
unsigned int max(unsigned int a, unsigned int b);


/**
 * @brief Funkce provede pravou rotaci
 * @param root List stromu, na kterém se provede operace
 * @return Vrátí nově vyšší list stromu
 */
nodePtr avl_node_rotate_right(nodePtr root);

/**
 * @brief Funkce provede levou rotaci
 * @param root List stromu, na kterém se provede operace
 * @return Vrátí nově vyšší list stromu
 */
nodePtr avl_node_rotate_left(nodePtr root);


// ===========================================================================
// ABSTRAKCE IMPLEMENTACE AVL STROMU
// ===========================================================================


/**
 * @brief Konstruktor AVL stromu
 * @return Vrátí pointer na strukturu stromu
 */
avl_treePtr tree_ctor();


/**
 * @brief Destruktor AVL stromu
 */
void tree_dtor(avl_treePtr tree);


/**
 * @brief Vyhledávání ve struktuře stromu
 * @param tree AVL strom
 * @param key hledaný klíč
 * @return Vrátí pointer na nalezený prvek, může být NULL
 */
nodePtr tree_get(avl_treePtr tree, char *key);


/**
 * @brief Vkládání do struktury stromu
 * @param tree AVL strom
 * @param key klíč nového/přepisovaného prvku
 * @param value hodnota nového prvku
 * @return Vrátí 0 při úspěchu, 1 při selhání
 */
int tree_insert(avl_treePtr tree, char *key, void *value);

#endif // AVL_TREE_H
