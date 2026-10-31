/**
 * @file avl_tree.c
 * @author Radek Večeřa (xvecerr01)
 *
 * @brief Implementace AVL stromu pro tabulku symbolů.
 * @date 24.10.2026
 */

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
nodePtr avl_rotate_right(nodePtr root) {
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
nodePtr avl_rotate_left(nodePtr root) {
    nodePtr right = root->right;
    root->right = right->left;
    right->left = root;

    // Oprava proměnné height
    root->height = max(avl_node_height(root->left), avl_node_height(root->right)) + 1;
    right->height = max(avl_node_height(right->left), avl_node_height(right->right)) + 1;
    return right;
}


/**
 * @brief Funkce pro vkládání/přepisování prvků stromu
 * @param root kořen stromu do kterého je vkládán nový prvek. Může být NULL.
 * @param key klíč pro přístup k hodnotě.
 * @param value hodnota uložená pod klíčem.
 * @return Vrátí pointer na vrchol stromu
 */
nodePtr avl_insert(nodePtr root, char *key, void *value) {
    // Strom nebo list neexistuje
    if (root==NULL)
        return avl_node_ctor(key, value);

    // Průchod stromem
    int result = strcmp(key, root->key);
    if (result==0) {
        root->value = value;
        return root;
    }
    if (result<0) {
        root->left = avl_insert(root->left, key, value);
        root->height = max(avl_node_height(root->left), avl_node_height(root->right)) + 1;
        if (avl_node_height(root->left) - avl_node_height(root->right) == 2) {
            return avl_rotate_right(root);
        }
        if (avl_node_height(root->right) - avl_node_height(root->left) == 2) {
            return avl_rotate_left(root);
        }
        return root;
    }
    root->right = avl_insert(root->right, key, value);
    root->height = max(avl_node_height(root->left), avl_node_height(root->right)) + 1;
    if (avl_node_height(root->left) - avl_node_height(root->right) == 2) {
        return avl_rotate_right(root);
    }
    if (avl_node_height(root->right) - avl_node_height(root->left) == 2) {
        return avl_rotate_left(root);
    }
    return root;
}


/**
 * @brief Funkce pro vkládání/přepisování prvků stromu
 * @param root kořen stromu.
 * @param key klíč pro přístup k hodnotě.
 * @return Vrátí pointer na hodnotu, nebo NULL
 */
nodePtr avl_get_node(nodePtr root, char *key) {
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
        return avl_get_node(root->left,key);
    }
    return avl_get_node(root->right,key);
}


// TODO: REMOVE
int main(int argc, char *argv[]) {
    nodePtr root = avl_insert(nullptr, "test1", "result1");
    root = avl_insert(root, "test2", "result2");
    root = avl_insert(root, "test3", "result3");
    root = avl_insert(root, "test3", "result4");
    root = avl_insert(root, "test0", "result0");

    printf("%s: %d\n%s: %d\n%s: %d\n%s: %d\n",
        root->key, root->height,
        root->left->key, root->left->height,
        root->left->left->key, root->left->left->height,
        root->right->key, root->right->height);

    nodePtr getTest = avl_get_node(root, "test3");
    printf("Search result: '%s'='%s' at Height: %d\n", getTest->key, getTest->value, getTest->height);
    return 0;
}