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

typedef int (InputReader_t)(void* UserData);
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

int const MAX_GRID_SIZE = 64;

struct Point_t {
    int X;
    int Y;
};

struct Game_t {
    int Step;
    int Score;
    Point_t Ball;
    Point_t Cursor;
    Point_t Paddle;
    int Grid[MAX_GRID_SIZE][MAX_GRID_SIZE];
};

int const TILE_EMPTY = 0;
int const TILE_WALL = 1;
int const TILE_BLOCK = 2;
int const TILE_PADDLE = 3;
int const TILE_BALL = 4;

int Signum(int Value) {
    return (Value > 0) - (Value < 0);
}

int ArcadeReader(void* UserData) {
    Game_t* Game = (Game_t*) UserData;
    return Signum(Game->Ball.X - Game->Paddle.X);
}

void ArcadeWriter(int64_t Value, void* UserData) {
    Game_t* Game = (Game_t*) UserData;
    
    if (Game->Step == 0) {
        Game->Cursor.X = Value;
    } else if (Game->Step == 1) {
        Game->Cursor.Y = Value;
    }  else if (Game->Step == 2) {
        int X = Game->Cursor.X;
        int Y = Game->Cursor.Y;
        if ((X == -1) && (Y == 0)) {
            Game->Score = Value;
        } else {
            Game->Grid[Y][X] = Value;

            if (Value == TILE_BALL) {
                Game->Ball.X = X;
                Game->Ball.Y = Y;
            } else if (Value == TILE_PADDLE) {
                Game->Paddle.X = X;
                Game->Paddle.Y = Y;
            }
        }
    }

    Game->Step += 1;
    Game->Step %= 3;
}

int SolvePuzzle1(Input_t Input) {
    Game_t Game = {};
    RunProgram(&Input, ArcadeReader, ArcadeWriter, &Game);

    int Count = 0;
    for (int Y = 0; Y < MAX_GRID_SIZE; ++Y) {
        for (int X = 0; X < MAX_GRID_SIZE; ++X) {
            if (Game.Grid[Y][X] == TILE_BLOCK) {
                Count += 1;
            }
        }
    }

    return Count;
}

int SolvePuzzle2(Input_t Input) {
    Game_t Game = {};
    Input.Memory[0] = 2;
    RunProgram(&Input, ArcadeReader, ArcadeWriter, &Game);
    return Game.Score;
}

extern "C"
int main(int argc, char const* argv[]) {
    Input_t Input = LoadInput();
    printf("Part 1: %d\n", SolvePuzzle1(Input));
    printf("Part 2: %d\n", SolvePuzzle2(Input));
    return EXIT_SUCCESS;
}
