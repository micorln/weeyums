/*
Code that loads a .bin file into PC memory
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "opcodes.h"

uint8_t * load_bin_file_into_memory(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        return NULL;
    }

    // Count lines
    size_t lines = 0;
    char buffer[256];

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        lines++;
    }

    // Allocate space for 3 uint16_t per line
    size_t count = lines * 3;
    uint8_t *values = malloc(count * sizeof(uint8_t));

    if (values == NULL) {
        perror("malloc");
        fclose(file);
        return NULL;
    }
    printf("LOADED OUTPUT\n");

    // Go back to the beginning
    rewind(file);

    // Read all values
    for (size_t i = 0; i < count; i++) {
        if (fscanf(file, "%hhx", &values[i]) != 1) {
            
            fprintf(stderr, "Invalid input\n");
            free(values);
            fclose(file);
            return NULL;
        }
        else 
        {
            if (values[i] == LOAD)
            {
                printf("%d\n", values[i]);
            }
            
        }
    }

    fclose(file);

    return values;
}