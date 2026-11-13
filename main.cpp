#include <iostream>
#include <fstream>
#include <map>
#include "ast.h"

extern Block* rootBlock;
extern int yyparse();

int main() {
    std::cout << "Введіть код на Mini-C (Ctrl+D для завершення):\n";
    
    if (yyparse() != 0) {
        std::cerr << "\n[!] Компіляцію зупинено через синтаксичні помилки.\n";
        return 1;
    }

    if (rootBlock) {
        std::ofstream pyFile("code.py");
        pyFile << rootBlock->generatePython(0);
        pyFile.close();
        std::cout << "\n[+] Згенеровано code.py\n";

        std::ofstream jsonFile("ast.json");
        jsonFile << rootBlock->toJson().dump(4);
        jsonFile.close();
        std::cout << "[+] Згенеровано ast.json\n";

        std::cout << "\n[>] Результат внутрішнього обчислення AST:\n";
        Environment env;
        try {
            rootBlock->eval(env);
            for (const auto& [var, val] : env) {
                std::cout << "    Змінна " << var << " = " << val << "\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "    Помилка виконання (Runtime Error): " << e.what() << "\n";
        }
    }

    return 0;
}