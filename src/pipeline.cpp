#include "pipeline.h"
#include "cache.h"
#include <iostream>

using namespace std;

void Pipeline::commit() {
    IF  = nextIF;
    ID  = nextID;
    EX  = nextEX;
    MEM = nextMEM;
    WB  = nextWB;

    nextIF = nextID = nextEX = nextMEM = nextWB = PipelineStage();
}

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

    if (!MEM.instr.has_value()) return;

    Instruction inst = MEM.instr.value();

        nextWB = MEM;


    if (inst.opcode == OpCode::LW) {
        int value;
        cache.read(MEM.alu_result, value);
        nextWB.mem_data = value;
        cout << "[MEM] LW from " << MEM.alu_result << endl;
    }
    else if (inst.opcode == OpCode::SW) {
        cache.write(MEM.alu_result, regs[inst.rs2]);
        memory[MEM.alu_result] = regs[inst.rs2];
        cout << "[MEM] SW to " << MEM.alu_result << endl;
    }

}
void Pipeline::stage_EX(int regs[]) {
    if (!EX.instr.has_value()) return;

    Instruction inst = EX.instr.value();

    int val1 = regs[inst.rs1];
    int val2 = regs[inst.rs2];

    if (MEM.instr.has_value()) {
        Instruction memInst = MEM.instr.value();
        if ((memInst.opcode == OpCode::ADD || memInst.opcode == OpCode::SUB || memInst.opcode == OpCode::LW) &&
            memInst.rd != 0) {
            if (memInst.rd == inst.rs1) {
                val1 = MEM.alu_result;
                cout << "[FORWARD] EX gets val1 from MEM stage for x" << inst.rs1 << endl;
            }
            if (memInst.rd == inst.rs2) {
                val2 = MEM.alu_result;
                cout << "[FORWARD] EX gets val2 from MEM stage for x" << inst.rs2 << endl;
            }
        }
    }

    if (WB.instr.has_value()) {
        Instruction wbInst = WB.instr.value();
        if ((wbInst.opcode == OpCode::ADD || wbInst.opcode == OpCode::SUB || wbInst.opcode == OpCode::LW) &&
            wbInst.rd != 0) {
            if (wbInst.rd == inst.rs1) {
                val1 = (wbInst.opcode == OpCode::LW) ? WB.mem_data : WB.alu_result;
                cout << "[FORWARD] EX gets val1 from WB stage for x" << inst.rs1 << endl;
            }
            if (wbInst.rd == inst.rs2) {
                val2 = (wbInst.opcode == OpCode::LW) ? WB.mem_data : WB.alu_result;
                cout << "[FORWARD] EX gets val2 from WB stage for x" << inst.rs2 << endl;
            }
        }
    }

    nextMEM = EX;

    switch (inst.opcode) {
        case OpCode::ADD:
            nextMEM.alu_result = val1 + val2;
            cout << "[EX] ADD x" << inst.rd << " = " << val1 << " + " << val2 << " -> " << nextMEM.alu_result << endl;
            break;
        case OpCode::SUB:
            nextMEM.alu_result = val1 - val2;
            cout << "[EX] SUB x" << inst.rd << " = " << val1 << " - " << val2 << " -> " << nextMEM.alu_result << endl;
            break;
        case OpCode::LW:
        case OpCode::SW:
            nextMEM.alu_result = val1 + inst.imm;
            cout << "[EX] Mem Addr Calc: " << val1 << " + " << inst.imm << " = " << nextMEM.alu_result << endl;
            break;
        case OpCode::BEQ:
            if (val1 == val2) {
                cout << "[EX] BEQ TAKEN to PC=" << inst.imm << endl;
            } else {
                cout << "[EX] BEQ NOT taken" << endl;
            }
            break;
        default:
            break;
    }

    EX.instr.reset();
}

void Pipeline::stage_ID() {

    if (EX.instr.has_value() && ID.instr.has_value()) {
        Instruction exInst = EX.instr.value();
        Instruction idInst = ID.instr.value();

        if (exInst.opcode == OpCode::LW &&
            (idInst.rs1 == exInst.rd || idInst.rs2 == exInst.rd)) {

            cout << "[STALL] Load-Use hazard detected\n";

            nextEX = PipelineStage();
            nextID = ID;             
            return;
        }
    }

    nextEX = ID;
    nextID = IF;
}


void Pipeline::stage_IF(const vector<Instruction> &program, int &pc) {

    if (nextID.instr.has_value()) return;  

    if (pc < program.size()) {
        nextIF.instr = program[pc++];
        cout << "[IF] Fetched instruction at PC " << pc-1 << endl;
    }
}
