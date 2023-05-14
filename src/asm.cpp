#include "asm.h"

AsmState::AsmState() : stack(std::stack<int>()), regs{ 0 } {}