#include <iostream>
#include <fstream>
#include "ast.h"

extern Block* rootBlock;
extern int yyparse();

int main() {
    yyparse();

    if (rootBlock) {
        std::ofstream pyFile("code.py");
        pyFile << rootBlock->generatePython(0);
        pyFile.close();

        std::ofstream jsonFile("ast.json");
        json j = rootBlock->toJson();
        jsonFile << j.dump(4);
        jsonFile.close();
    }

    return 0;
}
