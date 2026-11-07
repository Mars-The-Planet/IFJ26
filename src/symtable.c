/**
 * @file symtable.c
 * @author Radek Večeřa (xvecerr01)
 *
 * @brief Implementace tabulky symbolů pomocí zásobníku a AVL stromů
 * @date 7.11.2026
 */

#include "symtable.h"


/**
 * @brief Konstruktor tabulky symbolů
 * @return Vrátí pointer na tabulku symbolů
 */
scoped_symtablePtr symbol_table_ctor() {
    scoped_symtablePtr newSymtable = malloc(sizeof(scoped_symtable));
    if (newSymtable!=NULL) {
        newSymtable->scopes = stack_ctor();
        newSymtable->scope_id = GLOBAL_SYMTABLE_ID;

        if (newSymtable->scopes==NULL) {
            free(newSymtable);
            return nullptr;
        }

        avl_treePtr globalScope = tree_ctor();

        if (globalScope==NULL) {
            free(newSymtable->scopes);
            free(newSymtable);
            return nullptr;
        }

        stack_push(newSymtable->scopes, globalScope);
    }
    return newSymtable;
}


/**
 * @brief Destruktor tabulky symbolů
 */
void symbol_table_dtor(scoped_symtablePtr symtable) {
    while (stack_top(symtable->scopes) != NULL) {
        tree_dtor(stack_top(symtable->scopes));
        stack_pop(symtable->scopes);
    }
    free(symtable->scopes);
    free(symtable);
}


/**
 * @brief Přidá novou úroveň (scope) do tabulky symbolů
 * @param symtable tabulka symbolů
 * @return Vrátí 0 při úspěchu, 1 při selhání
 */
int scope_enter(scoped_symtablePtr symtable) {
    avl_treePtr newScope = tree_ctor();
    if (newScope==NULL) {
        return 1;
    }
    stack_push(symtable->scopes, newScope);
    symtable->scope_id += 1;
    return 0;
}


/**
 * @brief Odstraní úroveň (scope) z tabulky symbolů
 * @param symtable tabulka symbolů
 */
void scope_leave(scoped_symtablePtr symtable) {
    if (PROTECT_GLOBAL_SYMTABLE) {
        if (symtable->scope_id==GLOBAL_SYMTABLE_ID) {
            printf("[!] Pokus odejití z globální tabulka symbolů.");
            return;
        }
    }
    tree_dtor(stack_top(symtable->scopes));
    stack_pop(symtable->scopes);
    symtable->scope_id -= 1;
}


/**
 * @brief Získá čislo úrovně (scope) tabulky symbolů
 * @param symtable tabulka symbolů
 * @return Vrátí úroveň (scope) tabulky symbolů
 */
unsigned int get_scope_id(scoped_symtablePtr symtable) {
    return symtable->scope_id;
}


/**
 * @brief Vložení do nejvyšší úrovně tabulky symbolů
 * @param symtable tabulka symbolů
 * @return Vrátí 0 při úspěchu, 1 při selhání
 */
int insert_symbol(scoped_symtablePtr symtable, char *symbol, void *value) {
    return tree_insert(stack_top(symtable->scopes), symbol, value);
}


/**
 * @brief Vyhledání symbolu v tabulce symbolů
 * @param symtable tabulka symbolů
 * @param symbol hledaný klíč
 * @return Vrátí hodnotu při úspěchu, NULL při selhání
 */
char *get_symbol(scoped_symtablePtr symtable, char *symbol) {
    s_nodePtr currentScopeNode = symtable->scopes->top;
    nodePtr result = nullptr;
    while (currentScopeNode != NULL && result==NULL) {
        result = tree_get(currentScopeNode->value, symbol);
        currentScopeNode = currentScopeNode->next;
    }
    if (result!=NULL)
        return result->value;
    return nullptr;
}
