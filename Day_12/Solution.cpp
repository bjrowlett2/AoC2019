#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdint>
#include <cstdlib>

int const AXIS_COUNT = 3;

struct Planet_t {
    int Position[AXIS_COUNT];
    int Velocity[AXIS_COUNT];
};

int const MAX_PLANET_COUNT = 4;

struct Input_t {
    Planet_t Planets[MAX_PLANET_COUNT];
};

int ScanForStart(char* Buffer, int Base) {
    int Offset = Base;
    while (Buffer[Offset] != '-'
        && !isdigit(Buffer[Offset])) {
            Offset += 1;
    }

    return Offset;
}

int ScanForEnd(char* Buffer, int Base, char Symbol) {
    int Offset = Base;
    while (Buffer[Offset] != Symbol) {
        Offset += 1;
    }

    Buffer[Offset] = NULL;
    return Offset;
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

    int Base = 0;
    int Offset = 0;
    Input_t Result = {};
    for (int i = 0; i < MAX_PLANET_COUNT; ++i) {
        Base = ScanForStart(Buffer, Offset);
        Offset = ScanForEnd(Buffer, Base, ',');
        Result.Planets[i].Position[0] = atoi(Buffer + Base);

        Base = ScanForStart(Buffer, Offset);
        Offset = ScanForEnd(Buffer, Base, ',');
        Result.Planets[i].Position[1] = atoi(Buffer + Base);

        Base = ScanForStart(Buffer, Offset);
        Offset = ScanForEnd(Buffer, Base, '>');
        Result.Planets[i].Position[2] = atoi(Buffer + Base);
    }

    free(Buffer);
    return Result;
}

void SimulateUniverseAxis(Input_t* Input, int Axis) {
    for (int j = 0; j < MAX_PLANET_COUNT; ++j) {
        for (int i = j + 1; i < MAX_PLANET_COUNT; ++i) {
            Planet_t* A = &Input->Planets[i];
            Planet_t* B = &Input->Planets[j];

            if (A->Position[Axis] < B->Position[Axis]) {
                A->Velocity[Axis] += 1;
                B->Velocity[Axis] -= 1;
            } else if (B->Position[Axis] < A->Position[Axis]) {
                A->Velocity[Axis] -= 1;
                B->Velocity[Axis] += 1;
            }
        }
    }

    for (int i = 0; i < MAX_PLANET_COUNT; ++i) {
        Planet_t* Planet = &Input->Planets[i];
        Planet->Position[Axis] += Planet->Velocity[Axis];
    }
}

void SimulateUniverse(Input_t* Input) {
    for (int Axis = 0; Axis < AXIS_COUNT; ++Axis) {
        SimulateUniverseAxis(Input, Axis);
    }
}

int PotentialEnergy(Planet_t Planet) {
    return abs(Planet.Position[0])
         + abs(Planet.Position[1])
         + abs(Planet.Position[2]);
}

int KineticEnergy(Planet_t Planet) {
    return abs(Planet.Velocity[0])
         + abs(Planet.Velocity[1])
         + abs(Planet.Velocity[2]);
}

int TotalPlanetEnergy(Planet_t Planet) {
    return KineticEnergy(Planet)
         * PotentialEnergy(Planet);
}

int SolvePuzzle1(Input_t Input) {
    for (int Step = 0; Step < 1000; ++Step) {
        SimulateUniverse(&Input);
    }

    int TotalEnergy = 0;
    for (int i = 0; i < MAX_PLANET_COUNT; ++i) {
        Planet_t* Planet = &Input.Planets[i];
        TotalEnergy += TotalPlanetEnergy(*Planet);
    }

    return TotalEnergy;
}

bool BackToInitialState(Input_t* Input) {
    for (int i = 0; i < MAX_PLANET_COUNT; ++i) {
        Planet_t* Planet = &Input->Planets[i];
        if (TotalPlanetEnergy(*Planet) != 0) {
            return false;
        }
    }

    return true;
}

int SimulateUntilRepeats(Input_t* Input, int Axis) {
    int Steps = 0;
    do {
        Steps += 1;
        SimulateUniverseAxis(Input, Axis);
    } while (!BackToInitialState(Input));

    return 2 * Steps;
}

int GreatestCommonDivisor(int X, int Y) {
    if (Y == 0) {
        return X;
    }

    return GreatestCommonDivisor(Y, X % Y);
}

int64_t SolvePuzzle2(Input_t Input) {
    int64_t Steps[AXIS_COUNT] = {};
    for (int Axis = 0; Axis < AXIS_COUNT; ++Axis) {
        Steps[Axis] = SimulateUntilRepeats(&Input, Axis);
    }

    int64_t TotalSteps = Steps[0];
    for (int i = 1; i < AXIS_COUNT; ++i) {
        TotalSteps = (TotalSteps * Steps[i])
            / GreatestCommonDivisor(TotalSteps, Steps[i]);
    }

    return TotalSteps;
}

extern "C"
int main(int argc, char const* argv[]) {
    Input_t Input = LoadInput();
    printf("Part 1: %d\n", SolvePuzzle1(Input));
    printf("Part 2: %lld\n", SolvePuzzle2(Input));
    return EXIT_SUCCESS;
}
