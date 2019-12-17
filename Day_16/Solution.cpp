#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

int const MAX_ELEMENT_COUNT = 1024;

struct Input_t {
    int ElementCount;
    int Elements[MAX_ELEMENT_COUNT];
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

    Input_t Result = {};
    for (long i = 0; i < Length; ++i) {
        if (Buffer[i] == '\n') {
            continue;
        }

        Result.ElementCount += 1;
        assert(Result.ElementCount <= MAX_ELEMENT_COUNT);
        Result.Elements[Result.ElementCount - 1] = Buffer[i] - '0';
    }

    free(Buffer);
    return Result;
}

int const MAX_SIGNAL_LENGTH = 2048 * 2048;

struct Signal_t {
    int ElementCount;
    int Elements[MAX_SIGNAL_LENGTH];
};

void PushElement(Signal_t* Signal, int Value) {
    Signal->ElementCount += 1;
    assert(Signal->ElementCount <= MAX_SIGNAL_LENGTH);
    Signal->Elements[Signal->ElementCount - 1] = Value;
}

void CreatePattern(Signal_t* Signal, int Element, int Length) {
    int Index = 0;
    int const Values[4] = {
        0, 1, 0, -1
    };

    Signal->ElementCount = 0;
    for (int i = 1; i <= Length; ++i) {
        if ((i % Element) == 0) {
            Index += 1;
            Index = Index % 4;
        }

        PushElement(Signal, Values[Index]);
    }
}

int FlawedFrequencyTransmission(Signal_t* Signal, Signal_t* Pattern, int Element) {
    int Value = 0;
    for (int i = Element; i < Signal->ElementCount; ++i) {
        Value += Signal->Elements[i] * Pattern->Elements[i];
    }

    return abs(Value) % 10;
}

void Swap(Signal_t** A, Signal_t** B) {
    Signal_t* Temporary = *A; *A = *B; *B = Temporary;
}

int ReadDigits(int* Values, int Count) {
    int Value = 0;
    for (int i = 0; i < Count; ++i) {
        Value *= 10;
        Value += Values[i];
    }

    return Value;
}

int SolvePuzzle1(Input_t Input) {
    Signal_t* Signal = (Signal_t*) malloc(sizeof(Signal_t));
    Signal_t* Result = (Signal_t*) malloc(sizeof(Signal_t));
    Signal_t* Pattern = (Signal_t*) malloc(sizeof(Signal_t));

    for (int i = 0; i < Input.ElementCount; ++i) {
        PushElement(Signal, Input.Elements[i]);
        PushElement(Result, Input.Elements[i]);
    }

    for (int Phase = 0; Phase < 100; ++Phase) {
        for (int i = 0; i < Signal->ElementCount; ++i) {
            CreatePattern(Pattern, i + 1, Signal->ElementCount);
            Result->Elements[i] = FlawedFrequencyTransmission(Signal, Pattern, i);
        }

        Swap(&Signal, &Result);
    }

    // Leaks memory...
    return ReadDigits(Signal->Elements, 8);
}

int SolvePuzzle2(Input_t Input) {
    Signal_t* Signal = (Signal_t*) malloc(sizeof(Signal_t));

    int Offset = ReadDigits(Input.Elements, 7);
    for (int i = Offset; i < 10000 * Input.ElementCount; ++i) {
        PushElement(Signal, Input.Elements[i % Input.ElementCount]);
    }

    /// Note:
    ///     Full disclosure, I looked up some hints on the
    ///     subreddit. Feels bad man. https://bit.ly/38QVoEU
    ///                             - Brian Rowlett, 2019-12-17
    for (int Phase = 0; Phase < 100; ++Phase) {
        for (int i = 0; i < Signal->ElementCount - 1; ++i) {
            int Index = Signal->ElementCount - i - 2;
            Signal->Elements[Index] = (Signal->Elements[Index] + Signal->Elements[Index + 1]) % 10;
        }
    }

    // Leaks memory...
    return ReadDigits(Signal->Elements, 8);
}

extern "C"
int main(int argc, char const* argv[]) {
    Input_t Input = LoadInput();
    printf("Part 1: %d\n", SolvePuzzle1(Input));
    printf("Part 2: %d\n", SolvePuzzle2(Input));
    return EXIT_SUCCESS;
}
