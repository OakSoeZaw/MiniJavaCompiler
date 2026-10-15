%{
#include <stdio.h>
#include <string.h>
#include "parser.tab.h"
%}

%option noyywrap yylineno

%%
"return" { return RETURN; }
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
";" { return SEMI; }
"int" { return INT; }
"boolean" { return BOOLEAN;}
"if" { return IF; }
"else" { return ELSE; }
"while" { return WHILE; }
"." { return DOT; }
"System" {return SYSTEM; }
"out" {return OUT; }
"println" { return PRINTLN; }
"=" {return EQUAL; }
"&&" { return AND; }
"<" { return LESS; }
"+" { return ADD; }
"-" { return SUBT; }
"*" { return MULTI; }
"length" { return LENGTH; }
"true" { return TRUE; }
"false" { return FALSE; }
"this" { return THIS; }
"new" { return NEW; }
"!" { return EXCLAM; }
"," { return COMMA; }
"extends" { return EXTENDS; }

[0-9]+ { yylval.ival = atoi(yytext); return INTEGER_LITERAL; }

[a-zA-Z_][a-zA-Z0-9_]*   { yylval.sval = strdup(yytext); return IDENT; }
\n { /* skip newline */ }
[ \t\r] { /* skip */}
"//"[^\n]*  { /* skip comments */}


%%
