/**
 * @file dynamic_string.c
 * @author Matěj Růžička (xruzicm00)
 *
 * @brief Implementace nafukovacího retezce s vkládáním pouze na konec
 */

#include "dynamic_string.h"


int init_dynamic_string(dynamic_string_ptr string_ptr, int len) {

    if (len <= 0) {
        fprintf(stderr, "Velikost kladneho retezce musi byt kladna\n");
        return 1;
    }

    string_ptr->max_size = len;
    string_ptr->size = 0;

    // calloc nastaví všude \0
    string_ptr->val_ptr = calloc(len, sizeof(char));

    if (string_ptr->val_ptr == NULL) {
        fprintf(stderr, "Selhala alkoace noveho dynamickeho retezce\n");
        return 1;
    }
}


int insert_last(dynamic_string_ptr string_ptr, char ch) {

    // nafukování řetězce je-li potřeba
    if (string_ptr->size >= string_ptr->max_size - 1) {
        int new_max_size = string_ptr->max_size * 2;
        char *temp = realloc(string_ptr->val_ptr, new_max_size * sizeof(char));

        if (temp == NULL) {
            fprintf(stderr, "Selhalo rozsireni dynamickeho retezce\n");
            return 1;
        }

        string_ptr->max_size = new_max_size;
        string_ptr->val_ptr = temp;
    }

    string_ptr->val_ptr[string_ptr->size] = ch;
    string_ptr->val_ptr[string_ptr->size + 1] = '\0';

    string_ptr->size++;
    return 0;
}

void free_dyn_string(dynamic_string_ptr string_ptr) {
    free(string_ptr->val_ptr);
}