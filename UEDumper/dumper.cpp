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
        
        uintptr_t Result = Utils::UKismetStringLibrary::Conv_NameToString(*Name);
        printf("Result: %p\n", Result);

        printf("Function Start: %p\n", FunctionStart);
        printf("UFunction: %p\n", LastAddress);


        found += 1;
    }

    printf("Found: %i UFunctions!\n", found);

    return 0;
}

void Dumper::Dump() {
    //GetRealFunction_Test();
    //return;
	
    printf("Creating a whole dump now, this will take longer, if you don't need everything please change the settings.\n");

    // TODO: Find a way to determine the object array type.
    bool bNewObjectArray = false;
    printf("Offsets::GObjects: %p\n", Offsets::GObjects);
    GUObjectArray = *new FUObjectArray(Offsets::GObjects, bNewObjectArray);

    printf("We are dumping everything now...\n");

    printf("There are %d objects\n", GUObjectArray.Num());

	// TODO: UObject struct for GetClass() etc

    UClass* CoreUObjectFunction = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Function");
    UClass* CoreUObjectClass = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Class");

    for (uintptr_t i = 0; i < GUObjectArray.Num(); i++) {
        auto Item = GUObjectArray.IndexToObject(i);
        //printf("Item: %p\n", Item);
        if (Item) {
            UObjectBaseUtility* Object = (UObjectBaseUtility*)Item->Object;
            // printf("Object: %p\n", Object);

            // printf("Class: %p\n", Object->GetClass());
			
            //printf(Utils::UKismetSystemLibrary::GetPathName((uintptr_t*)Item->Object).ToString().c_str());
			
            
            if (Object->IsA(CoreUObjectFunction)) {
                // printf("This is a function\n");
                // printf(Utils::UKismetSystemLibrary::GetPathName((uintptr_t*)Item->Object).ToString().c_str());
            }
            else if (Object->IsA(CoreUObjectClass)) {
                // printf("This is a chlass\n");

				// Based on IterateToNext
				
                

                for (UStruct* CurrentStruct = (UStruct*)Object->GetClass(); CurrentStruct; CurrentStruct = (UStruct*)CurrentStruct->GetSuperStruct()) {
                    printf("CurrentStruct: %p\n", CurrentStruct);
                    printf("Children: %p\n", CurrentStruct->GetChildren());

                    UObjectPropertyBase* Field = (UObjectPropertyBase*)CurrentStruct->GetChildren();
                    printf("First field: %p\n", Field);
					
                    if (IsBadReadPtr(Field, 8)) continue;

                    for (UObjectPropertyBase* CurrentField = (UObjectPropertyBase*)Field->GetNext(); CurrentField; CurrentField = (UObjectPropertyBase*)CurrentField->GetNext()) {
                        printf("CurrentField: %p\n", CurrentField);
                    }
                }

                // TODO: Get the superthing
            }
        }
    }

   
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