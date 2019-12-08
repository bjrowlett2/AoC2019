#include <cassert>
#include <cstdio>
#include <cstdlib>

int const MAX_MEMORY_COUNT = 1024;

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

void RunProgram(Input_t* Input) {
    for (int i = 0; i < Input->Count; ++i) {
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
            printf("Input: ");

            int Z = Input->Memory[++i];
            scanf("%d", &Input->Memory[Z]);
        } else if (Instruction == 4) {
            int X = Input->Memory[++i];

            X = ParameterMode1 ? X : Input->Memory[X];

            printf("Output: %d\n", X);
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
            break;
        } else {
            printf("Invalid program state.\n");
            exit(EXIT_FAILURE);
        }
    }
}

int SolvePuzzle(Input_t Input) {
    RunProgram(&Input);
    return 0;
}

extern "C"
int main(int argc, char const* argv[]) {
    Input_t Input = LoadInput();
    printf("See output above.\n", SolvePuzzle(Input));
    printf("See output above.\n", SolvePuzzle(Input));
    return EXIT_SUCCESS;
}
