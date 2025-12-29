#pragma once

#include "pch.h"
#include "Tokenizer.h"

namespace Compiler {

    class Parser {
      private:
        // program     -> statement*
        // statement   -> identifier "=" expression ";"
        // expression  -> term (("+" | "-") term)*
        // term        -> factor (("*" | "/") factor)*
        // factor      -> NUMBER | IDENTIFIER | "(" expression ")"

        // ex:  "x = (3 + 8.5) * .2;"

        struct ASTNode {
            virtual ~ASTNode() = default;
        };

        struct Expression : public ASTNode { };

        struct NumberExpr : public Expression {
            double value;

            explicit NumberExpr(double val) : value(val) { }
        };

        struct BinaryExpr : public Expression {
            char op;
            std::unique_ptr<Expression> left;
            std::unique_ptr<Expression> right;

            BinaryExpr(char op, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
              : op(op),
                left(std::move(left)),
                right(std::move(right)) { }
        };

        struct Statement : public ASTNode { };

        struct AssignStmt : public Statement {
            std::string name;
            std::unique_ptr<Expression> value;

            AssignStmt(const std::string& name, std::unique_ptr<Expression> value)
              : name(name),
                value(std::move(value)) { }
        };

        struct Program : public ASTNode {
            std::vector<std::unique_ptr<Statement>> statements;
        };

        std::unique_ptr<Expression> ParseExpression() { }

        std::unique_ptr<Statement> ParseStatement() {
            Token name = expect(TokenType::IDENTIFIER, "Expected identifier");

            expect(TokenType::OPERATOR, "Expected '='");

            auto expr = ParseExpression();

            expect(TokenType::SEPARATOR, "Expected ';'");

            return std::make_unique<AssignStmt>(name.lexeme, std::move(expr));
        }

      public:
        explicit Parser(const std::vector<Token>& tokens) : m_Tokens(tokens) { }

        std::unique_ptr<Program> ParseProgram() {
            std::unique_ptr<Program> program = std::make_unique<Program>();

            while (peek().has_value()) {
                program->statements.push_back(ParseStatement());
            }

            return program;
        }

      private:
        [[nodiscard]] std::optional<Token> peek(const int offset = 0) const {
            if (m_Index + offset >= m_Tokens.size()) {
                return std::nullopt;
            }
            return m_Tokens[m_Index + offset];
        }

        Token consume() { return m_Tokens[m_Index++]; }

        bool match(TokenType type) {
            auto tok = peek();
            if (!tok || tok->type != type)
                return false;

            consume();
            return true;
        }

        Token expect(TokenType type, const char* errorMsg) {
            auto tok = peek();
            if (!tok || tok->type != type) {
                std::cerr << errorMsg << "\n";
                std::exit(1);
            }
            return consume();
        }

        const std::vector<Token> m_Tokens;
        size_t m_Index;
    };

}  // namespace Compiler
