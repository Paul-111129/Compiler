#include "pch.h"
#include "Generator.h"
#include "Parser.h"
#include "Tokenizer.h"

int main() {
    const char *inputFileName = "src.glassy";
    const char *outputFileName = "out.asm";

    // open files
    const std::filesystem::path path = std::filesystem::current_path();

    std::ifstream inputFile(path / inputFileName, std::ios::in | std::ios::binary);
    if (!inputFile) {
        std::cerr << "Failed to open file: " << inputFileName << "\n";
        return 1;
    }

    std::ofstream outputFile(path / outputFileName);
    if (!outputFile) {
        std::cerr << "Failed to create file: " << outputFileName << "\n";
        return 1;
    }

    std::string input = std::string(std::istreambuf_iterator<char>(inputFile), std::istreambuf_iterator<char>());

    Glassy::Tokenizer tokenizer(std::move(input));

    const auto tokens = tokenizer.Tokenize();

    Glassy::Parser parser(tokens);
    auto program = parser.ParseProgram();
    program->print(std::cout);

    Glassy::Generator generator(std::move(program));
    outputFile << generator.GenerateAssembly();

    std::cin.get();
    return 0;
}
