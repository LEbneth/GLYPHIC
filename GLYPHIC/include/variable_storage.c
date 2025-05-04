#include "variable_storage.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

static stack vars, globals;

void s_push (stack *stack, stackval val) {
  stack->vals = realloc(stack->vals, (stack->size + 1) * sizeof (stackval));
  assert(stack->vals != NULL);
  stack->vals[stack->size++] = val;
}

#define VAR_BORDER_FUNC 2
#define VAR_BORDER_BLOCK 1

static stackval *var_lookup (char *id, int border) {
  for (int i = vars.size-1; i >= 0; i--) {
    if (vars.vals[i].flags >= border)
      break;
    if (strcmp(vars.vals[i].id, id) == 0)
      return &vars.vals[i];
  }

  if (border == VAR_BORDER_BLOCK)
    return NULL;

  for (int i = globals.size-1; i >= 0; i--) {
    if (strcmp(globals.vals[i].id, id) == 0)
      return &globals.vals[i];
  }

  return NULL;
}

ast_value *var_declare_global (char *id, ast_value *val) {
  stackval *s = var_lookup (id, 0);
  if (s) {
    // Handle multiple declaration in same block
    // Here: Just ignore the new declaration, set new value
    s->val = val;
  } else {
    s_push(&globals, (stackval) { .val = val, .id = strdup(id) });
  }

  return val;
}

ast_value *var_declare (char *id, ast_value *val) {
  stackval *s = var_lookup (id, VAR_BORDER_BLOCK);
  if (s) {
    // Handle multiple declaration in same block
    // Here: Just ignore the new declaration, set new value
    s->val = val;
  } else {
    s_push(&vars, (stackval) { .val = val, .id = strdup(id) });
  }

  return val;
}

ast_value *var_set (char *id, ast_value *val) {
  stackval *s = var_lookup (id, VAR_BORDER_FUNC);
  if (s)
    s->val = val;
  else {
    // Handle usage of undeclared variable
    // Here: implicitly declare variable
    var_declare(id, val);
  }

  return val;
}

ast_value *var_get (char *id) {
  stackval *s = var_lookup (id, VAR_BORDER_FUNC);
  if (s)
    return s->val;
  else {
    printf("Error: variable %s not declared\n", id);
    // Handle usage of undeclared variable
    // Here: implicitly declare variable
    ast_value *tmp = calloc(sizeof *tmp, 1);
    var_declare(id, tmp);
    return tmp;
  }
}

void var_enter_block (void) {
  s_push(&vars, (stackval) { .flags = VAR_BORDER_BLOCK, .id = "" });
}

void var_leave_block (void) {
  int i;
  for (i = vars.size-1; i >= 0; i--) {
    if (vars.vals[i].flags == VAR_BORDER_BLOCK)
      break;
  }
  vars.size = i;
}

void var_enter_function (void) {
  s_push(&vars, (stackval) { .flags = VAR_BORDER_FUNC, .id = "" });
}

void var_leave_function (void) {
  int i;
  for (i = vars.size-1; i >= 0; i--) {
    if (vars.vals[i].flags == VAR_BORDER_FUNC)
      break;
  }
  vars.size = i;
}

void var_dump (void) {
  printf("-- TOP --\n");
  for (int i = vars.size-1; i >= 0; i--) {
    if (vars.vals[i].flags == VAR_BORDER_FUNC) {
      printf("FUNCTION\n");
    } else if (vars.vals[i].flags == VAR_BORDER_BLOCK) {
      printf("BLOCK\n");
    } else {
      printf("%s : %d\n", vars.vals[i].id, vars.vals[i].val->Vtype);
    }
  }
  printf("-- BOTTOM --\n");
  for (int i = globals.size-1; i >= 0; i--) {
      printf("%s : %d (global)\n", globals.vals[i].id, globals.vals[i].val->Vtype);
  }
  printf("-- GLOBALS --\n\n");
}
