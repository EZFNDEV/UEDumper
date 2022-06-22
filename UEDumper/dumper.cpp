#include "pch.h"

#include <map>

#include "utils.h"
#include "dumper.h"
#include "CoreUObject/UObject/UObjectHash.h"
#include "CoreUObject/UObject/UObjectBaseUtility.h"
#include "CoreUObject/UObject/UnrealTypePrivate.h"
#include <unordered_map>
#include "formatting/sdk.h"
#include "offsets/CoreUObject.h"

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
        printf("Name: %s\n", Utils::UKismetStringLibrary::Conv_NameToString(Name).ToString().c_str());
		

        printf("Function Start: %p\n", FunctionStart - (uintptr_t)GetModuleHandle(0));
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
	// We need that for our gameserver (Remove on release)
    //GetRealFunction_Test();
   // return;

    bool bNewObjectArray = IsOldObjectArray();
    GUObjectArray = *new FUObjectArray(Offsets::GObjects, !bNewObjectArray);

    UObjectBaseUtility* Object = (UObjectBaseUtility*)GUObjectArray.IndexToObject(0)->Object;

	// TODO: Check some stuff related to ChildProperties
	// TOOD: Move this into offsets.cpp
	
    static UClass* CoreUObjectFunction = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Function");
    static UClass* CoreUObjectClass = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Class");
    static UClass* CoreUObjectStruct = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Struct");
    static UClass* CoreUObjectScriptStruct = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.ScriptStruct");
    static UClass* CoreUObjectPackage = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Package");
    static UClass* CoreUObjectEnum = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Enum");

	// TODO: CLEAN UP SDK.CPP
    #ifdef DUMP_AS_SDK
        std::unordered_map<__int64, Ofstreams*> packagesSDK;

        std::ofstream Core(("SDK/" + std::string(SHORTNAME) + "_Core.hpp"));

        SDKFormatting::CreateSDKHeader(Core);
		
		// Im brain dead atm, I will just try something then 
		// explain how the sorting algorithm works
        std::unordered_map<__int64, __int64> packageStructs;
        std::vector<__int64> structsAdded;
    #endif

    for (uintptr_t i = 0; i < GUObjectArray.Num(); i++) {
        auto Item = GUObjectArray.IndexToObject(i);
        if (Item) {
            Object = (UObjectBaseUtility*)Item->Object;
				
            if (!Object)
                continue;

            if (Object->IsA(CoreUObjectClass)) {
                #ifdef DUMP_AS_SDK
                    std::ofstream Classes;
                    std::ofstream Functions;
                    std::ofstream Structs;

                    Ofstreams* streams = 0;

                    for (auto const& [key, val] : packagesSDK)
                    {
                        if (key == (__int64)Object->GetOuter()) {
                            streams = val;
                            break;
                        }
                    }

                    if (!streams) continue;

                    SDKFormatting::LupusFormatUClass((UClass*)Object, streams);
                #endif
            } else if (Object->IsA(CoreUObjectEnum)) { // Slow
				#ifdef DUMP_AS_SDK
                    std::ofstream Classes;
                    std::ofstream Functions;
                    std::ofstream Structs;

                    Ofstreams* streams = 0;

                    for (auto const& [key, val] : packagesSDK)
                    {
                        if (key == (__int64)Object->GetOuter()) {
                            streams = val;
                            break;
                        }
                    }

                    if (!streams) continue;
					
                    SDKFormatting::LupusFormatStruct((UClass*)Object, streams, &packageStructs, true, false, false, &structsAdded, packagesSDK);
					
                    //packageStructs.emplace((__int64)Object, i);
                    //streams->Structs << SDKFormatting::CreateStruct((UStruct*)Object, stru);
				#endif
            } else if (Object->IsA(CoreUObjectScriptStruct)) {  
                #ifdef DUMP_AS_SDK
                    std::ofstream Classes;
                    std::ofstream Functions;
                    std::ofstream Structs;

                    Ofstreams* streams = 0;

                    for (auto const& [key, val] : packagesSDK)
                    {
                        if (key == (__int64)Object->GetOuter()) {
                            streams = val;
                            break;
                        }
                    }

                    if (!streams) continue;

                    SDKFormatting::LupusFormatStruct((UClass*)Object, streams, &packageStructs, false, true, false, &structsAdded, packagesSDK);

                    //packageStructs.emplace((__int64)Object, i);
                    //streams->Structs << SDKFormatting::CreateStruct((UStruct*)Object, stru);
                #endif
            }
            else if (Object->IsA(CoreUObjectStruct)) {
                #ifdef DUMP_AS_SDK
                    std::ofstream Classes;
                    std::ofstream Functions;
                    std::ofstream Structs;

                    Ofstreams* streams = 0;

                    for (auto const& [key, val] : packagesSDK)
                    {
                        if (key == (__int64)Object->GetOuter()) {
                            streams = val;
                            break;
                        }
                    }

                    if (!streams) continue;

                    SDKFormatting::LupusFormatStruct((UClass*)Object, streams, &packageStructs, false, false, true, &structsAdded, packagesSDK);

                    //packageStructs.emplace((__int64)Object, i);
                    //streams->Structs << SDKFormatting::CreateStruct((UStruct*)Object, stru);
                #endif
            }
            else if (Object->IsA(CoreUObjectPackage)) {
                #ifdef DUMP_AS_SDK
                    std::string name = Object->GetName().ToString();
                    name = name.substr(name.find_last_of("/") + 1, name.length());

                    #ifdef UE_FILTER_CLASS
                        if (name == UE_FILTER_CLASS) 
                    #else
                    #endif
                    {
                        Ofstreams* streams = new Ofstreams{
                                .Classes = std::ofstream(("SDK/Packages/" + std::string(SHORTNAME) + "_" + name + "_classes.hpp")),
                                .Functions = std::ofstream(("SDK/Packages/" + std::string(SHORTNAME) + "_" + name + "_functions.cpp")),
                                .Structs = std::ofstream(("SDK/Packages/" + std::string(SHORTNAME) + "_" + name + "_structs.hpp"))
                        };

                        packagesSDK.emplace((__int64)Object, streams);

                        streams->Classes << ((std::format("#pragma once\n\n#include \"../{}\"\n\n", std::string(SHORTNAME) + "_Core.hpp")) + "namespace SDK {\n\n\n");
                        streams->Functions << std::format("#include \"../{}\"\n\n", std::string(SHORTNAME) + "_Core.hpp");
                        streams->Structs << ((std::format("#pragma once\n\n#include \"../{}\"\n\n", std::string(SHORTNAME) + "_Core.hpp")) + "namespace SDK {\n\n\n");

                        // Core << std::format("#include \"{}\"\n", ("./Packages/" + std::string(SHORTNAME) + "_" + name + "_structs.hpp"));
                        // Core << std::format("#include \"{}\"\n", ("./Packages/" + std::string(SHORTNAME) + "_" + name + "_classes.hpp"));
                        // Core << std::format("#include \"{}\"\n", ("./Packages/" + std::string(SHORTNAME) + "_" + name + "_functions.cpp"));
                    }
				#endif
            }
        }
    }

    #ifdef DUMP_AS_SDK
	    // All that were not generated until now, must be generated now...
    for (auto const& [key, val] : packageStructs)
    {
        
        // if (((UObjectBaseUtility*)val)->IsA(Core))

        Ofstreams* Nestreams = 0;

        for (auto const& [key, val] : packagesSDK)
        {
            if (key == (__int64)((UClass*)val)->GetOuter()) {
                Nestreams = val;
                break;
            }
        }

        if (!Nestreams) break;



        SDKFormatting::LupusFormatStruct((UClass*)val, Nestreams, &packageStructs, false, true, false, &structsAdded, packagesSDK);
        break;
    }

	
        for (auto const& [key, val] : packagesSDK)
        {
            val->Classes << "\n}";
            val->Structs << "\n}";
			
            val->Classes.close();
            val->Structs.close();
            val->Functions.close();
        }
    #endif
}

void Dumper::DumpObjectNames() {
    bool bNewObjectArray = IsOldObjectArray();
    GUObjectArray = *new FUObjectArray(Offsets::GObjects, !bNewObjectArray);

    // Calcuate time from starting and ending using std::chrono
    auto Start = std::chrono::high_resolution_clock::now();

    std::ofstream ObjectsDump("SDK/ObjectsDump.txt");

    for (uintptr_t i = 0; i < GUObjectArray.Num(); i++) {
        auto Item = GUObjectArray.IndexToObject(i);
        if (Item) {
            auto Object = (uintptr_t*)Item->Object;
            ObjectsDump << std::format("[{}] {}\n", i, Utils::UKismetSystemLibrary::GetPathName(Object).ToString());
        }
    }

    auto End = std::chrono::steady_clock::now();

    ObjectsDump.close();
    printf("Dumped %d objects in %.02f ms\n", GUObjectArray.Num(), (End - Start).count() / 1000000.);
}
