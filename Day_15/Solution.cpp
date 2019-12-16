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

            int Value = Reader(UserData);
            if (Value == 99) {
                break;
            }

            if (ParameterMode1 == POSITION_MODE) {
                Input->Memory[Z] = Value;
            } else if (ParameterMode1 == RELATIVE_MODE) {
                Input->Memory[RelativeBase + Z] = Value;
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

enum Direction_t {
    DIRECTION_NORTH = 1,
    DIRECTION_SOUTH = 2,
    DIRECTION_WEST  = 3,
    DIRECTION_EAST  = 4
};

enum Status_t {
    STATUS_WALL = 0,
    STATUS_MOVED = 1,
    STATUS_TARGET = 2
};

int const MAX_MAZE_SIZE = 42;

struct Maze_t {
    int X;
    int Y;
    int GoalX;
    int GoalY;
    bool Found;
    Direction_t Facing;
    Direction_t Testing;
    int Cells[MAX_MAZE_SIZE][MAX_MAZE_SIZE];
};

int const TILE_NONE = 0;
int const TILE_WALL = -1;
int const TILE_FLOOR = -2;

int64_t MazeReader(void* UserData) {
    Maze_t* Maze = (Maze_t*) UserData;
    if (Maze->Found) {
        return 99;
    }

    Maze->Cells[Maze->Y][Maze->X] = TILE_FLOOR;
    if (Maze->Facing == DIRECTION_NORTH) {
        Maze->Testing = DIRECTION_WEST;
    } else if (Maze->Facing == DIRECTION_SOUTH) {
        Maze->Testing = DIRECTION_EAST;
    } else if (Maze->Facing == DIRECTION_WEST) {
        Maze->Testing = DIRECTION_SOUTH;
    } else if (Maze->Facing == DIRECTION_EAST) {
        Maze->Testing = DIRECTION_NORTH;
    }

    return Maze->Testing;
}

void MazeWriter(int64_t Value, void* UserData) {
    Maze_t* Maze = (Maze_t*) UserData;
    if (Value == STATUS_WALL) {
        if (Maze->Testing == DIRECTION_NORTH) {
            Maze->Facing = DIRECTION_SOUTH;
            Maze->Cells[Maze->Y-1][Maze->X] = TILE_WALL;
        } else if (Maze->Testing == DIRECTION_SOUTH) {
            Maze->Facing = DIRECTION_NORTH;
            Maze->Cells[Maze->Y+1][Maze->X] = TILE_WALL;
        } else if (Maze->Testing == DIRECTION_WEST) {
            Maze->Facing = DIRECTION_EAST;
            Maze->Cells[Maze->Y][Maze->X-1] = TILE_WALL;
        } else if (Maze->Testing == DIRECTION_EAST) {
            Maze->Facing = DIRECTION_WEST;
            Maze->Cells[Maze->Y][Maze->X+1] = TILE_WALL;
        }
    } else {
        Maze->Facing = Maze->Testing;
        if (Maze->Testing == DIRECTION_NORTH) {
            Maze->Y -= 1;
        } else if (Maze->Testing == DIRECTION_SOUTH) {
            Maze->Y += 1;
        } else if (Maze->Testing == DIRECTION_WEST) {
            Maze->X -= 1;
        } else if (Maze->Testing == DIRECTION_EAST) {
            Maze->X += 1;
        }
    }

    if (Value == STATUS_TARGET) {
        Maze->GoalX = Maze->X;
        Maze->GoalY = Maze->Y;
        Maze->Cells[Maze->X][Maze->Y] = TILE_FLOOR;
    }

    if (Maze->X == (MAX_MAZE_SIZE / 2)) {
        if (Maze->Y == (MAX_MAZE_SIZE / 2)) {
            Maze->Found = true;
        }
    }
}

int const MAX_STACK_SIZE = 1024;

struct Stack_t {
    int ItemCount;
    int Items[MAX_STACK_SIZE][3];
};

void PushUpdate(Stack_t* Stack, int X, int Y, int Distance) {
    Stack->ItemCount += 1;
    assert(Stack->ItemCount <= MAX_STACK_SIZE);

    Stack->Items[Stack->ItemCount-1][0] = X;
    Stack->Items[Stack->ItemCount-1][1] = Y;
    Stack->Items[Stack->ItemCount-1][2] = Distance;
}

bool PopUpdate(Stack_t* Stack, int* X, int* Y, int* Distance) {
    if (Stack->ItemCount) {
        Stack->ItemCount -= 1;
        *X = Stack->Items[Stack->ItemCount][0];
        *Y = Stack->Items[Stack->ItemCount][1];
        *Distance = Stack->Items[Stack->ItemCount][2];
        return true;
    }

    return false;
}

void FloodFill(Maze_t* Maze, int X, int Y) {
    int Distance = 0;
    Stack_t Stack = {};
    PushUpdate(&Stack, X, Y, 0);
    while (PopUpdate(&Stack, &X, &Y, &Distance)) {
        if (Maze->Cells[Y][X] == TILE_FLOOR) {
            Maze->Cells[Y][X] = Distance;

            PushUpdate(&Stack, X-1, Y, Distance+1);
            PushUpdate(&Stack, X+1, Y, Distance+1);
            PushUpdate(&Stack, X, Y-1, Distance+1);
            PushUpdate(&Stack, X, Y+1, Distance+1);
        }
    }
}

int SolvePuzzle1(Input_t Input) {
    Maze_t Maze = {};
    Maze.X = MAX_MAZE_SIZE / 2;
    Maze.Y = MAX_MAZE_SIZE / 2;
    Maze.Facing = DIRECTION_NORTH;
    RunProgram(&Input, MazeReader, MazeWriter, &Maze);
    FloodFill(&Maze, Maze.X, Maze.Y);

    return Maze.Cells[Maze.GoalY][Maze.GoalX];
}

int SolvePuzzle2(Input_t Input) {
    Maze_t Maze = {};
    Maze.X = MAX_MAZE_SIZE / 2;
    Maze.Y = MAX_MAZE_SIZE / 2;
    Maze.Facing = DIRECTION_NORTH;
    RunProgram(&Input, MazeReader, MazeWriter, &Maze);
    FloodFill(&Maze, Maze.GoalX, Maze.GoalY);

    int MaxValue = 0;
    for (int Y = 0; Y < MAX_MAZE_SIZE; ++Y) {
        for (int X = 0; X < MAX_MAZE_SIZE; ++X) {
            if (Maze.Cells[Y][X] > MaxValue) {
                MaxValue = Maze.Cells[Y][X];
            }
        }
    }

    return MaxValue;
}

extern "C"
int main(int argc, char const* argv[]) {
    Input_t Input = LoadInput();
    printf("Part 1: %d\n", SolvePuzzle1(Input));
    printf("Part 2: %d\n", SolvePuzzle2(Input));
    return EXIT_SUCCESS;
}
