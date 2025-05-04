#ifndef AST_H
#define AST_H

#define MAX_CHILDREN 3
/**
 * A struct representing a dynamic array of double numbers.
 */
typedef struct number_arr_t{
    double *num_arr;
    int length;
} number_arr;

/**
 * A struct representing a dynamic array of strings.
 */
typedef struct string_arr_t{
    char **str_arr;
    int length;
} string_arr;

/**
 * A struct representing a dynamic array of booleans.
 */
typedef struct boolean_arr_t{
    char **boo_arr;
    int length;
} boolean_arr;

/**
 * A struct attached to a value representing the type of the value.
 */
typedef enum {
    Vnumber = 0,
    Vstring = 1,
    Videntifier = 2,
    Voperator = 3,
    Vboolean = 4,
    Vnumber_arr = 5,
    Vstring_arr = 6,
    Vboolean_arr = 7
} ast_value_type;

/**
 * A struct representing a value in the AST.
 */
typedef struct {
    ast_value_type Vtype;
    union {
        double num;
        char *str;
        char *id;
        char *op;
        char *boo;
        number_arr *num_arr;
        string_arr *str_arr;
        boolean_arr *boo_arr;
    } Vvalue;
} ast_value;

/**
 * A struct representing a node in the AST.
 */
typedef struct ast_t {
	int type;
    int node_id;
    ast_value value;
	struct ast_t *children [MAX_CHILDREN];
} ast;

/**
 * An enum representing the different types of custom-rulebased-nodes in the AST.
 */
enum {
    GLYPHIC = 10000,
    BLOCK,
    GSTATEMENT,
    STATEMENT,
    CALCULATION,
    CALL,
    FUNCNAME,
    PARAMCALL,
    FUNCTION,
    DECLARATION,
    IMPLEMENTATION,
    PARAMIMPL,
    BODY,
    RETURN,
    STRUCTURE,
    LOOP,
    CONDITION,
    CHECK,
    CSTATEMENT,
    EVAL,
    PRINT,
    TEXTOBJECTS,
    RANDOM,
    READNUM,
    READSTR,
    STRSPLIT,
    STRLEN,
    ARRLEN,
    VALUE,
    ACCESS,
    ACCESSARR,
    ASSIGNARR,
    GASSIGNARR,
    IDENTIFIER,
    NUMBER,
    STRING,
    OPERATOR,
    BOOLEAN,
    NUMARR,
    STRARR,
    BOOARR,

    // Custom nodes
    EQUALS,
    ISNT,
    PLUS,
    MINUS,
    TIMES,
    DIVBY,
    AND,
    OR,
    SMALLER,
    BIGGER,
    MODULO
};
/**
 * The enum values of the lexer tokens for reference.
 */
/*  
    _equals = 258
    _isnt = 259
    _plus = 260
    _minus = 261
    _times = 262
    _divby = 263
    _and = 264
    _or = 265
    _bigger = 266
    _smaller = 267
    _modulo = 268
    _declare = 269
    _define = 270
    _give = 271
    _end = 272
    _repeat = 273
    _if = 274
    _do = 275
    _otherwise = 276
    _print = 277
    _random = 278
    _readnum = 279
    _readstr = 280
    _strsplit = 281
    _strlen = 282
    _arrlen = 283

    identifier = 284
    number = 285
    string = 286
    boolean = 287
    num_arr = 288
    str_arr = 289
    boo_arr = 290
*/

/**
 * Functions to create a new AST node with given number of children.
 */
ast *node0(int type);

ast *node1(int type, ast *child1);

ast *node2(int type, ast *child1, ast *child2);

ast *node3(int type, ast *child1, ast *child2, ast *child3);

/**
 * Maps the enum value to a string representation for pretty printing in the tree output.
 */
char* map_enum_to_string(int state);

/**
 * Converts a node to a string representation for the tree output.
 */
char *node2str(ast *tree);

/**
 * Prints the AST to file "<filename>.gv" in dot format.
 * Compile to PNG using:
 * dot <filename>.gv -Tpng -o <filename>.png
 */
void print_ast (ast *tree, int depth, char* filename);

/**
 * Performs a calculation based on the operator and the two values.
 */
ast_value *perform_calculation(ast_value* op, ast_value* v1, ast_value* v2);

/**
 * Performs a statement.
 * 
 * If the operator is "equals" the value of the identifier is set to the value of the right-hand-side.
 * Otherwise a calculation is performed.
 */
ast_value *perform_statement(ast_value* op, ast_value* id, ast_value* rhs);

/**
 * Main execution function of the AST.
 */
ast_value *execute_ast (ast *tree);

/**
 * Optimizes the AST tree before executing.
 */
void optimize_ast ( ast *tree);

#endif