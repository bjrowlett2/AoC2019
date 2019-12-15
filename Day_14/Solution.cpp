#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

int64_t const MAX_NAME_LENGTH = 8;

struct Chemical_t {
    int64_t Quantity;
    char Name[MAX_NAME_LENGTH];
};

int64_t const MAX_CHEMICAL_COUNT = 64;

struct Reaction_t {
    int64_t Quantity;
    char Name[MAX_NAME_LENGTH];

    int64_t ChemicalCount;
    Chemical_t Chemicals[MAX_CHEMICAL_COUNT];
};

int64_t const MAX_REACTION_COUNT = 64;

struct Input_t {
    int64_t ReactionCount;
    Reaction_t Reactions[MAX_REACTION_COUNT];
    
    int64_t LeftoverCount;
    Chemical_t Leftovers[MAX_CHEMICAL_COUNT];
};

int64_t ScanForInt(char* Buffer, int64_t* Value) {
    for (int64_t i = 0; Buffer[i] != NULL; ++i) {
        if (Buffer[i] == ' ') {
            Buffer[i] = NULL;
            *Value = atoi(Buffer);

            return i + 1;
        }
    }

    return 0;
}

int64_t ScanForString(char* Buffer, char* Value) {
    for (int64_t i = 0; Buffer[i] != NULL; ++i) {
        if ((Buffer[i] == ' ') || (Buffer[i] == '\n') || (Buffer[i] == ',')) {
            Buffer[i] = NULL;
            strcpy(Value, Buffer);

            return i + 1;
        }
    }

    return 0;
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

    long Offset = 0;
    Input_t Result = {};
    while (Offset < Length) {
        Reaction_t Reaction = {};

        Reaction.ChemicalCount += 1;
        assert(Reaction.ChemicalCount <= MAX_CHEMICAL_COUNT);
        Chemical_t* Chemical = &Reaction.Chemicals[Reaction.ChemicalCount - 1];
        Offset += ScanForInt(Buffer + Offset, &Chemical->Quantity);
        Offset += ScanForString(Buffer + Offset, &Chemical->Name[0]);

        while (Buffer[Offset] == ' ') {
            Offset += 1; // Skip whitespace
            
            Reaction.ChemicalCount += 1;
            assert(Reaction.ChemicalCount <= MAX_CHEMICAL_COUNT);
            Chemical_t* Chemical = &Reaction.Chemicals[Reaction.ChemicalCount - 1];
            Offset += ScanForInt(Buffer + Offset, &Chemical->Quantity);
            Offset += ScanForString(Buffer + Offset, &Chemical->Name[0]);
        }

        Offset += 3; // Skip arrow and whitespace
        Offset += ScanForInt(Buffer + Offset, &Reaction.Quantity);
        Offset += ScanForString(Buffer + Offset, &Reaction.Name[0]);

        Result.ReactionCount += 1;
        assert(Result.ReactionCount <= MAX_REACTION_COUNT);
        Result.Reactions[Result.ReactionCount - 1] = Reaction;
    }

    Reaction_t Ore = {};
    Ore.Quantity = 1;
    strcpy(Ore.Name, "ORE");

    Result.ReactionCount += 1;
    assert(Result.ReactionCount <= MAX_REACTION_COUNT);
    Result.Reactions[Result.ReactionCount - 1] = Ore;

    free(Buffer);
    return Result;
}

Reaction_t* FindReaction(Input_t* Input, char* Name) {
    for (int64_t i = 0; i < Input->ReactionCount; ++i) {
        Reaction_t* Reaction = &Input->Reactions[i];
        if (strcmp(Name, Reaction->Name) == 0) {
            return Reaction;
        }
    }

    return NULL;
}

Chemical_t* FindLeftover(Input_t* Input, Chemical_t* Chemical) {
    for (int64_t i = 0; i < Input->LeftoverCount; ++i) {
        Chemical_t* Leftover = &Input->Leftovers[i];
        if (strcmp(Chemical->Name, Leftover->Name) == 0) {
            return Leftover;
        }
    }

    return NULL;
}

int64_t Min(int64_t A, int64_t B) {
    return (A < B) ? A : B;
}

/// Note:
///     Not very happy with this solution, very slow...
///     Would like to come back and optimize it some time.
///                             - Brian Rowlett, 2019-12-15
int64_t OreToProduce(Input_t* Input, int64_t Amount, char* Name) {
    int64_t TotalOre = 0;
    while (Amount > 0) {
        Reaction_t* Parent = FindReaction(Input, Name);
        for (int64_t i = 0; i < Parent->ChemicalCount; ++i) {
            Chemical_t Chemical = Parent->Chemicals[i];
            if (strcmp("ORE", Chemical.Name) == 0) {
                TotalOre += Chemical.Quantity;
                continue;
            }

            Reaction_t* Reaction = FindReaction(Input, Chemical.Name);
            Chemical_t* Leftover = FindLeftover(Input, &Chemical);

            if (Leftover != NULL) {
                int64_t Reuse = Min(Chemical.Quantity, Leftover->Quantity);
                Chemical.Quantity -= Reuse;
                Leftover->Quantity -= Reuse;
            }

            double Needed = Chemical.Quantity;
            double Producable = Reaction->Quantity;
            int64_t Factor = ceil(Needed / Producable);

            int64_t Actual = Factor * Reaction->Quantity;
            if (Chemical.Quantity < Actual) {
                if (Leftover == NULL) {
                    Input->LeftoverCount += 1;
                    assert(Input->LeftoverCount <= MAX_CHEMICAL_COUNT);
                    Leftover = &Input->Leftovers[Input->LeftoverCount - 1];
                    strcpy(Leftover->Name, Chemical.Name);
                }

                Leftover->Quantity += Actual - Chemical.Quantity;
            }

            if (Chemical.Quantity != 0) {
                TotalOre += OreToProduce(Input, Chemical.Quantity, Chemical.Name);
            }
        }

        Amount -= Parent->Quantity;
    }

    return TotalOre;
}

int64_t SolvePuzzle1(Input_t Input) {
    return OreToProduce(&Input, 1, "FUEL");
}

int64_t SolvePuzzle2(Input_t Input) {
    int64_t Count = 0;
    int64_t TotalOre = 1000000000000;
    while (TotalOre > 0) {
        Count += 1;
        TotalOre -= OreToProduce(&Input, 1, "FUEL");
    }

    return Count - 1;
}

extern "C"
int main(int argc, char const* argv[]) {
    Input_t Input = LoadInput();
    printf("Part 1: %lld\n", SolvePuzzle1(Input));
    printf("Part 2: %lld\n", SolvePuzzle2(Input));
    return EXIT_SUCCESS;
}
