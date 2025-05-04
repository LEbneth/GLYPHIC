%{
    // GLYPHIC //
    
    #include <stdlib.h>
    #include "GLYPHIC.tab.h"
    #include "ast.h"

    extern int yylineno;

    char *stringBuffer;

    void append_to_buffer(char character) {
        int len = strlen(stringBuffer);
        stringBuffer = realloc(stringBuffer, len + 2);
        stringBuffer[len] = character;
        stringBuffer[++len] = '\0';
    }

    int inArray = 0;

    number_arr *num_arr_tmp;
    string_arr *str_arr_tmp;
    boolean_arr *boo_arr_tmp;

    void array_start() {
        inArray = 1;

        num_arr_tmp = malloc(sizeof (struct number_arr*));
        num_arr_tmp->num_arr = malloc(sizeof (double*));
        num_arr_tmp->length = 0;
        
        str_arr_tmp = malloc(sizeof (struct string_arr*));
        str_arr_tmp->str_arr = malloc(sizeof (char*));
        str_arr_tmp->length = 0;
        
        boo_arr_tmp = malloc(sizeof (struct boolean_arr*));
        boo_arr_tmp->boo_arr = malloc(sizeof (char*));
        boo_arr_tmp->length = 0;
    }

    int array_end() {
        inArray = 0;

        if (num_arr_tmp->length > 0) {
            yylval.u_num_arr = num_arr_tmp;
            return num_arr;
        } else if (str_arr_tmp->length > 0) {
            yylval.u_str_arr = str_arr_tmp;
            return str_arr;
        } else if (boo_arr_tmp->length > 0) {
            yylval.u_boo_arr = boo_arr_tmp;
            return boo_arr;
        }
        return -1;
    }
%}
%option stack noyywrap noinput nounput noyy_top_state
%x S_STRING
%x S_LINE_COMMENT
%x S_BLOCK_COMMENT

R_IDENTIFIER \[([^\[\]]+)\]

R_DECIAML [+-]?[0-9]*\.?[0-9]+

R_RADIX [0-9a-xA-X]+r[0-9]+

R_UNARY u#+

R_EQUALS "equals"
R_ISNT "isnt"
R_PLUS "plus"
R_MINUS "minus"
R_TIMES "times"
R_DIVBY "divby"
R_AND "and"
R_OR "or"
R_BIGGER "bigger"
R_SMALLER "smaller"
R_MODULO "modulo"

R_BOOL (true|false)

R_DECLARE "->"
R_DEFINE "=>"
R_GIVE "give"
R_END "end"
R_REPEAT "repeat"
R_IF "if"
R_DO "do"
R_OTHERWISE "otherwise"
R_PRINT "&[print]"
R_READNUM "&[readnum]"
R_READSTR "&[readstr]"
R_RANDOM "&[random]"
R_STRSPLIT "&[strsplit]"
R_STRLEN "&[strlen]"
R_ARRLEN "&[arrlen]"
R_ARR_START [{]
R_ARR_END [}]

R_SYMBOL (&|~|\(|\))

R_WHITESPACE [ \t]+

R_NEWLINE \n

R_UNRECOGNIZED .

%%

<*>"~{" {yy_push_state(YYSTATE); BEGIN(S_BLOCK_COMMENT);}
<S_BLOCK_COMMENT>. {/*Do nothing*/}
<S_BLOCK_COMMENT>\n { yylineno++; }
<S_BLOCK_COMMENT>"}~" {yy_pop_state();}

<*>"~|" {yy_push_state(YYSTATE); BEGIN(S_LINE_COMMENT);}
<S_LINE_COMMENT>. {/*Do nothing*/}
<S_LINE_COMMENT>"\n" { yylineno++; yy_pop_state();}

"<<" {
    yy_push_state(YYSTATE); BEGIN(S_STRING);
    stringBuffer = malloc(1);
    stringBuffer[0] = '\0';
}
<S_STRING>">>" {
    yy_pop_state();
    if(inArray){
        str_arr_tmp->str_arr = realloc(str_arr_tmp->str_arr, (str_arr_tmp->length + 1) * sizeof(char*));
        str_arr_tmp->str_arr[str_arr_tmp->length++] = strdup(stringBuffer);
    } else{
        yylval.u_string = strdup(stringBuffer);
        return string;
    }
}
<S_STRING>"~n" {append_to_buffer('\n');}
<S_STRING>"~t" {append_to_buffer('\t');}
<S_STRING>~x[0-9A-Fa-f]{2} {
    char hex[3] = { yytext[2], yytext[3], '\0' };
    append_to_buffer(strtol(hex, NULL, 16));
}
<S_STRING>"~<" {append_to_buffer('<');}
<S_STRING>"~>" {append_to_buffer('>');}
<S_STRING>. {append_to_buffer(*yytext);}

{R_IDENTIFIER} {
    yylval.u_identifier = strdup(yytext);
    return identifier;
}

{R_DECIAML} {
    if(inArray){
        num_arr_tmp->num_arr = realloc(num_arr_tmp->num_arr, (num_arr_tmp->length + 1) * sizeof(double));
        num_arr_tmp->num_arr[num_arr_tmp->length++] = strtod(yytext , NULL);
    } else{
        yylval.u_number = strtod(yytext , NULL);
        return number;
    }
}

{R_RADIX} {
    char* delimiter_char = strrchr(yytext, 'r');
    int delimiter = delimiter_char - yytext;
    char* value = malloc(delimiter + 1);
    char* base = malloc(strlen(delimiter_char));

    strncpy(value, yytext, delimiter);
    value[delimiter] = '\0';
    strncpy(base, delimiter_char + 1, strlen(delimiter_char));

    if(inArray){
        num_arr_tmp->num_arr = realloc(num_arr_tmp->num_arr, (num_arr_tmp->length + 1) * sizeof(double));
        num_arr_tmp->num_arr[num_arr_tmp->length++] = strtol(value, NULL, atoi(base));
    } else {
        yylval.u_number = strtol(value, NULL, atoi(base)); 
        return number;
    }
}

{R_UNARY} {
    if(inArray){
        num_arr_tmp->num_arr = realloc(num_arr_tmp->num_arr, (num_arr_tmp->length + 1) * sizeof(double));
        num_arr_tmp->num_arr[num_arr_tmp->length++] = strlen(yytext) - 1;
    } else {
        yylval.u_number = strlen(yytext) - 1; 
        return number;
    }
}

{R_EQUALS} {return _equals;}
{R_ISNT} {return _isnt;}
{R_PLUS} {return _plus;}
{R_MINUS} {return _minus;}
{R_TIMES} {return _times;}
{R_DIVBY} {return _divby;}
{R_AND} {return _and;}
{R_OR} {return _or;}
{R_BIGGER} {return _bigger;}
{R_SMALLER} {return _smaller;}
{R_MODULO} {return _modulo;}


{R_BOOL} {
    if(inArray){
        boo_arr_tmp->boo_arr = realloc(boo_arr_tmp->boo_arr, (boo_arr_tmp->length + 1) * sizeof(char*));
        boo_arr_tmp->boo_arr[boo_arr_tmp->length++] = strdup(yytext);
    } else {
        yylval.u_boolean = strdup(yytext);
        return boolean;
    }
}

{R_DECLARE} {return _declare;}
{R_DEFINE} {return _define;}
{R_GIVE} {return _give;}
{R_END} {return _end;}
{R_REPEAT} {return _repeat;}
{R_IF} {return _if;}
{R_DO} {return _do;}
{R_OTHERWISE} {return _otherwise;}
{R_PRINT} {return _print;}
{R_READNUM} {return _readnum;}
{R_READSTR} {return _readstr;}
{R_RANDOM} {return _random;}
{R_STRSPLIT} {return _strsplit;}
{R_STRLEN} {return _strlen;}
{R_ARRLEN} {return _arrlen;}
{R_ARR_START} { array_start(); }
{R_ARR_END} { return array_end(); }

{R_SYMBOL} {return *yytext;}

{R_WHITESPACE} { /* Ignore whitespace */ }

{R_NEWLINE} { yylineno++; }

{R_UNRECOGNIZED} { return *yytext; }
