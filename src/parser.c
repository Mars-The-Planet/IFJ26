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

    token active_token;
    stackPtr stack_indent;



void check_for_eol(){

    while(active_token.type == SYMBOL && strcmp(active_token.value, "EOL") == 0){
        next_token(&active_token, stack_indent);
    }
}

dataType type_transfer(){

    if(strcmp(active_token.value, "Int") == 0) return TYPE_INT;
    if(strcmp(active_token.value, "Double") == 0) return TYPE_DOUBLE;
    if(strcmp(active_token.value, "String") == 0) return TYPE_STRING;
    if(strcmp(active_token.value, "Unit") == 0) return TYPE_UNIT;
    if(strcmp(active_token.value, "Boolean") == 0) return TYPE_BOOL;
    if(strcmp(active_token.value, "List") == 0) return TYPE_LIST;
    exit(2);
}

/**
 * 
 * @brief funkce pro zpravování prologu kodu
 */
void code_prolog (){
    
    // případ pro newline na začátku
    next_token(&active_token, stack_indent);
    check_for_eol();

    if(active_token.type == KEYWORD){
        if(strcmp(active_token.value, "import") != 0) exit(2);
    } else 
        exit(2);

    next_token(&active_token, stack_indent);
    if(active_token.type == ID){
        if(strcmp(active_token.value, "ifj26") != 0) exit(2);
    } else
        exit(2);
    
    next_token(&active_token, stack_indent);
    if(active_token.type == SYMBOL){
        if(strcmp(active_token.value, ".") != 0) exit(2);
    } else
        exit(2);

    next_token(&active_token, stack_indent);
    if(active_token.type == OPERATOR){
        if(strcmp(active_token.value, "*") != 0) exit(2);
    } else 
        exit(2);
    
    return;
}


void parse_expression(){}

head *parse_param(){
    
    head *params = malloc(sizeof(head));
    if(params == NULL){
        
        fprintf(stderr,"ERROR: Memory allocation failed!");
        exit(99);

    }
    
    ListParam *list_end = NULL;

    params->first = NULL;

    while(1){
        next_token(&active_token, stack_indent);

        if(active_token.type == SYMBOL){

            if(strcmp(active_token.value, "EOL") == 0) continue;
            if(strcmp(active_token.value, ")") == 0) return params;
        
        } else if(active_token.type == ID){

            ListParam *new_param = malloc(sizeof(ListParam));
            if(new_param == NULL){
        
                fprintf(stderr, "ERROR: Memory allocation failed!");
                exit(99);
        
            }

            new_param->next = NULL;
            new_param->param_name = strdup(active_token.value);
            
            if(params->first == NULL){ 
                params->first = new_param;
                list_end = new_param;
            } else {
                list_end->next = new_param;
                list_end = new_param;
            }
            
            next_token(&active_token, stack_indent);
            check_for_eol();
            
            if(active_token.type == SYMBOL){
                if(strcmp(active_token.value, ":") != 0) exit(2);

                next_token(&active_token, stack_indent);
                check_for_eol();
                if(active_token.type != KEYWORD) exit(2);
                list_end->type = type_transfer();
                
                if(strcmp(active_token.value, "List") == 0) {
                    next_token(&active_token, stack_indent);
                    check_for_eol();
                    if(active_token.type != SYMBOL) exit(2);
                    if(strcmp(active_token.value, "[") != 0) exit(2);
                    
                    next_token(&active_token, stack_indent);
                    check_for_eol();
                    if(active_token.type != KEYWORD) exit(2);
                    list_end->list_type = type_transfer();

                    next_token(&active_token, stack_indent);
                    check_for_eol();
                    if(active_token.type != SYMBOL) exit(2);
                    if(strcmp(active_token.value, "]") != 0) exit(2);

                }
                next_token(&active_token, stack_indent);
                check_for_eol();
                if(active_token.type != SYMBOL) exit(2);
                if(strcmp(active_token.value, ")") == 0) return params;
                if(strcmp(active_token.value, ",") == 0) continue;

            } else
                exit(2);
        }   
    }
}

void parse_fun_body(int origin_indent){
    
    next_token(&active_token, stack_indent);
    if(active_token.type == SYMBOL && strcmp(active_token.value, "EOL") == 0){

        next_token(&active_token, stack_indent);
        if(sum_stack(stack_indent) <= origin_indent) exit(2);

    } else parse_expression();

}

void parse_fun(){

    int indent = sum_stack(stack_indent);

    AST_Node *new_fun = malloc(sizeof(AST_Node));
    if(new_fun == NULL){
        fprintf(stderr, "ERROR: Memory allocation failed!");
        exit(99);
    }
    
    new_fun->type = FUN_DEF;

    next_token(&active_token, stack_indent);
    if(active_token.type == ID){
        new_fun->nodeData.funData.fun_name = strdup(active_token.value);
    } else 
        exit(2);

    next_token(&active_token, stack_indent);
    if(active_token.type == SYMBOL){
        if(strcmp(active_token.value, "(") != 0){
            exit(2);
        }
    } else exit(2);
    
    new_fun->nodeData.funData.param_list = parse_param();

    next_token(&active_token, stack_indent);
    if(active_token.type == SYMBOL){
        if(strcmp(active_token.value, ":") == 0){
            
            next_token(&active_token, stack_indent);
            if(active_token.type == KEYWORD){

                new_fun->nodeData.funData.return_type = type_transfer();

                next_token(&active_token, stack_indent);
                if (active_token.type == SYMBOL){
                    if(strcmp(active_token.value, "=") == 0){
                        parse_fun_body(indent);
                    }
                } else 
                    exit(2);
            } else
                exit(2);
        
        } else if (strcmp(active_token.value, "=") == 0){
            parse_fun_body(indent);
        }
    } else 
        exit(2);

    return;
}

// TODO: dodělat funkci na kontrolu existence funkce main
void has_main(){

}


void the_parser(){

    int indent;
    
    while(1){
        next_token(&active_token, stack_indent);
        switch (active_token.type) {

            case KEYWORD:
                if(strcmp(active_token.value, "def") == 0){
                    parse_fun();
                    break;
                }
                else if(strcmp(active_token.value, "@main") == 0){
                    has_main();
                }
                else 
                    exit(2);

            case SYMBOL:
                if(strcmp(active_token.value, "EOL") == 0) continue;
                else if(strcmp(active_token.value, "EOF") == 0) break;

            default:
                exit(2);

        }
        return;
    }

    return;
}



// dočasný main
int main(){

    code_prolog();

    return 1;
}


