#pragma once

#include "../CoreUObject/UObject/UnrealTypePrivate.h"

namespace Analyze {
	extern inline std::string ReturnReferenceInfo(UProperty* Property);

	extern inline uintptr_t FindMoreObjectFunction(UObject* Class);
}