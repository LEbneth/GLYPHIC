#include "ast.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>

#include "variable_storage.h"
#include "function_storage.h"
#include "function_params.h"

#define MAX_STR_LEN 1000

/**
 * current_node_id is necessary that all nodes in the final ast can be
 * seen individually in print_ast. Otherwise every eg "number" would
 * be squished into one node and make the visual representation useless.
 */
static int current_node_id = 0;

ast *node0(int type) {
    ast *buffer = calloc(sizeof *buffer, 1);
	buffer->type = type;
    buffer->node_id = current_node_id++;

	return buffer;
}

ast *node1(int type, ast *child1) {
    ast *buffer = node0(type);
	buffer->children[0] = child1;
    buffer->node_id = current_node_id++;

	return buffer;
}

ast *node2(int type, ast *child1, ast *child2) {
    ast *buffer = node1(type, child1);
	buffer->children[1] = child2;
    buffer->node_id = current_node_id++;

	return buffer;
}

ast *node3(int type, ast *child1, ast *child2, ast *child3) {
    ast *buffer = node2(type, child1, child2);
	buffer->children[2] = child3;
    buffer->node_id = current_node_id++;

	return buffer;
}

char *map_enum_to_string(int state) {
    switch (state)
    {
    /* Rule tokens */
    case GLYPHIC:
        return "GLYPHIC";
    case BLOCK:
        return "BLOCK";
    case GSTATEMENT:
        return "GSTATEMENT";
    case STATEMENT:
        return "STATEMENT";
    case CALCULATION:
        return "CALCULATION";
    case CALL:
        return "CALL";
    case FUNCNAME:
        return "FUNCNAME";
    case PARAMCALL:
        return "PARAMCALL";
    case FUNCTION:
        return "FUNCTION";
    case DECLARATION:
        return "DECLARATION";
    case IMPLEMENTATION:
        return "IMPLEMENTATION";
    case PARAMIMPL:
        return "PARAMIMPL";
    case BODY:
        return "BODY";
    case RETURN:
        return "RETURN";
    case STRUCTURE:
        return "STRUCTURE";
    case LOOP:
        return "LOOP";
    case CONDITION:
        return "CONDITION";
    case CHECK:
        return "CHECK";
    case CSTATEMENT:
        return "CSTATEMENT";
    case EVAL:
        return "EVAL";
    case PRINT:
        return "PRINT";
    case TEXTOBJECTS:
        return "TEXTOBJECTS";
    case RANDOM:
        return "RANDOM";
    case READNUM:
        return "READNUM";
    case READSTR:
        return "READSTR";
    case STRSPLIT:
        return "STRSPLIT";
    case STRLEN:
        return "STRLEN";
    case ARRLEN:
        return "ARRLEN";
    case VALUE:
        return "VALUE";
    case ACCESS:
        return "ACCESS";
    case ACCESSARR:
        return "ACCESSARR";
    case IDENTIFIER:
        return "IDENTIFIER";
    case NUMBER:
        return "NUMBER";
    case STRING:
        return "STRING";
    case OPERATOR:
        return "OPERATOR";
    case BOOLEAN:
        return "BOOLEAN";
    case NUMARR:
        return "NUMARR";
    case STRARR:
        return "STRARR";
    case BOOARR:
        return "BOOARR";
    case EQUALS:
        return "EQUALS";
    case ISNT:
        return "ISNT";
    case PLUS:
        return "PLUS";
    case MINUS:
        return "MINUS";
    case TIMES:
        return "TIMES";
    case DIVBY:
        return "DIVBY";
    case AND:
        return "AND";
    case OR:
        return "OR";
    case SMALLER:
        return "SMALLER";
    case BIGGER:
        return "BIGGER";
    case MODULO:
        return "MODULO";
    default: {
        char *tmp = malloc(100);
        sprintf(tmp, "%d", state);
        return tmp;
    }
    }
}

char *node2str(ast *tree) {
    char *str = malloc(MAX_STR_LEN);
    str[0] = '\0';
    strcat(str, map_enum_to_string(tree->type));

    if (tree->type == IDENTIFIER){
        strcat(str, ": ");
        strcat(str, tree->value.Vvalue.id);
    }
    if (tree->type == NUMBER){
        char *tmp = malloc(MAX_STR_LEN);
        tmp[0] = '\0';
        sprintf(tmp, ": %g", tree->value.Vvalue.num);
        strcat(str, tmp);
        free(tmp);
    }
    if (tree->type == STRING){
        strcat(str, ": ");
        strcat(str, tree->value.Vvalue.str);
    }
    if (tree->type == BOOLEAN){
        strcat(str, ": ");
        strcat(str, tree->value.Vvalue.boo);
    }
    if (tree->type == OPERATOR){
        strcat(str, ": ");
        strcat(str, tree->value.Vvalue.op);
    }
    if (tree->type == NUMARR){
        strcat(str, ": { ");
        for (size_t i = 0; i < tree->value.Vvalue.num_arr->length; i++){
            char *tmp = malloc(MAX_STR_LEN);
            tmp[0] = '\0';
            sprintf(tmp, "%g ", tree->value.Vvalue.num_arr->num_arr[i]);
            strcat(str, tmp);
            free(tmp);
        }
        strcat(str, "}");
    }
    if (tree->type == STRARR){
        strcat(str, ": { ");
        for (size_t i = 0; i < tree->value.Vvalue.str_arr->length; i++){
            strcat(str, tree->value.Vvalue.str_arr->str_arr[i]);
            strcat(str, " ");
        }
        strcat(str, "}");
    }
    if (tree->type == BOOARR){
        strcat(str, ": { ");
        for (size_t i = 0; i < tree->value.Vvalue.boo_arr->length; i++){
            strcat(str, tree->value.Vvalue.boo_arr->boo_arr[i]);
            strcat(str, " ");
        }
        strcat(str, "}");
    }

    return str;
}

void print_ast (ast *tree, int depth, char* filename) {
    if (!tree) return;
    
    static FILE* dot;
    char filename_final[MAX_STR_LEN];

    if (depth == 0) {
        struct stat st = {0};
        if (stat("graphs", &st) == -1) {
            mkdir("graphs", 0700);
        }

        snprintf(filename_final, sizeof(filename_final), "graphs/%s.gv", filename);
        dot = fopen(filename_final, "w");
        fprintf(dot, "digraph ast {\n");
    }

    fprintf(dot, "  n%d [label=\"%s\"]\n", tree->node_id, node2str(tree));
	for (int i = 0; i < MAX_CHILDREN; i++) {
		if (tree->children[i]) {
            fprintf(dot, "  n%d -> n%d\n", tree->node_id, tree->children[i]->node_id);
            print_ast(tree->children[i], depth+1, filename);
        }
	}

    if (depth == 0) {
        fprintf(dot, "}\n");
        fclose(dot);
    }
}

ast_value *perform_calculation(ast_value* op, ast_value* v1, ast_value* v2){
    ast_value *tmp = calloc(sizeof *tmp, 1);
    tmp->Vtype = Vnumber;
    tmp->Vvalue.num = 0;

    // both are not the same type, but either number or boolean
    // treat boolean as numbers (with true = 1 false = 0)
    if ((v1->Vtype != v2->Vtype) && (v1->Vtype == Vnumber || v1->Vtype == Vboolean) && (v2->Vtype == Vnumber || v2->Vtype == Vboolean)) {
        double t1 = v1->Vtype == Vnumber ? v1->Vvalue.num : (strncmp(v1->Vvalue.boo, "true", 5) == 0 ? 1 : 0);
        double t2 = v2->Vtype == Vnumber ? v2->Vvalue.num : (strncmp(v2->Vvalue.boo, "true", 5) == 0 ? 1 : 0);
        
        v1->Vtype = Vnumber;
        v1->Vvalue.num = t1;
        v2->Vtype = Vnumber;
        v2->Vvalue.num = t2;
    }
    // both are numbers
    if (v1->Vtype == Vnumber && v2->Vtype == Vnumber) {        
        double n1 = v1->Vvalue.num;
        double n2 = v2->Vvalue.num;
        
        // mathematical
        tmp->Vtype = Vnumber;
        if (strncmp(op->Vvalue.op, "plus", 4) == 0){ tmp->Vvalue.num = n1 + n2; return tmp; }
        if (strncmp(op->Vvalue.op, "minus", 5) == 0){ tmp->Vvalue.num = n1 - n2; return tmp; }
        if (strncmp(op->Vvalue.op, "times", 5) == 0){ tmp->Vvalue.num = n1 * n2; return tmp; }
        if (strncmp(op->Vvalue.op, "divby", 5) == 0){ tmp->Vvalue.num = n1 / n2; return tmp; }
        if (strncmp(op->Vvalue.op, "and", 3) == 0){ tmp->Vvalue.num = (int)n1 && (int)n2; return tmp; }
        if (strncmp(op->Vvalue.op, "or", 2) == 0){ tmp->Vvalue.num = (int)n1 || (int)n2; return tmp; }
        if (strncmp(op->Vvalue.op, "modulo", 6) == 0){ tmp->Vvalue.num = fmod(n1, n2); return tmp; }

        // logical
        tmp->Vtype = Vboolean;
        if (strncmp(op->Vvalue.op, "equals", 6) == 0){ tmp->Vvalue.boo = n1 == n2 ? "true" : "false"; return tmp; }
        if (strncmp(op->Vvalue.op, "isnt", 4) == 0){ tmp->Vvalue.boo = n1 != n2 ? "true" : "false"; return tmp; }
        if (strncmp(op->Vvalue.op, "bigger", 6) == 0){ tmp->Vvalue.boo = n1 > n2 ? "true" : "false"; return tmp; }
        if (strncmp(op->Vvalue.op, "smaller", 7) == 0){ tmp->Vvalue.boo = n1 < n2 ? "true" : "false"; return tmp; }
    }
    // both are boolean
    else if (v1->Vtype == Vboolean && v2->Vtype == Vboolean) {
        char* b1 = v1->Vvalue.boo;
        char* b2 = v2->Vvalue.boo;

        // logical
        tmp->Vtype = Vboolean;
        if (strncmp(op->Vvalue.op, "and", 3) == 0){ tmp->Vvalue.boo = (strncmp(b1, "true", 5) == 0 && strncmp(b2, "true", 5) == 0) ? "true" : "false"; return tmp; }
        if (strncmp(op->Vvalue.op, "or", 2) == 0){ tmp->Vvalue.boo = (strncmp(b1, "true", 5) == 0 || strncmp(b2, "true", 5) == 0) ? "true" : "false"; return tmp; }
        if (strncmp(op->Vvalue.op, "equals", 6) == 0){ tmp->Vvalue.boo = strncmp(b1, b2, 5) == 0 ? "true" : "false"; return tmp; }
        if (strncmp(op->Vvalue.op, "isnt", 4) == 0){ tmp->Vvalue.boo = strncmp(b1, b2, 5) != 0 ? "true" : "false"; return tmp; }
        
        // mathematical true = 1 false = 0
        tmp->Vtype = Vnumber;
        double n1 = strncmp(b1, "true", 5) == 0 ? 1 : 0;
        double n2 = strncmp(b1, "true", 5) == 0 ? 1 : 0;

        if (strncmp(op->Vvalue.op, "plus", 4) == 0){ tmp->Vvalue.num = n1 + n2; return tmp; }
        if (strncmp(op->Vvalue.op, "minus", 5) == 0){ tmp->Vvalue.num = n1 - n2; return tmp; }
        if (strncmp(op->Vvalue.op, "times", 5) == 0){ tmp->Vvalue.num = n1 * n2; return tmp; }
        if (strncmp(op->Vvalue.op, "divby", 5) == 0){ tmp->Vvalue.num = n1 / n2; return tmp; }
        if (strncmp(op->Vvalue.op, "bigger", 6) == 0){ tmp->Vvalue.num = n1 > n2; return tmp; }
        if (strncmp(op->Vvalue.op, "smaller", 7) == 0){ tmp->Vvalue.num = n1 < n2; return tmp; }
        if (strncmp(op->Vvalue.op, "modulo", 6) == 0){ tmp->Vvalue.num = fmod(n1, n2); return tmp; }
    }
    // both are strings
    else if (v1->Vtype == Vstring && v2->Vtype == Vstring) {
        char* s1 = v1->Vvalue.str;
        char* s2 = v2->Vvalue.str;
        int maxlen = (strlen(s1) > strlen(s2)) ? strlen(s1) : strlen(s2);

        // logical // bigger = longer // smaller = shorter // modulo = contains substring
        tmp->Vtype = Vboolean;
        if (strncmp(op->Vvalue.op, "equals", 6) == 0){ tmp->Vvalue.boo = strncmp(s1, s2, maxlen) == 0 ? "true" : "false"; return tmp; }
        if (strncmp(op->Vvalue.op, "isnt", 4) == 0){ tmp->Vvalue.boo = strncmp(s1, s2, maxlen) != 0 ? "true" : "false"; return tmp; }
        if (strncmp(op->Vvalue.op, "bigger", 6) == 0){ tmp->Vvalue.boo = strlen(s1) > strlen(s2) ? "true" : "false"; return tmp; }
        if (strncmp(op->Vvalue.op, "smaller", 7) == 0){ tmp->Vvalue.boo = strlen(s1) < strlen(s2) ? "true" : "false"; return tmp; }
        if (strncmp(op->Vvalue.op, "modulo", 6) == 0){ tmp->Vvalue.boo = strstr(s1, s2) != NULL ? "true" : "false"; return tmp; }

        // "mathematical"
        tmp->Vtype = Vstring;
        if (strncmp(op->Vvalue.op, "plus", 4) == 0){ 
            tmp->Vvalue.str = (char*)malloc(MAX_STR_LEN);
            strcpy(tmp->Vvalue.str, s1);
            strcat(tmp->Vvalue.str, s2);
            tmp->Vvalue.str = (char*) realloc(tmp->Vvalue.str, strlen(tmp->Vvalue.str));
            return tmp;
        }
    }
    
    return tmp;
}

ast_value *perform_statement(ast_value* op, ast_value* id, ast_value* rhs){
    if (strncmp(op->Vvalue.op, "equals", 6) == 0){ return rhs; }

    ast_value *tmp = calloc(sizeof *tmp, 1);
    tmp = var_get(id->Vvalue.id);

    return perform_calculation(op, tmp, rhs);
}

ast_value *execute_ast(ast *tree) {
    if (!tree) {
        ast_value *value = calloc(sizeof *value, 1);
        //printf("EMPTY\n");
        return value;
    }

    ast_value *tmp = calloc(sizeof *tmp, 1);

    switch (tree->type)
    {
    /* Rule tokens */
    case GLYPHIC: {
        execute_ast(tree->children[0]);
        execute_ast(tree->children[1]);
        break;
    }
    case BLOCK: {
        execute_ast(tree->children[0]);
        break;
    }
    case GSTATEMENT: {
        tmp = perform_statement(execute_ast(tree->children[1]), execute_ast(tree->children[0]), execute_ast(tree->children[2]));
        var_declare_global(tree->children[0]->value.Vvalue.id, tmp);
        break;
    }
    case STATEMENT: {
        tmp = perform_statement(execute_ast(tree->children[1]), execute_ast(tree->children[0]), execute_ast(tree->children[2]));
        var_set(tree->children[0]->value.Vvalue.id, tmp);
        break;
    }
    case CALCULATION: {
        printf("This should not happen: CALCULATION\n");
        break;
    }
    case CALL: {
        execute_ast(tree->children[1]);
        return execute_ast(func_get(execute_ast(tree->children[0])->Vvalue.id));
    }
    case FUNCNAME: {
        return execute_ast(tree->children[0]);
    }
    case PARAMCALL: {
        // enqueue params
        execute_ast(tree->children[0]);
        param_set(execute_ast(tree->children[1]));
        break;
    }
    case FUNCTION: {
        func_set(execute_ast(tree->children[0])->Vvalue.id, tree->children[1]);
        break;
    }
    case DECLARATION: {
        return execute_ast(tree->children[0]);
    }
    case IMPLEMENTATION: {
        var_enter_function();
        // retrieve params
        execute_ast(tree->children[0]);
        // execute function
        execute_ast(tree->children[1]);
        //execute return
        tmp = execute_ast(tree->children[2]);
        var_leave_function();
        return tmp;
    }
    case PARAMIMPL: {
        // dequeue params from queue
        execute_ast(tree->children[0]);
        var_set(tree->children[1]->value.Vvalue.id, param_get());
        break;
    }
    case BODY: {
        execute_ast(tree->children[0]);
        execute_ast(tree->children[1]);
        break;
    }
    case RETURN: {
        return execute_ast(tree->children[0]);
    }
    case STRUCTURE: {
        execute_ast(tree->children[0]);
        break;
    }
    case LOOP: {
        tmp = execute_ast(tree->children[0]);
        while (tmp->Vvalue.boo != NULL && strncmp(tmp->Vvalue.boo, "true", 5) == 0){
            var_enter_block();
            execute_ast(tree->children[1]);
            var_leave_block();
            tmp = execute_ast(tree->children[0]);
        }
        break;
    }
    case CONDITION: {
        tmp = execute_ast(tree->children[0]);
        if (tmp->Vvalue.boo != NULL && strncmp(tmp->Vvalue.boo, "true", 5) == 0){
            var_enter_block();
            execute_ast(tree->children[1]);
            var_leave_block();
        }
        else {
            var_enter_block();
            execute_ast(tree->children[2]);
            var_leave_block();
        }
        break;
    }
    case CHECK: {
        tmp = execute_ast(tree->children[0]);
        if (tmp->Vvalue.boo != NULL && strncmp(tmp->Vvalue.boo, "true", 5) == 0){
            var_enter_block();
            execute_ast(tree->children[1]);
            var_leave_block();
        }
        break;
    }
    case CSTATEMENT: {
        printf("This should not happen: CSTATEMENT\n");
        break;
    }
    case EVAL: {
        printf("This should not happen: EVAL\n");
        break;
    }
    case PRINT: {
        execute_ast(tree->children[0]);
        break;
    }
    case TEXTOBJECTS: {
        execute_ast(tree->children[0]);
        tmp = execute_ast(tree->children[1]);
        if (tmp->Vtype == Vstring){ printf("%s", tmp->Vvalue.str); }
        if (tmp->Vtype == Vnumber) { printf("%g", tmp->Vvalue.num); }
        if (tmp->Vtype == Vboolean) { printf("%s", tmp->Vvalue.boo); }
        if (tmp->Vtype == Vnumber_arr) {
            printf("{ ");
            for (size_t i = 0; i < tmp->Vvalue.num_arr->length; i++){
                printf("%g ", tmp->Vvalue.num_arr->num_arr[i]);
            }
            printf("}");
        }
        if (tmp->Vtype == Vstring_arr) {
            printf("{ ");
            for (size_t i = 0; i < tmp->Vvalue.str_arr->length; i++) {
                printf("%s ", tmp->Vvalue.str_arr->str_arr[i]);
            }
            printf("}");
        }
        if (tmp->Vtype == Vboolean_arr) {
            printf("{ ");
            for (size_t i = 0; i < tmp->Vvalue.boo_arr->length; i++) {
                printf("%s ", tmp->Vvalue.boo_arr->boo_arr[i]);
            }
            printf("}");
        }
        break;
    }
    case RANDOM: {
        ast_value* tmp1 = execute_ast(tree->children[0]);
        ast_value* tmp2 = execute_ast(tree->children[1]);
        tmp->Vtype = Vnumber;
        tmp->Vvalue.num = 0;

        if (tmp1->Vtype != Vnumber || tmp2->Vtype != Vnumber) {
            printf("Invalid range for random! Both params must be numbers.\n");
            return tmp;
        }

        srand(time(NULL));
        int lower = (int)tmp1->Vvalue.num;
        int upper = (int)tmp2->Vvalue.num;
        int random = lower + rand() % (upper - lower + 1);
        tmp->Vvalue.num = random;
        return tmp;
    }
    case READNUM: {
        double numbuf;
        tmp->Vtype = Vnumber;
        if (scanf("%lf", &numbuf) > 0){
            tmp->Vvalue.num = numbuf;
        }
        return tmp;
    }
    case READSTR: {
        char strbuf [MAX_STR_LEN];
        tmp->Vtype = Vstring;
        if (fgets(strbuf, sizeof(strbuf), stdin) != NULL){
            strbuf[strcspn(strbuf, "\r\n")] = 0;
            tmp->Vvalue.str = strdup(strbuf);
        }
        return tmp;
    }
    case STRSPLIT: {
        tmp = execute_ast(tree->children[0]);
        
        if (tmp->Vtype != Vstring) {
            printf("Invalid input for strsplit! Must be a string.\n");
            return tmp;
        }
        
        ast_value *tmp2 = calloc(sizeof *tmp2, 1);
        tmp2->Vtype = Vstring_arr;

        tmp2->Vvalue.str_arr = malloc(sizeof (*tmp2->Vvalue.str_arr));
        tmp2->Vvalue.str_arr->length = 0;
        char* str_copy = strdup(tmp->Vvalue.str);
        char* token = strtok(str_copy, " ");

        while (token) {
            (tmp2->Vvalue.str_arr->length)++;
            token = strtok(NULL, " ");
        }

        tmp2->Vvalue.str_arr->str_arr = malloc(tmp2->Vvalue.str_arr->length * sizeof(char*));

        strcpy(str_copy, tmp->Vvalue.str);
        token = strtok(str_copy, " ");
        for (int i = 0; i < tmp2->Vvalue.str_arr->length; i++) {
            tmp2->Vvalue.str_arr->str_arr[i] = strdup(token);
            token = strtok(NULL, " ");
        }

        free(str_copy);
        return tmp2;
    }
    case STRLEN: {
        tmp = execute_ast(tree->children[0]);

        if (tmp->Vtype != Vstring) {
            printf("Invalid input for strlen! Must be a string.\n");
            return tmp;
        }

        ast_value *tmp2 = calloc(sizeof *tmp2, 1);
        tmp2->Vtype = Vnumber;
        tmp2->Vvalue.num = strlen(tmp->Vvalue.str);
        return tmp2;
    }
    case ARRLEN: {
        tmp = execute_ast(tree->children[0]);
        ast_value *tmp2 = calloc(sizeof *tmp2, 1);
        tmp2->Vtype = Vnumber;
        tmp2->Vvalue.num = 0;

        if (tmp->Vtype == Vnumber_arr) {
            tmp2->Vvalue.num = tmp->Vvalue.num_arr->length;
        } else if (tmp->Vtype == Vstring_arr) {
            tmp2->Vvalue.num = tmp->Vvalue.str_arr->length;
        } else if (tmp->Vtype == Vboolean_arr) {
            tmp2->Vvalue.num = tmp->Vvalue.boo_arr->length;
        } else {
            printf("Invalid input for arrlen! Must be an array.\n");
        }

        return tmp2;
    }
    case VALUE: {
        return execute_ast(tree->children[0]);
    }
    case ACCESS: {
        tmp = var_get(execute_ast(tree->children[0])->Vvalue.id);
        return tmp;
    }
    case ACCESSARR: {
        tmp = execute_ast(tree->children[1]);
        ast_value *var = var_get(execute_ast(tree->children[0])->Vvalue.id);
        ast_value *tmp2 = calloc(sizeof *tmp, 1);
        tmp2->Vtype = Vstring;
        char buf[MAX_STR_LEN];
        sprintf(buf, "Invalid acces index on array: %s", execute_ast(tree->children[0])->Vvalue.id);
        tmp2->Vvalue.str= strdup(buf);

        if ((int)tmp->Vvalue.num < 0){
            return tmp2;
        }
        
        if (var->Vtype == Vnumber_arr) {
            if (var->Vvalue.num_arr->length > (int)tmp->Vvalue.num){
                tmp2->Vtype = Vnumber;
                tmp2->Vvalue.num = var->Vvalue.num_arr->num_arr[(int)tmp->Vvalue.num];
            }
        } else if (var->Vtype == Vstring_arr) {
            if (var->Vvalue.str_arr->length > (int)tmp->Vvalue.num){
                tmp2->Vtype = Vstring;
                tmp2->Vvalue.str = var->Vvalue.str_arr->str_arr[(int)tmp->Vvalue.num];
            }
        } else if (var->Vtype == Vboolean_arr) {
            if (var->Vvalue.boo_arr->length > (int)tmp->Vvalue.num){
                tmp2->Vtype = Vboolean;
                tmp2->Vvalue.boo = var->Vvalue.boo_arr->boo_arr[(int)tmp->Vvalue.num];
            }
        }
        return tmp2;
    }
    case ASSIGNARR: {
        tmp = execute_ast(tree->children[1]);
        ast_value *var = var_get(execute_ast(tree->children[0])->Vvalue.id);
    
        if ((int)tmp->Vvalue.num < 0){
            printf("Invalid acces index on array: %s", execute_ast(tree->children[0])->Vvalue.id);
            break;
        }

        if (var->Vtype == Vnumber_arr) {
            if (var->Vvalue.num_arr->length > (int)tmp->Vvalue.num){
                var->Vvalue.num_arr->num_arr[(int)tmp->Vvalue.num] = execute_ast(tree->children[2])->Vvalue.num;
                var_set(execute_ast(tree->children[0])->Vvalue.id, var);
                break;
            }
        } else if (var->Vtype == Vstring_arr) {
            if (var->Vvalue.str_arr->length > (int)tmp->Vvalue.num){
                var->Vvalue.str_arr->str_arr[(int)tmp->Vvalue.num] = execute_ast(tree->children[2])->Vvalue.str;
                var_set(execute_ast(tree->children[0])->Vvalue.id, var);
                break;
            }
        } else if (var->Vtype == Vboolean_arr) {
            if (var->Vvalue.boo_arr->length > (int)tmp->Vvalue.num){
                var->Vvalue.boo_arr->boo_arr[(int)tmp->Vvalue.num] = execute_ast(tree->children[2])->Vvalue.boo;
                var_set(execute_ast(tree->children[0])->Vvalue.id, var);
                break;
            }
        }
        printf("Invalid acces index on array: %s", execute_ast(tree->children[0])->Vvalue.id);
        break;
    } 
    case GASSIGNARR: {
        tmp = execute_ast(tree->children[1]);
        ast_value *var = var_get(execute_ast(tree->children[0])->Vvalue.id);
    
        if ((int)tmp->Vvalue.num < 0){
            printf("Invalid acces index on array: %s", execute_ast(tree->children[0])->Vvalue.id);
            break;
        }

        if (var->Vtype == Vnumber_arr) {
            if (var->Vvalue.num_arr->length > (int)tmp->Vvalue.num){
                var->Vvalue.num_arr->num_arr[(int)tmp->Vvalue.num] = execute_ast(tree->children[2])->Vvalue.num;
                var_declare_global(execute_ast(tree->children[0])->Vvalue.id, var);
                break;
            }
        } else if (var->Vtype == Vstring_arr) {
            if (var->Vvalue.str_arr->length > (int)tmp->Vvalue.num){
                var->Vvalue.str_arr->str_arr[(int)tmp->Vvalue.num] = execute_ast(tree->children[2])->Vvalue.str;
                var_declare_global(execute_ast(tree->children[0])->Vvalue.id, var);
                break;
            }
        } else if (var->Vtype == Vboolean_arr) {
            if (var->Vvalue.boo_arr->length > (int)tmp->Vvalue.num){
                var->Vvalue.boo_arr->boo_arr[(int)tmp->Vvalue.num] = execute_ast(tree->children[2])->Vvalue.boo;
                var_declare_global(execute_ast(tree->children[0])->Vvalue.id, var);
                break;
            }
        }
        printf("Invalid acces index on array: %s", execute_ast(tree->children[0])->Vvalue.id);
        break;
    }

    /* Terminals */
    case IDENTIFIER: {
        return &tree->value;
    }
    case NUMBER: {
        return &tree->value;
    }
    case STRING: {
        return &tree->value;
    }
    case OPERATOR: {
        return &tree->value;
    }
    case BOOLEAN: {
        return &tree->value;
    }
    case NUMARR: {
        return &tree->value;
    }
    case STRARR: {
        return &tree->value;
    }
    case BOOARR: {
        return &tree->value;
    }

    /* Calculations */
    case EQUALS: {
        ast_value *buf = calloc(sizeof *buf, 1);
        buf->Vtype = Voperator;
        buf->Vvalue.op = "equals";

        return perform_calculation(buf, execute_ast(tree->children[0]), execute_ast(tree->children[1]));
    }
    case ISNT: {
        ast_value *buf = calloc(sizeof *buf, 1);
        buf->Vtype = Voperator;
        buf->Vvalue.op = "isnt";

        return perform_calculation(buf, execute_ast(tree->children[0]), execute_ast(tree->children[1]));
    }
    case PLUS: {
        ast_value *buf = calloc(sizeof *buf, 1);
        buf->Vtype = Voperator;
        buf->Vvalue.op = "plus";

        return perform_calculation(buf, execute_ast(tree->children[0]), execute_ast(tree->children[1]));
    }
    case MINUS: {
        ast_value *buf = calloc(sizeof *buf, 1);
        buf->Vtype = Voperator;
        buf->Vvalue.op = "minus";

        return perform_calculation(buf, execute_ast(tree->children[0]), execute_ast(tree->children[1]));
    }
    case TIMES: {
        ast_value *buf = calloc(sizeof *buf, 1);
        buf->Vtype = Voperator;
        buf->Vvalue.op = "times";

        return perform_calculation(buf, execute_ast(tree->children[0]), execute_ast(tree->children[1]));
    }
    case DIVBY: {
        ast_value *buf = calloc(sizeof *buf, 1);
        buf->Vtype = Voperator;
        buf->Vvalue.op = "divby";

        return perform_calculation(buf, execute_ast(tree->children[0]), execute_ast(tree->children[1]));
    }
    case AND: {
        ast_value *buf = calloc(sizeof *buf, 1);
        buf->Vtype = Voperator;
        buf->Vvalue.op = "and";

        return perform_calculation(buf, execute_ast(tree->children[0]), execute_ast(tree->children[1]));
    }
    case OR: {
        ast_value *buf = calloc(sizeof *buf, 1);
        buf->Vtype = Voperator;
        buf->Vvalue.op = "or";

        return perform_calculation(buf, execute_ast(tree->children[0]), execute_ast(tree->children[1]));
    }
    case SMALLER: {
        ast_value *buf = calloc(sizeof *buf, 1);
        buf->Vtype = Voperator;
        buf->Vvalue.op = "smaller";

        return perform_calculation(buf, execute_ast(tree->children[0]), execute_ast(tree->children[1]));
    }
    case BIGGER: {
        ast_value *buf = calloc(sizeof *buf, 1);
        buf->Vtype = Voperator;
        buf->Vvalue.op = "bigger";

        return perform_calculation(buf, execute_ast(tree->children[0]), execute_ast(tree->children[1]));
    }
    case MODULO: {
        ast_value *buf = calloc(sizeof *buf, 1);
        buf->Vtype = Voperator;
        buf->Vvalue.op = "modulo";

        return perform_calculation(buf, execute_ast(tree->children[0]), execute_ast(tree->children[1]));
    }

    /* Default case */
    default: { break; }
    }

    ast_value *value = calloc(sizeof *value, 1);
    return value;
}

void optimize_ast(ast *tree){
    if (!tree) {
        return;
    }

    for (size_t i = 0; i < MAX_CHILDREN; i++) {
        optimize_ast(tree->children[i]);
    }

    ast_value *tmp = calloc(sizeof *tmp, 1);

    switch (tree->type)
    {
    /* Expressions */
    case EQUALS: {
        ast* v1 = tree->children[0]->children[0];
        ast* v2 = tree->children[1]->children[0];

        if (v1 != NULL && v2 != NULL && v1->type == NUMBER && v2->type == NUMBER) {
            tree->type = VALUE;
            tree->children[0] = NULL;
            tree->children[1] = NULL;

            tmp->Vtype = Vboolean;
            tmp->Vvalue.boo = v1->value.Vvalue.num == v2->value.Vvalue.num ? "true" : "false";
            
            ast* buf = node0(BOOLEAN);
            buf->value = *tmp;
            tree->children[0] = buf;
        }
        break;
    }
    case ISNT: {
        ast* v1 = tree->children[0]->children[0];
        ast* v2 = tree->children[1]->children[0];

        if (v1 != NULL && v2 != NULL && v1->type == NUMBER && v2->type == NUMBER) {
            tree->type = VALUE;
            tree->children[0] = NULL;
            tree->children[1] = NULL;

            tmp->Vtype = Vboolean;
            tmp->Vvalue.boo = v1->value.Vvalue.num == v2->value.Vvalue.num ? "true" : "false";
            
            ast* buf = node0(BOOLEAN);
            buf->value = *tmp;
            tree->children[0] = buf;
        }
        break;
    }
    case PLUS: {
        ast* v1 = tree->children[0]->children[0];
        ast* v2 = tree->children[1]->children[0];

        if (v1 != NULL && v2 != NULL && v1->type == NUMBER && v2->type == NUMBER) {
            tree->type = VALUE;
            tree->children[0] = NULL;
            tree->children[1] = NULL;

            tmp->Vtype = Vnumber;
            tmp->Vvalue.num = v1->value.Vvalue.num + v2->value.Vvalue.num;

            ast* buf = node0(NUMBER);
            buf->value = *tmp;
            tree->children[0] = buf;
        }
        break;
    }
    case MINUS: {
        ast* v1 = tree->children[0]->children[0];
        ast* v2 = tree->children[1]->children[0];

        if (v1 != NULL && v2 != NULL && v1->type == NUMBER && v2->type == NUMBER) {
            tree->type = VALUE;
            tree->children[0] = NULL;
            tree->children[1] = NULL;

            tmp->Vtype = Vnumber;
            tmp->Vvalue.num = v1->value.Vvalue.num - v2->value.Vvalue.num;
            
            ast* buf = node0(NUMBER);
            buf->value = *tmp;
            tree->children[0] = buf;
        }
        break;
    }
    case TIMES: {
        ast* v1 = tree->children[0]->children[0];
        ast* v2 = tree->children[1]->children[0];

        if (v1 != NULL && v2 != NULL && v1->type == NUMBER && v2->type == NUMBER) {
            tree->type = VALUE;
            tree->children[0] = NULL;
            tree->children[1] = NULL;

            tmp->Vtype = Vnumber;
            tmp->Vvalue.num = v1->value.Vvalue.num * v2->value.Vvalue.num;
            
            ast* buf = node0(NUMBER);
            buf->value = *tmp;
            tree->children[0] = buf;
        }
        break;
    }
    case DIVBY: {
        ast* v1 = tree->children[0]->children[0];
        ast* v2 = tree->children[1]->children[0];

        if (v1 != NULL && v2 != NULL && v1->type == NUMBER && v2->type == NUMBER) {
            tree->type = VALUE;
            tree->children[0] = NULL;
            tree->children[1] = NULL;

            tmp->Vtype = Vnumber;
            tmp->Vvalue.num = v1->value.Vvalue.num / v2->value.Vvalue.num;
            
            ast* buf = node0(NUMBER);
            buf->value = *tmp;
            tree->children[0] = buf;
        }
        break;
    }
    case AND: {
        ast* v1 = tree->children[0]->children[0];
        ast* v2 = tree->children[1]->children[0];

        if (v1 != NULL && v2 != NULL && v1->type == BOOLEAN && v2->type == BOOLEAN) {
            tree->type = VALUE;
            tree->children[0] = NULL;
            tree->children[1] = NULL;

            tmp->Vtype = Vboolean;
            tmp->Vvalue.boo = (strncmp(v1->value.Vvalue.boo, "true", 5) == 0 && strncmp(v2->value.Vvalue.boo, "true", 5) == 0) ? "true" : "false";
            
            ast* buf = node0(BOOLEAN);
            buf->value = *tmp;
            tree->children[0] = buf;
        }
        break;
    }
    case OR: {
        ast* v1 = tree->children[0]->children[0];
        ast* v2 = tree->children[1]->children[0];

        if (v1 != NULL && v2 != NULL && v1->type == BOOLEAN && v2->type == BOOLEAN) {
            tree->type = VALUE;
            tree->children[0] = NULL;
            tree->children[1] = NULL;

            tmp->Vtype = Vboolean;
            tmp->Vvalue.boo = (strncmp(v1->value.Vvalue.boo, "true", 5) == 0 || strncmp(v2->value.Vvalue.boo, "true", 5) == 0) ? "true" : "false";
            
            ast* buf = node0(BOOLEAN);
            buf->value = *tmp;
            tree->children[0] = buf;
        }
        break;
    }
    case SMALLER: {
        ast* v1 = tree->children[0]->children[0];
        ast* v2 = tree->children[1]->children[0];

        if (v1 != NULL && v2 != NULL && v1->type == NUMBER && v2->type == NUMBER) {
            tree->type = VALUE;
            tree->children[0] = NULL;
            tree->children[1] = NULL;

            tmp->Vtype = Vboolean;
            tmp->Vvalue.boo = v1->value.Vvalue.num < v2->value.Vvalue.num ? "true" : "false";
            
            ast* buf = node0(BOOLEAN);
            buf->value = *tmp;
            tree->children[0] = buf;
        }
        break;
    }
    case BIGGER: {
        ast* v1 = tree->children[0]->children[0];
        ast* v2 = tree->children[1]->children[0];

        if (v1 != NULL && v2 != NULL && v1->type == NUMBER && v2->type == NUMBER) {
            tree->type = VALUE;
            tree->children[0] = NULL;
            tree->children[1] = NULL;

            tmp->Vtype = Vboolean;
            tmp->Vvalue.boo = v1->value.Vvalue.num > v2->value.Vvalue.num ? "true" : "false";
            
            ast* buf = node0(BOOLEAN);
            buf->value = *tmp;
            tree->children[0] = buf;
        }
        break;
    }

    /* Dead Code Elimination */
    case CONDITION: {
        ast* v1 = tree->children[0]->children[0];

        if (v1 != NULL && v1->type == BOOLEAN) {
            if (strncmp(v1->value.Vvalue.boo, "true", 4) == 0) {
                memcpy(tree, tree->children[1], sizeof(*tree));
            } else if(strncmp(v1->value.Vvalue.boo, "false", 5) == 0){
                memcpy(tree, tree->children[2], sizeof(*tree));
            }
        } else if (v1 != NULL && v1->type == NUMBER) {
            if (v1->value.Vvalue.num != 0) {
                memcpy(tree, tree->children[1], sizeof(*tree));
            } else {
                memcpy(tree, tree->children[2], sizeof(*tree));
            }
        }
        break;
    }
    case CHECK: {
        ast* v1 = tree->children[0]->children[0];

        if (v1 != NULL && v1->type == BOOLEAN) {
            if (strncmp(v1->value.Vvalue.boo, "true", 4) == 0) {
                memcpy(tree, tree->children[1], sizeof(*tree));
            } else if(strncmp(v1->value.Vvalue.boo, "false", 5) == 0){
                ast* buf = node0(NUMBER);
                buf->value = (ast_value) { .Vtype = Vnumber, .Vvalue.num = 0 };
                ast* buf2 = node1(VALUE, buf);
                memcpy(tree, buf2, sizeof(*tree));
            }
        } else if (v1 != NULL && v1->type == NUMBER) {
            if (v1->value.Vvalue.num != 0) {
                memcpy(tree, tree->children[1], sizeof(*tree));
            } else {
                ast* buf = node0(NUMBER);
                buf->value = (ast_value) { .Vtype = Vnumber, .Vvalue.num = 0 };
                ast* buf2 = node1(VALUE, buf);
                memcpy(tree, buf2, sizeof(*tree));
            }
        }
        break;
    }
    case LOOP: {
        ast* v1 = tree->children[0]->children[0];

        if ((v1 != NULL && v1->type == BOOLEAN && strncmp(v1->value.Vvalue.boo, "false", 5) == 0 ) ||
            (v1 != NULL && v1->type == NUMBER && v1->value.Vvalue.num == 0)) {
            ast* buf = node0(NUMBER);
            buf->value = (ast_value) { .Vtype = Vnumber, .Vvalue.num = 0 };
            ast* buf2 = node1(VALUE, buf);
            memcpy(tree, buf2, sizeof(*tree));   
        }
        break;
    }

    default:
        break;
    }
}
