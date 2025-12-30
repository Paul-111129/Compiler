#include "pch.h"
#include "Parser.h"

namespace Glassy {

Parser::Parser(const std::vector<Token> &tokens) : m_Tokens(tokens), m_Index(0) {}

std::unique_ptr<Program> Parser::ParseProgram() {
    m_Index = 0;

    std::unique_ptr<Program> program = std::make_unique<Program>();

    while (peek().has_value()) {
        program->statements.push_back(std::move(parseStatement()));
    }

    return program;
}

std::unique_ptr<Expression> Parser::parseFactor() {
    auto tok = peek();
    if (!tok) {
        Error(m_Tokens[m_Tokens.size() - 1].line, "Expected factor");
    }

    if (tok->type == LITERAL) {
        consume();
        return std::make_unique<NumberExpr>(*tok->value);
    }

    if (tok->type == SEPARATOR && *std::get_if<Separator>(&*tok->subType) == L_PAREN) {
        consume(); // '('
        auto expr = parseExpression();
        expect(SEPARATOR, "Expected ')'");
        return expr;
    }

    Error(tok->line, "Unexpected token in factor");
    return nullptr; // never reached
}

std::unique_ptr<Expression> Parser::parseTerm() {
    auto left = parseFactor();

    while (true) {
        auto tok = peek();
        if (!tok || tok->type != OPERATOR) {
            break;
        }

        Operator op = *std::get_if<Operator>(&*tok->subType);

        if (op != STAR && op != SLASH) {
            break;
        }

        consume(); // consume operator
        auto right = parseFactor();

        left = std::make_unique<BinaryExpr>(OperatorToChar[op], std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<Expression> Parser::parseExpression() {
    auto left = parseTerm();

    while (true) {
        auto tok = peek();
        if (!tok || tok->type != OPERATOR) {
            break;
        }

        Operator op = *std::get_if<Operator>(&*tok->subType);
        if (op != Operator::PLUS && op != MINUS) {
            break;
        }

        consume(); // consume operator
        auto right = parseTerm();

        left = std::make_unique<BinaryExpr>(OperatorToChar[op], std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<Statement> Parser::parseStatement() {
    auto tok = peek();
    if (tok && tok->type == KEYWORD && *std::get_if<Keyword>(&*tok->subType) == EXIT) {
        consume(); // consume 'exit'

        uint8_t exitVal = static_cast<uint8_t>(*expect(LITERAL, "Expected literal").value);

        expect(SEPARATOR, "Expected ';'");
        return std::make_unique<ExitStmt>(exitVal);
    }

    std::string name = *expect(IDENTIFIER, "Expected identifier").identifer;

    expect(OPERATOR, "Expected '='");

    auto expr = parseExpression();

    expect(SEPARATOR, "Expected ';'");

    return std::make_unique<AssignStmt>(name, std::move(expr));
}

std::optional<Token> Parser::peek(const int offset) const {
    if (m_Index + offset >= m_Tokens.size()) {
        return std::nullopt;
    }
    return m_Tokens[m_Index + offset];
}

Token Parser::consume() {
    if (m_Index >= m_Tokens.size()) {
        Error(m_Tokens[m_Index - 1].line, "Unexpected end of input");
    }
    return m_Tokens[m_Index++];
}

bool Parser::match(TokenType type) {
    auto tok = peek();
    if (!tok || tok->type != type) {
        return false;
    }

    return true;
}

Token Parser::expect(TokenType type, const char *msg) {
    auto tok = peek();
    if (!tok) {
        Error(m_Tokens[m_Tokens.size() - 1].line, msg);
    }
    if (tok->type != type) {
        Error(tok->line, msg);
    }
    return consume();
}

} // namespace Glassy
