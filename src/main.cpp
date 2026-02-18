#include <iostream>
#include <vector>
#include "instruction.h"
#include "pipeline.h"
#include "cache.h"

using namespace std;

const int NUM_REGS = 32;
int regs[NUM_REGS] = {0};

const int MEM_SIZE = 1024;
int memory[MEM_SIZE] = {0};

Pipeline pipeline;
DirectMappedCache cache(4); 

int pc = 0;
int cycle_count = 0;


void stage_WB() {
    if (!pipeline.WB.instr.has_value()) return;

    Instruction inst = pipeline.WB.instr.value();

    switch(inst.opcode) {
        case OpCode::ADD:
        case OpCode::SUB:
            regs[inst.rd] = pipeline.WB.cycleEntered;
            cout << "  [WB] Wrote " << pipeline.WB.cycleEntered << " to x" << inst.rd << endl;
            break;
        case OpCode::LW:
            regs[inst.rd] = pipeline.WB.cycleEntered;
            cout << "  [WB] Loaded " << pipeline.WB.cycleEntered << " into x" << inst.rd << endl;
            break;
        default:
            break;
    }

    pipeline.WB.instr.reset();
}

void stage_MEM() {
    pipeline.WB = pipeline.MEM; 
    if (!pipeline.MEM.instr.has_value()) return;

    Instruction inst = pipeline.MEM.instr.value();
    int addr = pipeline.MEM.cycleEntered; 
    int val;

if (inst.opcode == OpCode::LW) {
    int value;
    bool hit = cache.read(EX_MEM.alu_result, value);
    MEM_WB.mem_read_data = value;
} else if (inst.opcode == OpCode::SW) {
    cache.write(EX_MEM.alu_result, regs[inst.rs2]);
}


    pipeline.MEM.instr.reset();
}

void stage_EX() {
    pipeline.MEM = pipeline.EX;

    if (!pipeline.EX.instr.has_value()) return;

    Instruction inst = pipeline.EX.instr.value();
    int result = 0;

    switch(inst.opcode) {
        case OpCode::ADD:
            result = regs[inst.rs1] + regs[inst.rs2];
            cout << "  [EX] ADD x" << inst.rd << " = " << regs[inst.rs1] << " + " << regs[inst.rs2] << " -> " << result << endl;
            break;
        case OpCode::SUB:
            result = regs[inst.rs1] - regs[inst.rs2];
            cout << "  [EX] SUB x" << inst.rd << " = " << regs[inst.rs1] << " - " << regs[inst.rs2] << " -> " << result << endl;
            break;
        case OpCode::LW:
        case OpCode::SW:
            result = regs[inst.rs1] + inst.imm;
            cout << "  [EX] Memory Addr Calc: " << result << endl;
            break;
        case OpCode::BEQ:
            if (regs[inst.rs1] == regs[inst.rs2]) {
                pc = inst.imm;
                branch_taken = true;
                cout << "  [EX] BEQ TAKEN to PC=" << pc << endl;
            } else {
                cout << "  [EX] BEQ NOT taken" << endl;
            }
            break;
    }

    pipeline.MEM.cycleEntered = result;
    pipeline.EX.instr.reset();
}


void stage_ID() {
    pipeline.EX = pipeline.ID;
    pipeline.ID.instr.reset();
}

void stage_IF(const vector<Instruction> &program) {
    if (pc < program.size()) {
        pipeline.ID.instr = program[pc++];
        cout << "  [IF] Fetched PC[" << pc-1 << "]" << endl;
    }
}

void run_cycle(const vector<Instruction>& program, DirectMappedCache &cache, int memory[], int regs[]) {
    stage_WB(MEM_WB, regs);
    stage_MEM(EX_MEM, MEM_WB, cache, memory);
    stage_EX(ID_EX, EX_MEM, regs);
    stage_ID(IF_ID, ID_EX);
    stage_IF(program, IF_ID);
}


int main() {
    vector<Instruction> program = {
        {OpCode::LW, 1, 0, 0, 10, "LW x1, 10(x0)"},
        {OpCode::LW, 2, 0, 0, 11, "LW x2, 11(x0)"},
        {OpCode::ADD,3,1,2,0, "ADD x3, x1, x2"},
        {OpCode::SW, 0, 0, 3, 20,"SW x3, 20(x0)"},
        {OpCode::SUB,1,1,2,0, "SUB x1, x1, x2"}
    };

    memory[10] = 500;
    memory[11] = 20;

    for (int i = 0; i < 10; i++) {
        run_cycle(program);
    }

    return 0;
}
