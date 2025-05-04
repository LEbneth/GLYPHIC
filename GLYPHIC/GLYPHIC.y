%{
    #include <stdio.h>
    #include <string.h>

    // path adjusted as *.tab.c and *.tab.h are compiled to ./build
    #include "../include/ast.h"

    extern FILE *yyin;
    extern int yylineno;
    
    int yylex(void);

    void yyerror (const char *msg) {
        printf("Parse error at line: %d: %s\n",yylineno, msg);
    }

    int yydebug = 0;
%}

%union {
    char *u_identifier;
    double u_number;
    char *u_string;
    char *u_boolean;
    struct number_arr_t *u_num_arr;
    struct string_arr_t *u_str_arr;
    struct boolean_arr_t *u_boo_arr;
    struct ast_t *ast;
}

%define parse.error detailed

%token _equals _isnt _plus _minus _times _divby _and _or _bigger _smaller _modulo _declare _define _give _end _repeat _if _do _otherwise _print _random _readnum _readstr _strsplit _strlen _arrlen
        <u_identifier> identifier
        <u_number> number
        <u_string> string
        <u_boolean> boolean
        <u_num_arr> num_arr
        <u_str_arr> str_arr
        <u_boo_arr> boo_arr

%type   <ast> GLYPHIC BLOCK GSTATEMENT STATEMENT CALCULATION CALL FUNCNAME PARAMCALL FUNCTION DECLARATION IMPLEMENTATION PARAMIMPL BODY RETURN STRUCTURE LOOP CONDITION CHECK CSTATEMENT EVAL PRINT TEXTOBJECTS RANDOM READNUM READSTR STRSPLIT STRLEN ARRLEN VALUE ACCESS ACCESSARR ASSIGNARR GASSIGNARR IDENTIFIER NUMBER STRING OPERATOR BOOLEAN NUMARR STRARR BOOARR

%right _equals _isnt
%left _bigger _smaller
%left _plus _minus
%left _times _divby _modulo
%right _and _or

%start START
%%

START: GLYPHIC { print_ast($1, 0, "unoptimized"); optimize_ast($1); print_ast($1, 0, "optimized"); execute_ast($1); }

GLYPHIC:
    GLYPHIC BLOCK { $$ = node2(GLYPHIC, $1, $2); }
    | %empty { $$ = NULL; }

BLOCK:
    GSTATEMENT { $$ = node1(BLOCK, $1); }
    | CALL { $$ = node1(BLOCK, $1); }
    | FUNCTION { $$ = node1(BLOCK, $1); }
    | STRUCTURE { $$ = node1(BLOCK, $1); }
    | PRINT { $$ = node1(BLOCK, $1); }
    | GASSIGNARR { $$ = node1(BLOCK, $1); }

GSTATEMENT:
    IDENTIFIER OPERATOR CALCULATION { $$ = node3(GSTATEMENT, $1, $2, $3); }

STATEMENT:
    IDENTIFIER OPERATOR CALCULATION { $$ = node3(STATEMENT, $1, $2, $3); }

CALCULATION:
    CALCULATION _plus CALCULATION { $$ = node2(PLUS, $1, $3); }
    | CALCULATION _minus CALCULATION { $$ = node2(MINUS, $1, $3); }
    | CALCULATION _times CALCULATION { $$ = node2(TIMES, $1, $3); }
    | CALCULATION _divby CALCULATION { $$ = node2(DIVBY, $1, $3); }
    | CALCULATION _and CALCULATION { $$ = node2(AND, $1, $3); }
    | CALCULATION _or CALCULATION { $$ = node2(OR, $1, $3); }
    | CALCULATION _smaller CALCULATION { $$ = node2(SMALLER, $1, $3); }
    | CALCULATION _bigger CALCULATION { $$ = node2(BIGGER, $1, $3); }
    | CALCULATION _modulo CALCULATION { $$ = node2(MODULO, $1, $3); }
    | VALUE

CALL:
    FUNCNAME '(' PARAMCALL ')' { $$ = node2(CALL, $1, $3); }

FUNCNAME:
    '&' IDENTIFIER { $$ = node1(FUNCNAME, $2); }

PARAMCALL:
    PARAMCALL VALUE { $$ = node2(PARAMCALL, $1, $2); }
    | %empty { $$ = NULL; }

FUNCTION:
    DECLARATION IMPLEMENTATION { $$ = node2(FUNCTION, $1, $2); }

DECLARATION:
    _declare IDENTIFIER ':' { $$ = node1(DECLARATION, $2); }

IMPLEMENTATION:
    '(' PARAMIMPL ')' _define BODY RETURN _end { $$ = node3(IMPLEMENTATION, $2, $5, $6); }

PARAMIMPL:
    PARAMIMPL IDENTIFIER { $$ = node2(PARAMIMPL, $1, $2); }
    | %empty { $$ = NULL; }

BODY:
    BODY STATEMENT { $$ = node2(BODY, $1, $2); }
    | BODY PRINT { $$ = node2(BODY, $1, $2); }
    | BODY RANDOM { $$ = node2(BODY, $1, $2); }
    | BODY READNUM { $$ = node2(BODY, $1, $2); }
    | BODY READSTR { $$ = node2(BODY, $1, $2); }
    | BODY STRSPLIT { $$ = node2(BODY, $1, $2); }
    | BODY CALL { $$ = node2(BODY, $1, $2); }
    | BODY STRUCTURE { $$ = node2(BODY, $1, $2); }
    | BODY ASSIGNARR { $$ = node2(BODY, $1, $2); }
    | %empty { $$ = NULL; }

RETURN:
    _give VALUE { $$ = node1(RETURN, $2); }

STRUCTURE:
    LOOP { $$ = node1(STRUCTURE, $1); }
    | CONDITION { $$ = node1(STRUCTURE, $1); }
    | CHECK { $$ = node1(STRUCTURE, $1); }

LOOP:
    _repeat ':' '<' CSTATEMENT '>' _do BODY _end { $$ = node2(LOOP, $4, $7); }

CONDITION:
    _if ':' '<' CSTATEMENT '>' BODY _otherwise ':' BODY _end { $$ = node3(CONDITION, $4, $6, $9); }

CHECK:
    _if ':' '<' CSTATEMENT '>' BODY _end { $$ = node2(CHECK, $4, $6); }

CSTATEMENT:
    CSTATEMENT _and CSTATEMENT { $$ = node2(AND, $1, $3); }
    | CSTATEMENT _or CSTATEMENT { $$ = node2(OR, $1, $3); }
    | EVAL

EVAL:
    EVAL _equals EVAL { $$ = node2(EQUALS, $1, $3); }
    | EVAL _isnt EVAL { $$ = node2(ISNT, $1, $3); }
    | EVAL _plus EVAL { $$ = node2(PLUS, $1, $3); }
    | EVAL _minus EVAL { $$ = node2(MINUS, $1, $3); }
    | EVAL _times EVAL { $$ = node2(TIMES, $1, $3); }
    | EVAL _divby EVAL { $$ = node2(DIVBY, $1, $3); }
    | EVAL _smaller EVAL { $$ = node2(SMALLER, $1, $3); }
    | EVAL _bigger EVAL { $$ = node2(BIGGER, $1, $3); }
    | EVAL _modulo EVAL { $$ = node2(MODULO, $1, $3); }
    | VALUE

PRINT:
    _print '(' TEXTOBJECTS ')' { $$ = node1(PRINT, $3); }

TEXTOBJECTS:
    TEXTOBJECTS VALUE { $$ = node2(TEXTOBJECTS, $1, $2); }
    | %empty { $$ = NULL; }

RANDOM:
    _random '(' NUMBER NUMBER ')' { $$ = node2(RANDOM, $3, $4); }

READNUM:
    _readnum '(' ')' { $$ = node0(READNUM); }

READSTR:
    _readstr '(' ')' { $$ = node0(READSTR); }

STRSPLIT:
    _strsplit '(' VALUE ')' { $$ = node1(STRSPLIT, $3); }

STRLEN:
    _strlen '(' VALUE ')' { $$ = node1(STRLEN, $3); }

ARRLEN:
    _arrlen '(' VALUE ')' { $$ = node1(ARRLEN, $3); }

VALUE:
    NUMBER { $$ = node1(VALUE, $1); }
    | BOOLEAN { $$ = node1(VALUE, $1); }
    | STRING { $$ = node1(VALUE, $1); }
    | ACCESS { $$ = node1(VALUE, $1); }
    | ACCESSARR { $$ = node1(VALUE, $1); }
    | CALL  { $$ = node1(VALUE, $1); }
    | RANDOM  { $$ = node1(VALUE, $1); }
    | READNUM  { $$ = node1(VALUE, $1); }
    | READSTR  { $$ = node1(VALUE, $1); }
    | STRSPLIT { $$ = node1(VALUE, $1); }
    | STRLEN { $$ = node1(VALUE, $1); }
    | ARRLEN { $$ = node1(VALUE, $1); }
    | NUMARR { $$ = node1(VALUE, $1); }
    | STRARR { $$ = node1(VALUE, $1); }
    | BOOARR { $$ = node1(VALUE, $1); }

ACCESS:
    '~' IDENTIFIER { $$ = node1(ACCESS, $2); }

ACCESSARR:
    '~' IDENTIFIER '(' VALUE ')' { $$ = node2(ACCESSARR, $2, $4); }

ASSIGNARR:
    IDENTIFIER '(' VALUE ')' _equals VALUE { $$ = node3(ASSIGNARR, $1, $3, $6); }

GASSIGNARR:
    IDENTIFIER '(' VALUE ')' _equals VALUE { $$ = node3(GASSIGNARR, $1, $3, $6); }

IDENTIFIER:
    identifier { $$ = node0(IDENTIFIER); $$->value.Vtype = Videntifier; $$->value.Vvalue.id = $1; }

NUMBER:
    number { $$ = node0(NUMBER); $$->value.Vtype = Vnumber; $$->value.Vvalue.num = $1; }

STRING:
    string { $$ = node0(STRING); $$->value.Vtype = Vstring; $$->value.Vvalue.str = $1; }

OPERATOR:
    _equals { $$ = node0(OPERATOR); $$->value.Vtype = Voperator; $$->value.Vvalue.op = "equals"; }
    | _isnt { $$ = node0(OPERATOR); $$->value.Vtype = Voperator; $$->value.Vvalue.op = "isnt"; }
    | _plus { $$ = node0(OPERATOR); $$->value.Vtype = Voperator; $$->value.Vvalue.op = "plus"; }
    | _minus { $$ = node0(OPERATOR); $$->value.Vtype = Voperator; $$->value.Vvalue.op = "minus"; }
    | _times { $$ = node0(OPERATOR); $$->value.Vtype = Voperator; $$->value.Vvalue.op = "times"; }
    | _divby { $$ = node0(OPERATOR); $$->value.Vtype = Voperator; $$->value.Vvalue.op = "divby"; }
    | _and { $$ = node0(OPERATOR); $$->value.Vtype = Voperator; $$->value.Vvalue.op = "and"; }
    | _or { $$ = node0(OPERATOR); $$->value.Vtype = Voperator; $$->value.Vvalue.op = "or"; }
    | _bigger { $$ = node0(OPERATOR); $$->value.Vtype = Voperator; $$->value.Vvalue.op = "bigger"; }
    | _smaller { $$ = node0(OPERATOR); $$->value.Vtype = Voperator; $$->value.Vvalue.op = "smaller"; }
    | _modulo { $$ = node0(OPERATOR); $$->value.Vtype = Voperator; $$->value.Vvalue.op = "modulo"; }


BOOLEAN:
    boolean { $$ = node0(BOOLEAN); $$->value.Vtype = Vboolean; $$->value.Vvalue.boo = $1; }

NUMARR:
    num_arr { $$ = node0(NUMARR); $$->value.Vtype = Vnumber_arr; $$->value.Vvalue.num_arr = $1; }

STRARR:
    str_arr { $$ = node0(STRARR); $$->value.Vtype = Vstring_arr; $$->value.Vvalue.str_arr = $1; }

BOOARR:
    boo_arr { $$ = node0(BOOARR); $$->value.Vtype = Vboolean_arr; $$->value.Vvalue.boo_arr = $1; }

%%

int main (int argc, char **argv) {
    if (argc==2) { 
        yyin = fopen(argv[1],"r");
    } else { 
        yyin = stdin;
    }

    int result = yyparse();

    printf("\n\nInput is %s GLYPHIC code!\n", result == 0 ? "valid" : "NOT valid");
    printf("Process returned %d\n", result);
    
    return result;
}