#pragma once

#include "../CoreUObject/UObject/UnrealTypePrivate.h"

namespace SDKFormatting {

	std::string UPropertyTypeToString(UObjectPropertyBase* Property);
	std::string CreateClass(UStruct* Class);
	std::string CreateFunctions(UStruct* Class);
	std::string CreateStruct(UStruct* Struct);
}