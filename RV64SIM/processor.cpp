/*
 * =====================================================================================
 *
 *       Filename:  procecssor.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/02/2018 01:28:52 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yi-Ting, Hsieh (a1691807), a1691807@student.adelaide.edu.au
 *   Organization:  
 *
 * =====================================================================================
 */
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstdio>

#include "processor.h"
#include "Instruction.h"
using namespace std;

// Constructor
processor::processor(memory *main_memory, bool verbose){
    memo = main_memory;

    this->PC = 0x00;
    memset(X,0,sizeof(X));
    BreakPoint = 0xffffffffffffffff;
    this->instruction_count = 0;
    this->verbose = verbose;
}

// Display PC value
void processor::show_pc(){
    cout << hex << setw(16) << setfill('0') << this->PC << endl;
}

// Set PC to new value
void processor::set_pc(uint64_t new_pc){
    PC = new_pc;
}

// Display register value
void processor::show_reg(unsigned int reg_num){
    //if(reg_num & 0x)
    uint64_t temp;
    if(reg_num==0)
        temp = 0x0000000000000000;
    else
        temp = X[reg_num];

    cout << hex << setw(16) << setfill('0') << temp << endl;

}

// Set register to new value
void processor::set_reg(unsigned int reg_num, uint64_t new_value){
    this->temp_reg = reg_num;
    if(reg_num!=0)
        this->X[reg_num] = new_value;
    else if(reg_num==0)
        this->X[reg_num] = 0x0000000000000000;
}

// Execute a number of instructions
void processor::execute(unsigned int num, bool breakpoint_check){

    for(int i=0;i<(int)num;i++){

        if(this->PC%4!=0){
            //cout << "Unaligned pc: " << setw(16) << setfill('0') << hex << PC << endl;

            set_csr(0x343,this->PC);

            this->interrupt = true;
            this->ebreak_cause_code = 0;
            //cout << hex << "this PC right now is: " << this->PC << endl;

            set_csr(0x342,0x0000000000000000);
            set_csr(0x341, 4*(this->PC/4));
            set_csr(0x300,0x0000000200001800);
            set_pc(mtvec_reg);


            return;
        }


        if(breakpoint_check&&(BreakPoint==this->PC)){
            cout << "Breakpoint reached at " << setw(16) << setfill('0') << hex << PC << endl;
            BreakPoint = 0xffffffffffffffff;
            break;
        }

        //uint64_t ret =
        checkInstruction(memo,this,this->X,this->PC,this->memo->read_doubleword(this->PC),this->verbose);


        if(breakpoint_check && (BreakPoint==this->PC)){
            cout << "Breakpoint reached at " << setw(16) << setfill('0') << hex << PC << endl;
            BreakPoint = 0xffffffffffffffff;
            break;
        }

        this->instruction_count++;

        if(ebreak){
            ebreak = false;
        }else{
            this->PC += 4;
        }
    }

}

// Clear breakpoint
void processor::clear_breakpoint(){
    BreakPoint = 0xffffffffffffffff;
}

// Set breakpoint at an address
void processor::set_breakpoint(uint64_t address){
    if(this->verbose){
        cout << "Breakpoint set at " << hex << setw(16) << setfill('0') << address << endl;
      }
    BreakPoint = address;
}

uint64_t processor::get_instruction_count(){
    return this->instruction_count;
}

// Used for Postgraduate assignment. Undergraduate assignment can return 0.
uint64_t processor::get_cycle_count(){
    return 0;
}

void processor::count_cycle(int cycle){
   //this->instruction_count += cycle;
}

uint64_t processor::get_PC(){
    return this->PC;
}

uint64_t processor::get_reg(uint64_t num){
    if(num == 0 ){
      return 0x00;
    }else{
      return X[num];
    }
}


// Show privilege level
void processor::show_prv(){
    if(prv_num==3)
        cout << this->prv_num << " (machine)"<< endl;
    else if (prv_num==0)
        cout << this->prv_num << " (user)"<< endl;
}

// Set privilege level
void processor::set_prv(unsigned int prv_num){
    this->prv_num = prv_num;
}

// Display CSR value
void processor::show_csr(unsigned int csr_num){

    if(csr_num==0XF11){
        cout << hex << setw(16) << setfill('0') << 0 << endl;
    }else if(csr_num==0XF12){
        cout << hex << setw(16) << setfill('0') << 0 << endl;
    }else if(csr_num==0XF13){
        cout << "2018020000000000" << endl;
    }else if(csr_num==0XF14){
        cout << hex << setw(16) << setfill('0') << 0 << endl;
    }else if(csr_num==0X300){
        cout << hex << setw(16) << setfill('0') << mstatus_reg << endl;
    }else if(csr_num==0X301){
        cout << hex << setw(16) << setfill('0') << misa_reg << endl;
    }else if(csr_num==0X304){
        cout << hex << setw(16) << setfill('0') << mie_reg << endl;
    }else if(csr_num==0X305){
        cout << hex << setw(16) << setfill('0') << mtvec_reg << endl;
    }else if(csr_num==0X340){
        cout  << hex << setw(16) << setfill('0') << mscratch_reg << endl;
    }else if(csr_num==0X341){
        cout << hex << setw(16) << setfill('0') << mepc_reg << endl;
    }else if(csr_num==0X342){
        cout << hex  << setw(16) << setfill('0') << mcause_reg << endl;
    }else if(csr_num==0X343){
        cout  << hex << setw(16) << setfill('0') << mtval_reg << endl;
    }else if(csr_num==0X344){
        cout << hex << setw(16) << setfill('0') << mip_reg << endl;
    }else{
        cout << "Illegal CSR number" << endl;
    }
}

// Set CSR to new value
void processor::set_csr(unsigned int csr_num, uint64_t new_value){

    //set_prv(3);

    if(csr_num==0XF11){
        cout << "Illegal write to read-only CSR" << endl;
    }else if(csr_num==0XF12){
        cout << "Illegal write to read-only CSR" << endl;
    }else if(csr_num==0XF13){
        cout << "Illegal write to read-only CSR" << endl;
    }else if(csr_num==0XF14){
        cout << "Illegal write to read-only CSR" << endl;
    }else if(csr_num==0X300){
        mstatus_reg = (0x200000000 | (new_value & 0x0000000000001888));
    }else if(csr_num==0X301){
        // misa: all bits fixed
        //misa_reg = (misa_reg | ());
    }else if(csr_num==0X304){
        // only usie, msie, utie, mtie, ueie, meie implemented
        mie_reg = (new_value & 0x0000000000000999);
    }else if(csr_num==0X305){
        //cout << hex << (new_value & 0x02) << endl;
        //Direct mode
        if((new_value & 0x01)==0x00){
            mtvec_reg = (new_value & 0xfffffffffffffffc);
            ebreak_tempValue = mtvec_reg - 4;
        }
        //Vector mode
        else if((new_value & 0x01)==0x01){
            //cout << "vector mode" << endl;
            mtvec_reg = (new_value & 0xffffffffffffff01);
            ebreak_tempValue = mtvec_reg - 4 + 4 * ebreak_cause_code;
        }
        else{
            mtvec_reg = 0x00;
//            ebreak_tempValue = 0;
        }



    }else if(csr_num==0X340){
        mscratch_reg = new_value;
    }else if(csr_num==0X341){
        mepc_reg = (new_value & 0xfffffffffffffffc);
    }else if(csr_num==0X342){
        mcause_reg = (new_value & 0x800000000000000f);
    }else if(csr_num==0X343){
        mtval_reg = new_value;
    }else if(csr_num==0X344){
        mip_reg = (new_value & 0x0000000000000999);
    }else{
        cout << "Illegal CSR number" << endl;
    }
}


uint64_t processor::get_csr(uint64_t csr_num){
    if(csr_num==0XF11){
        return 0x00;
    }else if(csr_num==0XF12){
        return 0x00;
    }else if(csr_num==0XF13){
        return 0x2018020000000000;
    }else if(csr_num==0XF14){
        return 0x00;
    }else if(csr_num==0X300){
        return mstatus_reg;
    }else if(csr_num==0X301){
        return misa_reg;
    }else if(csr_num==0X304){
        return mie_reg;
    }else if(csr_num==0X305){
        return mtvec_reg;
    }else if(csr_num==0X340){
        return mepc_reg;
    }else if(csr_num==0X341){
        return mscratch_reg;
    }else if(csr_num==0X342){
        return mcause_reg;
    }else if(csr_num==0X343){
        return mtval_reg;
    }else if(csr_num==0X344){
        return mip_reg;
    }else{
        cout << "Illegal CSR number" << endl;
    }


}
