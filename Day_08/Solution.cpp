#include <cassert>
#include <cstdio>
#include <cstdlib>

int const SIZE_X = 25;
int const SIZE_Y = 6;

int const MAX_LAYER_COUNT = 128;
int const MAX_PIXEL_COUNT = SIZE_X * SIZE_Y;

struct Layer_t {
    int Pixels[MAX_PIXEL_COUNT];
};

struct Input_t {
    int LayerCount;
    Layer_t Layers[MAX_LAYER_COUNT];
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
    for (int Index = 0; Index < Length - 1; ++Index) {
        if (Index % MAX_PIXEL_COUNT == 0) {
            Result.LayerCount += 1;
            assert(Result.LayerCount <= MAX_LAYER_COUNT);
        }

        int LayerIndex = Result.LayerCount - 1;
        int PixelIndex = Index % MAX_PIXEL_COUNT;

        Layer_t* Layer = &Result.Layers[LayerIndex];
        Layer->Pixels[PixelIndex] = (Buffer[Index] - '0');
    }

    free(Buffer);
    return Result;
}

int SolvePuzzle1(Input_t Input) {
    int FewestIndex = 0;
    int FewestZeros = INT_MAX;
    for (int LayerIndex = 0; LayerIndex < Input.LayerCount; ++LayerIndex) {
        int Count = 0;
        for (int PixelIndex = 0; PixelIndex < MAX_PIXEL_COUNT; ++PixelIndex) {
            Layer_t* Layer = &Input.Layers[LayerIndex];
            int PixelColor = Layer->Pixels[PixelIndex];

            if (PixelColor == 0) {
                Count += 1;
            }
        }

        if (Count < FewestZeros) {
            FewestZeros = Count;
            FewestIndex = LayerIndex;
        }
    }

    int Digits[3] = {};
    for (int PixelIndex = 0; PixelIndex < MAX_PIXEL_COUNT; ++PixelIndex) {
        Layer_t* Layer = &Input.Layers[FewestIndex];
        int PixelColor = Layer->Pixels[PixelIndex];
        Digits[PixelColor] += 1;
    }

    return Digits[1] * Digits[2];
}

int SolvePuzzle2(Input_t Input) {
    for (int Y = 0; Y < SIZE_Y; ++Y) {
        for (int X = 0; X < SIZE_X; ++X) {
            int PixelIndex = (Y * SIZE_X) + X;
            for (int LayerIndex = 0; LayerIndex < Input.LayerCount; ++LayerIndex) {
                Layer_t* Layer = &Input.Layers[LayerIndex];
                int PixelColor = Layer->Pixels[PixelIndex];

                if (PixelColor == 0) {
                    printf(" ");
                    break;
                } else if (PixelColor == 1) {
                    printf("%c", 219u);
                    break;
                }
            }
        }

        printf("\n");
    }
        
    return 0;
}

extern "C"
int main(int argc, char const* argv[]) {
    Input_t Input = LoadInput();
    printf("Part 1: %d\n", SolvePuzzle1(Input));
    printf("See output above.\n", SolvePuzzle2(Input));
    return EXIT_SUCCESS;
}
