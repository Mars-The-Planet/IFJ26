/**
 * IFJ26 - Implementace překladače imperativního jazyka IFJ26
 * @file symtable.h
 * @author Radek Večeřa (xvecerr01)
 *
 * @brief Hlavička kódu pro Implementaci tabulky symbolů pomocí zásobníku a AVL
 * stromů
 * @date 7.11.2026
 */
#ifndef SYMTABLE_H
#define SYMTABLE_H

#define GLOBAL_SYMTABLE_ID 0
#define PROTECT_GLOBAL_SYMTABLE 1

#include "bst_tree.h"
#include "stack.h"

/**
 * @brief Struktura tabulky symbolů
 */
typedef struct scoped_symtable {
  stackPtr scopes;
  unsigned int scope_id;
} scoped_symtable, *scoped_symtablePtr;

// ===========================================================================
// PROTOTYPY FUNKCÍ
// ===========================================================================

/**
 * @brief Konstruktor tabulky symbolů
 * @return Vrátí pointer na tabulku symbolů
 */
scoped_symtablePtr symbol_table_ctor();

/**
 * @brief Destruktor tabulky symbolů
 */
void symbol_table_dtor(scoped_symtablePtr symtable);

/**
 * @brief Přidá novou úroveň (scope) do tabulky symbolů
 * @param symtable tabulka symbolů
 * @return Vrátí 0 při úspěchu, 1 při selhání
 */
int scope_enter(scoped_symtablePtr symtable);

/**
 * @brief Odstraní úroveň (scope) z tabulky symbolů
 * @param symtable tabulka symbolů
 */
void scope_leave(scoped_symtablePtr symtable);

/**
 * @brief Získá čislo úrovně (scope) tabulky symbolů
 * @param symtable tabulka symbolů
 * @return Vrátí úroveň (scope) tabulky symbolů
 */
unsigned int get_scope_id(scoped_symtablePtr symtable);

/**
 * @brief Vložení do nejvyšší úrovně tabulky symbolů
 * @param symtable tabulka symbolů
 * @return Vrátí 0 při úspěchu, 1 při selhání
 */
int insert_symbol(scoped_symtablePtr symtable, char *symbol, void *value);

/**
 * @brief Upraví existující symbol v tabulce symbolů
 * @param symtable tabulka symbolů
 * @return Vrátí 0 při úspěchu, 1 při selhání
 */
int update_symbol(scoped_symtablePtr symtable, char *symbol, void *value);

/**
 * @brief Vyhledání symbolu v tabulce symbolů
 * @param symtable tabulka symbolů
 * @param symbol hledaný klíč
 * @return Vrátí node při úspěchu, NULL při selhání
 */
nodePtr get_symbol_node(scoped_symtablePtr symtable, char *symbol);

/**
 * @brief Vyhledání symbolu v tabulce symbolů
 * @param symtable tabulka symbolů
 * @param symbol hledaný klíč
 * @return Vrátí hodnotu při úspěchu, NULL při selhání
 */
void *get_symbol(scoped_symtablePtr symtable, char *symbol);

#endif // SYMTABLE_H
