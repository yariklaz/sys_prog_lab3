#ifndef AST_H
#define AST_H

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <stdexcept>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using Environment = std::map<std::string, int>;

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual std::string generatePython(int indent = 0) const = 0;
    virtual json toJson() const = 0;
    virtual int eval(Environment& env) const = 0; // Семантичне обчислення
};

class Expr : public ASTNode {};
class Stmt : public ASTNode {};

class NumberExpr : public Expr {
    int val;
public:
    NumberExpr(int v) : val(v) {}
    std::string generatePython(int) const override { return std::to_string(val); }
    json toJson() const override { return {{"name", std::to_string(val)}, {"type", "Number"}}; }
    int eval(Environment&) const override { return val; }
};

class VarExpr : public Expr {
    std::string name;
public:
    VarExpr(const std::string& n) : name(n) {}
    std::string generatePython(int) const override { return name; }
    json toJson() const override { return {{"name", name}, {"type", "Variable"}}; }
    int eval(Environment& env) const override {
        if (env.find(name) == env.end()) throw std::runtime_error("Undeclared variable: " + name);
        return env[name];
    }
};

class BinaryExpr : public Expr {
    std::string op;
    std::unique_ptr<Expr> left, right;
public:
    BinaryExpr(const std::string& o, Expr* l, Expr* r) : op(o), left(l), right(r) {}
    std::string generatePython(int ind) const override {
        std::string pyOp = (op == "/") ? "//" : op; // Ціле ділення для Python
        return "(" + left->generatePython(ind) + " " + pyOp + " " + right->generatePython(ind) + ")";
    }
    json toJson() const override {
        return {{"name", op}, {"type", "BinaryOp"}, {"children", {left->toJson(), right->toJson()}}};
    }
    int eval(Environment& env) const override {
        int l = left->eval(env);
        int r = right->eval(env);
        if (op == "+") return l + r;
        if (op == "-") return l - r;
        if (op == "*") return l * r;
        if (op == "/") {
            if (r == 0) throw std::runtime_error("Division by zero");
            return l / r;
        }
        if (op == "<") return l < r;
        if (op == ">") return l > r;
        if (op == "==") return l == r;
        return 0;
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
        return {{"name", "="}, {"type", "Assign"}, {"children", {{{"name", name}, {"type", "Variable"}}, expr->toJson()}}};
    }
    int eval(Environment& env) const override {
        int val = expr->eval(env);
        env[name] = val;
        return val;
    }
};

class Block : public Stmt {
public:
    std::vector<std::unique_ptr<Stmt>> statements;
    void add(Stmt* stmt) { statements.emplace_back(stmt); }
    std::string generatePython(int ind) const override {
        std::string res;
        for (const auto& s : statements) res += s->generatePython(ind);
        return res.empty() ? std::string(ind, ' ') + "pass\n" : res;
    }
    json toJson() const override {
        json j = {{"name", "Block"}, {"type", "Block"}, {"children", json::array()}};
        for (const auto& s : statements) j["children"].push_back(s->toJson());
        return j;
    }
    int eval(Environment& env) const override {
        for (const auto& s : statements) s->eval(env);
        return 0;
    }
};

class IfStmt : public Stmt {
    std::unique_ptr<Expr> cond;
    std::unique_ptr<Stmt> body;
public:
    IfStmt(Expr* c, Stmt* b) : cond(c), body(b) {}
    std::string generatePython(int ind) const override {
        std::string res = std::string(ind, ' ') + "if " + cond->generatePython(0) + ":\n";
        res += body->generatePython(ind + 4);
        return res;
    }
    json toJson() const override {
        return {{"name", "If"}, {"type", "ControlFlow"}, {"children", {cond->toJson(), body->toJson()}}};
    }
    int eval(Environment& env) const override {
        if (cond->eval(env)) body->eval(env);
        return 0;
    }
};

class WhileStmt : public Stmt {
    std::unique_ptr<Expr> cond;
    std::unique_ptr<Stmt> body;
public:
    WhileStmt(Expr* c, Stmt* b) : cond(c), body(b) {}
    std::string generatePython(int ind) const override {
        std::string res = std::string(ind, ' ') + "while " + cond->generatePython(0) + ":\n";
        res += body->generatePython(ind + 4);
        return res;
    }
    json toJson() const override {
        return {{"name", "While"}, {"type", "ControlFlow"}, {"children", {cond->toJson(), body->toJson()}}};
    }
    int eval(Environment& env) const override {
        while (cond->eval(env)) body->eval(env);
        return 0;
    }
};
#endif