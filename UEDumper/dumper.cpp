#include "pch.h"

#include <map>

#include "utils.h"
#include "dumper.h"
#include "CoreUObject/UObject/UObjectHash.h"
#include "CoreUObject/UObject/UObjectBaseUtility.h"
#include "CoreUObject/UObject/UnrealTypePrivate.h"
#include <unordered_map>
#include "formatting/sdk.h"

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
    // GetRealFunction_Test();
    // return;
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
	
    static UClass* CoreUObjectFunction = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Function");
    static UClass* CoreUObjectClass = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Class");
    static UClass* CoreUObjectStruct = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Struct");
    static UClass* CoreUObjectScriptStruct = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.ScriptStruct");
    static UClass* CoreUObjectPackage = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Package");
    static UClass* CoreUObjectEnum = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Enum");

    struct FreakingPackage {
        std::ofstream Classes;
        std::ofstream Functions;
        std::ofstream Structs;
        // std::ofstream Parameters;
    };

    std::unordered_map<__int64, FreakingPackage*> packages;
    auto Start = std::chrono::high_resolution_clock::now();

    #ifdef DUMP_JSON
        for (uintptr_t i = 0; i < GUObjectArray.Num(); i++) {
            auto Item = GUObjectArray.IndexToObject(i);
            if (Item) {
                Object = (UObjectBaseUtility*)Item->Object;
				
                if (!Object)
                    continue;

                if (Object->IsA(CoreUObjectClass)) {
                    std::ofstream Classes;
                    std::ofstream Functions;
                    std::ofstream Structs;

                    FreakingPackage* FEAKIGNPACKAGE = 0;
					
                    for (auto const& [key, val] : packages)
                    {
                        if (key == (__int64)Object->GetOuter()) {
                            FEAKIGNPACKAGE = val;
                            break;
                        }
                    }

                    if (!FEAKIGNPACKAGE) {
                        // printf("We hgaven't bruh");
                        continue;
                    }

                    FEAKIGNPACKAGE->Classes << SDKFormatting::CreateClass((UStruct*)Object);
                }
				
                else if (Object->IsA(CoreUObjectEnum) || Object->IsA(CoreUObjectScriptStruct) || Object->IsA(CoreUObjectStruct)) {
                    std::ofstream Classes;
                    std::ofstream Functions;
                    std::ofstream Structs;

                    FreakingPackage* FEAKIGNPACKAGE = 0;

                    for (auto const& [key, val] : packages)
                    {
                        if (key == (__int64)Object->GetOuter()) {
                            FEAKIGNPACKAGE = val;
                            break;
                        }
                    }

                    if (!FEAKIGNPACKAGE) {
                        // printf("We hgaven't bruh");
                        continue;
                    }
					
                    FEAKIGNPACKAGE->Structs << SDKFormatting::CreateStruct((UStruct*)Object);
                }
				
                else if (Object->IsA(CoreUObjectPackage)) {
                    std::string name = Utils::UKismetSystemLibrary::GetObjectName((uintptr_t*)Object).ToString();

                    name = name.substr(name.find_last_of("/") + 1, name.length());

                    // printf(name.c_str());

                    FreakingPackage* pkgs = new FreakingPackage {
                        .Classes = std::ofstream(("SDK/Packages/" + std::string(SHORTNAME) + "_" + name + "_classes.hpp")),
                        .Functions = std::ofstream(("SDK/Packages/" + std::string(SHORTNAME) + "_" + name + "_functions.cpp")),
                        .Structs = std::ofstream(("SDK/Packages/" + std::string(SHORTNAME) + "_" + name + "_structs.hpp"))
                    };

                    packages.emplace((__int64)Object, pkgs);

                    pkgs->Classes << "#pragma once\n\n";
                    pkgs->Structs << "#pragma once\n\n";

                    pkgs->Classes << "#include \"Core.hpp\"\n";
					pkgs->Functions << "#include \"Core.hpp\"\n";
					pkgs->Structs << "#include \"Core.hpp\"\n";
                }
            }
        }
    #endif

    for (auto const& [key, val] : packages)
    {
        val->Classes.close();
        val->Structs.close();
        val->Functions.close();
    }
	
    auto End = std::chrono::steady_clock::now();

    printf("Generated SDK in %.02f ms\n", (End - Start).count() / 1000000.);
}

void Dumper::DumpObjectNames() {
    bool bNewObjectArray = IsOldObjectArray();
    GUObjectArray = *new FUObjectArray(Offsets::GObjects, !bNewObjectArray);

    // Calcuate time from starting and ending using std::chrono
    auto Start = std::chrono::high_resolution_clock::now();

    std::ofstream ObjectsDump("SDK/ObjectsDump.txt");

    for (uintptr_t i = 0; i < GUObjectArray.Num(); i++) {
        auto Item = GUObjectArray.IndexToObject(i);
        //printf("Item: %p\n", Item);
        if (Item) {
            auto Object = (uintptr_t*)Item->Object;
            // printf("Object: %p\n", Object);

            ObjectsDump << std::format("[{}] {}\n", i, Utils::UKismetSystemLibrary::GetPathName(Object).ToString());
        }
    }

    auto End = std::chrono::steady_clock::now();

    ObjectsDump.close();
    printf("Dumped %d objects in %.02f ms\n", GUObjectArray.Num(), (End - Start).count() / 1000000.);
}