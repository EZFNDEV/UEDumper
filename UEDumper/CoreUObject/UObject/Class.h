#pragma once

#include "UObjectBase.h"
#include "Script.h"

class UField : public UObjectBase
{
	UField* GetNext() const
	{
		return *(UField**)((__int64)this + Offsets::UField::Next);
	}
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