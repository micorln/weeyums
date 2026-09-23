#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "opcodes.h"
#include "cpu.h"

#define REG_COUNT 8
#define REG_SIZE_IN_BYTES 1
#define DATA_POINTER 120
#define STACK_POINTER 200
#define PROGRAM_COUNTER 0
#define MEMORY_SIZE_IN_BYTES 65535
#define NUM_FLAGS 1
#define RAM_END MEMORY_SIZE_IN_BYTES - MEMORY_SIZE_IN_BYTES / 3
#define SERIAL_DEVICE 50000


struct instruction fetch(struct CPU* cpu)
{
    printf("Fetching instruction at PC: %d\n", cpu->pc);
    struct instruction insr = {cpu->memory[cpu->pc], cpu->memory[cpu->pc+1], cpu->memory[cpu->pc+2]};
    cpu->pc += 3;
    return insr;
}

void decode(struct CPU *cpu, struct instruction insr)
{
    switch(insr.op) {
        case LOAD:
            load(cpu, insr.operand1, insr.operand2);
            break;
        case MOV:
            mov(cpu, insr.operand1, insr.operand2);
            break;
        case ADD:
            add(cpu, insr.operand1, insr.operand2);
            break;
        case SUB:
            sub(cpu, insr.operand1, insr.operand2);
            break;  
        case MUL:
            mul(cpu, insr.operand1, insr.operand2);
            break;    
        case HALT:
            halt(cpu);
            break;
        case LOADM:
            loadm(cpu, insr.operand1, insr.operand2);
            break;
        case STOREM:
            storem(cpu, insr.operand1, insr.operand2);
            break;
        case CMP:
            cmp(cpu, insr.operand1, insr.operand2);
            break; 
        case JMP:
            jmp(cpu, insr.operand1);
            break;
        case JZ:
            jz(cpu, insr.operand1);
            break;
        case PUSH:
            push(cpu, insr.operand1);
            break;  
        case POP:
            pop(cpu, insr.operand1);
            break;
        case CALL:
            call(cpu, insr.operand1);
            break;
        case RET:
            ret(cpu);
            break;  
        default:
            printf("Unknown opcode: %d\n", insr.op);
            break;
    }
}

uint16_t mem_read(struct CPU *cpu, uint16_t addr)
{
    return cpu->memory[addr];
}

void mem_write(struct CPU *cpu, uint16_t addr, uint16_t value)
{
    if (addr >= MEMORY_SIZE_IN_BYTES) {
        printf("Error: Attempt to write outside of memory bounds at address %d\n", addr);
        return;
    }
    if (addr < RAM_END) {
        cpu->memory[addr] = value;
        return;
    }
    if (addr == SERIAL_DEVICE) {
        putchar(value);
        fflush(stdout);
        return;
    }
    cpu->memory[addr] = value;
}

void instantiate_cpu(struct CPU *cpu)
{
    cpu->registers = (uint16_t *) malloc(REG_COUNT * sizeof(uint16_t));
    cpu->pc = PROGRAM_COUNTER;
    cpu->sp = STACK_POINTER;
    cpu->halted = false;
    cpu->memory = (uint16_t *) malloc(MEMORY_SIZE_IN_BYTES * sizeof(uint16_t));
    cpu->flags = (uint16_t *) malloc(NUM_FLAGS * sizeof(uint16_t)); 
}

void start_cpu(struct CPU *cpu)
{
    while (!cpu->halted) 
    {
        struct instruction insr = fetch(cpu);
        decode(cpu, insr);
    }
}

void add_code_to_memory(struct CPU *cpu, uint16_t *inst_opcode, size_t inst_size, uint16_t memory_offset)
{
    if (inst_size > MEMORY_SIZE_IN_BYTES) {
        printf("Error: Code size exceeds memory size.\n");
        return;
    }
    memcpy(cpu->memory+memory_offset, inst_opcode, inst_size);
}

uint16_t *read_bin_from_file(char * filename, size_t *count)
{
    FILE *file = fopen(filename, "rb");

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    *count = file_size / sizeof(uint16_t);

    uint16_t *values = malloc(*count * sizeof(uint16_t));

    fread(values, sizeof(uint16_t), *count, file);

    fclose(file);
    return values;
}

int main(int argc, char *argv[]) 
{
    struct CPU cpu;
    instantiate_cpu(&cpu);

    size_t code_size;
    uint16_t * code = read_bin_from_file(argv[1], &code_size);
    if (code == NULL) {
        fprintf(stderr, "Error loading code file.\n");
        return 1;
    }

    for (int i = 0; i < code_size; i++) {
        printf("%d ", code[i]);
    }

    printf("\n");
    // uint16_t code[] = {
    //     LOAD, 0, 20,
    //     LOAD, 1, 20,
    //     ADD,  0, 1,
    //     STOREM, 0, 100,
    //     LOADM, 1, 100,
    //     ADD, 0, 1, 
    //     HALT, 0, 0
    // };

    for (int i = 0; i < code_size; i++) 
    {
        // printf("Adding instruction %d to memory at address %d\n", code[i], i);
        add_code_to_memory(&cpu, code+i, sizeof(code[i]), i);    
    }

    printf("Value of register 0 before execution: %d\n", cpu.registers[0]);
    
    start_cpu(&cpu);

    printf("Value of register 0 after execution: %d\n", cpu.registers[0]);

}