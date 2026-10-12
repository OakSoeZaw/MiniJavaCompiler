%{
#include <stdio.h>
%}

%%

[a-z]+ {printf("%s\n", yytext);}
%%

int main(void){
    yylex();
    return 0;
}