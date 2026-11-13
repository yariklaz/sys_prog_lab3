CXX = g++
CXXFLAGS = -std=c++17 -Wall

all: compiler

compiler: lex.yy.c parser.tab.c main.cpp
	$(CXX) $(CXXFLAGS) -o compiler parser.tab.c lex.yy.c main.cpp

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

clean:
	rm -f compiler lex.yy.c parser.tab.c parser.tab.h ast.json code.py
