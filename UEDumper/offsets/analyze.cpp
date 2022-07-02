#include "pch.h"
#include "analyze.h"
#include <vector>
#include "../include/memory.h"


// More than just a SDK dumper... This can be used to figure out the game logic, but it takes long, about 10 minutes...

static void FindFunctionReferences() {
    // Find all calls of virtual functions

}

static uintptr_t TryToFindIndex(uintptr_t addr) {
    for (uint16_t i = 0; i < 0xFF; i++) {
        if (
            *(uint8_t*)(addr + i) == 0x48 && 
            *(uint8_t*)(addr + i + 1) == 0xFF &&
            *(uint8_t*)(addr + i + 2) == 0xA0
        ) {
            return ((addr + i + 3 + 4) + *(int32_t*)(addr + i + 3));
        }
    }

    return 0;
}

std::string Analyze::GetVirtualFunctionSize(UClass* Class) { // Returns how many virtual functions there are...
    std::unordered_map<__int64, std::string> functions; // index, name

    __int64** vtable = *reinterpret_cast<__int64***>(Class);

    std::string result = "\n            // Vitrual Functions:\n\n";
    printf("vtable: %p\n", vtable);
  
        static uintptr_t base = (uintptr_t)GetModuleHandle(0);
        uintptr_t i = 0;
        while (true) {
            uintptr_t* function = (uintptr_t*)(__int64(vtable) + (i * 8));
            if (IsBadReadPtr((void*)function, 8)) break;

            if (*function < 0x7FF000000000 || *function > 0x7FFFFFFFFFFF) break;

            // TODO: Add xrefs
            result += std::format(" 	    void _0x{:x}(); // Index: 0x{:x}\n", (*function) - base, i);

            i++;
        }
   

    // Very bad for performace, but we will loop the members of the Class again and
    // get all UFunctions to check which VTables we can give a name, then

    printf(result.c_str());


    UFunction* Function = (UFunction*)Utils::StaticFindObject(L"todo");
    printf("UFunction: %p\n", Function->GetFunc());

    if (uintptr_t idx = TryToFindIndex((uintptr_t)Function->GetFunc()) != 0) {
        printf("Found VTable Index: %p\n", idx);

        printf("WTF: %p\n", (*(__int64(__fastcall**)(__int64, __int64))(*(__int64*)(__int64)Class + idx)));
    }

    /*
    
    FortniteClient-Win64-Shipping.exe+14CA8C0 - 48 8B 42 20           - mov rax,[rdx+20]
FortniteClient-Win64-Shipping.exe+14CA8C4 - 45 33 C0              - xor r8d,r8d
FortniteClient-Win64-Shipping.exe+14CA8C7 - 48 85 C0              - test rax,rax
FortniteClient-Win64-Shipping.exe+14CA8CA - 41 0F95 C0            - setne r8l
FortniteClient-Win64-Shipping.exe+14CA8CE - 4C 03 C0              - add r8,rax
FortniteClient-Win64-Shipping.exe+14CA8D1 - 4C 89 42 20           - mov [rdx+20],r8
FortniteClient-Win64-Shipping.exe+14CA8D5 - 48 8B 01              - mov rax,[rcx]
FortniteClient-Win64-Shipping.exe+14CA8D8 - 48 FF A0 E0 18 00 00     - jmp qword ptr [rax+000018E0] // this is the index
    
    */

    

    return result;
}

std::string Analyze::ReturnReferenceInfo(UProperty* Property) {
	std::string result = "";

    int x;
   // 

    uint32_t off = Property->GetOffset_Internal();
    if (off < 1000) return ""; // Too much

    std::string namee = Utils::UKismetStringLibrary::Conv_NameToString(Property->GetFName()).ToString();

    char* peIdxstr = (char*)((void*)&off);

    std::vector<int> vectorr{ 0x00, 0x00, 0x00, 0x00 };
    vectorr[0] = peIdxstr[3];
    vectorr[1] = peIdxstr[2];
   // vectorr[2] = peIdxstr[2];
  //  vectorr[3] = peIdxstr[3];

    uint64_t LastAddress = 0;
    int found = 0;
    while (true && found < 5) {
        LastAddress = Memory::Sexy::NO_STRPatternScan(vectorr, LastAddress == 0 ? LastAddress : LastAddress + 1);
        if (LastAddress == NULL) break;

       
        result += std::format(" Addr 0x{:x}", LastAddress - (uintptr_t)GetModuleHandle(0));
        found += 1;
    }

	return result;
}

uintptr_t Analyze::FindMoreObjectFunction(UObject* Class) {
    // Find (for now virtual) functions that contain the function name, example: 



    return 0;
}