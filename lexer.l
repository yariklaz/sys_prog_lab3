%{
#include "ast.h"
#include "parser.tab.h"
#include <string>
%}

%option noyywrap

%%
"int"                   { return INT_TYPE; }
[0-9]+                  { yylval.num = std::stoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]*  { yylval.str = new std::string(yytext); return IDENTIFIER; }
"="                     { return ASSIGN; }
"+"                     { return PLUS; }
"-"                     { return MINUS; }
"*"                     { return MULT; }
"/"                     { return DIV; }
";"                     { return SEMICOLON; }
[ \t\r\n]+              { }
.                       { printf("Невідомий символ: %s\n", yytext); }
%%
