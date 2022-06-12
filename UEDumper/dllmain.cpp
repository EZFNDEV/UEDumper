#include "pch.h"
#include <thread>
#include "utils.h"
#include "dumper.h"
#include "CoreUObject/UObject/UObjectBaseUtility.h"
#include <iostream>

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

        Utils::_StaticFindObject = decltype(Utils::_StaticFindObject)(Offsets::StaticFindObject);

        Offsets::UFunction::Func = OffsetsFinder::FindUFunctionOffset_Func();
		
        Offsets::ProcessEvent = OffsetsFinder::FindProcessEvent();
        if (!Offsets::ProcessEvent) {
			printf("Failed to find ProcessEvent\n");
			return;
        }
		
        Utils::_ProcessEvent = decltype(Utils::_ProcessEvent)(Offsets::ProcessEvent);

		Offsets::UObject::ProcessEvent = OffsetsFinder::FindUObject_PEVTableIndex();

        Offsets::UObject::InternalIndex = OffsetsFinder::FindUObjectInternalIndex();
        if (!Offsets::UObject::InternalIndex) {
            printf("Failed to find the offset for UObject::InternalIndex\n");
        } else if (Offsets::UObject::InternalIndex != 0x0C) {
            printf("UObject::InternalIndex is not 0x0C? Lol (Oh, maybe this will fail for you then...)");
        }

        Offsets::UObjectBase::ClassPrivate = OffsetsFinder::FindUObjectBase_ClassPrivate();
		
        Offsets::UClass::ChildProperties = OffsetsFinder::FindUClass_ChildProperties();
		// TODO: Next...

        Offsets::UStruct::SuperStruct = OffsetsFinder::FindUStruct_SuperStruct();

        Offsets::UField::Next = 0x20; // TODO: Get this automatically (Easy)

        Offsets::GObjects = OffsetsFinder::FindGObjects();
		if (!Offsets::GObjects) {
			printf("Failed to find GObjects\n");
			return;
		}
    #else
	    // Set your offsets here
        Offsets::ProcessEvent = 0;
    #endif

    // A few checks to make sure ChildProperties doesn't exist

        // TODO: LMFAO

    Offsets::UStruct::Children = Offsets::UClass::ChildProperties;

    // NOTE: Milxnor
    // If PropertiesSize is higher than maxint, we do know that it also has ChildProperties, meaning we need to loop them too
    // otherwise we need to chcek if its 0, if its 0 it might also just be a empty ChildProperty, but if its 0 and Children is 0 too
    // we do know that its PropertiesSize so yea, what we gonna do is Get the first obj from GObjects (it has no children) and then check everything :)

    #ifdef PRINT_OFFSETS
        printf("ProcessEvent: %p\n", Offsets::ProcessEvent);
        printf("StaticFindObject: %p\n", Offsets::StaticFindObject);
        printf("GObjects: %p\n", Offsets::GObjects);

        printf("    UFunction:\n");
        printf("        Func: %p\n", Offsets::UFunction::Func);

        printf("    UObject:\n");
        printf("        InternalIndex: %p\n", Offsets::UObject::InternalIndex);
        printf("        ProcessEvent VTable Index: %p\n", Offsets::UObject::ProcessEvent);

        printf("    UClass:\n");
        printf("        ChildProperties: %p\n", Offsets::UClass::ChildProperties);

        printf("    UObjectBase:\n");
        printf("        ClassPrivate: %p\n", Offsets::UObjectBase::ClassPrivate);

        printf("    UStruct:\n");
        printf("        SuperStruct: %p\n", Offsets::UStruct::SuperStruct);
    #endif

	// Note: Just temp, you can remove this if you dont inject on startup
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 60));
		
    #ifdef DUMP
        

        while (!Utils::UKismetStringLibrary::Init()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        while (!Utils::UKismetSystemLibrary::Init()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        Dumper::Dump();
    #endif

	/*
        UObjectBaseUtility* (__fastcall * _StaticFindObject) (uintptr_t * ObjectClass, uintptr_t * InObjectPackage, const wchar_t* OrigInName, bool ExactClass);
    _StaticFindObject = decltype(_StaticFindObject)(Offsets::StaticFindObject);

    UObjectBaseUtility* Engine = _StaticFindObject(0, 0, L"Transient.FortEngine_0_1", false);

    std::cout << "Engine " << Engine << '\n';

	UClass* PlayerControllerClass = (UClass*)_StaticFindObject(0, 0, L"FortniteGame.FortPlayerControllerAthena", false);

    std::cout << "PlayercontrollerClass: " << PlayerControllerClass << '\n';

    std::cout << "IsA: " << Engine->IsA(PlayerControllerClass) << '\n';
    */

    // TODO: Add UObject because we need it for Dump()
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

