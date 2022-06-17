#pragma once

#include "UObjectBase.h"
#include "Script.h"


class UField : public UObjectBase
{
	public:
		UField* GetNext() const
		{
			return *(UField**)((__int64)this + Offsets::UField::Next);
		}
};

//
// Reflection data for an enumeration.
//
class UEnum : public UField {
	public:
		/** How this enum is declared in C++, affects the internal naming of enum values */
		enum class ECppForm
		{
			Regular,
			Namespaced,
			EnumClass
		};




		/** List of pairs of all enum names and values. */
		//TArray</*TPair<*/FName, int64_t/*>*/> GetNames() {

	//	}

		/** How the enum was originally defined. */
		ECppForm GetCppForm() {

		}

		/** Enum flags. */
	//	EEnumFlags EnumFlags;

		/** pointer to function used to look up the enum's display name. Currently only assigned for UEnums generated for nativized blueprints */
	//	FEnumDisplayNameFn EnumDisplayNameFn;

		/** global list of all value names used by all enums in memory, used for property text import */
		//static TMap<FName, UEnum*> AllEnumNames;
		
		

		


		/** This will be the true type of the enum as a string, e.g. "ENamespacedEnum::InnerType" or "ERegularEnum" or "EEnumClass" */
		FString CppType;

		// Index is the internal index into the Enum array, and is not useful outside of the Enum system
		// Value is the value set in the Enum Class in C++ or Blueprint
		// Enums can be sparse, which means that not every valid Index is a proper Value, and they are not necessarily equal
		// It is not safe to cast an Index to a Enum Class, always do that with a Value instead

		/** Gets the internal index for an enum value. Returns INDEX_None if not valid */
		/*int32_t GetIndexByValue(int64_t InValue)
		{
			for (int32_t i = 0; i < Names.Num(); ++i)
			{
				if (Names[i].Value == InValue)
				{
					return i;
				}
			}
			return INDEX_NONE;
		}*/

		/** Gets enum value by index in Names array. Asserts on invalid index */
		/*int64_t GetValueByIndex(int32_t Index) const
		{
			// check(Names.IsValidIndex(Index));
			return Names[Index].Value;
		}*/

		/**
		 * @return	 The number of enum names.
		*/
		/*int32_t NumEnums() const
		{
			return Names.Num();
		}*/

};

class UStruct : public UField
{
	public:
		UStruct* GetSuperStruct() const
		{
			return *(UStruct**)((__int64)this + Offsets::UStruct::SuperStruct);
		}

		/** Returns true if this struct either is SomeBase, or is a child of SomeBase. This will not crash on null structs */
		bool IsChildOf(const UStruct* SomeBase) const
		{
			for (const UStruct* Struct = this; Struct; Struct = Struct->GetSuperStruct())
			{
				if (Struct == SomeBase)
					return true;
			}

			return false;
		}

		UField* GetChildren() {
			return *(UField**)((__int64)this + Offsets::UStruct::Children);
		}

		struct FField* GetChildProperties() {
			if (!Offsets::UStruct::ChildProperties) return 0;
			
			return 0;
		}
};

class UClass : public UStruct
{
	
};

//
// Reflection data for a replicated or Kismet callable function.
//
class UFunction : public UStruct
{
	public:
		/** EFunctionFlags set defined for this function */
		EFunctionFlags GetFunctionFlags() {
			return *(EFunctionFlags*)((__int64)this + Offsets::UFunction::FunctionFlags);
		}

		/** C++ function this is bound to */
		void* GetFunc() {
			return *(void**)((__int64)this + Offsets::UFunction::Func);
		}

		/**
		 * Returns the native func pointer.
		 *
		 * @return The native function pointer.
		*/
		/*FNativeFuncPtr GetNativeFunc() const
		{
			return GetFunc();
		}*/

		
};