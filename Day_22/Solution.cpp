#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

enum Type_t {
    CUT = 1,
    DEAL_INTO_NEW_STACK = 2,
    DEAL_WITH_INCREMENT = 3
};

struct Step_t {
    int Type;
    int Value;
};

int const MAX_STEP_COUNT = 100;

struct Input_t {
    int StepCount;
    Step_t Steps[MAX_STEP_COUNT];
};

bool StartsWith(char const* A, char const* B) {
    int Length = strlen(B);
    return (Length <= strlen(A))
        && (strncmp(A, B, Length) == 0);
}

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

    long Offset = 0;
    Input_t Result = {};
    for (long i = 0; i < Length; ++i) {
        if (Buffer[i] == '\n') {
            Buffer[i] = NULL;

            if (StartsWith(Buffer + Offset, "cut")) {
                Result.StepCount += 1;
                assert(Result.StepCount <= MAX_STEP_COUNT);
                Result.Steps[Result.StepCount - 1].Type = CUT;
                Result.Steps[Result.StepCount - 1].Value = atoi(Buffer + Offset + 4);
            } else if (StartsWith(Buffer + Offset, "deal into new stack")) {
                Result.StepCount += 1;
                assert(Result.StepCount <= MAX_STEP_COUNT);
                Result.Steps[Result.StepCount - 1].Type = DEAL_INTO_NEW_STACK;
            } else if (StartsWith(Buffer + Offset, "deal with increment")) {
                Result.StepCount += 1;
                assert(Result.StepCount <= MAX_STEP_COUNT);
                Result.Steps[Result.StepCount - 1].Type = DEAL_WITH_INCREMENT;
                Result.Steps[Result.StepCount - 1].Value = atoi(Buffer + Offset + 20);
            }

            Offset = i + 1;
        }
    }

    free(Buffer);
    return Result;
}

int SolvePuzzle1(Input_t Input) {
    int Position = 2019;
    int const DeckSize = 10007;
    for (int i = 0; i < Input.StepCount; ++i) {
        if (Input.Steps[i].Type == CUT) {
            int Value = Input.Steps[i].Value;
            if (Value < 0) {
                Value = DeckSize - abs(Value);
            }

            if (Value < Position) {
                Position -= Value;
            } else if (Value >= Position) {
                Position += (DeckSize - Value);
            }
        } else if (Input.Steps[i].Type == DEAL_INTO_NEW_STACK) {
            Position = (DeckSize - Position - 1);
        } else if (Input.Steps[i].Type == DEAL_WITH_INCREMENT) {
            Position *= Input.Steps[i].Value;
            Position %= DeckSize;
        }
    }

    return Position;
}

int64_t SolvePuzzle2(Input_t Input) {
    /// Note:
    ///     Did not finish part 2 of todays puzzle...
    ///     Would like to come back and try it again some time.
    ///                             - Brian Rowlett, 2019-12-15
    return 0;
}

extern "C"
int main(int argc, char const* argv[]) {
    Input_t Input = LoadInput();
    printf("Part 1: %d\n", SolvePuzzle1(Input));
    printf("Part 2: %lld\n", SolvePuzzle2(Input));
    return EXIT_SUCCESS;
}
