#ifndef FUNCTION_STORAGE_H
#define FUNCTION_STORAGE_H

#include "ast.h"

/**
 * A struct representing a function with its name and start node in the tree.
 */
typedef struct {
  char *name;
  ast *start;
} function;

/**
 * A struct representing a store of all functions declared in the program.
 */
typedef struct {
  function *functions;
  int size;
} f_store;

/**
 * Adds a function to the store.
 */
ast* func_set(char* name, ast* start);

/**
 * Finds a function in the store.
 * If the function is not found, it is declared.
 */
ast* func_get(char* name);
#endif