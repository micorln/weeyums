#include <stdbool.h>

#ifndef CPU_H
#define CPU_H

struct CPU {
    uint16_t * registers;
    uint16_t pc;
    uint16_t sp;
    uint16_t ir;
    uint16_t *memory;
    bool halted;
    uint16_t * flags;
};

uint16_t mem_read(struct CPU *cpu, uint16_t addr);

void mem_write(struct CPU *cpu, uint16_t addr, uint16_t value);

#endif