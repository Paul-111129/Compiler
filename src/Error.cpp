#include "Error.h"

#include <format>
#include <iostream>

namespace Glassy {

void Error(SourceLocation loc, const std::string& msg) {
    std::cerr << std::format("{} [Ln {}, Col {}]\n", msg, loc.line, loc.column);
    std::cin.get();
    std::exit(1);
}

void Error(const std::string& msg) {
    std::cerr << msg << "\n";
    std::cin.get();
    std::exit(1);
}

} // namespace Glassy
