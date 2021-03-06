#include <cassert>
#include <cstdio>
#include <cstdlib>

int const MAX_WEIGHT_COUNT = 256;

struct Input_t {
    int Count;
    int Weights[MAX_WEIGHT_COUNT];
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
        if (Buffer[i] == '\n') {
            Buffer[i] = NULL;

            Result.Count += 1;
            assert(Result.Count <= MAX_WEIGHT_COUNT);
            Result.Weights[Result.Count-1] = atoi(Buffer + Offset);

            Offset = i + 1;
        }
    }

    free(Buffer);
    return Result;
}

int ComputeFuel(int mass) {
    return (mass / 3) - 2;
}

int SolvePuzzle1(Input_t Input) {
    int Fuel = 0;
    for (int i = 0; i < Input.Count; ++i) {
        Fuel += ComputeFuel(Input.Weights[i]);
    }

    return Fuel;
}

int SolvePuzzle2(Input_t Input) {
    int TotalFuel = 0;
    for (int i = 0; i < Input.Count; ++i) {
        int Fuel = ComputeFuel(Input.Weights[i]);

        int AdditionalFuel = ComputeFuel(Fuel);
        while (AdditionalFuel > 0) {
            Fuel += AdditionalFuel;
            AdditionalFuel = ComputeFuel(AdditionalFuel);
        }

        TotalFuel += Fuel;
    }

    return TotalFuel;
}

extern "C"
int main(int argc, char const* argv[]) {
    Input_t Input = LoadInput();
    printf("Part 1: %d\n", SolvePuzzle1(Input));
    printf("Part 2: %d\n", SolvePuzzle2(Input));
    return EXIT_SUCCESS;
}
