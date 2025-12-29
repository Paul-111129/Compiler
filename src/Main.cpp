#include "pch.h"
#include "Tokenizer.h"
#include "Parser.h"

int main() {
    const std::string_view input = "x = 3 + 4 * 5;";

    Compiler::Tokenizer tokenizer(input);
    const auto tokens = tokenizer.Tokenize();

    Compiler::Parser parser(tokens);
    const auto program = parser.ParseProgram();

    std::cout << "Tokens: \n";
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];

        std::cout << i + 1 << ": " << token.lexeme;
        if (token.type == Compiler::TokenType::LITERAL) {
            std::cout << " (" << token.value << ")";
        }
        std::cout << "\n";
    }

    std::cin.get();
    return 0;
}
