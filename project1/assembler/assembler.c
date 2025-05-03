/* Assembler code fragment for LC-2K */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000
#define MAX_LABELS 1000

char labels[MAX_LABELS][MAXLINELENGTH];
int addresses[MAX_LABELS];
int labelCount = 0;

// 파일에서 한 줄을 읽고 와서 각각 label, opcode, arg0, arg1, arg2로 분리
int readAndParse(FILE *, char *, char *, char *, char *, char *);

// 문자인지 숫자인지 판별
int isNumber(char *);

// 숫자는 숫자로, 레이블이면 주소로
int resolveValue(char *);

// 
int calcOffset(char *, int);

// 레지스터 범위 에러처리
void validateRegister(char *);

int main(int argc, char *argv[]) 
{
	// 파일 이름
	char *inFileString, *outFileString; 

	// 파일 포인터
	FILE *inFilePtr, *outFilePtr; 
	
	// 한 줄을 파싱해서 저장할 배열들
	char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH], 
			 arg1[MAXLINELENGTH], arg2[MAXLINELENGTH]; 
	
	// 인자가 2개를 받아야 함 (입력파일 : ~.as, 출력파일 : ~.mc)
	if (argc != 3) {
		printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
				argv[0]);
		exit(1);
	}

	inFileString = argv[1];
	outFileString = argv[2];

	inFilePtr = fopen(inFileString, "r");
	if (inFilePtr == NULL) {
		printf("error in opening %s\n", inFileString);
		exit(1);
	}
	outFilePtr = fopen(outFileString, "w");
	if (outFilePtr == NULL) {
		printf("error in opening %s\n", outFileString);
		exit(1);
	}

	/* here is an example for how to use readAndParse to read a line from
		 inFilePtr */
	// if (!readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
	// 	/* reached end of file */
	// }
	
	/* TODO: Phase-1 label calculation */
	// 한 줄씩 읽어서 레이블을 저장 
	// undefinied labels, duplicated definition 에러 처리가 필요함
	int currentLine = 0;
	while (1) {
		int parsed = readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2);
		if (!parsed) break;

		if (strlen(label) > 0) {
			for (int i = 0; i < labelCount; i++) {
				if (!strcmp(labels[i], label)) {
					printf("error: duplicate label %s\n", label);
					exit(1);
				}
			}
			strcpy(labels[labelCount], label);
			addresses[labelCount] = currentLine;
			labelCount++;
		}


		if (strlen(opcode) > 0 && strcmp(opcode, ".fill") != 0) {
			currentLine++;
		}
		
	}


	/* this is how to rewind the file ptr so that you start reading from the
		 beginning of the file */
	rewind(inFilePtr);

	/* TODO: Phase-2 generate machine codes to outfile */
	currentLine = 0;

	while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
		int machineCode = 0;
        int thisLine = currentLine;
	
		if (!strcmp(opcode, "add") || !strcmp(opcode, "nor")) {
			validateRegister(arg0);
			validateRegister(arg1);
			validateRegister(arg2);
		
			int opcodeNum = (!strcmp(opcode, "add")) ? 0 : 1;
			int regA = atoi(arg0);
			int regB = atoi(arg1);
			int destReg = atoi(arg2);
		
			machineCode = (opcodeNum << 22)
						| (regA << 19)
						| (regB << 16)
						| (destReg & 0x7);  // Only the lowest 3 bits used for destReg
		}
		
		else if (!strcmp(opcode, "lw") || !strcmp(opcode, "sw")) {
			validateRegister(arg0);
			validateRegister(arg1);
			int offset = resolveValue(arg2);  // ✅ 절대주소로 처리
			machineCode = ((strcmp(opcode, "lw") == 0 ? 2 : 3) << 22)
							| (atoi(arg0) << 19)
							| (atoi(arg1) << 16)
							| (offset & 0xFFFF);
		}
		
		else if (!strcmp(opcode, "beq")) {
			validateRegister(arg0);
			validateRegister(arg1);
			machineCode = (4 << 22) | (atoi(arg0) << 19) | (atoi(arg1) << 16) | calcOffset(arg2, thisLine);
		}
		else if (!strcmp(opcode, "jalr")) {
			validateRegister(arg0);
			validateRegister(arg1);
			machineCode = (5 << 22) | (atoi(arg0) << 19) | (atoi(arg1) << 16);
		}
		else if (!strcmp(opcode, "halt")) {
			machineCode = (6 << 22);
		}
		else if (!strcmp(opcode, "noop")) {
			machineCode = (7 << 22);
		}
		else if (!strcmp(opcode, ".fill")) {
			machineCode = resolveValue(arg0);
		}
		else {
			// unrecognized opcodes 에러 처리
			printf("error: unrecognized opcode %s\n", opcode);
			exit(1);
		}
	
		fprintf(outFilePtr, "%d\n", machineCode);
		currentLine++;
	}
	

	/* after doing a readAndParse, you may want to do the following to test the
		 opcode */
	// if (!strcmp(opcode, "add")) {
	// 	/* do whatever you need to do for opcode "add" */
	// }

	if (inFilePtr) {
		fclose(inFilePtr);
	}
	if (outFilePtr) {
		fclose(outFilePtr);
	}
	return(0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
		char *arg1, char *arg2)
{
	char line[MAXLINELENGTH];
	char *ptr = line;

	/* delete prior values */
	label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

	/* read the line from the assembly-language file */
	if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
		/* reached end of file */
		return(0);
	}

	/* check for line too long (by looking for a \n) */
	if (strchr(line, '\n') == NULL) {
		/* line too long */
		printf("error: line too long\n");
		exit(1);
	}

	/* is there a label? */
	ptr = line;
	if (sscanf(ptr, "%[^\t\n\r ]", label)) {
		/* successfully read label; advance pointer over the label */
		ptr += strlen(label);
	}

	/*
	 * Parse the rest of the line.  Would be nice to have real regular
	 * expressions, but scanf will suffice.
	 */
	sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%"
			"[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]", opcode, arg0, arg1, arg2);
	return(1);
}

int isNumber(char *string)
{
	/* return 1 if string is a number */
	int i;
	return( (sscanf(string, "%d", &i)) == 1);
}

int resolveValue(char *field) {
    if (isNumber(field)) {
        return atoi(field);
    }
    for (int i = 0; i < labelCount; i++) {
        if (!strcmp(labels[i], field)) {
            return addresses[i];
        }
    }
    printf("error: undefined label %s\n", field);
    exit(1);
}

int calcOffset(char *field, int currentLine) {
    int offset;
    if (isNumber(field)) {
        offset = atoi(field);
    } else {
        int value = resolveValue(field);
        offset = value - (currentLine + 1);
    }

    if (offset < -32768 || offset > 32767) {
        printf("error: offsetField %d out of range\n", offset);
        exit(1);
    }
    return offset & 0xFFFF;
}


void validateRegister(char *regStr) {
    if (!isNumber(regStr)) {
        printf("error: register %s is not a number\n", regStr);
        exit(1);
    }
    int reg = atoi(regStr);
    if (reg < 0 || reg > 7) {
        printf("error: register %d out of range\n", reg);
        exit(1);
    }
}


