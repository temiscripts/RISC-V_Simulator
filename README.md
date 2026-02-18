# RISC-V Cycle-Accurate Simulator & Cache Model

[![Language](https://img.shields.io/badge/language-C++-blue)](https://isocpp.org/)  
[![License](https://img.shields.io/badge/license-MIT-green)](LICENSE)

## **Project Overview**

This project is a **cycle-accurate software simulator** for a subset of the RISC-V instruction set. It models a **full 5-stage pipeline (IF, ID, EX, MEM, WB)** and integrates a **direct-mapped cache** to analyze memory access patterns. The simulator is designed for **educational and verification purposes**, providing a detailed view of pipeline execution, hazards, and performance metrics.

---

## **Features**

- **5-Stage Pipeline Simulation:** Instruction Fetch (IF), Instruction Decode (ID), Execute (EX), Memory Access (MEM), Write-Back (WB).  
- **Hazard Detection:** Implements **data hazard detection**, **load-use stall handling**, and **pipeline flushing** for correctness.  
- **Cache Simulation:** Direct-mapped cache tracks **hits, misses, and memory access patterns**.  
- **Logging & Debugging:** Automated **cycle-by-cycle logs** for registers, memory, and pipeline stages.  
- **Modular C++ Design:** Easily extendable for more instructions or pipeline enhancements.

---

## **Getting Started**

### **Prerequisites**

- C++ compiler supporting C++17  
- Linux environment or Windows with MinGW/WSL  

### **Build & Run**

1. Clone the repository and navigate to the directory:
   ```bash
   git clone https://github.com/<your-username>/riscv-simulator.git
   cd riscv-simulator
2. Compile the source code:

```bash
g++ -std=c++17 -Iinclude src/main.cpp src/pipeline.cpp src/cache.cpp -o riscv_sim
```
3. Run the simulator:

```bash
./riscv_sim
```

4. Check the terminal output or logs/ folder for the cycle-by-cycle execution trace.

## Learning Outcomes
Deepened understanding of hardware verification and pipeline architecture.

Practical experience with cycle-accurate simulation and cache modeling.

Hands-on skills in C++ systems programming, modular design, and debugging complex execution flows.

## Contributing
Contributions are welcome! Please open an issue or submit a pull request for:

Additional instructions (RISC-V extensions)

Superscalar pipeline support

Cache simulation enhancements

## License
This project is licensed under the MIT License. See LICENSE for details.