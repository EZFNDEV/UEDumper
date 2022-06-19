#pragma once

#include "../CoreUObject/UObject/UnrealTypePrivate.h"

namespace SDKFormatting {

	std::string UPropertyTypeToString(UProperty* Property, const std::string& PropName);
	std::string CreateStruct(UStruct* Struct);
	std::string CreateEnum(UEnum* Enum);

	// Faster
	void FormatUClass(UClass* Struct, Ofstreams* streams);
}