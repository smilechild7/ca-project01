/* LC-2K Instruction-level simulator */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000 

typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);
int convertNum(int num);

int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    int instruction;
    int instructionCount = 0;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }

    // 메모리 초기화
    for (int i = 0; i < NUMMEMORY; i++) {
        state.mem[i] = 0;
    }

    // 레지스터 초기화
    for (int i = 0; i < NUMREGS; i++) {
        state.reg[i] = 0;
    }

    state.pc = 0;
    
    
    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
            state.numMemory++) {

        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }

		/* TODO: */

    // 명령어 실행
    while (1) {
        printState(&state);
        instruction = state.mem[state.pc];
        int opcode = (instruction >> 22) & 0x7;
        int regA = (instruction >> 19) & 0x7;
        int regB = (instruction >> 16) & 0x7;
        int destReg = instruction & 0x7;
        int offsetField = convertNum(instruction & 0xFFFF);

        instructionCount++;

        switch (opcode) {
            case 0: // add
                state.reg[destReg] = state.reg[regA] + state.reg[regB];
                state.pc++;
                break;
            case 1: // nor
                state.reg[destReg] = ~(state.reg[regA] | state.reg[regB]);
                state.pc++;
                break;
            case 2: // lw
                state.reg[regB] = state.mem[state.reg[regA] + offsetField];
                state.pc++;
                break;
            case 3: // sw
                state.mem[state.reg[regA] + offsetField] = state.reg[regB];
                state.pc++;
                break;
            case 4: // beq
                if (state.reg[regA] == state.reg[regB]) {
                    state.pc = state.pc + 1 + offsetField;
                } else {
                    state.pc++;
                }
                break;
            case 5: // jalr
                state.reg[regB] = state.pc + 1;
                state.pc = state.reg[regA];
                break;
            case 6: // halt
                state.pc++;
                printf("machine halted\n");
                printf("total of %d instructions executed\n", instructionCount);
                printf("final state of machine:\n");
                printState(&state);
                exit(0);
            case 7: // noop
                state.pc++;
                break;
            default:
                printf("error: unrecognized opcode %d\n", opcode);
                exit(1);
        }
    }

	if (filePtr) {
		fclose(filePtr);
	}
    return(0);
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i = 0; i < statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i = 0; i < NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
}

int convertNum(int num)
{
	/* convert a 16-bit number into a 32-bit Linux integer */
	if (num & (1 << 15)) {
		num -= (1 << 16);
	}
	return (num);
}
