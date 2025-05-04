#ifndef VARIBLE_STORAGE_H
#define VARIBLE_STORAGE_H

#include "ast.h"

/**
 * A struct representing a variable in the program.
 */
typedef struct {
  char *id;
  ast_value *val;
  int flags;
} stackval;

/**
 * A struct representing a stack of variables.
 */
typedef struct {
  stackval *vals;
  int size;
} stack;

ast_value *var_declare_global (char *id, ast_value *val);
ast_value *var_declare (char *id, ast_value *val);
ast_value *var_set (char *id, ast_value *val);
ast_value *var_get (char *id);
void var_enter_block (void);
void var_leave_block (void);
void var_enter_function (void);
void var_leave_function (void);
void var_dump (void);

#endif
