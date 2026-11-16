%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "semantic.h"

int yylex(void);
void yyerror(const char *s);
extern int yylineno;
extern int yydebug;

extern char *yytext;

PROGRAM *root = NULL;
%}

%code requires {
    #include "ast.h"
    #include "semantic.h"
}

%locations

%union {
    int ival;
    char *sval;
    EXP *exp;
    EXP_LIST *elist;
    STMT *stmt;
    STMT_LIST *slist;
    TYPE *type;
    VAR_DECL *vdecl;
    BODY *body;
    METHOD *method;
    CLASS_DECL *cls;
    MAIN_CLASS *mainc;
    PROGRAM *prog;
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


%type <type> Type
%type <vdecl> VarDeclaration VarDeclarationList VarDeclList1
%type <vdecl>  TypeIdentList OptionalTypeIdent
%type <body> MethodBody
%type <method> MethodDeclaration MethodDeclarationList
%type <cls> ClassDeclaration ClassDeclarationList
%type <sval> ExtendOptional
%type<mainc> MainClass
%type <prog> Goal
%type <sval> Identifier
%type <exp> Expression
%type <elist> ExpressionList OptionalExpression
%type <stmt> Statement
%type <slist> StatementList StatementList1
 
%define parse.error verbose

%%



StatementList 
    : %empty                    { $$ = NULL; }
    | StatementList1            { $$ = $1; }
    ;

Statement
    : LCURLY StatementList RCURLY                                   { $$ = makeSTMTblock(@1.first_line, $2); }
    | IF LPAR Expression RPAR Statement ELSE Statement              { $$ = makeSTMTif(@1.first_line, $3, $5, $7);}
    | WHILE LPAR Expression RPAR Statement                          { $$ = makeSTMTwhile(@1.first_line, $3, $5); }
    | SYSTEM DOT OUT DOT PRINTLN LPAR Expression RPAR SEMI          { $$ = makeSTMTprint(@1.first_line, $7);}
    | Identifier EQUAL Expression SEMI                              { $$ = makeSTMTassign(@1.first_line, $1, $3);}
    | Identifier LBRACKET Expression RBRACKET EQUAL Expression SEMI { $$ = makeSTMTarrayAssign(@1.first_line, $1, $3, $6);}
    | error SEMI                                                    { $$ = NULL; yyerrok; }
    ;

ExpressionList
    : Expression                        { $$ = makeEXPList($1, NULL); }
    | Expression COMMA ExpressionList   { $$ = makeEXPList($1, $3); }
    ;

OptionalExpression
    : %empty                { $$ = NULL; }
    | ExpressionList        { $$ = $1; }
    ;

ExtendOptional
    : %empty                        { $$ = NULL;}
    | EXTENDS Identifier            { $$ = $2; }
    ;

VarDeclarationList
    : %empty                                    { $$ = NULL;}
    | VarDeclaration VarDeclarationList         { $1->next = $2; $$ = $1;}
    ;

TypeIdentList
    : Type Identifier                           { $$ = makeVARDECL(@1.first_line, $1, $2, NULL);}
    | Type Identifier COMMA TypeIdentList       { $$ =makeVARDECL(@1.first_line, $1, $2, $4 ); }
    ;

OptionalTypeIdent
    : %empty                { $$ = NULL; }
    | TypeIdentList         { $$ = $1; }
    ;

MethodDeclaration
    : PUBLIC Type Identifier LPAR OptionalTypeIdent RPAR LCURLY MethodBody  RCURLY
    { $$ = makeMETHOD(@3.first_line, $2, $3, $5, $8, NULL);}
    | PUBLIC error RCURLY { $$ = NULL; yyerrok; }
    ;

MethodBody
    : RETURN Expression SEMI                                { $$ = makeBODY(NULL, NULL, $2);}
    | StatementList1 RETURN Expression SEMI                 { $$ = makeBODY(NULL, $1, $3); }
    | VarDeclList1 RETURN Expression SEMI                   { $$ = makeBODY($1, NULL, $3);}
    | VarDeclList1 StatementList1 RETURN Expression SEMI    { $$ = makeBODY($1, $2, $4); }
    ;

VarDeclList1
    : VarDeclaration                    { $$ = $1;}
    | VarDeclList1 VarDeclaration
        {
            VAR_DECL *t = $1;
            while (t->next) t = t->next;
            t->next = $2;
            $$ = $1;
        }
    ;

StatementList1
    : Statement                             { $$ = $1 ? makeSTMTList($1, NULL): NULL; }
    | Statement StatementList1              { $$ = $1 ? makeSTMTList($1, $2) : $2; }
    ;

ClassDeclarationList
    : %empty                                    { $$ = NULL;}
    | ClassDeclaration ClassDeclarationList     { if($1) { $1->next = $2; $$ = $1;} else $$ = $2;}
    ;

Goal
    : MainClass ClassDeclarationList            { root = makePROGRAM($1, $2); $$ = root;}
    ;

MethodDeclarationList
    : %empty                                    { $$ = NULL;}
    | MethodDeclaration MethodDeclarationList   { if($1) { $1->next = $2; $$ = $1;} else $$ = $2; }
    ;


ClassDeclaration
    : CLASS Identifier ExtendOptional LCURLY VarDeclarationList MethodDeclarationList RCURLY
    { $$ = makeCLASS(@2.first_line, $2, $3, $5, $6, NULL);}
    | CLASS error RCURLY { $$ = NULL; yyerrok; }
    ;

Expression
    : Expression AND Expression                   { $$ = makeEXPbinop(@2.first_line, andOp, $1, $3); }
    | Expression LESS Expression                  { $$ = makeEXPbinop(@2.first_line, lessOp, $1, $3); }
    | Expression ADD Expression                   { $$ =  makeEXPbinop(@2.first_line, plusOp, $1, $3);}
    | Expression SUBT Expression                  { $$ = makeEXPbinop(@2.first_line, minusOp, $1, $3); }
    | Expression OR Expression                    { $$ = makeEXPbinop(@2.first_line, orOp, $1, $3); }
    | Expression EQ Expression                    { $$ = makeEXPbinop(@2.first_line, eqOp, $1, $3); }
    | Expression NEQ Expression                   { $$ = makeEXPbinop(@2.first_line, neqOp, $1, $3); }
    | Expression LE Expression                    { $$ = makeEXPbinop(@2.first_line, leOp, $1, $3); }
    | Expression GE Expression                    { $$ = makeEXPbinop(@2.first_line, geOp, $1, $3);}
    | Expression GT Expression                    { $$ = makeEXPbinop(@2.first_line, gtOp, $1, $3); }
    | Expression DIV Expression                   { $$ = makeEXPbinop(@2.first_line, divOp, $1, $3); }
    | Expression MOD Expression                   { $$ = makeEXPbinop(@2.first_line, modOp, $1, $3); }
    | Expression MULTI Expression                 { $$ = makeEXPbinop(@2.first_line, timesOp, $1, $3); }
    | Expression LBRACKET Expression RBRACKET     { $$ = makeEXParrayIndex(@2.first_line, $1, $3); }
    | Expression DOT LENGTH                       { $$ = makeEXParrayLength(@2.first_line, $1); }
    | Expression DOT Identifier LPAR OptionalExpression RPAR { $$ = makeEXPmethodCall(@2.first_line, $1, $3, $5); }
    | INTEGER_LITERAL                             { $$ = makeEXPintconst(@1.first_line, $1); }
    | TRUE                                        { $$ = makeEXPboolconst(@1.first_line, true); }
    | FALSE                                       { $$ = makeEXPboolconst(@1.first_line, false);  }
    | NULL_LIT                                    { $$ = makeEXPnull(@1.first_line); }
    | Identifier                                  { $$ = makeEXPid(@1.first_line, $1); }
    | THIS                                        { $$ = makeEXPthis(@1.first_line);}
    | NEW INT LBRACKET Expression RBRACKET        { $$ = makeEXParrayNew(@1.first_line, $4); }
    | NEW Identifier LPAR RPAR                    { $$ = makeEXPobjectNew(@1.first_line, $2); }
    | EXCLAM Expression                           { $$ = makeEXPunop(@1.first_line, notOp, $2); }
    | LPAR Expression RPAR                        { $$ = $2; }
    ;

VarDeclaration
    : Type Identifier SEMI             { $$ = makeVARDECL(@1.first_line, $1, $2, NULL);}
    ;

Type
    : INT LBRACKET RBRACKET            { $$ = makeTYPE(@1.first_line, intArrayT, NULL);}
    | BOOLEAN                          { $$ = makeTYPE(@1.first_line, boolT, NULL);}
    | INT                              { $$ = makeTYPE(@1.first_line, intT, NULL);}
    | Identifier                       { $$ = makeTYPE(@1.first_line, classT, $1);}
    ;

Identifier
    : IDENT { $$ = $1; }
    ;

MainClass
    : CLASS Identifier LCURLY PUBLIC STATIC VOID MAIN LPAR STRING_TYPE 
    LBRACKET RBRACKET Identifier RPAR LCURLY Statement RCURLY RCURLY 
    { $$ = makeMAINCLASS(@2.first_line, $2, $12, $15 );}
    | CLASS Identifier LCURLY PUBLIC STATIC VOID MAIN LPAR STRING_TYPE
      LBRACKET RBRACKET Identifier RPAR LCURLY error RCURLY RCURLY
      { $$ = NULL; yyerrok; }
    ;

%%

int error_count = 0;

void yyerror(const char *s) {
    error_count++;
    fprintf(stderr, "Syntax error at line %d: %s\n", yylineno, s);
}

int main(void) {
    /* yydebug = 1; */
    int rc = yyparse();
    if(rc != 0){
        printf("Parse failed (unrecoverable)\n");
        return 1;
    }
    if (error_count > 0){
        printf("Parse completed with %d syntax error(recovered)\n", error_count);
        return 1;
    }
    int sem = checkProgram(root);
    if(sem > 0){
        printf("Semantic check completed with %d errors\n",sem);
        return 1;
    }
    printf("Parse successful \n");
    return 0;
}