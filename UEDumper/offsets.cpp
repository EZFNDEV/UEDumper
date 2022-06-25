#include "pch.h"
#include "offsets.h"

#include <thread>
#include <iostream>
#include "utils.h"
#include "CoreUObject/UObject/Class.h"
#include "offsets/CoreUObject.h"

// Utils
uintptr_t OffsetsFinder::FindRealFunction(uintptr_t* _Function) { // THIS ONLY WORKS FOR UKISMETSYSTEMLIBRARY 
	UFunction* Function = (UFunction*)_Function;

    void* Func = Function->GetFunc();
	uintptr_t Address = (uintptr_t)Func;

    // Try to find the ret
    for (uint16_t i = 0; i < 500; i++) {
        if (*(uint8_t*)(Address + i) == 0xC3) {
            Address = Address + i;
            break;
        }
    }

    if (Address == (uintptr_t)Func) {
		printf("Failed to find ret\n");
		return 0;
	}

	// Now we can go backwards and get the 2nd last function
	// the last one is a __security_check_cookie NOO
	
    bool firstFound = true; // FAKED
	for (uint8_t i = 0; i < 255; i++) {
        if (*(uint8_t*)(Address - i) == 0xE8) {
            if (firstFound) {
                return ((Address - i + 1 + 4) + *(int32_t*)(Address - i + 1));
            }
			
            firstFound = true;
        }
    }
	
    return 0;
}

// UClass stuff
uint16_t OffsetsFinder::FindUObjectBase_ClassPrivate() {
    uintptr_t* Function = Utils::StaticFindObject(L"Engine.KismetSystemLibrary.SetBoolPropertyByName");
    if (!Function) return 0;

    uintptr_t RealFunction = OffsetsFinder::FindRealFunction(Function);
    if (!RealFunction) return 0;
	
    for (uint8_t i = 0; i < 255; i++) {
        if (
            *(uint8_t*)(RealFunction + i) == 0x48 &&
            *(uint8_t*)(RealFunction + i + 1) == 0x8B &&
            (
                *(uint8_t*)(RealFunction + i + 2) == 0x41 || // rax
                *(uint8_t*)(RealFunction + i + 2) == 0x49 || // rcx
                *(uint8_t*)(RealFunction + i + 2) == 0x51 // rdx
			)
        ) {
            return *(uint8_t*)(RealFunction + i + 3);
        }
    }

    return 0;
}

uint16_t OffsetsFinder::FindUObjectBase_NamePrivate() {
	// NOTE: I think this is always right, we might do a check though...

    // Check idea :)

	// We get Engine.Engine, after that we can get the children
	// when we got the first Field, we get the name of TinyFont and then we
	// can check the memory of Field, to find the name
	// easy :)

	// TinyFontName
    uintptr_t* Engine = Utils::StaticFindObject(L"Engine.Engine");

	

    // printf("Name: %p\n", Utils::UKismetStringLibrary::Conv_StringToName(L"TinyFontName"));
	
    return Offsets::UObjectBase::ClassPrivate + 0x8;
}

uint16_t OffsetsFinder::FindUStruct_SuperStruct() {
    uintptr_t* Object = Utils::StaticFindObject(L"Engine.World");
    while (!Object) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

	/* New
    UStruct* SuperStruct;
    UField* Children;
    FField* ChildProperties;*/

    /* Old
    UStruct* SuperStruct;
    UField* Children;*/

	// So, if World - 16 is null (bc the bytes before super struct are null)
	// we know that its the old version

	// I know it's tricky, but yea
    // TODO: We are gonna check if its really the SuperStruct
    if (*(__int64*)((__int64)Object + (Offsets::UStruct::ChildProperties - 16)) == 0) {
        // printf("Old version\n");
        return Offsets::UStruct::ChildProperties - 8;
    } else {
        return Offsets::UStruct::ChildProperties - 16;
    }

    // This was just testing to make sure all other patterns work

	// Go down 8 in a loop, make sure its a pointer, then get the object name, if its right we know
	// that we got the SuperStruct offsets :)

    for (
        uint16_t i = Offsets::UStruct::ChildProperties;
        i > Offsets::UObject::InternalIndex;
        i -= 8
    ) {
	    // So, it will be SuperStruct, or Children...
		uintptr_t* Object = (uintptr_t*)((__int64)Object + i);
		
        // printf("Object: %p");
    }
    
    return 0;
}

uint16_t OffsetsFinder::FindUStruct_ChildProperties() {
    uintptr_t* Function = Utils::StaticFindObject(L"Engine.KismetSystemLibrary.SetBoolPropertyByName");
    if (!Function) return 0;
	
    uintptr_t RealFunction = OffsetsFinder::FindRealFunction(Function);
    if (!RealFunction) return 0;

    uint16_t SizeOfFunction = 0;
    for (uint16_t i = 0; i < 500; i++) {
        if (
            *(uint8_t*)(RealFunction + i) == 0xC3 && 
            *(uint8_t*)(RealFunction + i - 1) == 0x5F
        ) {
            SizeOfFunction = i;
            break;
        }
    }

    if (SizeOfFunction == 0) return 0;

    uintptr_t StartPoint = RealFunction;

    if (SizeOfFunction < 100) { // Small = its called FindField/FindFProperty
		// It's using FindField/FindFProperty, so we nee to find the (first) E8
        uintptr_t FindProperty = 0;
        for (uint8_t i = 0; i < 255; i++) {
            if (*(uint8_t*)(RealFunction + i) == 0xE8) {
                FindProperty = ((RealFunction + i + 1 + 4) + *(int32_t*)(RealFunction + i + 1));
                // printf("FindProperty: %p\n", FindProperty);
                break;
            }
        }

        if (FindProperty == 0) return 0;

        StartPoint = FindProperty;
    }

    for (uint8_t i = 0; i < 255; i++) {
        if (
            *(uint8_t*)(StartPoint + i) == 0x74 &&
            *(uint8_t*)(StartPoint + i + 1) == 0x0B
        ) {
            return *(uint8_t*)(StartPoint + i + 5);
        }
    }

    return 0;
}

// Using ProcessEvent
uint16_t OffsetsFinder::FindUObjectInternalIndex() {
    uintptr_t CurrentAddress = Offsets::ProcessEvent;

    // Let's skip the first pushs
    for (uint8_t i = 0; i < 256; i++) {
        if (*(uint8_t*)(CurrentAddress + i) == 0x48 && *(uint8_t*)(CurrentAddress + i + 1) == 0x81 && *(uint8_t*)(CurrentAddress + i + 2) == 0xEC) {
            CurrentAddress += i + 3;
            break;
        }

        if (i == 255) return false;
    }
	
    // InternalIndex = mov eax, [rcx+0Ch]
    // InternalIndex = movsxd rax, dword ptr [rcx+0Ch]
    for (uint8_t i = 0; i < 256; i++) {
        if (*(uint8_t*)(CurrentAddress + i) == 0x41) {
            return *(uint8_t*)(CurrentAddress + i + 1);
        }

        if (i == 255) return false;
    }
	
    return 0;
}

uint16_t OffsetsFinder::FindUFunctionOffset_Func() {
    uintptr_t* (__fastcall * _StaticFindObject) (uintptr_t * ObjectClass, uintptr_t * InObjectPackage, const wchar_t* OrigInName, bool ExactClass);
    _StaticFindObject = decltype(_StaticFindObject)(Offsets::StaticFindObject);

    // We need to wait until this is found, and it will be found for UE4.0+
    uintptr_t* Object = 0;
    uint16_t tries = 0;
    while (Object == 0 && tries < 0x100) {
        Object = _StaticFindObject(0, 0, L"Engine.PlayerController.ServerUpdateLevelVisibility", false);
        tries += 1;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (!Object) return 0;

    for (uint8_t i = 8; i < 255; i++) { // Mostly it's 0xD8 or 0xE0
		// Let's find a valid memory address
		if (*(uint8_t*)((__int64)Object + i) == 0x7F) {
            // We might have found a valid memory address
			
            uintptr_t address = *(uintptr_t*)((__int64)Object + i - 5);
            if (address > 0x7FF000000000 && address < 0x7FFFFFFFFFFFF) {
                return i - 5;
            }
        }
    }

    return 0;
}


uint16_t OffsetsFinder::FindUField_Next() {
	// We are smart
    printf("ASOIDASNDASD\n");
    uintptr_t* (__fastcall * _StaticFindObject) (uintptr_t * ObjectClass, uintptr_t * InObjectPackage, const wchar_t* OrigInName, bool ExactClass);
    _StaticFindObject = decltype(_StaticFindObject)(Offsets::StaticFindObject);

    // We need to wait until this is found, and it will be found for UE4.0+
    uintptr_t* Object = 0;
    uint16_t tries = 0;
    while (Object == 0 && tries < 0x100) {
        Object = _StaticFindObject(0, 0, L"Engine.Engine", false);
        tries += 1;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (!Object) return 0;

    printf("WE got the engine: %p\n", Object);

	// I hate life

    if (((UStruct*)Object)->GetChildren()) {
        UField* FirstProperty = (UField*)((UStruct*)Object)->GetChildren();

        uint64_t TinyFontName = Utils::UKismetStringLibrary::Conv_StringToName_G(L"TinyFontName");
        
        for (uint16_t i = Offsets::UObjectBase::NamePrivate; i < 0xFF; i += 8) { // bruh FF and uint16, im so smart
            uintptr_t Property = *(uintptr_t*)((__int64)FirstProperty + i);

            if (IsBadReadPtr((void*)Property, 8)) continue;

            if (
				*(uint64_t*)((__int64)Property + Offsets::UObjectBase::NamePrivate) == TinyFontName
            ) {

                printf("Well um yea sorry milxnor: %i\n", i);
                printf("FirstProperty: %p\n", FirstProperty);
                return i;
            }
        }
    }
}

uint16_t OffsetsFinder::FindUObjectPropertyBase_PropertyClass() {
	// NOTE: THIS IS WORSE, BUT IT WORKS FOR THE VERSIONS I TESTED
	
    uintptr_t* Function = Utils::StaticFindObject(L"Engine.KismetSystemLibrary.SetObjectPropertyByName");
    if (!Function) return 0;

    uintptr_t RealFunction = OffsetsFinder::FindRealFunction(Function);
    if (!RealFunction) return 0;

    for (uint16_t i = 0; i < 0x100; i++) {
        if (
			*(uint8_t*)(RealFunction + i) == 0x8B &&
            *(uint8_t*)(RealFunction + i + 4) == 0x8B &&
            *(uint8_t*)(RealFunction + i + 7) == 0x49
        ) {
            return *(uint8_t*)(RealFunction + i + 2);
        }
    }
	
    return 0;
}

uint16_t OffsetsFinder::FindUProperty_OffsetInternal() {
    // NOTE: THIS IS EVEN MORE WORSE, BUT IT WORKS FOR THE VERSIONS I TESTED

	

    uintptr_t* Function = Utils::StaticFindObject(L"Engine.KismetSystemLibrary.SetVectorPropertyByName");
    if (!Function) return 0;

    uintptr_t RealFunction = OffsetsFinder::FindRealFunction(Function);
    if (!RealFunction) return 0;

    for (uint16_t i = 0; i < 0x100; i++) {
        if (
            *(uint8_t*)(RealFunction + i) == 0x75 &&
            *(uint8_t*)(RealFunction + i + 1) == 0x14
        ) {
            return *(uint8_t*)(RealFunction + i + 5);
        }
    }

    return 0;
}

// Note: This is one more of the stupid things, but as said, it only takes a few milliseconds and makes it more readable
uint16_t OffsetsFinder::FindUObject_PEVTableIndex() {
    // Find any UObject class
    uintptr_t* (__fastcall * _StaticFindObject) (uintptr_t * ObjectClass, uintptr_t * InObjectPackage, const wchar_t* OrigInName, bool ExactClass);
    _StaticFindObject = decltype(_StaticFindObject)(Offsets::StaticFindObject);

    uintptr_t* Object = Object = _StaticFindObject(0, 0, L"CoreUObject.Object", false);
    if (Object == 0) return 0;

    __int64** vtable = *reinterpret_cast<__int64***>(Object);
    uint16_t index = 0;
    while (!IsBadReadPtr((void*)((__int64)vtable + (index * 8)), 8)) {
        if (*(__int64*)((__int64)vtable + (index * 8)) == Offsets::ProcessEvent) {
            return index;
        }

        index++;
    }

    return 0;
}


// Things below are tricky and might not always work, if it fails you need to hardcode them, or try to fix it :)


static bool VerifyProcessEvent(uintptr_t Address)
{
    // Now this is atleast for Fortnite, but between 1.8 and latest ProcessEvent has always started with this besides a few seasons between like s16-18

    return (
        *(uint8_t*)(Address) == 0x40 &&
        *(uint8_t*)(Address + 1) == 0x55 &&
        *(uint8_t*)(Address + 2) == 0x56 &&
        *(uint8_t*)(Address + 3) == 0x57 &&
        *(uint8_t*)(Address + 4) == 0x41 &&
        *(uint8_t*)(Address + 5) == 0x54 &&
        *(uint8_t*)(Address + 6) == 0x41 &&
        *(uint8_t*)(Address + 7) == 0x55 &&
        *(uint8_t*)(Address + 8) == 0x41 &&
        *(uint8_t*)(Address + 9) == 0x56 &&
        *(uint8_t*)(Address + 10) == 0x41
    );

    // Most of the time UE4 Games have a string above ProcessEvent like 2 functions above "AccessNoNoneContext", we may be able to check if this string is above it.
    return false;
}

static uintptr_t FindPE_1() {
    // Find any UObject class
    uintptr_t* (__fastcall * _StaticFindObject) (uintptr_t * ObjectClass, uintptr_t * InObjectPackage, const wchar_t* OrigInName, bool ExactClass);
    _StaticFindObject = decltype(_StaticFindObject)(Offsets::StaticFindObject);

    uintptr_t* Object = Object = _StaticFindObject(0, 0, L"CoreUObject.Object", false);
    if (Object == 0) return 0;

    __int64** vtable = *reinterpret_cast<__int64***>(Object);
    uint16_t index = 0;
    while (!IsBadReadPtr((void*)((__int64)vtable + (index * 8)), 8)) {
        if (VerifyProcessEvent(__int64(*(__int64*)((__int64)vtable + (index * 8)))))
        {
            return *(__int64*)((__int64)vtable + (index * 8));
        }
		
        index++;
    }

    return 0;
}

static uintptr_t FindPE_2() {
    uint64_t LastAddress = 0;
    while (true) {
        LastAddress = Memory::Sexy::PatternScan("0F 85 ? ? 00 00 F7 ? ? 00 00 00 00 04 00 00", LastAddress); // test dword ptr [rsi+98h], 400h
        if (!LastAddress) break;

        // A small check to make sure its process event ("0F 85 ? ? 00 00")
        // Maximal 10 bytes away ig, I tested it on a UE4 project and there was a mov between the two insturctions so yea
        bool isProcessEvent = false;
        for (uint8_t i = 0; i < 10; i++) {
            if (*(uint8_t*)(LastAddress - i) == 0x0F && *(uint8_t*)(LastAddress - i + 1) == 0x85 /*&& *(uint8_t*)(LastAddress - i + 4) == 0x00 && *(uint8_t*)(LastAddress - i + 5) == 0x00*/) {
                isProcessEvent = true;
                break;
            }
        }

        if (isProcessEvent) {
            for (uint8_t i = 0; i < 255; i++) {
                if (*(uint8_t*)(LastAddress - i) == 0x40 && *(uint8_t*)(LastAddress - i + 1) == 0x55) {
                    return LastAddress - i;
                }
            }
        }

        // We might reach this, but doesnt matter bc then we will find the correct function
        return 0;
    }
    return 0;
}

uintptr_t OffsetsFinder::FindProcessEvent() {
    // "Hacky" ways to find process event:
        // 1. Search a UObject and loop until you find a function starting with "40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC" (maybe a bit less, but this works for Fortnite...)
            // We can then get 3-4 other objects and check if they have the same function at that index, if all have that we can most likely assume that this is process event
        // 2. There is a small pattern that (almost) every UE4 game has in the PE pattern, so we can search that and go back to the begin of the function
    
    uintptr_t ProcessEvent = FindPE_1();

    // printf("PE1 works: %p\n", ProcessEvent);

    if (!ProcessEvent)
    {
        ProcessEvent = FindPE_2();

        if (!VerifyProcessEvent(ProcessEvent))
			// Very unlikly to happen
            ProcessEvent = 0;
    }

    return ProcessEvent;
}

uintptr_t OffsetsFinder::FindStaticFindObject() {
    uintptr_t StaticFindObjectStrAddr = Memory::FortKit::FindPattern("49 00 6C 00 6C 00 65 00 67 00 61 00 6C 00 20 00 63 00 61 00 6C 00 6C 00 20 00 74 00 6F 00 20 00 53 00 74 00 61 00 74 00 69 00 63 00 46 00 69 00 6E 00 64 00 4F 00 62 00 6A 00 65 00 63 00 74 00 28 00 29 00 20 00 77 00 68 00 69 00 6C 00 65 00 20 00 73 00 65 00 72 00 69 00 61 00 6C 00 69 00 7A 00 69 00 6E 00 67 00 20 00 6F 00 62 00 6A 00 65 00 63 00 74 00 20 00 64 00 61 00 74 00 61 00 21");
    uintptr_t StaticFindObjectAddr = (uintptr_t)Memory::FortKit::FindXREF(StaticFindObjectStrAddr);

    // printf("StaticFindObjectAddr: %p\n", StaticFindObjectAddr);

    if (StaticFindObjectAddr == 0) {
        // Find it by Temp
        StaticFindObjectAddr = (uintptr_t)Memory::FortKit::FindByString(L"/Temp/%s", { Memory::FortKit::ASM::LEA });
        return 0;
        return StaticFindObjectAddr;
    }

    for (StaticFindObjectAddr; StaticFindObjectAddr > 0; StaticFindObjectAddr -= 1) {
        // if (GetBytes(StaticFindObjectAddr, 5).starts_with("48 89 5C 24 08")) // do something like this
		// NOTE (Lupus): This might be slower...
        if (
            *(uint8_t*)StaticFindObjectAddr == 0x48 &&
            *(uint8_t*)(StaticFindObjectAddr + 1) == 0x89 &&
            *(uint8_t*)(StaticFindObjectAddr + 2) == 0x5C &&
            *(uint8_t*)(StaticFindObjectAddr + 3) == 0x24 &&
            *(uint8_t*)(StaticFindObjectAddr + 4) == 0x08
        ) {
            return StaticFindObjectAddr;
        }
    }

    return 0;
}

uintptr_t OffsetsFinder::FindGObjects() {
	// Find GObjects by Process Event
	
    // Bc it uses https://github.com/EpicGames/UnrealEngine/blob/46544fa5e0aa9e6740c19b44b0628b72e7bbd5ce/Engine/Source/Runtime/CoreUObject/Private/UObject/UObjectArray.cpp#L365
    // its easily possible to get it :)

    uint8_t found = 0;
    for (uint16_t i = 0; i < 0x256; i++) {
        if (
            *(uint8_t*)(Offsets::ProcessEvent + i) == 0x48 &&
            *(uint8_t*)(Offsets::ProcessEvent + i + 1) == 0x8B &&
            *(uint8_t*)(Offsets::ProcessEvent + i + 2) == 0x05
        ) {
            found += 1;

            if (found == 2) {
                // Relative
                uintptr_t addr = Offsets::ProcessEvent + i;
                return ((addr + 3 + 4) + *(int32_t*)(addr + 3));
            }
        }
    }

	return 0;
}


bool OffsetsFinder::FindAll() {
    Offsets::StaticFindObject = OffsetsFinder::FindStaticFindObject();
    if (!Offsets::StaticFindObject) {
        printf("Failed to find StaticFindObject\n");
        return false;
    }

    Utils::_StaticFindObject = decltype(Utils::_StaticFindObject)(Offsets::StaticFindObject);

    Offsets::UFunction::Func = OffsetsFinder::FindUFunctionOffset_Func();
    // TODO: FIND!
    Offsets::UFunction::FunctionFlags = Offsets::UFunction::Func - 0x28;

    Offsets::ProcessEvent = OffsetsFinder::FindProcessEvent();
    if (!Offsets::ProcessEvent) {
        printf("Failed to find ProcessEvent\n");
        return false;
    }
    Offsets::GObjects = OffsetsFinder::FindGObjects();
    if (!Offsets::GObjects) {
        printf("Failed to find GObjects\n");
        return false;
    }

    Utils::_ProcessEvent = decltype(Utils::_ProcessEvent)(Offsets::ProcessEvent);

    Offsets::UObject::ProcessEvent = OffsetsFinder::FindUObject_PEVTableIndex();

    CoreUObjectOffsetFinder::Init();
	
    Offsets::UObjectBase::ClassPrivate = CoreUObjectOffsetFinder::_UObjectBase::FindClassPrivate();
    Offsets::UObjectBase::NamePrivate = CoreUObjectOffsetFinder::_UObjectBase::FindNamePrivate();

    Offsets::UObject::InternalIndex = 0xC;

    Offsets::UStruct::SuperStruct = CoreUObjectOffsetFinder::_UStruct::FindSuperStruct();
    Offsets::UStruct::Children = CoreUObjectOffsetFinder::_UStruct::FindChildren();
    Offsets::UStruct::ChildProperties = CoreUObjectOffsetFinder::_UStruct::FindChildProperties();

    Offsets::UField::Next = CoreUObjectOffsetFinder::_UField::FindNext();

    Offsets::UProperty::Offset_Internal = CoreUObjectOffsetFinder::_UProperty::FindOffset_Internal();


    uint16_t UPropertyEnd = CoreUObjectOffsetFinder::_UObjectPropertyBase::FindPropertyClass();
	
    Offsets::UObjectPropertyBase::PropertyClass = UPropertyEnd;

    Offsets::UArrayProperty::Inner = Offsets::UObjectPropertyBase::PropertyClass;
    Offsets::UEnumProperty::UnderlyingProp = Offsets::UObjectPropertyBase::PropertyClass;
    Offsets::UEnumProperty::Enum = Offsets::UEnumProperty::UnderlyingProp + 8;

    Offsets::UMulticastDelegateProperty::SignatureFunction = Offsets::UObjectPropertyBase::PropertyClass;
    Offsets::UDelegateProperty::SignatureFunction = Offsets::UMulticastDelegateProperty::SignatureFunction;

    Offsets::UStructProperty::Struct = Offsets::UObjectPropertyBase::PropertyClass;
    Offsets::UClassProperty::MetaClass = Offsets::UObjectPropertyBase::PropertyClass + 8;
    

    Offsets::UObjectPropertyBase::PropertyClass = CoreUObjectOffsetFinder::_UObjectPropertyBase::FindPropertyClass();

    // NOTE: This could be wrong... But tbh I have no idea how to auto find it
    Offsets::UStruct::PropertiesSize = Offsets::UStruct::Children + 8;

    Offsets::UProperty::ElementSize = CoreUObjectOffsetFinder::_UProperty::FindElementSize();

	// UProperties
	// NOTE: We can "hardcode" them, because we know how they are built, no 
    // game would modify that...

    Offsets::UBoolProperty::FieldSize = UPropertyEnd;
    Offsets::UBoolProperty::ByteOffset = UPropertyEnd + 1;
    Offsets::UBoolProperty::ByteMask = UPropertyEnd + 2;
    Offsets::UBoolProperty::FieldMask = UPropertyEnd + 3;

    return true; // TODO: Check if all are valid
}