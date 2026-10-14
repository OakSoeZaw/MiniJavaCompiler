%{
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
void yyerror(const char *s);
extern int yylineno;
%}


%token CLASS
%token PUBLIC
%token STATIC
%token VOID
%token MAIN
%token STRING_TYPE
%token LCURLY
%token RCURLY
%token LPAR
%token RPAR
%token LBRACKET
%token RBRACKET
%token IDENT

%start MainClass

%%
Statement
    : %empty
    ;

MainClass
    : CLASS IDENT LCURLY PUBLIC STATIC VOID MAIN LPAR STRING_TYPE 
    LBRACKET RBRACKET IDENT RPAR LCURLY Statement RCURLY RCURLY ;

%%

void yyerror(const char *s){
    fprintf(stderr, "Syntax error at line %d: %s\n", yylineno, s);
}

int main(void){
    if(yyparse() == 0){
        printf("Parse successful \n");
        return 0;
    }else{
        printf("Parse failed \n");
        return 1;
    }
}