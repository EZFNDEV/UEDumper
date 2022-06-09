#include "pch.h"
#include <thread>
#include "utils.h"
#include "dumper.h"

#define SEARCH_OFFSETS
#define PRINT_OFFSETS
#define DUMP

static void Main() {

    AllocConsole();

    FILE* file;
    freopen_s(&file, "CONOUT$", "w", stdout);

    #ifdef SEARCH_OFFSETS 
	    // Some of this is "dumb", for example we search a function 2 times, but we do that
        // to make the code a bit more clear, it also only takes a few milliseconds so yea...
	
        printf("Searching for offsets\n");

        Offsets::StaticFindObject = OffsetsFinder::FindStaticFindObject();
        if (!Offsets::StaticFindObject) {
            printf("Failed to find StaticFindObject\n");
            return;
        }

        Offsets::UFunction::Func = OffsetsFinder::FindUFunctionOffset_Func();
		
        Offsets::ProcessEvent = OffsetsFinder::FindProcessEvent();
        if (!Offsets::ProcessEvent) {
			printf("Failed to find ProcessEvent\n");
			return;
        }

		Offsets::UObject::ProcessEvent = OffsetsFinder::FindUObject_PEVTableIndex();

        Offsets::GObjects = OffsetsFinder::FindGObjects();
		if (!Offsets::GObjects) {
			printf("Failed to find GObjects\n");
			return;
		}
    #else
	// Set your offsets here
    Offsets::ProcessEvent = 0;
    #endif

    #ifdef PRINT_OFFSETS
        printf("ProcessEvent: %p\n", Offsets::ProcessEvent);
        printf("StaticFindObject: %p\n", Offsets::StaticFindObject);
        printf("GObjects: %p\n", Offsets::GObjects);

        printf("    UFunction:\n");
        printf("        Func: %p\n", Offsets::UFunction::Func);

        printf("    UObject:\n");
        printf("        ProcessEvent VTable Index: %p\n", Offsets::UObject::ProcessEvent);
    #endif

	// Note: Just temp, you can remove this if you dont inject on startup
    std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 60));
		
    #ifdef DUMP
        Utils::_StaticFindObject = decltype(Utils::_StaticFindObject)(Offsets::StaticFindObject);
        Utils::_ProcessEvent = decltype(Utils::_ProcessEvent)(Offsets::ProcessEvent);

        Utils::UKismetStringLibrary::Init();

        Dumper::Dump();
    #endif

    // TODO: Add FString, FName, UObject, UStruct because we need it for Dump()

    // UFunction: https://github.com/EpicGames/UnrealEngine/blob/46544fa5e0aa9e6740c19b44b0628b72e7bbd5ce/Engine/Source/Runtime/CoreUObject/Public/UObject/Class.h#L1887
        // Note Milxnor: So we put it into CoreUObject/UObject/Class.h

	// FString: https://github.com/EpicGames/UnrealEngine/blob/46544fa5e0aa9e6740c19b44b0628b72e7bbd5ce/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h#L50
        // Note Milxnor: Core/Containers/UnrealString.h

	
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)&Main, nullptr, NULL, nullptr);

    return TRUE;
}

