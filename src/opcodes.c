#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "opcodes.h"
#include "cpu.h"

void load(struct CPU *cpu, uint16_t reg, uint16_t ptr) 
{
    cpu->registers[reg] = ptr;
    printf("Loaded value %d into register %d\n", ptr, reg);
}

void mov(struct CPU *cpu, uint16_t reg1, uint16_t reg2)
{
    cpu->registers[reg1] = cpu->registers[reg2];
    printf("Moved value from register %d to register %d\n", reg2, reg1);
}

void add(struct CPU *cpu, uint16_t reg1, uint16_t reg2)
{
    cpu->registers[reg1] += cpu->registers[reg2];
    printf("Added value from register %d to register %d\n", reg2, reg1);
}

void sub(struct CPU *cpu, uint16_t reg1, uint16_t reg2)
{
    cpu->registers[reg1] -= cpu->registers[reg2];
    printf("Subtracted value from register %d from register %d\n", reg2, reg1);
}

void mul(struct CPU *cpu, uint16_t reg1, uint16_t reg2)
{
    cpu->registers[reg1] *= cpu->registers[reg2];
    printf("Multiplied value from register %d with register %d\n", reg2, reg1);
}

void halt(struct CPU *cpu) 
{
    cpu->halted = true;
    printf("CPU halted.\n");
}

void loadm(struct CPU *cpu, uint16_t reg, uint16_t addr) 
{
    cpu->registers[reg] = mem_read(cpu, addr);
    printf("Loaded value from memory address %d into register %d\n", addr, reg);
}

void storem(struct CPU *cpu, uint16_t reg, uint16_t addr) 
{
    mem_write(cpu, addr, cpu->registers[reg]);
    printf("Stored value from register %d into memory address %d\n", reg, addr);
}

void cmp(struct CPU *cpu, uint16_t reg1, uint16_t reg2) 
{
    if (cpu->registers[reg1] == cpu->registers[reg2]) {
        cpu->flags[0] = 1; // Set zero flag
        printf("Registers %d and %d are equal.\n", reg1, reg2);
    } else {
        cpu->flags[0] = 0; // Clear zero flag
        printf("Registers %d and %d are not equal.\n", reg1, reg2);
    }
}

void push(struct CPU *cpu, uint16_t reg) 
{
    cpu->sp--;
    mem_write(cpu, cpu->sp, cpu->registers[reg]);
    printf("Pushed value from register %d onto stack.\n", reg);
}

void jmp(struct CPU *cpu, uint16_t addr) 
{
    cpu->pc = addr;
    printf("Jumped to address %d.\n", addr);
}

void jz(struct CPU *cpu, uint16_t addr) 
{
    if (cpu->flags[0] == 1) {
        cpu->pc = addr;
        printf("Jumped to address %d because zero flag is set.\n", addr);
    } else {
        printf("Did not jump to address %d because zero flag is not set.\n", addr);
    }
}

void pop(struct CPU *cpu, uint16_t reg) 
{
    cpu->registers[reg] = mem_read(cpu, cpu->sp);
    cpu->sp++;
    printf("Popped value from stack into register %d.\n", reg);
}

void call(struct CPU *cpu, uint16_t addr) 
{
    cpu->sp--;
    push(cpu, cpu->pc); // Push the current PC onto the stack
    cpu->pc = addr;
    printf("Called subroutine at address %d.\n", addr);
}

void ret(struct CPU *cpu) 
{
    pop(cpu, cpu->pc); // Pop the return address from the stack into PC
    printf("Returned from subroutine to address %d.\n", cpu->pc);
}