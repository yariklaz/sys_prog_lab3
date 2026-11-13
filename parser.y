%{
#include "ast.h"
#include <iostream>

extern int yylex();
void yyerror(const char* s);
Block* rootBlock = nullptr;
%}

%union {
    int num;
    std::string* str;
    Expr* expr;
    Stmt* stmt;
    Block* block;
}

%token <num> NUMBER
%token <str> IDENTIFIER
%token INT_TYPE ASSIGN PLUS MINUS MULT DIV SEMICOLON

%type <expr> expr
%type <stmt> stmt
%type <block> program

%left PLUS MINUS
%left MULT DIV

%%
program:
    { rootBlock = new Block(); $$ = rootBlock; }
    | program stmt { $1->add($2); $$ = $1; }
    ;

stmt:
    INT_TYPE IDENTIFIER ASSIGN expr SEMICOLON { 
        $$ = new AssignStmt(*$2, $4); 
        delete $2; 
    }
    | IDENTIFIER ASSIGN expr SEMICOLON { 
        $$ = new AssignStmt(*$1, $3); 
        delete $1; 
    }
    ;

expr:
    NUMBER                  { $$ = new NumberExpr($1); }
    | IDENTIFIER            { $$ = new VarExpr(*$1); delete $1; }
    | expr PLUS expr        { $$ = new BinaryExpr('+', $1, $3); }
    | expr MINUS expr       { $$ = new BinaryExpr('-', $1, $3); }
    | expr MULT expr        { $$ = new BinaryExpr('*', $1, $3); }
    | expr DIV expr         { $$ = new BinaryExpr('/', $1, $3); }
    ;

%%
void yyerror(const char* s) {
    std::cerr << s << std::endl;
}
