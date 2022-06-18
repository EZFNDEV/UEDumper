#pragma once

#include "../CoreUObject/UObject/UnrealTypePrivate.h"

namespace SDKFormatting {

	std::string UPropertyTypeToString(UObjectPropertyBase* Property);
	std::string CreateStruct(UStruct* Struct);

	// Faster
	void FormatUClass(UClass* Struct, Ofstreams* streams);
}