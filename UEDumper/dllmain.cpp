#include "pch.h"
#include <thread>
#include "utils.h"
#include "dumper.h"
#include "CoreUObject/UObject/UObjectBaseUtility.h"
#include <iostream>
#include <filesystem>

#include "buildSettings.h"
#include "offsets/CoreUObject.h"

static void Main() {
    AllocConsole();
    FILE* file;
    freopen_s(&file, "CONOUT$", "w", stdout);

    #ifdef SEARCH_OFFSETS 
        // Some of this is "dumb", for example we search a function 2 times, but we do that
        // to make the code a bit more clear, it also only takes a few milliseconds so yea...
        printf("Searching for offsets\n");

	    // Small delay since it crashes sometimes
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        if (!OffsetsFinder::FindAll()) {
            printf("Please check which offsets were not found, or which do look invalid, you can then manually patch them or open a bug report so we can fix it.\n");
            return;
        }
    #else
	    // Set your offsets here
        Offsets::ProcessEvent = 0;
    #endif

    // TODO: Find them automatically,
    Offsets::UProperty::PropertyFlags = 0x38;
    Offsets::UProperty::ArrayDim = 0x30;

    // TODO: (Inside offsets.cpp maybe) Check if child properties exists, or if it is children

    #ifdef PRINT_OFFSETS
        printf("Memory base: %p\n", GetModuleHandle(0));
        printf("ProcessEvent: %p\n", Offsets::ProcessEvent);
        printf("StaticFindObject: %p\n", Offsets::StaticFindObject);
        printf("GObjects: %p\n", Offsets::GObjects);

        printf("    UFunction:\n");
        printf("        Func: %p\n", Offsets::UFunction::Func);

        printf("    UObject:\n");
        printf("        InternalIndex: %p\n", Offsets::UObject::InternalIndex);
        printf("        ProcessEvent VTable Index: %p\n", Offsets::UObject::ProcessEvent);

        printf("    UObjectBase:\n");
        printf("        NamePrivate: %p\n", Offsets::UObjectBase::NamePrivate);
        printf("        ClassPrivate: %p\n", Offsets::UObjectBase::ClassPrivate);

        printf("    UStruct:\n");
        printf("        ChildProperties: %p\n", Offsets::UStruct::ChildProperties);
        printf("        SuperStruct: %p\n", Offsets::UStruct::SuperStruct);

        printf("    UObjectPropertyBase:\n");
        printf("        PropertyClass: %p\n", Offsets::UObjectPropertyBase::PropertyClass);

        printf("    UProperty:\n");
        printf("        ElementSize: %p\n", CoreUObjectOffsetFinder::_UProperty::FindElementSize());
        printf("        Offset_Internal: %p\n", Offsets::UProperty::Offset_Internal);

        printf("    UField:\n");
        printf("        Next: %p\n", Offsets::UField::Next);
    #endif

	// Note: Just temp, you can remove this if you dont inject on startup
    std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 15));

    MakeDirs();
    wprintf(L"Output path: %s\\SDK\\\n", GetCurrentDir().c_str());

    #ifdef DUMP_OBJECT_NAMES
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Dumper::DumpObjectNames, 0, 0, 0); // Tbh if we dump object names and sdk we might as well just loop objects once
    #endif

    #if defined(DUMP_AS_JSON) || defined(DUMP_AS_SDK)
        auto Start = std::chrono::high_resolution_clock::now();
        Dumper::Dump();
        printf("Generated SDK in %.02f ms\n", (std::chrono::steady_clock::now() - Start).count() / 1000000.);
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

