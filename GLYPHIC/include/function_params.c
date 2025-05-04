#include "function_params.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

static parameter_queue queue;

void param_set(ast_value* value){
    queue.values = realloc(queue.values, (queue.size + 1) * sizeof(ast_value));
    assert(queue.values != NULL);
    queue.values[queue.size++] = (ast_value) {.Vtype = value->Vtype, .Vvalue=value->Vvalue};
}

ast_value *param_get(){
    if (queue.size <= 0){
        return NULL;
    }

    ast_value *tmp = malloc(sizeof(ast_value));
    *tmp = queue.values[0];

    for (int i = 1; i < queue.size; i++) {
        queue.values[i - 1] = queue.values[i];
    }

    queue.size--;
    queue.values = realloc(queue.values, queue.size * sizeof(ast_value));

    return tmp;
}
