#include <cassert>
#include <cstdio>
#include <cstdlib>

struct Point_t {
    int X, Y, Steps;
};

#define MAX_POINT_COUNT 512

struct Wire_t {
    int Count;
    Point_t Points[MAX_POINT_COUNT];
};

struct Input_t {
    Wire_t Wires[2];
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
    Result.Wires[0].Count = 1;
    Result.Wires[0].Points[0].X = 0;
    Result.Wires[0].Points[0].Y = 0;
    Result.Wires[0].Points[0].Steps = 0;

    Result.Wires[1].Count = 1;
    Result.Wires[1].Points[0].X = 0;
    Result.Wires[1].Points[0].Y = 0;
    Result.Wires[1].Points[0].Steps = 0;

    int Offset = 0;
    int WireIndex = 0;
    int PointIndex = 1;
    for (int i = 0; i < Length; ++i) {
        bool NextWire = false;
        if (Buffer[i] == '\n') {
            NextWire = true;
        }

        if (Buffer[i] == ',' || Buffer[i] == '\n') {
            Buffer[i] = NULL;

            char Direction = *(Buffer + Offset);
            int Distance = atoi(Buffer + Offset + 1);

            Result.Wires[WireIndex].Count += 1;
            assert(Result.Wires[WireIndex].Count <= MAX_POINT_COUNT);

            Point_t* Point = Result.Wires[WireIndex].Points + PointIndex;
            Point_t* Previous = Result.Wires[WireIndex].Points + PointIndex - 1;

            if (Direction == 'U') {
                Point->X = Previous->X;
                Point->Y = Previous->Y + Distance;
            } else if (Direction == 'D') {
                Point->X = Previous->X;
                Point->Y = Previous->Y - Distance;
            } else if (Direction == 'L') {
                Point->X = Previous->X - Distance;
                Point->Y = Previous->Y;
            } else if (Direction == 'R') {
                Point->X = Previous->X + Distance;
                Point->Y = Previous->Y;
            }

            Point->Steps = Previous->Steps + abs(Distance);

            Offset = i + 1;
            PointIndex += 1;
        }

        if (NextWire) {
            WireIndex += 1;
            PointIndex = 1;
        }
    }

    free(Buffer);
    return Result;
}

bool LinesIntersect(Point_t V1, Point_t V2, Point_t H1, Point_t H2) {
    bool T1 = ((H1.X <= V1.X) && (V1.X <= H2.X))
           || ((H2.X <= V1.X) && (V1.X <= H1.X));
    bool T2 = ((V1.Y <= H1.Y) && (H1.Y <= V2.Y))
           || ((V2.Y <= H1.Y) && (H1.Y <= V1.Y));
    return T1 && T2;
}

bool LinesIntersect(Point_t A1, Point_t A2, Point_t B1, Point_t B2, Point_t* HitPoint) {
    bool Hit = false;

    // A is Vertical, B is Horizontal
    if ((A1.X == A2.X) && (B1.Y == B2.Y)) {
        if (LinesIntersect(A1, A2, B1, B2)) {
            Hit = true;
            HitPoint->X = A1.X;
            HitPoint->Y = B1.Y;
        }
    }
    
    // A is Horizontal, B is Vertical
    else if ((A1.Y == A2.Y) && (B1.X == B2.X)) {
        if (LinesIntersect(B1, B2, A1, A2)) {
            Hit = true;
            HitPoint->X = B1.X;
            HitPoint->Y = A1.Y;
        }
    }

    return Hit;
}

int Manhattan(Point_t A, Point_t B) {
    return abs(A.X - B.X) + abs(A.Y - B.Y);
}

int SolvePuzzle1(Input_t Input) {
    int HitDistance = INT_MAX;
    for (int i = 0; i < Input.Wires[0].Count - 1; ++i) {
        Point_t A1 = Input.Wires[0].Points[i];
        Point_t A2 = Input.Wires[0].Points[i+1];

        for (int j = 0; j < Input.Wires[1].Count - 1; ++j) {
            Point_t B1 = Input.Wires[1].Points[j];
            Point_t B2 = Input.Wires[1].Points[j+1];

            Point_t Origin = {};
            Point_t HitPoint = {};
            if (LinesIntersect(A1, A2, B1, B2, &HitPoint)) {
                int Distance = Manhattan(Origin, HitPoint);
                if ((0 < Distance) && (Distance < HitDistance)) {
                    HitDistance = Distance;
                }
            }
        }
    }

    return HitDistance;
}

int SolvePuzzle2(Input_t Input) {
    int TotalSteps = INT_MAX;
    for (int i = 0; i < Input.Wires[0].Count - 1; ++i) {
        Point_t A1 = Input.Wires[0].Points[i];
        Point_t A2 = Input.Wires[0].Points[i+1];

        for (int j = 0; j < Input.Wires[1].Count - 1; ++j) {
            Point_t B1 = Input.Wires[1].Points[j];
            Point_t B2 = Input.Wires[1].Points[j+1];

            Point_t HitPoint = {};
            if (LinesIntersect(A1, A2, B1, B2, &HitPoint)) {
                int Steps = A1.Steps + Manhattan(A1, HitPoint)
                          + B1.Steps + Manhattan(B1, HitPoint);
                if ((0 < Steps) && (Steps < TotalSteps)) {
                    TotalSteps = Steps;
                }
            }
        }
    }

    return TotalSteps;
}

extern "C"
int main(int argc, char const* argv[]) {
    Input_t Input = LoadInput();
    printf("Part 1: %d\n", SolvePuzzle1(Input));
    printf("Part 2: %d\n", SolvePuzzle2(Input));
    return EXIT_SUCCESS;
}
