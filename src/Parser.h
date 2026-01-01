#pragma once

#include "Arena.h"
#include "Error.h"
#include "Tokenizer.h"

#include <optional>
#include <ostream>

namespace Glassy {

/*
    TODO:
    - unary operators
    - variable expressions
*/

struct LiteralExpr;
struct IdentifierExpr;
struct BinaryExpr;
struct AssignStmt;
struct DeclarStmt;
struct ExitStmt;
struct Program;

struct AstVisitor {
    virtual void visit(const LiteralExpr& node) = 0;
    virtual void visit(const IdentifierExpr& node) = 0;
    virtual void visit(const BinaryExpr& node) = 0;
    virtual void visit(const AssignStmt& node) = 0;
    virtual void visit(const DeclarStmt& node) = 0;
    virtual void visit(const ExitStmt& node) = 0;
    virtual void visit(const Program& node) = 0;

    virtual ~AstVisitor() = default;
};

struct ASTNode {
    ASTNode() = default;
    virtual ~ASTNode() = default;
    ASTNode(const ASTNode&) = delete;
    ASTNode& operator=(const ASTNode&) = delete;

    virtual void print(std::ostream& out) const = 0;
    virtual void accept(AstVisitor& visitor) const = 0;

    friend std::ostream& operator<<(std::ostream& os, const ASTNode& node) {
        node.print(os);
        return os;
    }
};

struct Expression : ASTNode {
    virtual ~Expression() = default;
};

struct LiteralExpr : Expression {
    explicit LiteralExpr(Literal val) : value(val) {}

    void print(std::ostream& out) const override { out << value; }
    void accept(AstVisitor& visitor) const override { visitor.visit(*this); }

    Literal value;
};

struct IdentifierExpr : Expression {
    explicit IdentifierExpr(std::string_view id) : name(id) {}

    void print(std::ostream& out) const override { out << name; }
    void accept(AstVisitor& visitor) const override { visitor.visit(*this); }

    Identifier name;
};

struct BinaryExpr : Expression {
    BinaryExpr(Operator op, Expression* lhs, Expression* rhs) : op(op), left(lhs), right(rhs) {}

    void print(std::ostream& out) const override {
        out << *left << ' ' << OperatorToStr[size_t(op)][0] << ' ' << *right;
    }
    void accept(AstVisitor& visitor) const override { visitor.visit(*this); }

    Operator op;
    Expression* left;
    Expression* right;
};

struct Statement : ASTNode {
    virtual ~Statement() = default;
};

struct AssignStmt : Statement {
    AssignStmt(std::string_view name, Expression* expr) : identifier(name), expr(expr) {}

    void print(std::ostream& out) const override { out << identifier << " = " << *expr << ";"; }
    void accept(AstVisitor& visitor) const override { visitor.visit(*this); }

    Identifier identifier;
    Expression* expr;
};

struct DeclarStmt : Statement {
    DeclarStmt(std::string_view name, Expression* expr) : identifier(name), expr(expr) {}

    void print(std::ostream& out) const override { out << "let " << identifier << " = " << *expr << ";"; }
    void accept(AstVisitor& visitor) const override { visitor.visit(*this); }

    Identifier identifier;
    Expression* expr;
};

struct ExitStmt : Statement {
    ExitStmt(Expression* e) : expr(e) {}

    void print(std::ostream& out) const override { out << "exit " << *expr << ";"; }
    void accept(AstVisitor& visitor) const override { visitor.visit(*this); }

    Expression* expr;
};

struct Program : ASTNode {
    void print(std::ostream& out) const override {
        out << "Program:\n";
        for (const auto& stmt : statements) {
            out << *stmt << '\n';
        }
    }
    void accept(AstVisitor& visitor) const override { visitor.visit(*this); }

    std::vector<Statement*> statements;
};

class Parser {
  public:
    explicit Parser(const std::vector<Token>& tokens);
    Program* ParseProgram();

  private:
    Expression* parseFactor();
    Expression* parseTerm();
    Expression* parseExpression();
    Statement* parseStatement();

    std::optional<Token> peek(const int offset = 0) const {
        if (m_Index + offset >= m_Tokens.size()) {
            return std::nullopt;
        }
        return m_Tokens[m_Index + offset];
    }

    Token consume() {
        if (m_Index >= m_Tokens.size()) {
            Error(m_Tokens[m_Index - 1].location, "Unexpected end of file");
        }
        return m_Tokens[m_Index++];
    }

    template <typename T, typename... Ts>
    std::optional<T> match(T first, Ts... rest) {
        auto tok = peek();
        if (!tok) {
            return std::nullopt;
        }

        if (auto val = tok->GetValue<T>()) {
            if (((*val == first) || ... || (*val == rest))) {
                consume();
                return *val;
            }
        }
        return std::nullopt;
    }

    Token expect(TokenType type, const char* msg) {
        auto tok = peek();
        if (!tok) {
            Error(m_Tokens.back().location, msg);
        }
        if (tok->type != type) {
            Error(tok->location, msg);
        }
        return consume();
    }

    ArenaAllocator m_Allocator;

    const std::vector<Token> m_Tokens;
    size_t m_Index = 0;
};

} // namespace Glassy
