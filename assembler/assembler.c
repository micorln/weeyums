#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "opcodes.h"
#include <errno.h>

struct assembler 
{
    char * filename;
    uint16_t * code;
};

enum opcode get_opcode(char *str) {
    if (strcmp(str, "LOAD") == 0)
        return LOAD;
    if (strcmp(str, "ADD") == 0)
        return ADD;
    if (strcmp(str, "SUB") == 0)
        return SUB;
    if (strcmp(str, "MUL") == 0)
        return MUL; 
    if (strcmp(str, "LOADM") == 0)
        return LOADM;
    if (strcmp(str, "STOREM") == 0)
        return STOREM;
    if (strcmp(str, "CMP") == 0)    
        return CMP;
    if (strcmp(str, "JMP") == 0)
        return JMP;
    if (strcmp(str, "JZ") == 0)
        return JZ;
    if (strcmp(str, "PUSH") == 0)
        return PUSH;
    if (strcmp(str, "POP") == 0)
        return POP;
    if (strcmp(str, "CALL") == 0)
        return CALL;
    if (strcmp(str, "RET") == 0)
        return RET;
    if (strcmp(str, "HALT") == 0)
        return HALT;
    if (strcmp(str, "MOV") == 0)
        return MOV;

    return -1;  // invalid opcode
}

int is_keyword_line(const char *str) {
    return str[strlen(str) - 1] == ':';
}

int is_uint16(const char *str)
{
    char *end;

    errno = 0; // Reset errno before the conversion

    unsigned long value = strtoul(str, &end, 10);

    // Nothing was parsed
    if (end == str)
        return 0;

    // There were invalid characters
    if (*end != '\0')
        return 0;

    // Conversion overflowed
    if (errno == ERANGE)
        return 0;

    // Doesn't fit in uint16_t
    if (value > UINT16_MAX)
        return 0;

    return 1;
}

int count_lines_in_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        return -1;
    }

    int line_count = 0;
    char line[256];

    while (fgets(line, sizeof(line), file) != NULL) {
        line_count++;
    }

    fclose(file);
    return line_count;
}

char ** build_keyword_table(const char *filename, int line_count, uint16_t *indices, int *number_of_keywords, uint16_t *memory_index) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        return NULL;
    }

    char **keywords = (char **)malloc(line_count * sizeof(char *));
    if (keywords == NULL) {
        perror("malloc");
        fclose(file);
        return NULL;
    }

    char line[256];

    *number_of_keywords = 0;
    *memory_index = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        char *word = strtok(line, " \t\n,");
        if (is_keyword_line(line)) {
            word[strlen(word) - 1] = '\0'; // Remove the colon
            keywords[*number_of_keywords] = malloc(strlen(word) + 1);
            strcpy(keywords[*number_of_keywords], word);
            indices[*number_of_keywords] = *memory_index;
            (*number_of_keywords)++;
            printf("HERE\n");
        } else {
            *memory_index += 3; // Each instruction has 3 parts: opcode and two operands
        }
    }

    fclose(file);
    return keywords;
}

void write_to_file(const char *filename, uint16_t *values, size_t count) {
    FILE *file = fopen(filename, "wb");

    if (file == NULL) {
        perror("fopen");
        return;
    }

    fwrite(values, sizeof(uint16_t), count, file);

    fclose(file);
}


int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    
    int line_count = count_lines_in_file(argv[1]);
    if (line_count == -1) {
        fprintf(stderr, "Error counting lines in file.\n");
        return 1;
    }
    FILE *file = fopen(argv[1], "r");

    if (file == NULL) {
        perror("Could not open file");
        return 1;
    }

    char line[256];

    int number_of_keywords = 0;
    uint16_t *indices = (uint16_t *)malloc(line_count * sizeof(uint16_t));
    if (indices == NULL) {
        perror("malloc");
        fclose(file);
        return 1;
    }

    uint16_t memory_index = 0;
    char ** keywords = build_keyword_table(argv[1], line_count, indices, &number_of_keywords, &memory_index);
    for (int i = 0; i < number_of_keywords; i++) {
        printf("Keyword: %s, Address: %u\n", keywords[i], indices[i]);
    }
    
    uint16_t *code = (uint16_t *)malloc(memory_index * sizeof(uint16_t));
    file = fopen(argv[1], "r");
    int code_index = 0;

    while (fgets(line, sizeof(line), file) != NULL) {

        char *word = strtok(line, " \t\n,");
        if (word == NULL || is_keyword_line(line)) {
            continue; // Skip empty lines and keyword lines
        }
        
        while (word != NULL) {
            if (get_opcode(word) != -1) {
                code[code_index] = get_opcode(word);
            } else {
                if (is_uint16(word)) 
                {
                    uint16_t operand = (uint16_t)strtoul(word, NULL, 0);
                    code[code_index] = operand;
                } 
                else 
                {
                    uint16_t operand = 0;
                    for (int i = 0; i < number_of_keywords; i++) {
                        if (strcmp(word, keywords[i]) == 0) {
                            operand = indices[i];
                        }
                    }
                    if (operand != 0) {
                        printf("label: %s, address: %u\n", word, operand);
                        code[code_index] = operand;
                    } else {
                        fprintf(stderr, "Invalid operand: %s\n", word);
                    }

                    
                }
            }

            code_index++;
            word = strtok(NULL, " \t\n,");
        }
    }

    for (int i = 0; i < code_index; i++) {
        printf("%u ", code[i]);
    }
    printf("\n");

    write_to_file("bin_files/program.bin", code, code_index);
    return 0;


}