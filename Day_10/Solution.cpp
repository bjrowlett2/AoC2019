#include <cassert>
#include <cfloat>
#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>
#include <cstdlib>

struct Input_t {
    int Width;
    int Height;
    char* Locations;
};

struct Station_t {
    int X;
    int Y;
    int Count;
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
    bool FirstRow = true;
    for (long i = 0; i < Length; ++i) {
        if (FirstRow) {
            Result.Width += 1;
        }
        
        if (Buffer[i] == '\n') {
            FirstRow = false;
            Result.Height += 1;
        }
    }

    Result.Width -= 1;
    int Size = sizeof(bool);
    int Count = Result.Width * Result.Height;
    Result.Locations = (char*) malloc(Count * Size);

    for (int Y = 0; Y < Result.Height; ++Y) {
        for (int X = 0; X < Result.Width; ++X) {
            int Index = Y * Result.Width + X;

            int Offset = Index + Y;
            Result.Locations[Index] = Buffer[Offset];
        }
    }

    free(Buffer);
    return Result;
}

bool WithinBounds(int Lower, int Value, int Upper) {
    return ((Lower <= Value) && (Value < Upper));
}

bool WithinAllBounds(Input_t Input, int X, int Y) {
    return WithinBounds(0, X, Input.Width)
        && WithinBounds(0, Y, Input.Height);
}

bool HitAlongLine(Input_t Input, int* X, int* Y, int dX, int dY) {
    while (WithinAllBounds(Input, *X + dX, *Y + dY)) {
        *X += dX;
        *Y += dY;

        int Index = *Y * Input.Width + *X;
        if (Input.Locations[Index] == '#') {
            return true;
        }
    }
    
    return false;
}

int GreatestCommonDivisor(int X, int Y) {
    if (Y == 0) {
        return X;
    }

    return GreatestCommonDivisor(Y, X % Y);
}

Station_t SolvePuzzle1(Input_t Input) {
    Station_t Station = {};
    Station.Count = INT_MIN;
    for (int Y = 0; Y < Input.Height; ++Y) {
        for (int X = 0; X < Input.Width; ++X) {
            int Index = Y * Input.Width + X;
            if (Input.Locations[Index] == '#') {
                int Count = 0;
                for (int dY = -Y; dY < Input.Height - Y; ++dY) {
                    for (int dX = -X; dX < Input.Width - X; ++dX) {
                        if ((dX == 0) && (dY == 0)) {
                            continue;
                        }

                        if (abs(GreatestCommonDivisor(dX, dY)) == 1) {
                            int CollideX = X;
                            int CollideY = Y;
                            if (HitAlongLine(Input, &CollideX, &CollideY, dX, dY)) {
                                Count += 1;
                            }
                        }
                    }
                }

                if (Count > Station.Count) {
                    Station.X = X;
                    Station.Y = Y;
                    Station.Count = Count;
                }
            }
        }
    }

    return Station;
}

int SolvePuzzle2(Input_t Input, Station_t Station) {
    int X = 0;
    int Y = 0;
    int Count = 0;
    float PrevAngle = -M_PI_2 - FLT_EPSILON;
    while (Count < 200) {
        float MinAngle = FLT_MAX;
        float MinDistance = FLT_MAX;
        for (int dY = -Station.Y; dY < Input.Height - Station.Y; ++dY) {
            for (int dX = -Station.X; dX < Input.Width - Station.X; ++dX) {
                if ((dX == 0) && (dY == 0)) {
                    continue;
                }

                if (abs(GreatestCommonDivisor(dX, dY)) == 1) {
                    int CollideX = Station.X;
                    int CollideY = Station.Y;
                    if (HitAlongLine(Input, &CollideX, &CollideY, dX, dY)) {
                        float Distance = sqrt(dX * dX + dY* dY);
                        float Angle = atan2(dY / Distance, dX / Distance);
                        if ((PrevAngle < Angle) && (Angle < MinAngle)) {
                            X = CollideX;
                            Y = CollideY;

                            MinAngle = Angle;
                        } else if (Angle == MinAngle) {
                            if (Distance < MinDistance) {
                                X = CollideX;
                                Y = CollideY;

                                MinDistance = Distance;
                            }
                        }
                    }
                }
            }
        }

        Count += 1;
        int Index = Y * Input.Width + X;
        Input.Locations[Index] = ' ';

        PrevAngle = MinAngle;
        if (fabs(M_PI - PrevAngle) < FLT_EPSILON) {
            PrevAngle -= (2 * M_PI);
        }
    }

    return X * 100 + Y;
}

extern "C"
int main(int argc, char const* argv[]) {
    Input_t Input = LoadInput();
    Station_t Station = SolvePuzzle1(Input);
    printf("Part 1: %d\n", Station.Count);
    printf("Part 2: %d\n", SolvePuzzle2(Input, Station));
    return EXIT_SUCCESS;
}
