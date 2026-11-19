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

#define USAGE_ERR "Usage: ./main.exe FILE\n"

#include <stdio.h>
#include "generator.h"
#include "parser.h"

int has_main;

bool youre_main_node;

AST_Node *main_node;

scoped_symtablePtr symbol_table;
token active_token;
stackPtr stack_indent;

stackPtr used_symbols;

char *ignored_funcs[] = {"print", "readLine", "readInt", "readDouble", "toStr", "toInt", "strLength", "subStr", "ord", "chr", NULL};

#endif // MAIN_H
