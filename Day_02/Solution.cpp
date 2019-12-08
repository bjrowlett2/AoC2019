#include <cassert>
#include <cstdio>
#include <cstdlib>

int const MAX_MEMORY_COUNT = 256;

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

void RunProgram(Input_t* Input) {
    for (int i = 0; i < Input->Count; i += 4) {
        if (Input->Memory[i] == 1) {
            int A = Input->Memory[i+1];
            int B = Input->Memory[i+2];
            int C = Input->Memory[i+3];
            Input->Memory[C] = Input->Memory[A] + Input->Memory[B];
        } else if (Input->Memory[i] == 2) {
            int A = Input->Memory[i+1];
            int B = Input->Memory[i+2];
            int C = Input->Memory[i+3];
            Input->Memory[C] = Input->Memory[A] * Input->Memory[B];
        } else if (Input->Memory[i] == 99) {
            break;
        } else {
            printf("Invalid program state.\n");
            exit(EXIT_FAILURE);
        }
    }
}

int SolvePuzzle1(Input_t Input) {
    Input.Memory[1] = 12;
    Input.Memory[2] = 2;

    RunProgram(&Input);
    return Input.Memory[0];
}

int SolvePuzzle2(Input_t Input) {
    int Noun, Verb;
    for (Noun = 0; Noun <= 99; ++Noun) {
        for (Verb = 0; Verb <= 99; ++Verb) {
            Input_t Copy = Input;
            Copy.Memory[1] = Noun;
            Copy.Memory[2] = Verb;

            RunProgram(&Copy);
            if (Copy.Memory[0] == 19690720) {
                return 100 * Noun + Verb;
            }
        }
    }

    printf("No solution found.\n");
    exit(EXIT_FAILURE);
}

extern "C"
int main(int argc, char const* argv[]) {
    Input_t Input = LoadInput();
    printf("Part 1: %d\n", SolvePuzzle1(Input));
    printf("Part 2: %d\n", SolvePuzzle2(Input));
    return EXIT_SUCCESS;
}
