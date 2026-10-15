%{
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
void yyerror(const char *s);
extern int yylineno;
%}

%union {
    int ival;
    char *sval;
}


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
%token RBRACKET
%token <sval> IDENT
%token BOOLEAN
%token INT
%token SEMI
%token IF
%token ELSE
%token WHILE
%token SYSTEM
%token OUT
%token PRINTLN
%token EQUAL
%token TRUE
%token FALSE
%token THIS
%token NEW
%token COMMA
%token EXTENDS
%token RETURN

%left AND
%left LESS
%left ADD SUBT
%left  MULTI
%right EXCLAM
%left LBRACKET DOT

%token LENGTH
%token <ival> INTEGER_LITERAL
%start Goal
 


%%



StatementList 
    : %empty
    | StatementList Statement
    ;

Statement
    : LCURLY StatementList RCURLY
    | IF LPAR Expression RPAR Statement ELSE Statement
    | WHILE LPAR Expression RPAR Statement
    | SYSTEM DOT OUT DOT PRINTLN LPAR Expression RPAR SEMI
    | Identifier EQUAL Expression SEMI
    | Identifier LBRACKET Expression RBRACKET EQUAL Expression SEMI
    ;

ExpressionList
    : Expression
    | ExpressionList COMMA Expression
    ;

OptionalExpression
    : %empty
    | ExpressionList
    ;

ExtendOptional
    : %empty
    | EXTENDS Identifier
    ;

VarDeclarationList
    : %empty
    | VarDeclarationList VarDeclaration
    ;

TypeIdentList
    : Type Identifier
    | TypeIdentList COMMA Type Identifier
    ;

OptionalTypeIdent
    : %empty
    | TypeIdentList
    ;

MethodDeclaration
    : PUBLIC Type Identifier LPAR OptionalTypeIdent RPAR LCURLY VarDeclarationList StatementList RETURN Expression SEMI RCURLY
    ;

ClassDeclarationList
    : %empty
    | ClassDeclarationList ClassDeclaration
    ;

Goal
    : MainClass ClassDeclarationList
    ;

MethodDeclarationList
    : %empty
    | MethodDeclarationList MethodDeclaration
    ;



ClassDeclaration
    : CLASS Identifier ExtendOptional LCURLY VarDeclarationList MethodDeclarationList RCURLY
    ;

Expression
    : Expression AND Expression
    | Expression LESS Expression
    | Expression ADD Expression
    | Expression SUBT Expression
    | Expression MULTI Expression
    | Expression LBRACKET Expression RBRACKET
    | Expression DOT LENGTH
    | Expression DOT Identifier LPAR OptionalExpression RPAR
    | INTEGER_LITERAL
    | TRUE
    | FALSE
    | Identifier
    | THIS
    | NEW INT LBRACKET Expression RBRACKET
    | NEW Identifier LPAR RPAR
    | EXCLAM Expression
    | LPAR Expression RPAR
    ;

VarDeclaration
    : Type Identifier SEMI;

Type
    : INT LBRACKET RBRACKET
    | BOOLEAN
    | INT
    | Identifier
    ;

Identifier
    : IDENT
    ;

MainClass
    : CLASS Identifier LCURLY PUBLIC STATIC VOID MAIN LPAR STRING_TYPE 
    LBRACKET RBRACKET Identifier RPAR LCURLY Statement RCURLY RCURLY 
    { printf("Parse MainClass successful\n"); }
    ;

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