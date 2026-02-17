#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>

enum class OpCode { ADD, SUB, LW, SW, BEQ };

struct Instruction {
    OpCode opcode;
    int rd;   
    int rs1;   
    int rs2; 
    int imm;    // Immediate value, if applicable
    std::string raw; // Optional: original instruction string for debugging
};

#endif
