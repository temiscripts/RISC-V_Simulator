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
    auto &wb = pipeline.WB.instr;
    if (!wb.has_value()) return;

    Instruction inst = wb.value();
    if (inst.opcode == OpCode::ADD || inst.opcode == OpCode::SUB || inst.opcode == OpCode::LW) {
        regs[inst.rd] = inst.opcode == OpCode::LW ? pipeline.WB.cycleEntered : regs[inst.rd];
        cout << "  [WB] Wrote to x" << inst.rd << endl;
    }

    pipeline.WB.instr.reset();
}

void stage_MEM() {
    pipeline.WB = pipeline.MEM; 
    auto &mem_inst = pipeline.MEM.instr;
    if (!mem_inst.has_value()) return;

    Instruction inst = mem_inst.value();

    if (inst.opcode == OpCode::LW) {
        int data = memory[inst.imm]; 
        cache.read(inst.imm, data);  
        pipeline.WB.cycleEntered = data; 
    } else if (inst.opcode == OpCode::SW) {
        int data = regs[inst.rs2];
        memory[inst.imm] = data;
        cache.write(inst.imm, data);
        cout << "  [MEM] Stored " << data << " at Addr " << inst.imm << endl;
    }

    pipeline.MEM.instr.reset();
}

void stage_EX() {
    pipeline.MEM = pipeline.EX; 
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

void run_cycle(const vector<Instruction> &program) {
    cout << "\n=== CYCLE " << ++cycle_count << " ===" << endl;
    stage_WB();
    stage_MEM();
    stage_EX();
    stage_ID();
    stage_IF(program);
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
