#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define MAX_CHILD_COUNT 16

struct Object_t {
    char Name[4];
    Object_t* Parent;

    int ChildCount;
    Object_t* Children[MAX_CHILD_COUNT];
};

#define MAX_OBJECT_COUNT 1536

struct Input_t {
    int ObjectCount;
    Object_t* Objects;
};

Input_t NewInput() {
    Input_t Result = {};
    Result.Objects = (Object_t*)
        malloc(MAX_OBJECT_COUNT * sizeof(Object_t));
    return Result;
}

void FreeInput(Input_t Input) {
    free(Input.Objects);
}

Object_t* FindObject(Input_t Input, char* Name) {
    for (int i = 0; i < Input.ObjectCount; ++i) {
        if (strcmp(Name, Input.Objects[i].Name) == 0) {
            return &Input.Objects[i];
        }
    }

    return NULL;
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

    int Offset = 0;
    char* ChildName = NULL;
    char* ParentName = NULL;

    Input_t Result = NewInput();
    for (int i = 0; i < Length; ++i) {
        if (Buffer[i] == ')') {
            Buffer[i] = NULL;
            ParentName = Buffer + Offset;

            Offset = i + 1;
        }
        else if (Buffer[i] == '\n') {
            Buffer[i] = NULL;
            ChildName = Buffer + Offset;

            Object_t* Child = FindObject(Result, ChildName);
            Object_t* Parent = FindObject(Result, ParentName);

            if (Child == NULL) {
                Result.ObjectCount += 1;
                assert(Result.ObjectCount <= MAX_OBJECT_COUNT);

                int ChildIndex = Result.ObjectCount - 1;
                strcpy(Result.Objects[ChildIndex].Name, ChildName);

                Child = &Result.Objects[ChildIndex];
            }

            if (Parent == NULL) {
                Result.ObjectCount += 1;
                assert(Result.ObjectCount <= MAX_OBJECT_COUNT);

                int ParentIndex = Result.ObjectCount - 1;
                strcpy(Result.Objects[ParentIndex].Name, ParentName);

                Parent = &Result.Objects[ParentIndex];
            }

            Child->Parent = Parent;

            Parent->ChildCount += 1;
            assert(Parent->ChildCount <= MAX_CHILD_COUNT);
            Parent->Children[Parent->ChildCount - 1] = Child;

            Offset = i + 1;
        }
    }

    free(Buffer);
    return Result;
}

int CountOrbits(Input_t Input, Object_t* Object, int Depth) {
    int Count = Depth;
    if (Object != NULL) {
        for (int i = 0; i < Object->ChildCount; ++i) {
            Object_t* Child = Object->Children[i];
            Count += CountOrbits(Input, Child, Depth + 1);
        }
    }

    return Count;
}

Object_t* FindCommonAncestor(Object_t* A, Object_t* B) {
    Object_t* Goal = A;
    while (Goal->Parent != NULL) {
        Object_t* Test = B;
        while (Test->Parent != NULL) {
            if (strcmp(Goal->Name, Test->Name) == 0) {
                return Goal;
            }

            Test = Test->Parent;
        }

        Goal = Goal->Parent;
    }

    printf("No ancestor found.\n");
    exit(EXIT_FAILURE);
}

int CountOrbitalTransfers(Object_t* A, Object_t* B) {
    int Count = 0;
    while (A != B) {
        Count += 1;
        A = A->Parent;
    }

    return Count;
}

int SolvePuzzle1(Input_t Input) {
    Object_t* Root = FindObject(Input, "COM");
    return CountOrbits(Input, Root, 0);
}

int SolvePuzzle2(Input_t Input) {
    Object_t* San = FindObject(Input, "SAN");
    Object_t* You = FindObject(Input, "YOU");

    Object_t* Ancestor = FindCommonAncestor(San, You);
    return CountOrbitalTransfers(San->Parent, Ancestor)
         + CountOrbitalTransfers(You->Parent, Ancestor);
}

extern "C"
int main(int argc, char const* argv[]) {
    Input_t Input = LoadInput();
    printf("Part 1: %d\n", SolvePuzzle1(Input));
    printf("Part 2: %d\n", SolvePuzzle2(Input));

    FreeInput(Input);
    return EXIT_SUCCESS;
}
