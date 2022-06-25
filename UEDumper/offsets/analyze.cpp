#include "pch.h"
#include "analyze.h"
#include <vector>
#include "../include/memory.h"

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