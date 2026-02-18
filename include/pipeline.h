#ifndef PIPELINE_H
#define PIPELINE_H

#include "instruction.h"
#include <queue>
#include <optional>
#include "cache.h"
#include <vector>


struct PipelineStage {
    std::optional<Instruction> instr;
    int cycleEntered = 0;

    int alu_result = 0; 
    int mem_data = 0;   
};


struct Pipeline {
    PipelineStage IF, ID, EX, MEM, WB;

    void stage_IF(const std::vector<Instruction>& program, int &pc);
    void stage_ID(int regs[]);
    void stage_EX(int regs[]);
    void stage_MEM(DirectMappedCache &cache, int memory[], int regs[]);
    void stage_WB(int regs[]);
};

#endif
