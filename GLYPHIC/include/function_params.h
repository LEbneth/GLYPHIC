#ifndef FUNCTION_PARAMS_H
#define FUNCTION_PARAMS_H

#include "ast.h"

/**
 * A struct representing a queue of parameters that are being passed to a function.
 */
typedef struct{
    ast_value *values;
    int size;
} parameter_queue;

/**
 * Enqueues a parameter to the queue.
 */
void param_set(ast_value *value);

/**
 * Dequeues a parameter from the queue.
 */
ast_value *param_get();

#endif