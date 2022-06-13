#include "pch.h"
#include "utils.h"
#include "dumper.h"
#include "CoreUObject/UObject/UObjectHash.h"
#include "CoreUObject/UObject/UObjectBaseUtility.h"
#include "CoreUObject/UObject/UnrealTypePrivate.h"

static uintptr_t GetRealFunction_Test() {
    // Let's find all UFunctions

    // FF 93 (ProcessEvent Index * 8)

    // bad.
    uint32_t PEIdx = Offsets::UObject::ProcessEvent * 8;

    int x;
    char* peIdxstr = static_cast<char*>(static_cast<void*>(&PEIdx));

    std::vector<int> vectorr{ 0xFF, 0x93, 0x00, 0x00, 0x00, 0x00 };
    vectorr[2] = peIdxstr[0];
    vectorr[3] = peIdxstr[1];
    vectorr[4] = peIdxstr[2];
    vectorr[5] = peIdxstr[3];

    uint64_t LastAddress = 0;
    int found = 0;
    while (true) {
        LastAddress = Memory::Sexy::NO_STRPatternScan(vectorr, LastAddress == 0 ? LastAddress : LastAddress + 1);
        if (LastAddress == NULL) break;

        // Get the function start
        uintptr_t FunctionStart = LastAddress;
        for (uint8_t i = 0; i < 255; i++) {
            if (
                *(uint8_t*)(FunctionStart - i) == 0x48 &&
                *(uint8_t*)(FunctionStart - i + 1) == 0x89 &&
                *(uint8_t*)(FunctionStart - i + 2) == 0x5C
                ) {
                FunctionStart = FunctionStart - i;
                break;
            }
        }

        if (FunctionStart == LastAddress) continue; // Not the function we want

        uintptr_t NameAddr = 0;
        for (uint8_t i = 0; i < 255; i++) {
            if (
                *(uint8_t*)(FunctionStart + i) == 0x48 &&
                *(uint8_t*)(FunctionStart + i + 1) == 0x8B &&
                *(uint8_t*)(FunctionStart + i + 2) == 0x15
                ) {
                // Relative
                NameAddr = ((FunctionStart + i + 3 + 4) + *(int32_t*)(FunctionStart + i + 3));
                break;
            }
        }

        if (NameAddr == 0) continue; // Not the function we want

        FName* Name = (FName*)(NameAddr);
        printf("Name: %i\n", Name->GetComparisonIndex().Value);
        
        FString Result = Utils::UKismetStringLibrary::Conv_NameToString(*Name);
        printf("Result: %p\n", Result);

        printf("Function Start: %p\n", FunctionStart);
        printf("UFunction: %p\n", LastAddress);


        found += 1;
    }

    printf("Found: %i UFunctions!\n", found);

    return 0;
}

// Small tools (We should set that in dllmain.cpp but then it will not work from here, can u fix that? Milxnor)
static bool IsOldObjectArray() {
	// At least thats how it is for Fortnite... If it doesn't work for you feel free to fix it. Nvm...
	// return *(__int64*)((__int64)Offsets::GObjects + 0x10) == 0xFFFFFFFFFFFFFFFF;

    if ((new FUObjectArray(Offsets::GObjects, true))->Num() == -1) {
        return true;
    } else {
        return false;
    }
}

void Dumper::Dump() {
    //GetRealFunction_Test();
    //return;
    printf("Creating a whole dump now, this will take longer, if you don't need everything please change the settings.\n");

    bool bNewObjectArray = IsOldObjectArray();
    GUObjectArray = *new FUObjectArray(Offsets::GObjects, !bNewObjectArray);

    // NOTE: Milxnor
    // If PropertiesSize is higher than maxint, we do know that it also has ChildProperties, meaning we need to loop them too
    // otherwise we need to chcek if its 0, if its 0 it might also just be a empty ChildProperty, but if its 0 and Children is 0 too
    // we do know that its PropertiesSize so yea, what we gonna do is Get the first obj from GObjects (it has no children) and then check everything :)

    // Cases:
        // 1. PropertiesSize isnt actual a int32
        // 2. PropertiesSize is 0

    UObjectBaseUtility* Object = (UObjectBaseUtility*)GUObjectArray.IndexToObject(0)->Object;
	
    UClass* CoreUObjectFunction = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Function");
    UClass* CoreUObjectClass = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Class");

    #ifdef DUMP_JSON
        for (uintptr_t i = 0; i < GUObjectArray.Num(); i++) {
            auto Item = GUObjectArray.IndexToObject(i);
            if (Item) {
                Object = (UObjectBaseUtility*)Item->Object;
                //printf(Utils::UKismetSystemLibrary::GetPathName((uintptr_t*)Item->Object).ToString().c_str());
			
                if (Object->IsA(CoreUObjectFunction)) {
                    // printf("This is a function\n");
                    // printf(Utils::UKismetSystemLibrary::GetPathName((uintptr_t*)Item->Object).ToString().c_str());
                }
                else if (Object->IsA(CoreUObjectClass)) {
                    // printf("This is a chlass\n");

			        // Based on IterateToNext
				
                

                    for (UStruct* CurrentStruct = (UStruct*)Object->GetClass(); CurrentStruct; CurrentStruct = (UStruct*)CurrentStruct->GetSuperStruct()) {
                        UObjectPropertyBase* Field = (UObjectPropertyBase*)CurrentStruct->GetChildren();
					
                        if (IsBadReadPtr(Field, 8)) continue;

                        for (UObjectPropertyBase* CurrentField = (UObjectPropertyBase*)Field->GetNext(); CurrentField; CurrentField = (UObjectPropertyBase*)CurrentField->GetNext()) {
                            printf("STr: %s\n", CurrentField->GetFName().ToString().c_str());
                        }
                    }

                    // TODO: Get the superthing
                }
            }
        }
    #endif
   
}

void Dumper::DumpObjectNames() {
    GUObjectArray = *new FUObjectArray();
    // TODO: Figure out if its a new object or old
    GUObjectArray.useNewObjectArray = true;
    GUObjectArray.ObjObjectsNew = (FChunkedFixedUObjectArray*)Offsets::GObjects;
	
    for (uintptr_t i = 0; i < GUObjectArray.ObjObjectsNew->Num(); i++) {
        auto Item = GUObjectArray.IndexToObject(i);
        //printf("Item: %p\n", Item);
        if (Item) {
            auto Object = (uintptr_t*)Item->Object;
            printf("Object: %p\n", Object);
        }
    }
}