#include "Parser.h"
#include "Error.h"

namespace Glassy {

Parser::Parser(const std::vector<Token>& tokens)
    : m_Tokens(tokens), m_Index(0), m_Allocator(4 * 1024 * 1024) {} // 4 MB

Program* Parser::ParseProgram() {
    m_Index = 0;
    Program* program = m_Allocator.alloc<Program>();

    while (peek()) {
        program->statements.push_back(parseStatement());
    }

    return program;
}

Expression* Parser::parseFactor() {
    auto tok = peek();
    if (!tok) {
        Error(m_Tokens.back().location, "Expected factor");
    }

    if (tok->type == LITERAL) {
        consume();
        return m_Allocator.alloc<LiteralExpr>(*tok->GetValue<Literal>());
    } else if (tok->type == IDENTIFIER) {
        consume();
        return m_Allocator.alloc<IdentifierExpr>(*tok->GetValue<Identifier>());
    }

    if (match<Separator>(Separator::L_PAREN)) {
        auto expr = parseExpression();
        expect(SEPARATOR, "Expected ')'");
        return expr;
    }

    Error(tok->location, "Unexpected token in factor");
    return nullptr; // never reached
}

Expression* Parser::parseTerm() {
    auto left = parseFactor();

    while (auto op = match<Operator>(Operator::STAR, Operator::SLASH)) {
        auto right = parseFactor();
        left = m_Allocator.alloc<BinaryExpr>(*op, left, right);
    }

    return left;
}

Expression* Parser::parseExpression() {
    auto left = parseTerm();

    while (auto op = match<Operator>(Operator::PLUS, Operator::MINUS)) {
        auto right = parseTerm();
        left = m_Allocator.alloc<BinaryExpr>(*op, left, right);
    }

    return left;
}

Statement* Parser::parseStatement() {
    if (match<Keyword>(Keyword::EXIT)) {
        auto expr = parseExpression();

        expect(SEPARATOR, "Expected ';'");

        return m_Allocator.alloc<ExitStmt>(expr);
    } else if (match<Keyword>(Keyword::LET)) {
        Identifier name = *expect(IDENTIFIER, "Expected identifer").GetValue<Identifier>();

        expect(OPERATOR, "Expected '='");
        auto expr = parseExpression();
        expect(SEPARATOR, "Expected ';'");

        return m_Allocator.alloc<DeclarStmt>(name, expr);
    }

    Identifier name = *expect(IDENTIFIER, "Expected identifier").GetValue<Identifier>();

    expect(OPERATOR, "Expected '='");
    
    auto expr = parseExpression();
    expect(SEPARATOR, "Expected ';");

    return m_Allocator.alloc<AssignStmt>(name, expr);
}

} // namespace Glassy
