#include <cassert>
#include <cstdio>
#include <cstdlib>

struct Input_t {
    int LowerBound;
    int UpperBound;
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
        if (Buffer[i] == '-') {
            Buffer[i] = NULL;
            Result.LowerBound = atoi(Buffer + Offset);

            Offset = i + 1;
        } else if (Buffer[i] == '\n') {
            Buffer[i] = NULL;
            Result.UpperBound = atoi(Buffer + Offset);

            Offset = i + 1;
        }
    }

    free(Buffer);
    return Result;
}

void ExtractDigits(int Password, int Digits[6]) {
    Digits[5] = Password % 10;
    Digits[4] = (Password / 10) % 10;
    Digits[3] = (Password / 100) % 10;
    Digits[2] = (Password / 1000) % 10;
    Digits[1] = (Password / 10000) % 10;
    Digits[0] = (Password / 100000) % 10;
}

bool NeverDecrease(int Digits[6]) {
    return (Digits[0] <= Digits[1])
        && (Digits[1] <= Digits[2])
        && (Digits[2] <= Digits[3])
        && (Digits[3] <= Digits[4])
        && (Digits[4] <= Digits[5]);
}

bool TwoAdjacentSame(int Digits[6]) {
    return (Digits[0] == Digits[1])
        || (Digits[1] == Digits[2])
        || (Digits[2] == Digits[3])
        || (Digits[3] == Digits[4])
        || (Digits[4] == Digits[5]);
}

bool TwoAdjacentSameNotPartOfLargerGroup(int Digits[6]) {
    return (                            (Digits[0] == Digits[1]) && (Digits[1] != Digits[2]))
        || ((Digits[0] != Digits[1]) && (Digits[1] == Digits[2]) && (Digits[2] != Digits[3]))
        || ((Digits[1] != Digits[2]) && (Digits[2] == Digits[3]) && (Digits[3] != Digits[4]))
        || ((Digits[2] != Digits[3]) && (Digits[3] == Digits[4]) && (Digits[4] != Digits[5]))
        || ((Digits[3] != Digits[4]) && (Digits[4] == Digits[5])                            );
}

int SolvePuzzle1(Input_t Input) {
    int Count = 0;
    for (int i = Input.LowerBound; i <= Input.UpperBound; ++i) {
        int Digits[6];
        ExtractDigits(i, Digits);

        bool Valid = NeverDecrease(Digits)
                  && TwoAdjacentSame(Digits);

        if (Valid) {
            Count += 1;
        }
    }

    return Count;
}

int SolvePuzzle2(Input_t Input) {
    int Count = 0;
    for (int i = Input.LowerBound; i <= Input.UpperBound; ++i) {
        int Digits[6];
        ExtractDigits(i, Digits);

        bool Valid = NeverDecrease(Digits)
                  && TwoAdjacentSameNotPartOfLargerGroup(Digits);

        if (Valid) {
            Count += 1;
        }
    }

    return Count;
}

extern "C"
int main(int argc, char const* argv[]) {
    Input_t Input = LoadInput();
    printf("Part 1: %d\n", SolvePuzzle1(Input));
    printf("Part 2: %d\n", SolvePuzzle2(Input));
    return EXIT_SUCCESS;
}
