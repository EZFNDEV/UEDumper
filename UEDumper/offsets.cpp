#include "pch.h"
#include "offsets.h"

#include <thread>
#include <iostream>

uint8_t OffsetsFinder::FindUFunctionOffset_Func() {
    uintptr_t* (__fastcall * _StaticFindObject) (uintptr_t * ObjectClass, uintptr_t * InObjectPackage, const wchar_t* OrigInName, bool ExactClass);
    _StaticFindObject = decltype(_StaticFindObject)(Offsets::StaticFindObject);

    // We need to wait until this is found, and it will be found for UE4.0+
    uintptr_t* Object = 0;
    uint16_t tries = 0;
    while (Object == 0 && tries < 0x100) {
        Object = _StaticFindObject(nullptr, nullptr, L"Engine.PlayerController.ServerUpdateLevelVisibility", false);
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

std::string GetBytes(uintptr_t Address, int count = 10) { // temp
    std::string Bytes;

    for (int i = 0; i < count; i++) {
        Bytes += std::format("{:x} ", *(uint8_t*)(Address + i) & 0xff);
    }

    return Bytes;
}

static bool VerifyProcessEvent(uintptr_t Address, int i = 0)
{
    // Now this is atleast for Fortnite, but between 1.8 and latest ProcessEvent has always started with this besides a few seasons between like s16-18

    // std::string startsWith = "40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ?";
    // std::string startsWithNoSpace = "4055565741544155415641574881EC????488D6C24?48899D????488B05????4833C5488985????";

	// ^ would be better, but like idk how to implement the wild cards.

    std::string startsWith = "40 55 56 57 41 54 41 55 41 56 41";
    std::string startsWithNoSpace = "4055565741544155415641";


    if (GetBytes(Address, startsWithNoSpace.length()).starts_with(startsWith))
        return true;

    // Most of the time UE4 Games have a string above ProcessEvent like 2 functions above "AccessNoNoneContext", we may be able to check if this string is above it.
    return false;
}

struct UObject // TEMPORARY idk where u want to put lupus
{
    void** VFTable;
};

static uintptr_t FindPE_1() {
    // loop through all the functions in the vtable and find the processevent one
    UObject* (__fastcall * _StaticFindObject) (uintptr_t* ObjectClass, uintptr_t* InObjectPackage, const wchar_t* OrigInName, bool ExactClass);
    _StaticFindObject = decltype(_StaticFindObject)(Offsets::StaticFindObject);
	
    UObject* Object = _StaticFindObject(0, 0, L"/Script/CoreUObject", false);

    if (Object)
    {
        for (int i = 0; i < 69; i++) // i never see it above 69
        {
            auto func = Object->VFTable[i];

            if (!func)
                break;

            if (VerifyProcessEvent(__int64(func), i))
            {
                // std::cout << std::format("PE 1: {:x} Idx: {}\n", __int64(func), i);
                return __int64(func);
            }
        }
    }
    else
        printf("Failed to find CoreUObject!\n");

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

    if (!ProcessEvent)
    {
        ProcessEvent = FindPE_2();

        if (!VerifyProcessEvent(ProcessEvent))
            ProcessEvent = 0;
    }

    return ProcessEvent;
}

uintptr_t OffsetsFinder::FindStaticFindObject() {
    uintptr_t StaticFindObjectAddr = (uintptr_t)Memory::FortKit::FindByString(L"Illegal call to StaticFindObject() while serializing object data!", { Memory::FortKit::ASM::LEA });

    if (StaticFindObjectAddr == 0) {
        // Find it by Temp
        StaticFindObjectAddr = (uintptr_t)Memory::FortKit::FindByString(L"/Temp/%s", { Memory::FortKit::ASM::LEA });
        return 0;
        return StaticFindObjectAddr;
    }

    for (StaticFindObjectAddr; StaticFindObjectAddr > 0; StaticFindObjectAddr -= 1) {
        // if (GetBytes(StaticFindObjectAddr, 5).starts_with("48 89 5C 24 08")) // do something like this
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