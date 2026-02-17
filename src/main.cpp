#include <iostream>
#include <vector>
#include "instruction.h"
#include "pipeline.h"
#include "cache.h"

using namespace std;

// --- 1. Register File and Memory ---
int regs[32] = {0};
int memory[1024] = {0};

// --- 2. Pipeline Stages ---
Pipeline pipeline;

// --- 3. Program Counter ---
int pc = 0;
bool branch_taken = false;

// --- 4. Small Test Program ---
vector<Instruction> program = {
    {OpCode::LW,  1, 0, 0, 10, "LW x1, 10(x0)"},
    {OpCode::LW,  2, 0, 0, 11, "LW x2, 11(x0)"},
    {OpCode::ADD, 3, 1, 2, 0,  "ADD x3, x1, x2"},
    {OpCode::SW,  0, 0, 3, 20, "SW x3, 20(x0)"},
    {OpCode::BEQ, 0, 0, 0, 6,  "BEQ x0, x0, 6"},
    {OpCode::SUB, 1, 1, 2, 0,  "SUB x1, x1, x2"},
    {OpCode::ADD, 4, 3, 3, 0,  "ADD x4, x3, x3"}
};

// --- 5. Direct-Mapped Cache ---
DirectMappedCache cache(4);

// --- Helper: Print Pipeline State ---
void printPipelineState(int cycle) {
    cout << "\n=== CYCLE " << cycle << " ===\n";
    cout << "IF : " << (pipeline.IF.instr ? pipeline.IF.instr->raw : "Empty") << endl;
    cout << "ID : " << (pipeline.ID.instr ? pipeline.ID.instr->raw : "Empty") << endl;
    cout << "EX : " << (pipeline.EX.instr ? pipeline.EX.instr->raw : "Empty") << endl;
    cout << "MEM: " << (pipeline.MEM.instr ? pipeline.MEM.instr->raw : "Empty") << endl;
    cout << "WB : " << (pipeline.WB.instr ? pipeline.WB.instr->raw : "Empty") << endl;
}

// --- 6. Pipeline Stage Functions ---
void stageWB() {
    if (!pipeline.WB.instr) return;
    auto inst = pipeline.WB.instr.value();
    if (inst.opcode == OpCode::LW) {
        regs[inst.rd] = memory[inst.imm];
        cout << "  [WB] Wrote " << memory[inst.imm] << " to x" << inst.rd << endl;
    } else if (inst.opcode == OpCode::ADD) {
        regs[inst.rd] = regs[inst.rs1] + regs[inst.rs2];
        cout << "  [WB] Wrote " << regs[inst.rd] << " to x" << inst.rd << endl;
    } else if (inst.opcode == OpCode::SUB) {
        regs[inst.rd] = regs[inst.rs1] - regs[inst.rs2];
        cout << "  [WB] Wrote " << regs[inst.rd] << " to x" << inst.rd << endl;
    }
}

void stageMEM() {
    if (!pipeline.MEM.instr) return;
    auto inst = pipeline.MEM.instr.value();
    if (inst.opcode == OpCode::LW) {
        int dummy;
        bool hit = cache.read(inst.imm, dummy);
        memory[inst.imm] = memory[inst.imm]; // memory already has value
    } else if (inst.opcode == OpCode::SW) {
        int dummy;
        cache.write(inst.imm, regs[inst.rs2]);
        memory[inst.imm] = regs[inst.rs2];
        cout << "  [MEM] Stored " << regs[inst.rs2] << " to Addr " << inst.imm << endl;
    }
}

void stageEX() {
    if (!pipeline.EX.instr) return;
    auto inst = pipeline.EX.instr.value();
    if (inst.opcode == OpCode::BEQ) {
        if (regs[inst.rs1] == regs[inst.rs2]) {
            pc = inst.imm;
            branch_taken = true;
            cout << "  [EX] Branch TAKEN! PC -> " << pc << endl;
        } else {
            cout << "  [EX] Branch NOT taken.\n";
        }
    }
}

// --- 7. Main Simulation Loop ---
int main() {
    // Initialize memory values for LW testing
    memory[10] = 500;
    memory[11] = 20;

    int cycle = 1;

    while (pc < program.size() || pipeline.IF.instr || pipeline.ID.instr || pipeline.EX.instr || pipeline.MEM.instr || pipeline.WB.instr) {
        printPipelineState(cycle);

        // Advance pipeline (WB last)
        stageWB();
        stageMEM();
        stageEX();

        pipeline.WB = pipeline.MEM;
        pipeline.MEM = pipeline.EX;
        pipeline.EX = pipeline.ID;
        pipeline.ID = pipeline.IF;

        // Fetch next instruction
        if (!branch_taken && pc < program.size()) {
            pipeline.IF.instr = program[pc++];
        } else {
            pipeline.IF.instr = {};
            branch_taken = false;
        }

        cycle++;
    }

    cout << "\n=== Final Registers ===\n";
    for (int i = 0; i < 8; i++)
        cout << "x" << i << " = " << regs[i] << endl;

    return 0;
}
