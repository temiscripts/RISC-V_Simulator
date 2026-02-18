#include <iostream>
#include <vector>
#include "instruction.h"
#include "pipeline.h"
#include "cache.h"

using namespace std;

int main() {
    int regs[32] = {0};
    int memory[1024] = {0};
    int pc = 0;

    Pipeline pipeline;
    DirectMappedCache cache(4);

    vector<Instruction> program = {
        {OpCode::LW, 1, 0, 0, 10, "LW x1, 10(x0)"},
        {OpCode::LW, 2, 0, 0, 11, "LW x2, 11(x0)"},
        {OpCode::ADD,3,1,2,0, "ADD x3, x1, x2"},
        {OpCode::SW, 0, 0, 3, 20,"SW x3, 20(x0)"},
        {OpCode::SUB,1,1,2,0, "SUB x1, x1, x2"}
    };

    memory[10] = 500;
    memory[11] = 20;

    int total_cycles = program.size() + 4;

    for (int cycle = 0; cycle < total_cycles; cycle++) {
        cout << "\n=== Cycle " << cycle+1 << " ===\n";

        pipeline.stage_WB(regs);
        pipeline.stage_MEM(cache, memory, regs);
        pipeline.stage_EX(regs);
        pipeline.stage_ID();
        pipeline.stage_IF(program, pc);
        pipeline.commit();
    }

    return 0;
}
