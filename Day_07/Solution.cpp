#include <cassert>
#include <cstdio>
#include <cstdlib>

#define MAX_MEMORY_COUNT 1024

struct Input_t {
    int Count;
    int Memory[MAX_MEMORY_COUNT];
};

Input_t LoadInput() {
    FILE* File = fopen("Data/input.txt", "r");
    if (File == NULL) {
        printf("ERROR(%d): fopen failed.", errno);
        exit(EXIT_FAILURE);
    }

    if (fseek(File, 0, SEEK_END) != 0) {
        printf("ERROR(%d): fseek failed.", errno);
        exit(EXIT_FAILURE);
    }

    long Length = ftell(File);
    if (Length == -1) {
        printf("ERROR(%d): ftell failed.", errno);
        exit(EXIT_FAILURE);
    }

    if (fseek(File, 0, SEEK_SET) != 0) {
        printf("ERROR(%d): fseek failed.", errno);
        exit(EXIT_FAILURE);
    }

    char* Buffer = (char*) malloc(Length);
    if (fread(Buffer, 1, Length, File) != Length) {
        printf("ERROR(%d): fread failed.", errno);
        exit(EXIT_FAILURE);
    }

    if (fclose(File) == EOF) {
        printf("ERROR(%d): fclose failed.", errno);
        exit(EXIT_FAILURE);
    }

    int Offset = 0;
    Input_t Result = {};
    for (int i = 0; i < Length; ++i) {
        if (Buffer[i] == ',' || Buffer[i] == '\n') {
            Buffer[i] = NULL;

            Result.Count += 1;
            assert(Result.Count <= MAX_MEMORY_COUNT);
            Result.Memory[Result.Count-1] = atoi(Buffer + Offset);

            Offset = i + 1;
        }
    }

    free(Buffer);
    return Result;
}

int GetInstruction(int Memory) {
    int E = Memory % 10;
    int D = (Memory / 10) % 10;
    return (10 * D) + E;
}

int GetParameterMode(int Memory, int Index) {
    int Modes[3];
    Modes[0] = (Memory / 100) % 10;
    Modes[1] = (Memory / 1000) % 10;
    Modes[2] = (Memory / 10000) % 10;
    return Modes[Index];
}

struct Amplifier_t {
    int Offset;
    Input_t Program;
};

bool RunProgram(Amplifier_t* Amplifier, int* Inputs, int* Output) {
    int InputIndex = 0;
    Input_t* Input = &Amplifier->Program;
    for (int i = Amplifier->Offset; i < Input->Count; ++i) {
        int Instruction = GetInstruction(Input->Memory[i]);
        int ParameterMode1 = GetParameterMode(Input->Memory[i], 0);
        int ParameterMode2 = GetParameterMode(Input->Memory[i], 1);

        if (Instruction == 1) {
            int X = Input->Memory[++i];
            int Y = Input->Memory[++i];
            int Z = Input->Memory[++i];

            X = ParameterMode1 ? X : Input->Memory[X];
            Y = ParameterMode2 ? Y : Input->Memory[Y];

            Input->Memory[Z] = X + Y;
        } else if (Instruction == 2) {
            int X = Input->Memory[++i];
            int Y = Input->Memory[++i];
            int Z = Input->Memory[++i];

            X = ParameterMode1 ? X : Input->Memory[X];
            Y = ParameterMode2 ? Y : Input->Memory[Y];

            Input->Memory[Z] = X * Y;
        } else if (Instruction == 3) {
            int Z = Input->Memory[++i];
            Input->Memory[Z] = Inputs[InputIndex++];
        } else if (Instruction == 4) {
            int X = Input->Memory[++i];

            Amplifier->Offset = i + 1;
            *Output = ParameterMode1 ? X : Input->Memory[X];
            break;
        } else if (Instruction == 5) {
            int X = Input->Memory[++i];
            int Y = Input->Memory[++i];

            X = ParameterMode1 ? X : Input->Memory[X];
            Y = ParameterMode2 ? Y : Input->Memory[Y];

            i = (X != 0) ? Y - 1 : i;
        } else if (Instruction == 6) {
            int X = Input->Memory[++i];
            int Y = Input->Memory[++i];

            X = ParameterMode1 ? X : Input->Memory[X];
            Y = ParameterMode2 ? Y : Input->Memory[Y];

            i = (X == 0) ? Y - 1 : i;
        } else if (Instruction == 7) {
            int X = Input->Memory[++i];
            int Y = Input->Memory[++i];
            int Z = Input->Memory[++i];

            X = ParameterMode1 ? X : Input->Memory[X];
            Y = ParameterMode2 ? Y : Input->Memory[Y];

            Input->Memory[Z] = (X < Y) ? 1 : 0;
        } else if (Instruction == 8) {
            int X = Input->Memory[++i];
            int Y = Input->Memory[++i];
            int Z = Input->Memory[++i];

            X = ParameterMode1 ? X : Input->Memory[X];
            Y = ParameterMode2 ? Y : Input->Memory[Y];

            Input->Memory[Z] = (X == Y) ? 1 : 0;
        } else if (Instruction == 99) {
            return true;
        } else {
            printf("Invalid program state.\n");
            exit(EXIT_FAILURE);
        }
    }

    return false;
}

#define NUM_AMPLIFIERS 5
#define NUM_PERMUTATIONS 120

int PluckElement(int* Values, int Index) {
    int Result = Values[Index];
    for (int i = Index; i < NUM_AMPLIFIERS; ++i) {
        Values[i] = Values[i + 1];
    }

    return Result;
}

int SolvePuzzle1(Input_t Input) {
    int MaxOutput = 0;
    for (int i = 0; i < NUM_PERMUTATIONS; ++i) {
        int Options[NUM_AMPLIFIERS] = { 0, 1, 2, 3, 4 };

        int PhaseSettings[NUM_AMPLIFIERS] = {
            PluckElement(Options, (i / (NUM_PERMUTATIONS / 5)) % 5),
            PluckElement(Options, (i / (NUM_PERMUTATIONS / 5 / 4)) % 4),
            PluckElement(Options, (i / (NUM_PERMUTATIONS / 5 / 4 / 3)) % 3),
            PluckElement(Options, (i / (NUM_PERMUTATIONS / 5 / 4 / 3 / 2)) % 2),
            PluckElement(Options, (i / (NUM_PERMUTATIONS / 5 / 4 / 3 / 2 / 1)) % 1)
        };

        int Output = 0;
        for (int j = 0; j < NUM_AMPLIFIERS; ++j) {
            Amplifier_t Amplifier = { 0, Input };
            int Inputs[2] = { PhaseSettings[j], Output };
            if (RunProgram(&Amplifier, Inputs, &Output)) {
                printf("Program halted early.\n");
                exit(EXIT_FAILURE);
            }

            if (Output > MaxOutput) {
                MaxOutput = Output;
            }
        }
    }

    return MaxOutput;
}

int SolvePuzzle2(Input_t Input) {
    int MaxOutput = 0;
    for (int i = 0; i < NUM_PERMUTATIONS; ++i) {
        int Options[NUM_AMPLIFIERS] = { 5, 6, 7, 8, 9 };

        int PhaseSettings[NUM_AMPLIFIERS] = {
            PluckElement(Options, (i / (NUM_PERMUTATIONS / 5)) % 5),
            PluckElement(Options, (i / (NUM_PERMUTATIONS / 5 / 4)) % 4),
            PluckElement(Options, (i / (NUM_PERMUTATIONS / 5 / 4 / 3)) % 3),
            PluckElement(Options, (i / (NUM_PERMUTATIONS / 5 / 4 / 3 / 2)) % 2),
            PluckElement(Options, (i / (NUM_PERMUTATIONS / 5 / 4 / 3 / 2 / 1)) % 1)
        };

        int Output = 0;
        Amplifier_t Amplifiers[NUM_AMPLIFIERS] = {};
        for (int Index = 0; Index < NUM_AMPLIFIERS; ++Index) {
            Amplifiers[Index].Offset = 0;
            Amplifiers[Index].Program = Input;

            int Inputs[2] = { PhaseSettings[Index], Output };
            if (RunProgram(&Amplifiers[Index], Inputs, &Output)) {
                printf("Program halted early.\n");
                exit(EXIT_FAILURE);
            }
        }

        bool Halted = false;
        while (Halted == false) {
            for (int Index = 0; Index < NUM_AMPLIFIERS; ++Index) {
                if (RunProgram(&Amplifiers[Index], &Output, &Output)) {
                    Halted = true;
                    break;
                }
            }
        }

        if (Output > MaxOutput) {
            MaxOutput = Output;
        }
    }

    return MaxOutput;
}

extern "C"
int main(int argc, char const* argv[]) {
    Input_t Input = LoadInput();
    printf("Part 1: %d\n", SolvePuzzle1(Input));
    printf("Part 2: %d\n", SolvePuzzle2(Input));
    return EXIT_SUCCESS;
}
