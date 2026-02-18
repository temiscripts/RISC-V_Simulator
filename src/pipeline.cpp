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

    nextMEM = EX;

    if (inst.opcode == OpCode::ADD || inst.opcode == OpCode::SUB) {
        nextMEM.alu_result = regs[inst.rs1] + regs[inst.rs2];
    }
    else if (inst.opcode == OpCode::LW || inst.opcode == OpCode::SW) {
        nextMEM.alu_result = regs[inst.rs1] + inst.imm;
    }

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
