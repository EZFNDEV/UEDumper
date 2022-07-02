#pragma once
#include "../CoreUObject/UObject/UObjectHash.h"
#include "../CoreUObject/UObject/Object.h"
#include "../CoreUObject/UObject/UnrealTypePrivate.h"

namespace Analyze {
	extern inline std::string ReturnReferenceInfo(UProperty* Property);

	extern inline uintptr_t FindMoreObjectFunction(UObject* Class);
	extern inline std::string GetVirtualFunctionSize(UClass* Class);
}