%{
#include <stdio.h>
#include "parser.tab.h"
%}

%option noyywrap

%%
"class" { return CLASS; }
"{" { return LCURLY; }
"public" { return PUBLIC; }
"static" { return STATIC; }
"void" { return VOID; }
"main" { return MAIN; }
"(" { return LPAR;}
"String" { return STRING_TYPE; }
"[" { return LBRACKET; }
"]" { return RBRACKET; }
")" { return RPAR; }
"}" { return RCURLY; } 
[a-zA-Z][a-zA-Z0-9]*   { return IDENT; }
[ \n] { yylineno++; }
[\t] { /* */}
%%
