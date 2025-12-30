#pragma once

#include "pch.h"
#include "Tokenizer.h"

namespace Glassy {

/*
    TODO:
    - parenthesis precendence
    - unary operators
    - variable expressions
*/

/*
    Grammar:

    program    -> statement*
    statement  -> ((identifier "=" expression) | (exit NUMBER)) ";"
    expression -> term (("+" | "-") term)*
    term       -> factor (("*" | "/") factor)*
    factor     -> NUMBER | IDENTIFIER | "(" expression ")"
*/

struct ASTNode {
    virtual ~ASTNode() = default;
    virtual void print(std::ostream &out) const = 0;

    friend std::ostream &operator<<(std::ostream &os, const ASTNode &node) {
        node.print(os);
        return os;
    }
};

struct Expression : public ASTNode {};

struct NumberExpr : public Expression {
    explicit NumberExpr(double val) : value(val) {}

    void print(std::ostream &out) const override { out << value; }

    double value;
};

struct BinaryExpr : public Expression {
    BinaryExpr(char op, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : op(op),
          left(std::move(left)),
          right(std::move(right)) {}

    void print(std::ostream &out) const override { out << *left << ' ' << op << ' ' << *right; }

    char op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
};

struct Statement : public ASTNode {
    virtual void GenerateCode(std::string &out) const = 0;
};

struct AssignStmt : public Statement {
    AssignStmt(const std::string &name, std::unique_ptr<Expression> value)
        : name(name),
          value(std::move(value)) {}

    void GenerateCode(std::string &out) const override { out += "; assign " + name + "\n"; }

    void print(std::ostream &out) const override { out << name << " = " << *value << ";"; }

    std::string name;
    std::unique_ptr<Expression> value;
};

struct ExitStmt : public Statement {
    ExitStmt(uint8_t value) : exitValue(value) {}

    void GenerateCode(std::string &out) const override {
        out += "mov rax, 60\nmov rdi, " + std::to_string(exitValue) + "\nsyscall\n";
    }

    void print(std::ostream &out) const override {
        out << "exit " << static_cast<unsigned int>(exitValue) << ";";
    }

    uint8_t exitValue;
};

struct Program : public ASTNode {
    void print(std::ostream &out) const override {
        out << "Program:\n";
        for (const auto &stmt : statements) {
            out << *stmt << '\n';
        }
    }

    std::vector<std::unique_ptr<Statement>> statements;
};

class Parser {
  public:
    explicit Parser(const std::vector<Token> &tokens);

    std::unique_ptr<Program> ParseProgram();

  private:
    std::unique_ptr<Expression> parseFactor();
    std::unique_ptr<Expression> parseTerm();
    std::unique_ptr<Expression> parseExpression();
    std::unique_ptr<Statement> parseStatement();

    [[nodiscard]] std::optional<Token> peek(const int offset = 0) const;
    Token consume();
    bool match(TokenType type);
    Token expect(TokenType type, const char *msg);

    const std::vector<Token> m_Tokens;
    size_t m_Index = 0;
};

} // namespace Glassy
