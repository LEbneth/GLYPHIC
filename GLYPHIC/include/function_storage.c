#include "function_storage.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

static f_store fstore;

void f_push (f_store *stack, function val) {
  stack->functions = realloc(stack->functions, (stack->size + 1) * sizeof (function));
  assert(stack->functions != NULL);
  stack->functions[stack->size++] = val;
}

static function* func_lookup(char* name) {
    for (int i = fstore.size-1; i >= 0; i--) {
        if (strcmp(fstore.functions[i].name, name) == 0) {
            return &fstore.functions[i];
        }
    }
    return NULL;
}

ast* func_declare(char* name, ast* start) {
    function* tmp = func_lookup(name);
    if (tmp) {
        tmp->start = start;
    }
    else {
        f_push(&fstore, (function) { .name = strdup(name), .start = start});
    }

    return start;
}

ast* func_set(char* name, ast* start) {
    function* tmp = func_lookup(name);
    if (tmp) {
        tmp->start = start;
    }
    else {
        func_declare(name, start);
    }

    return start;
}

ast* func_get(char* name) {
    function* tmp = func_lookup(name);
    if (tmp) {
        return tmp->start;
    }
    else {
        ast* tmp = calloc(sizeof *tmp, 1);
        func_declare (name, tmp);
        return tmp;
    }
}