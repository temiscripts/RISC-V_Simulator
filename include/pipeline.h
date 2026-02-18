#ifndef PIPELINE_H
#define PIPELINE_H

#include "instruction.h"
#include <optional>
#include "cache.h"
#include <vector>


struct PipelineStage {
    std::optional<Instruction> instr1;
    std::optional<Instruction> instr2;

    int cycleEntered = 0;
    int alu_result1 = 0, alu_result2 = 0;
    int mem_data1 = 0, mem_data2 = 0;
};



struct Pipeline {
    PipelineStage IF, ID, EX, MEM, WB;

    PipelineStage nextIF, nextID, nextEX, nextMEM, nextWB;

    void stage_IF(const std::vector<Instruction>& program, int &pc);
    void stage_ID();
    void stage_EX(int regs[]);
    void stage_MEM(DirectMappedCache &cache, int memory[], int regs[]);
    void stage_WB(int regs[]);

    void commit();  
};


#endif
