%{
#include "ast.h"
#include "parser.tab.h"
#include <string>
%}

%option noyywrap
%option yylineno

%%
"int"                   { return INT_TYPE; }
"if"                    { return IF; }
"while"                 { return WHILE; }
[0-9]+                  { yylval.num = std::stoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]*  { yylval.str = new std::string(yytext); return IDENTIFIER; }
"=="                    { return EQ; }
"<"                     { return LT; }
">"                     { return GT; }
"="                     { return ASSIGN; }
"+"                     { return PLUS; }
"-"                     { return MINUS; }
"*"                     { return MULT; }
"/"                     { return DIV; }
"("                     { return LPAREN; }
")"                     { return RPAREN; }
"{"                     { return LBRACE; }
"}"                     { return RBRACE; }
";"                     { return SEMICOLON; }
[ \t\r\n]+              { }
.                       { printf("Невідомий символ на рядку %d: %s\n", yylineno, yytext); }
%%