/**
 * Register renaming simulator
 * Author: Mantha Sai Gopal
 * Reg.no: 23358
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NUM_REGISTERS 32

typedef struct {
    int value;
    int valid;
} Register;

typedef struct {
    int op;
    int src1;
    int src2;
    int dest;
    char outputRenamed;
    char memAddressMode;
    int memAddress;
} Instruction;

Register physicalRegisters[NUM_REGISTERS];
int nextPhysicalRegister = 0;
int architecturalToPhysical[NUM_REGISTERS];

int allocatePhysicalRegister() {
    if (nextPhysicalRegister < NUM_REGISTERS) {
        return nextPhysicalRegister++;
    } else {
        printf("\nSufficient registers not available\n");
        return -1;
    }
}

void allocatePhysicalRegisters(Instruction *instr) {
    if (instr->src1 >= 0 && !physicalRegisters[instr->src1].valid) {
        instr->src1 = architecturalToPhysical[instr->src1];
    }
    if (instr->src2 >= 0 && !physicalRegisters[instr->src2].valid) {
        instr->src2 = architecturalToPhysical[instr->src2];
    }
    if (instr->dest != -1) {
        instr->dest = allocatePhysicalRegister();
        instr->outputRenamed = 1;
        physicalRegisters[instr->dest].valid = 1;
        architecturalToPhysical[instr->dest] = instr->dest;
    }
}

void issue(Instruction instr, FILE *outputFile) {
    allocatePhysicalRegisters(&instr);

    // Simulate instruction issue and write the renamed instruction to the output file
    // fprintf(outputFile, "Issue: ");
    if (instr.outputRenamed) {
        switch (instr.op) {
            case 0: fprintf(outputFile, "ADD"); break;
            case 1: fprintf(outputFile, "SUB"); break;
            case 2: fprintf(outputFile, "LD"); break;
            case 3: fprintf(outputFile, "SD"); break;
            case 4: fprintf(outputFile, "MUL"); break;
            // Add more cases for additional instruction types
        }
        fprintf(outputFile, " R%d, ", instr.dest);
        if (instr.memAddressMode == 0) {
            fprintf(outputFile, "R%d, R%d\n", instr.src1, instr.src2);
        } else {
            fprintf(outputFile, "M%d, R%d\n", instr.memAddress, instr.src1);
        }
    } else {
        switch (instr.op) {
            case 0: fprintf(outputFile, "ADD"); break;
            case 1: fprintf(outputFile, "SUB"); break;
            case 2: fprintf(outputFile, "LD"); break;
            case 3: fprintf(outputFile, "SD"); break;
            case 4: fprintf(outputFile, "MUL"); break;
            // Add more cases for additional instruction types
        }
        fprintf(outputFile, " R%d, ", instr.dest);
        if (instr.memAddressMode == 0) {
            fprintf(outputFile, "R%d, R%d\n", instr.src1, instr.src2);
        } else {
            fprintf(outputFile, "M%d, R%d\n", instr.memAddress, instr.src1);
        }
    }
}

int main() {
    FILE *inputFile = fopen("input.txt", "r");
    if (inputFile == NULL) {
        printf("Failed to open input file.\n");
        return 1;
    }

    FILE *outputFile = fopen("output.txt", "w");
    if (outputFile == NULL) {
        printf("Failed to open output file.\n");
        fclose(inputFile);
        return 1;
    }

    for (int i = 0; i < NUM_REGISTERS; i++) {
        physicalRegisters[i].value = 0;
        physicalRegisters[i].valid = 0;
        architecturalToPhysical[i] = i;
    }

    int numInstructions;
    fscanf(inputFile, "%d", &numInstructions);

    for (int i = 1; i <= numInstructions; i++) {
        char operation[4];
        int dest, src1, src2;
        int memAddressMode = 0;
        int memAddress = 0;

        if (fscanf(inputFile, "%s R%d, R%d, R%d", operation, &dest, &src1, &src2) == 4) {
            Instruction instr;
            if (strcmp(operation, "ADD") == 0) {
                instr.op = 0;
            } else if (strcmp(operation, "SUB") == 0) {
                instr.op = 1;
            } else if (strcmp(operation, "LD") == 0) {
                instr.op = 2;
            } else if (strcmp(operation, "SD") == 0) {
                instr.op = 3;
            } else if (strcmp(operation, "MUL") == 0) {
                instr.op = 4;
            } else {
                printf("Unknown instruction: %s\n", operation);
                i--;
                continue;
            }

            instr.src1 = src1;
            instr.src2 = src2;
            instr.dest = dest;
            instr.outputRenamed = 0;
            instr.memAddressMode = memAddressMode;
            instr.memAddress = memAddress;

            issue(instr, outputFile);
        } else {
            if (fscanf(inputFile, "%s R%d, %d(R%d)", operation, &dest, &memAddress, &src1) == 4) {
                for (int i = 0; operation[i]; i++) {
                    operation[i] = toupper(operation[i]);
                }

                Instruction instr;
                if (strcmp(operation, "LD") == 0) {
                    instr.op = 2;
                } else if (strcmp(operation, "SD") == 0) {
                    instr.op = 3;
                } else {
                    printf("Unknown instruction: %s\n", operation);
                    i--;
                    continue;
                }

                instr.src1 = src1;
                instr.dest = dest;
                instr.outputRenamed = 0;
                instr.memAddressMode = 1;
                instr.memAddress = memAddress;

                issue(instr, outputFile);
            } else {
                printf("Invalid instruction format. Please use 'ADD R1, R2, R3' or 'LD R1, 100(R2)' format.\n");
                i--;
            }
        }
    }

    fclose(inputFile);
    fclose(outputFile);

    return 0;
}
