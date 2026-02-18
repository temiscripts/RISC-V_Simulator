#include "pipeline.h"
#include "cache.h"
#include <iostream>

using namespace std;

void Pipeline::commit() {
    WB  = nextWB;
    MEM = nextMEM;
    EX  = nextEX;
    ID  = nextID;
    IF  = nextIF;

    nextWB = nextMEM = nextEX = nextID = nextIF = PipelineStage();

}

void Pipeline::stage_WB(int regs[]) {

    auto writeback = [&](optional<Instruction>& instr,
                         int alu_result,
                         int mem_data)
    {
        if (!instr.has_value()) return;

        Instruction inst = instr.value();

        if (inst.opcode == OpCode::ADD ||
            inst.opcode == OpCode::SUB)
        {
            regs[inst.rd] = alu_result;
            cout << "[WB] x" << inst.rd << " = "
                 << alu_result << endl;
        }
        else if (inst.opcode == OpCode::LW)
        {
            regs[inst.rd] = mem_data;
            cout << "[WB] x" << inst.rd << " = "
                 << mem_data << endl;
        }
        total_instructions++;

    };

    writeback(WB.instr1, WB.alu_result1, WB.mem_data1);
    writeback(WB.instr2, WB.alu_result2, WB.mem_data2);
}

void Pipeline::stage_MEM(DirectMappedCache& cache,
                         int memory[],
                         int regs[])
{
    nextWB = MEM;

    auto mem_access = [&](optional<Instruction>& instr,
                          int alu_result,
                          int& mem_data)
    {
        if (!instr.has_value()) return;

        Instruction inst = instr.value();

        if (inst.opcode == OpCode::LW) {
            cache.read(alu_result, mem_data);
            cout << "[MEM] LW from "
                 << alu_result << endl;
        }
        else if (inst.opcode == OpCode::SW) {
            cache.write(alu_result, regs[inst.rs2]);
            memory[alu_result] = regs[inst.rs2];
            cout << "[MEM] SW to "
                 << alu_result << endl;
        }
    };

    mem_access(MEM.instr1,
               MEM.alu_result1,
               nextWB.mem_data1);

    mem_access(MEM.instr2,
               MEM.alu_result2,
               nextWB.mem_data2);
}

void Pipeline::stage_EX(int regs[]) {

    nextMEM = EX;

    auto execute = [&](optional<Instruction>& instr,
                       int& alu_result)
    {
        if (!instr.has_value()) return;

        Instruction inst = instr.value();

        int val1 = regs[inst.rs1];
        int val2 = regs[inst.rs2];

        switch (inst.opcode) {

            case OpCode::ADD:
                alu_result = val1 + val2;
                break;

            case OpCode::SUB:
                alu_result = val1 - val2;
                break;

            case OpCode::LW:
            case OpCode::SW:
                alu_result = val1 + inst.imm;
                break;

            default:
                break;
        }
    };

    execute(EX.instr1, nextMEM.alu_result1);
    execute(EX.instr2, nextMEM.alu_result2);
}

void Pipeline::stage_ID() {

    nextEX = PipelineStage();

    if (!ID.instr1.has_value()) {
        nextID = IF;
        return;
    }

    nextEX.instr1 = ID.instr1;

    lane1_issued++;
    if (ID.instr2.has_value()) {

        Instruction i1 = ID.instr1.value();
        Instruction i2 = ID.instr2.value();

        bool dependency = false;

        if (i2.rs1 == i1.rd && i1.rd != 0)
            dependency = true;

        if (i2.rs2 == i1.rd && i1.rd != 0)
            dependency = true;

        if (i1.rd == i2.rd && i1.rd != 0)
            dependency = true;

        if (dependency) {
            stall_count++;
            cout << "[ISSUE] Lane2 stalled due to intra-cycle dependency\n";
        }
        else {
            nextEX.instr2 = ID.instr2;
            lane2_issued++;

        }
    }

    nextID = IF;
}

void Pipeline::stage_IF(const vector<Instruction>& program,
                        int& pc)
{
    if (nextID.instr1.has_value() ||
        nextID.instr2.has_value())
        return;

    if (pc < program.size()) {
        nextIF.instr1 = program[pc++];
        cout << "[IF] Fetched PC "
             << pc-1 << endl;
    }

    if (pc < program.size()) {
        nextIF.instr2 = program[pc++];
        cout << "[IF] Fetched PC "
             << pc-1 << endl;
    }
}
