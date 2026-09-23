#include <stdint.h>
#include "cpu.h"

#ifndef ENUMS_H
#define ENUMS_H


enum opcode {
    LOAD = 0x0000,
    MOV = 0x0001,
    ADD = 0x0002,
    SUB = 0x0003,
    MUL = 0x0004,
    LOADM = 0x0005,
    STOREM = 0x0006,
    CMP = 0x0007,
    JMP = 0x0008,
    JZ = 0x0009,
    PUSH = 0x000A,
    POP = 0x000B,
    CALL = 0x000C,
    RET = 0x000D,
    HALT = 0x000E
};

struct instruction 
{
    enum opcode op;
    uint16_t operand1;
    uint16_t operand2;
};

void load(struct CPU *cpu, uint16_t reg, uint16_t ptr);

void halt(struct CPU *cpu);

void mov(struct CPU *cpu, uint16_t reg1, uint16_t reg2);

void add(struct CPU *cpu, uint16_t reg1, uint16_t reg2);

void sub(struct CPU *cpu, uint16_t reg1, uint16_t reg2);

void mul(struct CPU *cpu, uint16_t reg1, uint16_t reg2);

void loadm(struct CPU *cpu, uint16_t reg, uint16_t addr);

void storem(struct CPU *cpu, uint16_t reg, uint16_t addr);

void cmp(struct CPU *cpu, uint16_t reg1, uint16_t reg2);

void push(struct CPU *cpu, uint16_t reg);

void pop(struct CPU *cpu, uint16_t reg);

void jmp(struct CPU *cpu, uint16_t addr);

void jz(struct CPU *cpu, uint16_t addr);

void call(struct CPU *cpu, uint16_t addr);

void ret(struct CPU *cpu);

#endif

/*
NOTE ON ISA : 
LOAD   R1, 42
MOV    R1, R2
ADD    R1, R2
SUB    R1, R2
MUL    R1, R2

LOADM  R1, 1000
STOREM R1, 1000

CMP    R1, R2
JMP    address
JZ     address

PUSH   R1
POP    R1
CALL   address
RET

HALT
*/