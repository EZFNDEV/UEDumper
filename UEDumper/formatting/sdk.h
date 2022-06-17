#pragma once

#include "../CoreUObject/UObject/UnrealTypePrivate.h"

namespace SDKFormatting {
	struct Class4Writing {
		std::string name;
	};

	std::string UPropertyTypeToString(UObjectPropertyBase* Property);
	std::string CreateClass(UStruct* Class);
}