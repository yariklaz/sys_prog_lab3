#ifndef AST_H
#define AST_H

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <nlohmann/json.hpp>

using json = nlohmann::json;


class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual std::string generatePython(int indent = 0) const = 0;
    virtual json toJson() const = 0;
};

class Expr : public ASTNode {};
class Stmt : public ASTNode {};

class NumberExpr : public Expr {
    int val;
public:
    NumberExpr(int v) : val(v) {}
    std::string generatePython(int) const override { return std::to_string(val); }
    json toJson() const override { return {{"name", std::to_string(val)}, {"type", "Number"}}; }
};

class VarExpr : public Expr {
    std::string name;
public:
    VarExpr(const std::string& n) : name(n) {}
    std::string generatePython(int) const override { return name; }
    json toJson() const override { return {{"name", name}, {"type", "Variable"}}; }
};

class BinaryExpr : public Expr {
    char op;
    std::unique_ptr<Expr> left, right;
public:
    BinaryExpr(char o, Expr* l, Expr* r) : op(o), left(l), right(r) {}
    std::string generatePython(int ind) const override {
        return "(" + left->generatePython(ind) + " " + op + " " + right->generatePython(ind) + ")";
    }
    json toJson() const override {
        return {{"name", std::string(1, op)}, {"children", {left->toJson(), right->toJson()}}};
    }
};

class AssignStmt : public Stmt {
    std::string name;
    std::unique_ptr<Expr> expr;
public:
    AssignStmt(const std::string& n, Expr* e) : name(n), expr(e) {}
    std::string generatePython(int ind) const override {
        return std::string(ind, ' ') + name + " = " + expr->generatePython(0) + "\n";
    }
    json toJson() const override {
        return {{"name", "="}, {"children", {{{"name", name}}, expr->toJson()}}};
    }
};

class Block : public ASTNode {
public:
    std::vector<std::unique_ptr<Stmt>> statements;
    void add(Stmt* stmt) { statements.emplace_back(stmt); }
    std::string generatePython(int ind) const override {
        std::string res;
        for (const auto& s : statements) res += s->generatePython(ind);
        return res.empty() ? std::string(ind, ' ') + "pass\n" : res;
    }
    json toJson() const override {
        json j = {{"name", "Block"}, {"children", json::array()}};
        for (const auto& s : statements) j["children"].push_back(s->toJson());
        return j;
    }
};
#endif
