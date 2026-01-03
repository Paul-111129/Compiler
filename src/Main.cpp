#include "pch.h"
#include "Generator.h"
#include "Parser.h"
#include "Tokenizer.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        Glassy::Error("Usage: <program> <source_file>.glassy");
    }

    std::filesystem::path inputFilePath = argv[1];

    if (inputFilePath.extension() != ".glassy") {
        Glassy::Error(inputFilePath.string() + " is not a Glassy source file");
    }
    if (!inputFilePath.has_extension()) {
        inputFilePath.replace_extension(".glassy");
    }

    std::filesystem::path outputFilePath = inputFilePath;
    outputFilePath.replace_extension(".asm");

    std::ifstream inputFile(inputFilePath, std::ios::in | std::ios::binary);
    if (!inputFile) {
        Glassy::Error("Failed to open file: " + inputFilePath.string());
    }

    std::string sourceCode((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    sourceCode += '\n'; // ensure last line ends with newline
    inputFile.close();

    // tokenize, parse, and generate assembly
    Glassy::Tokenizer tokenizer(sourceCode);
    Glassy::Parser parser(tokenizer.Tokenize());
    Glassy::Generator generator(parser.ParseProgram());

    // write output file
    std::ofstream outputFile(outputFilePath);
    if (!outputFile) {
        Glassy::Error("Failed to write to file: " + outputFilePath.string());
    }
    outputFile << generator.GenerateAsm();
    outputFile.close();

    std::cout << "Output written to " << outputFilePath << "\n";

    return 0;
}
