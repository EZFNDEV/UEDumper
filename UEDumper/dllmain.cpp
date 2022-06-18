#include "pch.h"
#include <thread>
#include "utils.h"
#include "dumper.h"
#include "CoreUObject/UObject/UObjectBaseUtility.h"
#include <iostream>
#include <filesystem>

#include "buildSettings.h"

namespace fs = std::filesystem;

bool MakeDirectories() // Lupus put this somewhere
{
    bool a = true;

    if (fs::exists("SDK/"))
    {
        try {
            a = fs::remove_all("SDK/");
        } catch (...) {
			return false;
		}
    }
	
    auto b = fs::create_directory("SDK/");
    auto c = fs::create_directory("SDK/Packages/");

    printf("Created directories!\n");

    return a && b && c;
}

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
    }
    else if (Offsets::UObject::InternalIndex != 0xC) {
        printf("UObject::InternalIndex is not 0x0C? Lol (Oh, maybe this will fail for you then...)");
    }

    Offsets::UObjectBase::ClassPrivate = OffsetsFinder::FindUObjectBase_ClassPrivate();


    // TOOD: Idk, but we use this for everything, because its the first after UProperty, so we can calucate everything
    // else with it, it if we should
    // SOAHDSAIHDISABDVASDVASIUDVFASBFSAHIFG
    Offsets::UObjectPropertyBase::PropertyClass = OffsetsFinder::FindUObjectPropertyBase_PropertyClass();

    Offsets::UEnumProperty::UnderlyingProp = Offsets::UObjectPropertyBase::PropertyClass;
    Offsets::UEnumProperty::Enum = Offsets::UEnumProperty::UnderlyingProp + 8;

    Offsets::UMulticastDelegateProperty::SignatureFunction = Offsets::UObjectPropertyBase::PropertyClass;
    Offsets::UDelegateProperty::SignatureFunction = Offsets::UMulticastDelegateProperty::SignatureFunction;
	
	Offsets::UStructProperty::Struct = Offsets::UObjectPropertyBase::PropertyClass;
        
     Offsets::UStruct::ChildProperties = OffsetsFinder::FindUStruct_ChildProperties();

    Offsets::UStruct::SuperStruct = OffsetsFinder::FindUStruct_SuperStruct();

    // This required ChildProperties and Children
    Offsets::UObjectBase::NamePrivate = OffsetsFinder::FindUObjectBase_NamePrivate();

    Offsets::GObjects = OffsetsFinder::FindGObjects();
    if (!Offsets::GObjects) {
        printf("Failed to find GObjects\n");
        return;
    }

    Offsets::UStruct::Children = Offsets::UStruct::ChildProperties;

    // For the next offsets we need some more stuff
    while (!Utils::UKismetStringLibrary::Init()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    while (!Utils::UKismetSystemLibrary::Init()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    Offsets::UField::Next = OffsetsFinder::FindUField_Next();

    Offsets::UProperty::Offset_Internal = OffsetsFinder::FindUProperty_OffsetInternal();
	
    Offsets::UObjectPropertyBase::PropertyClass = OffsetsFinder::FindUObjectPropertyBase_PropertyClass();

    #else
	// Set your offsets here
    Offsets::ProcessEvent = 0;
    #endif

        
	
    // A few checks to make sure ChildProperties doesn't exist

        // TODO: Take the check from Dump::Dump and put it into here (not possible bc then the array will be 0 in dump, fuck... Milxnor help)

    #ifdef PRINT_OFFSETS
    printf("ProcessEvent: %p\n", Offsets::ProcessEvent);
    printf("StaticFindObject: %p\n", Offsets::StaticFindObject);
    printf("GObjects: %p\n", Offsets::GObjects);

    printf("    UFunction:\n");
    printf("        Func: %p\n", Offsets::UFunction::Func);

    printf("    UObject:\n");
    printf("        InternalIndex: %p\n", Offsets::UObject::InternalIndex);
    printf("        ProcessEvent VTable Index: %p\n", Offsets::UObject::ProcessEvent);

    printf("    UObjectBase:\n");
    printf("        ClassPrivate: %p\n", Offsets::UObjectBase::ClassPrivate);

    printf("    UStruct:\n");
    printf("        ChildProperties: %p\n", Offsets::UStruct::ChildProperties);
    printf("        SuperStruct: %p\n", Offsets::UStruct::SuperStruct);

    printf("    UObjectPropertyBase:\n");
    printf("        PropertyClass: %p\n", Offsets::UObjectPropertyBase::PropertyClass);

    printf("    UProperty:\n");
    printf("        Offset_Internal: %p\n", Offsets::UProperty::Offset_Internal);
    #endif

	// Note: Just temp, you can remove this if you dont inject on startup
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 60));
		
    if (!MakeDirectories())
    {
        printf("Failed to create directories!\n");
        FreeLibraryAndExitThread(GetModuleHandleW(0), 0);
        // return;
    }

    #ifdef DUMP
    printf("Dumping SDK!\n");
    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Dumper::DumpObjectNames, 0, 0, 0); // Tbh if we dump object names and sdk we might as well just loop objects once
    Dumper::Dump();
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

