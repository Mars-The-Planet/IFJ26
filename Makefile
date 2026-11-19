# IFJ26 - Implementace překladače imperativního jazyka IFJ26
# Autor: Matěj Růžička (xruzicm00)

CC = clang
MANDATORY_FLAGS = -std=c23 -Wall -Wextra
ALL_FLAGS = $(MANDATORY_FLAGS) -Werror -pedantic
# musi se vytvorit
MAIN = main
SRC = src/
# avl_tree se musi prejmenovat na bst
DEPS = $(SRC)$(MAIN).c $(SRC)avl_tree.c $(SRC)dynamic_string.c $(SRC)parser.c $(SRC)scanner.c $(SRC)stack.c $(SRC)symtable.c $(SRC)generator.c

build: $(DEPS)
	$(CC) $(MANDATORY_FLAGS) $(DEPS) -o $(MAIN)

build_padentic: $(DEPS)
	$(CC) $(ALL_FLAGS) $(DEPS) -o $(MAIN)
