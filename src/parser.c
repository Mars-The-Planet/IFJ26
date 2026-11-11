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

    while(active_token.type == SYMBOL_TYPE && strcmp(active_token.value, "EOL") == 0){
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

    if(active_token.type == KEYWORD_TYPE){
        if(strcmp(active_token.value, "import") != 0) exit(2);
    } else 
        exit(2);

    next_token(&active_token, stack_indent);
    if(active_token.type == ID_TYPE){
        if(strcmp(active_token.value, "ifj26") != 0) exit(2);
    } else
        exit(2);
    
    next_token(&active_token, stack_indent);
    if(active_token.type == SYMBOL_TYPE){
        if(strcmp(active_token.value, ".") != 0) exit(2);
    } else
        exit(2);

    next_token(&active_token, stack_indent);
    if(active_token.type == OPERATOR_TYPE){
        if(strcmp(active_token.value, "*") != 0) exit(2);
    } else 
        exit(2);
    
    return;
}


AST_Node *parse_expression(){

}

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

        if(active_token.type == SYMBOL_TYPE){

            if(strcmp(active_token.value, "EOL") == 0) continue;
            if(strcmp(active_token.value, ")") == 0) return params;
        
        } else if(active_token.type == ID_TYPE){

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
            
            if(active_token.type == SYMBOL_TYPE){
                if(strcmp(active_token.value, ":") != 0) exit(2);

                next_token(&active_token, stack_indent);
                check_for_eol();
                if(active_token.type != KEYWORD_TYPE) exit(2);
                list_end->type = type_transfer();
                
                if(strcmp(active_token.value, "List") == 0) {
                    next_token(&active_token, stack_indent);
                    check_for_eol();
                    if(active_token.type != SYMBOL_TYPE) exit(2);
                    if(strcmp(active_token.value, "[") != 0) exit(2);
                    
                    next_token(&active_token, stack_indent);
                    check_for_eol();
                    if(active_token.type != KEYWORD_TYPE) exit(2);
                    list_end->list_type = type_transfer();

                    next_token(&active_token, stack_indent);
                    check_for_eol();
                    if(active_token.type != SYMBOL_TYPE) exit(2);
                    if(strcmp(active_token.value, "]") != 0) exit(2);

                }
                next_token(&active_token, stack_indent);
                check_for_eol();
                if(active_token.type != SYMBOL_TYPE) exit(2);
                if(strcmp(active_token.value, ")") == 0) return params;
                if(strcmp(active_token.value, ",") == 0) continue;

            } else
                exit(2);
        }   
    }
}

AST_Node *parse_var(){

    AST_Node *temp_var = malloc(sizeof(AST_Node));
    if(temp_var == NULL){
        fprintf(stderr, "Memory allocation failed!");
        exit(99);
    }

    temp_var->type = VAR_DEF;

    if(strcmp(active_token.value, "var") != 0) temp_var->nodeData.varData.isvar = 0;
    else 
        temp_var->nodeData.varData.isvar = 1;

    next_token(&active_token, stack_indent);
    if(active_token.type == ID_TYPE) {
        temp_var->nodeData.varData.var_name = strdup(active_token.value);
    } else 
        exit(2);

    next_token(&active_token, stack_indent);
    if(active_token.type == SYMBOL_TYPE){
        if(strcmp(active_token.value, ":") == 0){

            next_token(&active_token, stack_indent);
            if(active_token.type == KEYWORD_TYPE){
                temp_var->nodeData.varData.varType = type_transfer();

                if(strcmp(active_token.value, "List") == 0){
                
                next_token(&active_token, stack_indent);
                if(active_token.type != SYMBOL_TYPE) exit(2);
                if(strcmp(active_token.value, "[") != 0) exit(2);
                
                next_token(&active_token, stack_indent);
                if(active_token.type != KEYWORD_TYPE) exit(2);
                
                temp_var->nodeData.varData.varListType = type_transfer();

                next_token(&active_token, stack_indent);
                if(active_token.type != SYMBOL_TYPE) exit(2);
                if(strcmp(active_token.value, "]") != 0) exit(2);
                }
                
                next_token(&active_token, stack_indent);
                if(active_token.type == OPERATOR_TYPE){
                    if(strcmp(active_token.value, "=") != 0) exit(2);
                    next_token(&active_token, stack_indent);
                    temp_var->nodeData.varData.expression = parse_expression();

                } else 
                    exit(2);
            } else 
                exit(2);
        } 
    } else if(active_token.type == OPERATOR_TYPE){
        // rovná se znamená že proměnná nemá ještě definovaný typ
        if(strcmp(active_token.value, "=") == 0){
            temp_var->nodeData.varData.varType = TYPE_UNDEF;
            temp_var->nodeData.varData.expression = parse_expression();
        }
    } else 
        exit(2);

    return temp_var;
}

AST_Node *parse_fun_body(int origin_indent){
    
    AST_Node *statements = NULL;
    AST_Node *temp_statement = NULL;
    AST_Node *last_statement = NULL;

    next_token(&active_token, stack_indent);
    // pokud je tam odsazení jedná se o tělo funkce, jinak je to výraz
    if(active_token.type == SYMBOL_TYPE && strcmp(active_token.value, "EOL") == 0){
        check_for_eol();
        if(active_token.type == SYMBOL_TYPE && strcmp(active_token.value, "EOF") == 0) exit(2);
        // tady může mít stack_indent mít hodnotu 0
        if(sum_stack(stack_indent) <= origin_indent) exit(2);
        origin_indent = sum_stack(stack_indent);

        while(1){
            check_for_eol();
            if(active_token.type == SYMBOL_TYPE && strcmp(active_token.value, "EOF") == 0) return statements;
            if(sum_stack(stack_indent) < origin_indent) return statements;
            // odsazení se nesmí jen tak zvětšit
            if(sum_stack(stack_indent) > origin_indent) exit(2);
            
            switch(active_token.type){

                case KEYWORD_TYPE:
                // var(1) a val(0) se rozlišuje podle bool 
                if(strcmp(active_token.value, "val") == 0 || strcmp(active_token.value, "var") == 0) temp_statement = parse_var();
                else if(strcmp(active_token.value, "while") == 0) temp_statement = parse_while();
                else if(strcmp(active_token.value, "if") == 0) temp_statement = parse_if();
                else if(strcmp(active_token.value, "for") == 0) temp_statement = parse_for();
                else if(strcmp(active_token.value, "return") == 0) temp_statement = parse_return();
                else 
                    exit(2);
                break;

                case ID_TYPE:
                    // funkce která už bude rozhodovat co s id vlastně dělat
                    temp_statement = parse_id();
                    break;
                
                // Jelikož pro všechny case se bude volat stejná funkce, použíl jsem case featuru propadnutí
                case INT_TYPE:
                case DOUBLE_TYPE:
                case STRING_TYPE:
                case SYMBOL_TYPE:
                case OPERATOR_TYPE:
                    temp_statement = parse_expression();
                    break;
            } 
            
            if(statements == NULL){
                statements = temp_statement;
                last_statement = temp_statement;
            } else {
                last_statement->next = temp_statement;
                last_statement = temp_statement;
            }

        }

    } else 
        return parse_expression();
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
    if(active_token.type == ID_TYPE){
        new_fun->nodeData.funData.fun_name = strdup(active_token.value);
    } else 
        exit(2);

    next_token(&active_token, stack_indent);
    if(active_token.type == SYMBOL_TYPE){
        if(strcmp(active_token.value, "(") != 0){
            exit(2);
        }
    } else exit(2);
    
    new_fun->nodeData.funData.param_list = parse_param();

    next_token(&active_token, stack_indent);
    if(active_token.type == SYMBOL_TYPE){
        if(strcmp(active_token.value, ":") == 0){
            
            next_token(&active_token, stack_indent);
            if(active_token.type == KEYWORD_TYPE){

                new_fun->nodeData.funData.return_type = type_transfer();
                
                //případ kde by vrácená hodnota byl list
                if(strcmp(active_token.value, "List") == 0){
                
                next_token(&active_token, stack_indent);
                if(active_token.type != SYMBOL_TYPE) exit(2);
                if(strcmp(active_token.value, "[") != 0) exit(2);
                
                next_token(&active_token, stack_indent);
                if(active_token.type != KEYWORD_TYPE) exit(2);
                // Uložení hodnoty které bude mít vracený list v sobě uložené
                new_fun->nodeData.funData.return_list = type_transfer();

                next_token(&active_token, stack_indent);
                if(active_token.type != SYMBOL_TYPE) exit(2);
                if(strcmp(active_token.value, "]") != 0) exit(2);
                }

                next_token(&active_token, stack_indent);
                if (active_token.type == OPERATOR_TYPE){
                    if(strcmp(active_token.value, "=") == 0){
                        parse_fun_body(indent);
                    } else 
                        exit(2);
                } else 
                    exit(2);
            } else
                exit(2);
        
        } 
    } else if(active_token.type == OPERATOR_TYPE){
        if(strcmp(active_token.value, "=") != 0) exit(2);
        parse_fun_body(indent);
    }

    return;
}

// TODO: dodělat funkci na kontrolu existence funkce main
void has_main(){

}


void the_parser(){

    int indent;
    
    while(1){
        next_token(&active_token, stack_indent);
        switch(active_token.type) {

            case KEYWORD_TYPE:
                if(strcmp(active_token.value, "def") == 0){
                    parse_fun();
                    break;
                }
                else if(strcmp(active_token.value, "@main") == 0){
                    has_main();
                }
                else 
                    exit(2);

            case SYMBOL_TYPE:
                if(strcmp(active_token.value, "EOL") == 0) continue;
                else if(strcmp(active_token.value, "EOF") == 0) break;

            default:
                exit(2);

        }
    }

    return;
}



// dočasný main
int main(){

    stack_indent = stack_ctor();

    if(set_target_file("test.txt") != 0) exit(99);

    code_prolog();

    stack_dtor(stack_indent);
    return 0;
}


