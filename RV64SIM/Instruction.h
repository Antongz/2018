/*
 * =====================================================================================
 *
 *       Filename:  Instruction.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/12/2018 01:24:15 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yi-Ting, Hsieh (a1691807), a1691807@student.adelaide.edu.au
 *   Organization:  
 *
 * =====================================================================================
 */

#include "memory.h"
#include "processor.h"


uint64_t getImm_B(uint64_t ir);
uint64_t getImm_U(uint64_t ir);
void checkInstruction(memory* mem,processor* proc,uint64_t* X,uint64_t& PC,uint64_t ir,bool verbose);
int checkType(uint64_t ir);

