/**
* @file parser.c
* @author Jonáš Vávra (x273960)
*
* @brief Imlementace syntaxtického analyzátoru
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"
#include "parser.h"

void code_prolog (tokenPtr token, stackPtr stack) {
    
    next_token(token, stack);
    if(token->type == KEYWORD){
        if(strcmp(token->value, "import") != 0) exit(2);
    } else 
        exit(2);

    next_token(token, stack);
    if(token->type == ID){
        if(strcmp(token->value, "ifj26") != 0) exit(2);
    } else
        exit(2);
    
    next_token(token, stack);
    if(token->type == SYMBOL){
        if(strcmp(token->value, ".") != 0) exit(2);
    } else
        exit(2);

    next_token(token, stack);
    if(token->type == OPERATOR){
        if(strcmp(token->value, "*") != 0) exit(2);
    } else 
        exit(2);
    
    return;
}



// dočasný main
int main(){
    
    token active_token;    


    return 1;
}


