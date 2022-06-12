#pragma once

#include "Class.h"
#include "ObjectMacros.h"

class UProperty : public UField
{
	public:
		int32_t GetArrayDim() {
			return 0;
		}

		int32_t GetElementSize() {
			return 0;
		}

		EPropertyFlags GetPropertyFlags() {
			return *(EPropertyFlags*)((__int64)this + Offsets::UProperty::PropertyFlags);
		}

		uint16_t GetRepIndex() {
			return 0;
		}

		// In memory variables (generated during Link()).
		int32_t GetOffset_Internal() {
			return 0;
		}

		bool HasAnyPropertyFlags(uint64_t FlagsToCheck) const
		{
			return (*(uint64_t*)((__int64)this + Offsets::UProperty::PropertyFlags) & FlagsToCheck) != 0 || FlagsToCheck == CPF_AllFlags;
		}
};

class UObjectPropertyBase : public UProperty
{
	public:
		UClass* GetPropertyClass() {
			return 0; // TODO: But later..
		}
};