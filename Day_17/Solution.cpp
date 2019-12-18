#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

int const MAX_MEMORY_COUNT = 10240;

struct Input_t {
    int64_t Count;
    int64_t Memory[MAX_MEMORY_COUNT];
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

    long Offset = 0;
    Input_t Result = {};
    for (long i = 0; i < Length; ++i) {
        if (Buffer[i] == ',' || Buffer[i] == '\n') {
            Buffer[i] = NULL;

            Result.Count += 1;
            assert(Result.Count <= MAX_MEMORY_COUNT);

            char* End = (char*) i + 1;
            Result.Memory[Result.Count-1] = strtoll(Buffer + Offset, &End, 10);

            Offset = i + 1;
        }
    }

    free(Buffer);
    return Result;
}

int64_t GetInstruction(int64_t Memory) {
    int64_t E = Memory % 10;
    int64_t D = (Memory / 10) % 10;
    return (10 * D) + E;
}

int64_t GetParameterMode(int64_t Memory, int64_t Index) {
    int64_t Modes[3];
    Modes[0] = (Memory / 100) % 10;
    Modes[1] = (Memory / 1000) % 10;
    Modes[2] = (Memory / 10000) % 10;
    return Modes[Index];
}

int64_t const POSITION_MODE = 0;
int64_t const IMMEDIATE_MODE = 1;
int64_t const RELATIVE_MODE = 2;

typedef int64_t (InputReader_t)(void* UserData);
typedef void (OutputWriter_t)(int64_t Value, void* UserData);

void RunProgram(Input_t* Input, InputReader_t* Reader, OutputWriter_t* Writer, void* UserData) {
    int64_t RelativeBase = 0;
    for (int64_t i = 0; i < Input->Count; ++i) {
        int64_t Instruction = GetInstruction(Input->Memory[i]);
        int64_t ParameterMode1 = GetParameterMode(Input->Memory[i], 0);
        int64_t ParameterMode2 = GetParameterMode(Input->Memory[i], 1);
        int64_t ParameterMode3 = GetParameterMode(Input->Memory[i], 2);

        if (Instruction == 1) {
            int64_t X = Input->Memory[++i];
            if (ParameterMode1 == POSITION_MODE) {
                X = Input->Memory[X];
            } else if (ParameterMode1 == RELATIVE_MODE) {
                X = Input->Memory[RelativeBase + X];
            }

            int64_t Y = Input->Memory[++i];
            if (ParameterMode2 == POSITION_MODE) {
                Y = Input->Memory[Y];
            } else if (ParameterMode2 == RELATIVE_MODE) {
                Y = Input->Memory[RelativeBase + Y];
            }

            int64_t Z = Input->Memory[++i];
            if (ParameterMode3 == POSITION_MODE) {
                Input->Memory[Z] = X + Y;
            } else if (ParameterMode3 == RELATIVE_MODE) {
                Input->Memory[RelativeBase + Z] = X + Y;
            }
        } else if (Instruction == 2) {
            int64_t X = Input->Memory[++i];
            if (ParameterMode1 == POSITION_MODE) {
                X = Input->Memory[X];
            } else if (ParameterMode1 == RELATIVE_MODE) {
                X = Input->Memory[RelativeBase + X];
            }

            int64_t Y = Input->Memory[++i];
            if (ParameterMode2 == POSITION_MODE) {
                Y = Input->Memory[Y];
            } else if (ParameterMode2 == RELATIVE_MODE) {
                Y = Input->Memory[RelativeBase + Y];
            }

            int64_t Z = Input->Memory[++i];
            if (ParameterMode3 == POSITION_MODE) {
                Input->Memory[Z] = X * Y;
            } else if (ParameterMode3 == RELATIVE_MODE) {
                Input->Memory[RelativeBase + Z] = X * Y;
            }
        } else if (Instruction == 3) {
            int64_t Z = Input->Memory[++i];
            if (ParameterMode1 == POSITION_MODE) {
                Input->Memory[Z] = Reader(UserData);
            } else if (ParameterMode1 == RELATIVE_MODE) {
                Input->Memory[RelativeBase + Z] = Reader(UserData);
            }
        } else if (Instruction == 4) {
            int64_t X = Input->Memory[++i];
            if (ParameterMode1 == POSITION_MODE) {
                X = Input->Memory[X];
            } else if (ParameterMode1 == RELATIVE_MODE) {
                X = Input->Memory[RelativeBase + X];
            }

            Writer(X, UserData);
        } else if (Instruction == 5) {
            int64_t X = Input->Memory[++i];
            if (ParameterMode1 == POSITION_MODE) {
                X = Input->Memory[X];
            } else if (ParameterMode1 == RELATIVE_MODE) {
                X = Input->Memory[RelativeBase + X];
            }

            int64_t Y = Input->Memory[++i];
            if (ParameterMode2 == POSITION_MODE) {
                Y = Input->Memory[Y];
            } else if (ParameterMode2 == RELATIVE_MODE) {
                Y = Input->Memory[RelativeBase + Y];
            }

            i = (X != 0) ? Y - 1 : i;
        } else if (Instruction == 6) {
            int64_t X = Input->Memory[++i];
            if (ParameterMode1 == POSITION_MODE) {
                X = Input->Memory[X];
            } else if (ParameterMode1 == RELATIVE_MODE) {
                X = Input->Memory[RelativeBase + X];
            }

            int64_t Y = Input->Memory[++i];
            if (ParameterMode2 == POSITION_MODE) {
                Y = Input->Memory[Y];
            } else if (ParameterMode2 == RELATIVE_MODE) {
                Y = Input->Memory[RelativeBase + Y];
            }

            i = (X == 0) ? Y - 1 : i;
        } else if (Instruction == 7) {
            int64_t X = Input->Memory[++i];
            if (ParameterMode1 == POSITION_MODE) {
                X = Input->Memory[X];
            } else if (ParameterMode1 == RELATIVE_MODE) {
                X = Input->Memory[RelativeBase + X];
            }

            int64_t Y = Input->Memory[++i];
            if (ParameterMode2 == POSITION_MODE) {
                Y = Input->Memory[Y];
            } else if (ParameterMode2 == RELATIVE_MODE) {
                Y = Input->Memory[RelativeBase + Y];
            }

            int64_t Z = Input->Memory[++i];
            if (ParameterMode3 == POSITION_MODE) {
                Input->Memory[Z] = (X < Y) ? 1 : 0;
            } else if (ParameterMode3 == RELATIVE_MODE) {
                Input->Memory[RelativeBase + Z] = (X < Y) ? 1 : 0;
            }
        } else if (Instruction == 8) {
            int64_t X = Input->Memory[++i];
            if (ParameterMode1 == POSITION_MODE) {
                X = Input->Memory[X];
            } else if (ParameterMode1 == RELATIVE_MODE) {
                X = Input->Memory[RelativeBase + X];
            }

            int64_t Y = Input->Memory[++i];
            if (ParameterMode2 == POSITION_MODE) {
                Y = Input->Memory[Y];
            } else if (ParameterMode2 == RELATIVE_MODE) {
                Y = Input->Memory[RelativeBase + Y];
            }

            int64_t Z = Input->Memory[++i];
            if (ParameterMode3 == POSITION_MODE) {
                Input->Memory[Z] = (X == Y) ? 1 : 0;
            } else if (ParameterMode3 == RELATIVE_MODE) {
                Input->Memory[RelativeBase + Z] = (X == Y) ? 1 : 0;
            }
        } else if (Instruction == 9) {
            int64_t X = Input->Memory[++i];
            if (ParameterMode1 == POSITION_MODE) {
                X = Input->Memory[X];
            } else if (ParameterMode1 == RELATIVE_MODE) {
                X = Input->Memory[RelativeBase + X];
            }

            RelativeBase += X;
        } else if (Instruction == 99) {
            break;
        } else {
            printf("Invalid program state.\n");
            exit(EXIT_FAILURE);
        }
    }
}

int const IMAGE_WIDTH = 55;
int const IMAGE_HEIGHT = 37;

int const FACING_UP = 0;
int const FACING_DOWN = 1;
int const FACING_LEFT = 2;
int const FACING_RIGHT = 3;

struct Image_t {
    int X;
    int Y;
    int RobotX;
    int RobotY;
    int RobotFacing;
    char Pixels[IMAGE_HEIGHT][IMAGE_WIDTH];
};

void CameraWriter(int64_t Value, void* UserData) {
    Image_t* Image = (Image_t*) UserData;

    if (Value == 10) {
        Image->X = 0;
        Image->Y += 1;
    } else if (Value == '^') {
        Image->RobotX = Image->X;
        Image->RobotY = Image->Y;
        Image->RobotFacing = FACING_UP;
        Image->Pixels[Image->Y][Image->X] = '#';
        Image->X += 1;
    } else if (Value == 'v') {
        Image->RobotX = Image->X;
        Image->RobotY = Image->Y;
        Image->RobotFacing = FACING_DOWN;
        Image->Pixels[Image->Y][Image->X] = '#';
        Image->X += 1;
    } else if (Value == '<') {
        Image->RobotX = Image->X;
        Image->RobotY = Image->Y;
        Image->RobotFacing = FACING_LEFT;
        Image->Pixels[Image->Y][Image->X] = '#';
        Image->X += 1;
    } else if (Value == '>') {
        Image->RobotX = Image->X;
        Image->RobotY = Image->Y;
        Image->RobotFacing = FACING_RIGHT;
        Image->Pixels[Image->Y][Image->X] = '#';
        Image->X += 1;
    } else {
        Image->Pixels[Image->Y][Image->X] = (char) Value;
        Image->X += 1;
    }
}

int SolvePuzzle1(Input_t Input) {
    Image_t Image = {};
    RunProgram(&Input, NULL, CameraWriter, &Image);

    int Value = 0;
    for (int Y = 1; Y < IMAGE_HEIGHT - 1; ++Y) {
        for (int X = 1; X < IMAGE_WIDTH - 1; ++X) {
            if (Image.Pixels[Y][X] == '#') {
                bool Intersection = ((Image.Pixels[Y][X - 1] == '#')
                                  && (Image.Pixels[Y][X + 1] == '#')
                                  && (Image.Pixels[Y - 1][X] == '#')
                                  && (Image.Pixels[Y + 1][X] == '#'));
                if (Intersection) {
                    Value += X * Y;
                }
            }
        }
    }

    return Value;
}

bool IsScaffold(Image_t* Image, int X, int Y) {
    return ((0 <= X) && (X < IMAGE_WIDTH))
        && ((0 <= Y) && (Y < IMAGE_HEIGHT))
        && (Image->Pixels[Y][X] == '#');
}

void PrintEncoding(Image_t* Image) {
    while (true) {
        int Distance = 0;
        char Direction = NULL;

        int X = Image->RobotX;
        int Y = Image->RobotY;
        if (Image->RobotFacing == FACING_UP) {
            if (IsScaffold(Image, X - 1, Y)) {
                Direction = 'L';
                Image->RobotFacing = FACING_LEFT;
            } else if (IsScaffold(Image, X + 1, Y)) {
                Direction = 'R';
                Image->RobotFacing = FACING_RIGHT;
            }
        } else if (Image->RobotFacing == FACING_DOWN) {
            if (IsScaffold(Image, X - 1, Y)) {
                Direction = 'R';
                Image->RobotFacing = FACING_LEFT;
            } else if (IsScaffold(Image, X + 1, Y)) {
                Direction = 'L';
                Image->RobotFacing = FACING_RIGHT;
            }
        } else if (Image->RobotFacing == FACING_LEFT) {
            if (IsScaffold(Image, X, Y - 1)) {
                Direction = 'R';
                Image->RobotFacing = FACING_UP;
            } else if (IsScaffold(Image, X, Y + 1)) {
                Direction = 'L';
                Image->RobotFacing = FACING_DOWN;
            }
        } else if (Image->RobotFacing == FACING_RIGHT) {
            if (IsScaffold(Image, X, Y - 1)) {
                Direction = 'L';
                Image->RobotFacing = FACING_UP;
            } else if (IsScaffold(Image, X, Y + 1)) {
                Direction = 'R';
                Image->RobotFacing = FACING_DOWN;
            }
        }

        int const Deltas[4][2] = {
            {  0, -1 }, // FACING_UP
            {  0, +1 }, // FACING_DOWN
            { -1,  0 }, // FACING_LEFT
            { +1,  0 }  // FACING_RIGHT
        };

        int DeltaX = Deltas[Image->RobotFacing][0];
        int DeltaY = Deltas[Image->RobotFacing][1];
        while (IsScaffold(Image, Image->RobotX + DeltaX, Image->RobotY + DeltaY)) {
            Image->RobotX += DeltaX;
            Image->RobotY += DeltaY;
        }

        Distance = abs(X - Image->RobotX)
                 + abs(Y - Image->RobotY);
        if ((Direction == NULL) && (Distance == 0)) {
            break;
        }

        printf("%c%d ", Direction, Distance);
    }

    printf("\n");
}

int64_t RobotReader(void* UserData) {
    /// Note:
    ///     Not very happy with this solution, by hand...
    ///     Would like to come back and try it again some time.
    ///                             - Brian Rowlett, 2019-12-15
    static int Index = 0;
    char const* Commands = "A,B,A,C,B,C,B,A,C,B\n"  // Main routine
                           "L,6,R,8,R,12,L,6,L,8\n" // Function A
                           "L,10,L,8,R,12\n"        // Function B
                           "L,8,L,10,L,6,L,6\n"     // Function C
                           "n";                     // Continuous video feed
    return Commands[Index++];
}

void RobotWriter(int64_t Value, void* UserData) {
    int* Answer = (int*) UserData;
    *Answer = Value;
}

int SolvePuzzle2(Input_t Input) {
    // Image_t Image = {};
    // RunProgram(&Input, NULL, CameraWriter, &Image);
    // PrintEncoding(&Image);

    int Value = 0;
    Input.Memory[0] = 2;
    RunProgram(&Input, RobotReader, RobotWriter, &Value);
    return Value;
}

extern "C"
int main(int argc, char const* argv[]) {
    Input_t Input = LoadInput();
    printf("Part 1: %d\n", SolvePuzzle1(Input));
    printf("Part 2: %d\n", SolvePuzzle2(Input));
    return EXIT_SUCCESS;
}
