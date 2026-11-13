%{
#include "ast.h"
#include <iostream>

extern int yylex();
extern int yylineno;
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
%token INT_TYPE IF WHILE
%token ASSIGN PLUS MINUS MULT DIV SEMICOLON
%token LPAREN RPAREN LBRACE RBRACE
%token EQ LT GT

%type <expr> expr
%type <stmt> stmt
%type <block> program block_stmts

/* Пріоритети операторів (знизу вверх = від найменшого до найбільшого) */
%left EQ LT GT
%left PLUS MINUS
%left MULT DIV
%right UMINUS

%%
program:
    { rootBlock = new Block(); $$ = rootBlock; }
    | program stmt { $1->add($2); $$ = $1; }
    ;

stmt:
    INT_TYPE IDENTIFIER ASSIGN expr SEMICOLON { 
        $$ = new AssignStmt(*$2, $4); delete $2; 
    }
    | IDENTIFIER ASSIGN expr SEMICOLON { 
        $$ = new AssignStmt(*$1, $3); delete $1; 
    }
    | IF LPAREN expr RPAREN LBRACE block_stmts RBRACE {
        $$ = new IfStmt($3, $6);
    }
    | WHILE LPAREN expr RPAREN LBRACE block_stmts RBRACE {
        $$ = new WhileStmt($3, $6);
    }
    ;

block_stmts:
    { $$ = new Block(); }
    | block_stmts stmt { $1->add($2); $$ = $1; }
    ;

expr:
    NUMBER                  { $$ = new NumberExpr($1); }
    | IDENTIFIER            { $$ = new VarExpr(*$1); delete $1; }
    | expr PLUS expr        { $$ = new BinaryExpr("+", $1, $3); }
    | expr MINUS expr       { $$ = new BinaryExpr("-", $1, $3); }
    | expr MULT expr        { $$ = new BinaryExpr("*", $1, $3); }
    | expr DIV expr         { $$ = new BinaryExpr("/", $1, $3); }
    | expr EQ expr          { $$ = new BinaryExpr("==", $1, $3); }
    | expr LT expr          { $$ = new BinaryExpr("<", $1, $3); }
    | expr GT expr          { $$ = new BinaryExpr(">", $1, $3); }
    | LPAREN expr RPAREN    { $$ = $2; }
    | MINUS expr %prec UMINUS { $$ = new BinaryExpr("-", new NumberExpr(0), $2); }
    ;

%%
void yyerror(const char* s) {
    std::cerr << "Помилка синтаксису (рядок " << yylineno << "): " << s << std::endl;
}