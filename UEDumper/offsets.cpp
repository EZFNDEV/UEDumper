#include "pch.h"
#include "offsets.h"

#include <thread>

bool OffsetsFinder::FindUFunctionOffset_Func() {
    // Find ProcessEvent by StaticFindObject
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

    if (Object == 0) return 0;

    for (uint8_t i = 8; i < 255; i++) { // Mostly it's 0xD8 or 0xE0
		// Let's find a valid memory address
		if (*(uint8_t*)((__int64)Object + i) == 0x7F) {
            // We might have found a valid memory address
			
            uintptr_t address = *(uintptr_t*)((__int64)Object + i - 5);
            if (address > 0x7FF000000000 && address < 0x7FFFFFFFFFFFF) {
                Offsets::UFunction::Func = i - 5;
                return true;
            }
        }
    }

    return false;
}

static uintptr_t FindPE_2() {
    uint64_t LastAddress = 0;
    while (true) {
        LastAddress = Memory::Sexy::PatternScan("0F 85 ? ? 00 00 F7 ? ? 00 00 00 00 04 00 00", LastAddress); // test dword ptr [rsi+98h], 400h
        if (LastAddress == NULL) break;

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
        // FUCK THIS SHIT
        return 0;
    }
    return 0;
}

uintptr_t OffsetsFinder::FindProcessEvent() {
    // "Hacky" was to find process event:
        // 1. Search a UObject and loop until you find a function starting with "40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC" (maybe a bit less, but this works for Fortnite...)
            // We can then get 3-4 other objects and check if they have the same function at that index, if all have that we can most likely assume that this is process event
        // 2. There is a small pattern that (almost) every UE4 game has in the PE pattern, so we can search that and go back to the begin of the function
    
    uintptr_t ProcessEvent = FindPE_2();
    if (ProcessEvent != 0)
        return ProcessEvent;
	
    printf("TODO: Get process event method 1\n");
	
	return 0;
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