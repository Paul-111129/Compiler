#pragma once

#include "Tokenizer.h"

namespace Glassy {

void Error(SourceLocation loc, const std::string& msg);
void Error(const std::string& msg);

} // namespace Glassy
