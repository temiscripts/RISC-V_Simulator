#include "pipeline.h"
#include "cache.h"
#include <iostream>

using namespace std;


void Pipeline::stage_WB(int regs[]) {
    if (!WB.instr.has_value()) return;

    Instruction inst = WB.instr.value();

    if (inst.opcode == OpCode::ADD || inst.opcode == OpCode::SUB) {
        regs[inst.rd] = WB.alu_result;
        cout << "[WB] Writing " << WB.alu_result << " to x" << inst.rd << endl;
    }
    else if (inst.opcode == OpCode::LW) {
        regs[inst.rd] = WB.mem_data;
        cout << "[WB] Writing " << WB.mem_data << " to x" << inst.rd << endl;
    }

    WB.instr.reset();
}

void Pipeline::stage_MEM(DirectMappedCache &cache, int memory[], int regs[]) {
    WB = MEM;

    if (!MEM.instr.has_value()) return;

    Instruction inst = MEM.instr.value();

    if (inst.opcode == OpCode::LW) {
        int value;
        cache.read(MEM.alu_result, value);
        WB.mem_data = memory[MEM.alu_result];
        cout << "[MEM] LW from " << MEM.alu_result << endl;
    }
    else if (inst.opcode == OpCode::SW) {
        cache.write(MEM.alu_result, regs[inst.rs2]);
        memory[MEM.alu_result] = regs[inst.rs2];
        cout << "[MEM] SW to " << MEM.alu_result << endl;
    }

    MEM.instr.reset();
}

void Pipeline::stage_EX(int regs[]) {
    MEM = EX;

    if (!EX.instr.has_value()) return;

    Instruction inst = EX.instr.value();

    if (inst.opcode == OpCode::ADD) {
        MEM.alu_result = regs[inst.rs1] + regs[inst.rs2];
    }
    else if (inst.opcode == OpCode::SUB) {
        MEM.alu_result = regs[inst.rs1] - regs[inst.rs2];
    }
    else if (inst.opcode == OpCode::LW || inst.opcode == OpCode::SW) {
        MEM.alu_result = regs[inst.rs1] + inst.imm;
    }

    EX.instr.reset();
}

void Pipeline::stage_ID(int regs[]) {
    EX = ID;
    ID.instr.reset();
}

void Pipeline::stage_IF(const vector<Instruction> &program, int &pc) {
    if (pc < program.size()) {
        ID.instr = program[pc++];
        cout << "[IF] Fetched instruction at PC " << pc-1 << endl;
    }
}

