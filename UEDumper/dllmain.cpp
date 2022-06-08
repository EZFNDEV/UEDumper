#include "pch.h"

#define SEARCH_OFFSETS
#define PRINT_OFFSETS
// #define DUMP

static void Main() {

    #ifdef SEARCH_OFFSETS 
	    // Some of this is "dumb", for example we search a function 2 times, but we do that
        // to make the code a bit more clear, it also only takes a few milliseconds so yea...
	
        printf("Searching for offsets\n");

        Offsets::StaticFindObject = OffsetsFinder::FindStaticFindObject();
        if (!Offsets::StaticFindObject) {
            printf("Failed to find StaticFindObject\n");
            return;
        }

        OffsetsFinder::FindUFunctionOffset_Func();
		
        Offsets::ProcessEvent = OffsetsFinder::FindProcessEvent();
        if (!Offsets::ProcessEvent) {
			printf("Failed to find ProcessEvent\n");
			return;
        }

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
    #endif

	

    #ifdef DUMP
	    
    #endif
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

