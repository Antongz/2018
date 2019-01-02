#ifndef PROCESSOR_H
#define PROCESSOR_H

/* ****************************************************************
   RISC-V Instruction Set Simulator
   Computer Architecture, Semester 1, 2018

   Class for processor

**************************************************************** */

#include "memory.h"
#include <map>
#include <set>

using namespace std;





class processor {

private:

    // TODO: Add private members here
    memory* memo;
    uint64_t PC;  //program counter
    uint64_t BreakPoint;
    uint64_t X[32];

    bool verbose;
    uint64_t temp_reg;



public:
    bool ebreak = false;
    bool interrupt = false;
    int ebreak_cause_code = 0;
    uint64_t ebreak_tempValue = 0x00;
    uint64_t instruction_count;
    unsigned int prv_num = 3;

    unsigned int mpp = 0;   // machine prev privilege
    unsigned int mpie = 0;  // machine prev interrupt enable
    unsigned int mie = 0;   // machine prev interrupt enable

    unsigned int xPIE;      // holds the value of interrupt-enable bit active prior to the trap
    unsigned int xPP;       // holds the previous privilege mode


    uint64_t mstatus_reg = 0x200000000;
    uint64_t misa_reg = 0x8000000000100100;
    uint64_t mie_reg = 0x00;
    uint64_t mtvec_reg = 0x00;
    uint64_t mepc_reg = 0x00;
    uint64_t mscratch_reg = 0x00;
    uint64_t mcause_reg = 0x00;
    uint64_t mtval_reg = 0x00;
    uint64_t mip_reg = 0x00;

    // Consructor
    processor(memory* main_memory, bool verbose);

    // Display PC value
    void show_pc();

    // Set PC to new value
    void set_pc(uint64_t new_pc);

    // Display register value
    void show_reg(unsigned int reg_num);

    // Set register to new value
    void set_reg(unsigned int reg_num, uint64_t new_value);

    // Execute a number of instructions
    void execute(unsigned int num, bool breakpoint_check);

    // Clear breakpoint
    void clear_breakpoint();

    // Set breakpoint at an address
    void set_breakpoint(uint64_t address);

    uint64_t get_instruction_count();

    // Used for Postgraduate assignment. Undergraduate assignment can return 0.
    uint64_t get_cycle_count();

    void count_cycle(int cycle);

    // Show privilege level
    void show_prv();

    // Set privilege level
    void set_prv(unsigned int prv_num);

    // Display CSR value
    void show_csr(unsigned int csr_num);

    // Set CSR to new value
    void set_csr(unsigned int csr_num, uint64_t new_value);

    uint64_t get_PC();
    uint64_t get_reg(uint64_t num);

    uint64_t get_csr(uint64_t num);


};

#endif
