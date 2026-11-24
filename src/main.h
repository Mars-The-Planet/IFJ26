/**
* IFJ26 - Implementace překladače imperativního jazyka IFJ26
 * @file main.h
 * @author Radek Večeřa (xvecerr01)
 *
 * @brief Hlavička implementace spustitelného souboru pro překladač
 * @date 18.11.2026
 */

#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include "generator.h"
#include "parser.h"

#define ERROR "[Chyba] "
#define NO_MAIN ERROR"Programu chybí @main.\n"
#define MAIN_HAS_PARAMS ERROR"Funkce označená @main nesmí mít parametry.\n"
#define MAIN_WRONG_RETURN ERROR"Funkce označená @main musí vracet Unit.\n"

int has_main;

bool youre_main_node;

AST_Node *main_node;

scoped_symtablePtr symbol_table;
token active_token;
stackPtr stack_indent;

stackPtr used_symbols;

int exit_code;

stackPtr memory_allocs;

char *ignored_funcs[] = {"print", "readLine", "readInt", "readDouble", "toStr", "toInt", "strLength", "subStr", "ord", "chr", NULL};

#endif // MAIN_H
