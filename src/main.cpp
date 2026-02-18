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
        {OpCode::LW, 1, 0, 0, 10},  
    {OpCode::LW, 2, 0, 0, 11}, 

    {OpCode::ADD, 3, 1, 2, 0},  
    {OpCode::SUB, 4, 2, 1, 0},

    {OpCode::SW, 0, 0, 3, 20},  
    {OpCode::SW, 0, 0, 4, 21}, 

    {OpCode::ADD, 5, 1, 3, 0}, 
    };

    memory[10] = 500;
    memory[11] = 20;

int max_cycles = program.size() + 8;

for (int cycle = 0; cycle < max_cycles; cycle++) {

    cout << "\n=== Cycle " << cycle+1 << " ===\n";

    pipeline.total_cycles++;   

    pipeline.stage_WB(regs);
    pipeline.stage_MEM(cache, memory, regs);
    pipeline.stage_EX(regs);
    pipeline.stage_ID();
    pipeline.stage_IF(program, pc);
    pipeline.commit();
}
    cout << "\n===== PERFORMANCE METRICS =====\n";
cout << "Total Cycles: " << pipeline.total_cycles << endl;
cout << "Total Instructions: " << pipeline.total_instructions << endl;
cout << "IPC: "
     << (double)pipeline.total_instructions / pipeline.total_cycles
     << endl;
cout << "Stalls: " << pipeline.stall_count << endl;
cout << "Lane1 Issued: " << pipeline.lane1_issued << endl;
cout << "Lane2 Issued: " << pipeline.lane2_issued << endl;
cout << "Lane Utilization: "
     << (double)(pipeline.lane1_issued + pipeline.lane2_issued) /
        (2 * pipeline.total_cycles) * 100
     << "%\n";

    return 0;
}
