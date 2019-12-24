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

int const MAX_PACKET_COUNT = 1024;

struct Computer_t {
    int64_t Index;
    int64_t RelativeBase;

    bool Halted;
    int IdleCount;

    Input_t Program;
    Computer_t* Network;
    Computer_t* NatDevice;

    int Step;
    int64_t Buffer[3];

    int PacketCount;
    int64_t Packets[MAX_PACKET_COUNT];
};

void PushPacket(Computer_t* Computer, int64_t Packet) {
    Computer->PacketCount += 1;
    assert(Computer->PacketCount <= MAX_PACKET_COUNT);
    Computer->Packets[Computer->PacketCount - 1] = Packet;
}

bool PopPacket(Computer_t* Computer, int64_t* Packet) {
    if (Computer->PacketCount) {
        *Packet = Computer->Packets[0];

        Computer->PacketCount -= 1;
        for (int i = 0; i < Computer->PacketCount; ++i) {
            Computer->Packets[i] = Computer->Packets[i + 1];
        }

        return true;
    }

    return false;
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

typedef int64_t (InputReader_t)(Computer_t* Computer);
typedef void (OutputWriter_t)(Computer_t* Computer, int64_t Value);

void StepProgram(Computer_t* Computer, InputReader_t* Reader, OutputWriter_t* Writer) {
    if (Computer->Halted == false) {
        int64_t* Memory = Computer->Program.Memory;

        int64_t Instruction = GetInstruction(Memory[Computer->Index]);
        int64_t ParameterMode1 = GetParameterMode(Memory[Computer->Index], 0);
        int64_t ParameterMode2 = GetParameterMode(Memory[Computer->Index], 1);
        int64_t ParameterMode3 = GetParameterMode(Memory[Computer->Index], 2);

        if (Instruction == 1) {
            int64_t X = Memory[++Computer->Index];
            if (ParameterMode1 == POSITION_MODE) {
                X = Memory[X];
            } else if (ParameterMode1 == RELATIVE_MODE) {
                X = Memory[Computer->RelativeBase + X];
            }

            int64_t Y = Memory[++Computer->Index];
            if (ParameterMode2 == POSITION_MODE) {
                Y = Memory[Y];
            } else if (ParameterMode2 == RELATIVE_MODE) {
                Y = Memory[Computer->RelativeBase + Y];
            }

            int64_t Z = Memory[++Computer->Index];
            if (ParameterMode3 == POSITION_MODE) {
                Memory[Z] = X + Y;
            } else if (ParameterMode3 == RELATIVE_MODE) {
                Memory[Computer->RelativeBase + Z] = X + Y;
            }
        } else if (Instruction == 2) {
            int64_t X = Memory[++Computer->Index];
            if (ParameterMode1 == POSITION_MODE) {
                X = Memory[X];
            } else if (ParameterMode1 == RELATIVE_MODE) {
                X = Memory[Computer->RelativeBase + X];
            }

            int64_t Y = Memory[++Computer->Index];
            if (ParameterMode2 == POSITION_MODE) {
                Y = Memory[Y];
            } else if (ParameterMode2 == RELATIVE_MODE) {
                Y = Memory[Computer->RelativeBase + Y];
            }

            int64_t Z = Memory[++Computer->Index];
            if (ParameterMode3 == POSITION_MODE) {
                Memory[Z] = X * Y;
            } else if (ParameterMode3 == RELATIVE_MODE) {
                Memory[Computer->RelativeBase + Z] = X * Y;
            }
        } else if (Instruction == 3) {
            int64_t Z = Memory[++Computer->Index];
            if (ParameterMode1 == POSITION_MODE) {
                Memory[Z] = Reader(Computer);
            } else if (ParameterMode1 == RELATIVE_MODE) {
                Memory[Computer->RelativeBase + Z] = Reader(Computer);
            }
        } else if (Instruction == 4) {
            int64_t X = Memory[++Computer->Index];
            if (ParameterMode1 == POSITION_MODE) {
                X = Memory[X];
            } else if (ParameterMode1 == RELATIVE_MODE) {
                X = Memory[Computer->RelativeBase + X];
            }

            Writer(Computer, X);
        } else if (Instruction == 5) {
            int64_t X = Memory[++Computer->Index];
            if (ParameterMode1 == POSITION_MODE) {
                X = Memory[X];
            } else if (ParameterMode1 == RELATIVE_MODE) {
                X = Memory[Computer->RelativeBase + X];
            }

            int64_t Y = Memory[++Computer->Index];
            if (ParameterMode2 == POSITION_MODE) {
                Y = Memory[Y];
            } else if (ParameterMode2 == RELATIVE_MODE) {
                Y = Memory[Computer->RelativeBase + Y];
            }

            if (X != 0) {
                Computer->Index = Y - 1;
            }
        } else if (Instruction == 6) {
            int64_t X = Memory[++Computer->Index];
            if (ParameterMode1 == POSITION_MODE) {
                X = Memory[X];
            } else if (ParameterMode1 == RELATIVE_MODE) {
                X = Memory[Computer->RelativeBase + X];
            }

            int64_t Y = Memory[++Computer->Index];
            if (ParameterMode2 == POSITION_MODE) {
                Y = Memory[Y];
            } else if (ParameterMode2 == RELATIVE_MODE) {
                Y = Memory[Computer->RelativeBase + Y];
            }

            if (X == 0) {
                Computer->Index = Y - 1;
            }
        } else if (Instruction == 7) {
            int64_t X = Memory[++Computer->Index];
            if (ParameterMode1 == POSITION_MODE) {
                X = Memory[X];
            } else if (ParameterMode1 == RELATIVE_MODE) {
                X = Memory[Computer->RelativeBase + X];
            }

            int64_t Y = Memory[++Computer->Index];
            if (ParameterMode2 == POSITION_MODE) {
                Y = Memory[Y];
            } else if (ParameterMode2 == RELATIVE_MODE) {
                Y = Memory[Computer->RelativeBase + Y];
            }

            int64_t Z = Memory[++Computer->Index];
            if (ParameterMode3 == POSITION_MODE) {
                Memory[Z] = (X < Y) ? 1 : 0;
            } else if (ParameterMode3 == RELATIVE_MODE) {
                Memory[Computer->RelativeBase + Z] = (X < Y) ? 1 : 0;
            }
        } else if (Instruction == 8) {
            int64_t X = Memory[++Computer->Index];
            if (ParameterMode1 == POSITION_MODE) {
                X = Memory[X];
            } else if (ParameterMode1 == RELATIVE_MODE) {
                X = Memory[Computer->RelativeBase + X];
            }

            int64_t Y = Memory[++Computer->Index];
            if (ParameterMode2 == POSITION_MODE) {
                Y = Memory[Y];
            } else if (ParameterMode2 == RELATIVE_MODE) {
                Y = Memory[Computer->RelativeBase + Y];
            }

            int64_t Z = Memory[++Computer->Index];
            if (ParameterMode3 == POSITION_MODE) {
                Memory[Z] = (X == Y) ? 1 : 0;
            } else if (ParameterMode3 == RELATIVE_MODE) {
                Memory[Computer->RelativeBase + Z] = (X == Y) ? 1 : 0;
            }
        } else if (Instruction == 9) {
            int64_t X = Memory[++Computer->Index];
            if (ParameterMode1 == POSITION_MODE) {
                X = Memory[X];
            } else if (ParameterMode1 == RELATIVE_MODE) {
                X = Memory[Computer->RelativeBase + X];
            }

            Computer->RelativeBase += X;
        } else if (Instruction == 99) {
            Computer->Halted = true;
        } else {
            printf("Invalid program state.\n");
            exit(EXIT_FAILURE);
        }

        Computer->Index += 1;
    }
}

int64_t NetworkReader(Computer_t* Computer) {
    int64_t Packet = 0;
    if (PopPacket(Computer, &Packet)) {
        Computer->IdleCount = 0;
        return Packet;
    }

    Computer->IdleCount += 1;
    return -1;
}

void NetworkWriter(Computer_t* Computer, int64_t Value) {
    Computer->IdleCount = 0;

    Computer->Step += 1;
    assert(Computer->Step <= 3);
    Computer->Buffer[Computer->Step - 1] = Value;

    if (Computer->Step == 3) {
        Computer->Step = 0;

        int Address = Computer->Buffer[0];
        if (Address == 255) {
            Computer->NatDevice->PacketCount = 0;
            PushPacket(Computer->NatDevice, Computer->Buffer[1]);
            PushPacket(Computer->NatDevice, Computer->Buffer[2]);
        } else {
            PushPacket(&Computer->Network[Address], Computer->Buffer[1]);
            PushPacket(&Computer->Network[Address], Computer->Buffer[2]);
        }
    }
}

int const NETWORK_SIZE = 50;

int SolvePuzzle1(Input_t Input) {
    Computer_t NatDevice = {};
    Computer_t* Computers = (Computer_t*)
        malloc(NETWORK_SIZE * sizeof(Computer_t));

    for (int i = 0; i < NETWORK_SIZE; ++i) {
        Computers[i].Program = Input;
        Computers[i].Network = Computers;
        Computers[i].NatDevice = &NatDevice;
        
        PushPacket(&Computers[i], i);
    }

    bool Done = false;
    while (Done == false) {
        for (int i = 0; i < NETWORK_SIZE; ++i) {
            StepProgram(&Computers[i], NetworkReader, NetworkWriter);

            if (NatDevice.PacketCount) {
                Done = true;
                break;
            }
        }
    }

    free(Computers);
    return NatDevice.Packets[1];
}

int const MAX_VALUE_COUNT = 32;

int SolvePuzzle2(Input_t Input) {
    Computer_t NatDevice = {};
    Computer_t* Computers = (Computer_t*)
        malloc(NETWORK_SIZE * sizeof(Computer_t));

    for (int i = 0; i < NETWORK_SIZE; ++i) {
        Computers[i].Program = Input;
        Computers[i].Network = Computers;
        Computers[i].NatDevice = &NatDevice;
        
        PushPacket(&Computers[i], i);
    }

    int ValueCount = 0;
    int64_t Values[MAX_VALUE_COUNT] = {};

    bool Done = false;
    while (Done == false) {
        for (int i = 0; i < NETWORK_SIZE; ++i) {
            StepProgram(&Computers[i], NetworkReader, NetworkWriter);
        }

        bool Idle = true;
        for (int i = 0; i < NETWORK_SIZE; ++i) {
            if (Computers[i].IdleCount < 3) {
                Idle = false;
                break;
            }
        }

        if (Idle) {
            int64_t X, Y;
            if (PopPacket(&NatDevice, &X)
                && PopPacket(&NatDevice, &Y)) {
                    ValueCount += 1;
                    assert(ValueCount <= MAX_VALUE_COUNT);
                    Values[ValueCount - 1] = Y;

                    PushPacket(&Computers[0], X);
                    PushPacket(&Computers[0], Y);

                    for (int i = 0; i < NETWORK_SIZE; ++i) {
                        Computers[i].IdleCount = 0;
                    }
            } else {
                Done = true;
            }
        }
    }

    int64_t ValueIndex = -1;
    for (int i = 0; i < ValueCount; ++i) {
        for (int j = i + 1; j < ValueCount; ++j) {
            if (Values[i] == Values[j]) {
                ValueIndex = i;
                break;
            }
        }

        if (ValueIndex >= 0) {
            break;
        }
    }

    free(Computers);
    return Values[ValueIndex];
}

extern "C"
int main(int argc, char const* argv[]) {
    Input_t Input = LoadInput();
    printf("Part 1: %d\n", SolvePuzzle1(Input));
    printf("Part 2: %d\n", SolvePuzzle2(Input));
    return EXIT_SUCCESS;
}
