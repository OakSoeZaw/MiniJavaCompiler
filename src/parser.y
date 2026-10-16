%{
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
void yyerror(const char *s);
extern int yylineno;
extern int yydebug;

extern char *yytext;
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
%token NULL_LIT

%left OR
%left AND
%left EQ NEQ    
%left LESS LE GT GE
%left ADD SUBT
%left  MULTI DIV MOD
%right EXCLAM
%left LBRACKET DOT

%token LENGTH
%token <ival> INTEGER_LITERAL
%start Goal
%type <sval> Identifier
 

%%



StatementList 
    : %empty
    | StatementList Statement
    ;

Statement
    : LCURLY StatementList RCURLY                                  { printf("Parse Statement successful\n"); }
    | IF LPAR Expression RPAR Statement ELSE Statement            { printf("Parse Statement successful\n"); }
    | WHILE LPAR Expression RPAR Statement                        { printf("Parse Statement successful\n"); }
    | SYSTEM DOT OUT DOT PRINTLN LPAR Expression RPAR SEMI        { printf("Parse Statement successful\n"); }
    | Identifier EQUAL Expression SEMI                            { printf("Parse Statement successful\n"); }
    | Identifier LBRACKET Expression RBRACKET EQUAL Expression SEMI { printf("Parse Statement successful\n"); }
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
    : PUBLIC Type Identifier LPAR OptionalTypeIdent RPAR LCURLY MethodBody  RCURLY
    {  printf("Parsed MethodDeclaration '%s'\n", $3); }
    ;

MethodBody
    : RETURN Expression SEMI
    | StatementList1 RETURN Expression SEMI
    | VarDeclList1 RETURN Expression SEMI
    | VarDeclList1 StatementList1 RETURN Expression SEMI
    ;

VarDeclList1
    : VarDeclaration
    | VarDeclList1 VarDeclaration
    ;

StatementList1
    : Statement
    | StatementList1 Statement
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
    { printf("Parse ClassDeclaration successful\n"); }
    ;

Expression
    : Expression AND Expression                   { printf("Parse Expression successful\n"); }
    | Expression LESS Expression                  { printf("Parse Expression successful\n"); }
    | Expression ADD Expression                   { printf("Parse Expression successful\n"); }
    | Expression SUBT Expression                  { printf("Parse Expression successful\n"); }
    | Expression OR Expression                    { printf("Parse Expression successful\n"); }
    | Expression EQ Expression                    { printf("Parse Expression successful\n"); }
    | Expression NEQ Expression                   { printf("Parse Expression successful\n"); }
    | Expression LE Expression                    { printf("Parse Expression successful\n"); }
    | Expression GE Expression                    { printf("Parse Expression successful\n"); }
    | Expression GT Expression                    { printf("Parse Expression successful\n"); }
    | Expression DIV Expression                   { printf("Parse Expression successful\n"); }
    | Expression MOD Expression                   { printf("Parse Expression successful\n"); }
    | Expression MULTI Expression                 { printf("Parse Expression successful\n"); }
    | Expression LBRACKET Expression RBRACKET     { printf("Parse Expression successful\n"); }
    | Expression DOT LENGTH                       { printf("Parse Expression successful\n"); }
    | Expression DOT Identifier LPAR OptionalExpression RPAR { printf("Parse Expression successful\n"); }
    | INTEGER_LITERAL                             { printf("Parse Expression successful\n"); }
    | TRUE                                        { printf("Parse Expression successful\n"); }
    | FALSE                                       { printf("Parse Expression successful\n"); }
    | NULL_LIT                                    { printf("Parse Expression successful\n"); }
    | Identifier                                  { printf("Parse Expression successful\n"); }
    | THIS                                        { printf("Parse Expression successful\n"); }
    | NEW INT LBRACKET Expression RBRACKET        { printf("Parse Expression successful\n"); }
    | NEW Identifier LPAR RPAR                    { printf("Parse Expression successful\n"); }
    | EXCLAM Expression                           { printf("Parse Expression successful\n"); }
    | LPAR Expression RPAR                        { printf("Parse Expression successful\n"); }
    ;

VarDeclaration
    : Type Identifier SEMI
    { printf("Parse VarDeclaration successful\n"); }
    ;

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

void yyerror(const char *s) {
    fprintf(stderr,
            "Syntax error at line %d near '%s': %s\n",
            yylineno, yytext, s);
}

int main(void) {
    /* yydebug = 1; */

    if (yyparse() == 0) {
        printf("Parse successful\n");
        return 0;
    }

    printf("Parse failed\n");
    return 1;
}