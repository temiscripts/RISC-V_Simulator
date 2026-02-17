#ifndef PIPELINE_H
#define PIPELINE_H

#include "instruction.h"
#include <queue>
#include <optional>

struct PipelineStage {
    std::optional<Instruction> instr; 
    int cycleEntered = 0;    
};

struct Pipeline {
    PipelineStage IF, ID, EX, MEM, WB;
};

#endif
