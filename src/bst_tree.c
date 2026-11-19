/**
 * IFJ26 - Implementace překladače imperativního jazyka IFJ26
 * @file bst_tree.c
 * @author Radek Večeřa (xvecerr01)
 *
 * @brief Implementace AVL stromu pro tabulku symbolů.
 * @date 24.10.2026
 */

#include "bst_tree.h"

/**
 * @brief Konstruktor listů pro AVL strom
 * @param key klíč pro přístup k hodnotě.
 * @param value hodnota uložená pod klíčem.
 * @return Vrátí pointer na vytvořený list, nebo NULL
 */
nodePtr avl_node_ctor(char *key, void *value) {
    nodePtr newNode = malloc(sizeof(node));
    if (newNode!=NULL) {
        newNode->key = key;
        newNode->value = value;
        newNode->left = nullptr;
        newNode->right = nullptr;
        newNode->height = 1;
    }
    return newNode;
}


/**
 * @brief Destruktor AVL stromu
 * @param root Kořen stromu
 */
void avl_node_dtor(nodePtr root) {
    if (root!=NULL) {
        avl_node_dtor(root->left);
        avl_node_dtor(root->right);
        free(root);
    }
}


/**
 * @brief Funkce vrátí výšku listu
 * @param root List stromu. Může být NULL.
 * @return Vrátí výšku. V případě NULL vrátí 0.
 */
unsigned int avl_node_height(nodePtr root) {
    if (root==NULL)
        return 0;
    return root->height;
}


/**
 * @brief Funkce vrátí větší hodnotu
 * @param a První hodnota
 * @param b druhá hodnota
 * @return Vrátí větší hodnotu
 */
unsigned int max(unsigned int a, unsigned int b) {
    if (a>b)
        return a;
    return b;
}


/**
 * @brief Funkce provede pravou rotaci
 * @param root List stromu, na kterém se provede operace
 * @return Vrátí nově vyšší list stromu
 */
nodePtr avl_node_rotate_right(nodePtr root) {
    nodePtr left = root->left;
    root->left = left->right;
    left->right = root;

    // Oprava proměnné height
    root->height = max(avl_node_height(root->left), avl_node_height(root->right)) + 1;
    left->height = max(avl_node_height(left->left), avl_node_height(left->right)) + 1;
    return left;
}


/**
 * @brief Funkce provede levou rotaci
 * @param root List stromu, na kterém se provede operace
 * @return Vrátí nově vyšší list stromu
 */
nodePtr avl_node_rotate_left(nodePtr root) {
    nodePtr right = root->right;
    root->right = right->left;
    right->left = root;

    // Oprava proměnné height
    root->height = max(avl_node_height(root->left), avl_node_height(root->right)) + 1;
    right->height = max(avl_node_height(right->left), avl_node_height(right->right)) + 1;
    return right;
}


/**
 * @brief Funkce provede rotaci, pokud má nastat
 * @param root List stromu, na kterém se může provést rotace
 * @return Vrátí nový kořen, nebo původní nezměněný kořen
 */
nodePtr avl_node_rotate(nodePtr root) {
    if (avl_node_height(root->left) - avl_node_height(root->right) == 2) {
        return avl_node_rotate_right(root);
    }
    if (avl_node_height(root->right) - avl_node_height(root->left) == 2) {
        return avl_node_rotate_left(root);
    }
    return root;
}


/**
 * @brief Funkce pro vkládání/přepisování prvků stromu
 * @param root kořen stromu do kterého je vkládán nový prvek. Může být NULL.
 * @param key klíč pro přístup k hodnotě.
 * @param value hodnota uložená pod klíčem.
 * @param failed příznak chyby alokace paměti
 * @return Vrátí pointer na vrchol stromu
 */
nodePtr avl_node_insert(nodePtr root, char *key, void *value, int *failed) {
    // Strom nebo list neexistuje
    if (root==NULL) {
        nodePtr newNode = avl_node_ctor(key, value);
        if (newNode==NULL)
            *failed = 1;
        return newNode;
    }

    // Průchod stromem
    int result = strcmp(key, root->key);
    if (result==0) {
        root->value = value;
        return root;
    }
    if (result<0) {
        root->left = avl_node_insert(root->left, key, value, failed);
        root->height = max(avl_node_height(root->left), avl_node_height(root->right)) + 1;
        return avl_node_rotate(root);
    }
    root->right = avl_node_insert(root->right, key, value, failed);
    root->height = max(avl_node_height(root->left), avl_node_height(root->right)) + 1;
    return avl_node_rotate(root);
}


/**
 * @brief Funkce pro vkládání/přepisování prvků stromu
 * @param root kořen stromu.
 * @param key klíč pro přístup k hodnotě.
 * @return Vrátí pointer na hodnotu, nebo NULL
 */
nodePtr avl_node_get(nodePtr root, char *key) {
    // Nenalezeno
    if (root==NULL) {
        return nullptr;
    }

    // Průchod stromem
    int result = strcmp(key,root->key);
    if (result==0) {
        return root;
    }
    if (result<0) {
        return avl_node_get(root->left,key);
    }
    return avl_node_get(root->right,key);
}


// ===========================================================================
// ABSTRAKCE IMPLEMENTACE AVL STROMU
// ===========================================================================


/**
 * @brief Konstruktor AVL stromu
 * @return Vrátí pointer na strukturu stromu
 */
avl_treePtr tree_ctor() {
    avl_treePtr newTree = malloc(sizeof(avl_tree));
    if (newTree!=NULL) {
        newTree->root = nullptr;
    }
    return newTree;
}


/**
 * @brief Destruktor AVL stromu
 */
void tree_dtor(avl_treePtr tree) {
    avl_node_dtor(tree->root);
    free(tree);
}


/**
 * @brief Vyhledávání ve struktuře stromu
 * @param tree AVL strom
 * @param key hledaný klíč
 * @return Vrátí pointer na nalezený prvek, může být NULL
 */
nodePtr tree_get(avl_treePtr tree, char *key) {
    return avl_node_get(tree->root, key);
}


/**
 * @brief Vkládání do struktury stromu
 * @param tree AVL strom
 * @param key klíč nového/přepisovaného prvku
 * @param value hodnota nového prvku
 * @return Vrátí 0 při úspěchu, 1 při selhání
 */
int tree_insert(avl_treePtr tree, char *key, void *value) {
    int failed = 0;
    tree->root = avl_node_insert(tree->root, key, value, &failed);
    return failed;
}
