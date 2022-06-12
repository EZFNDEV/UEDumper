#pragma once

#include "UObjectBase.h"
#include "Class.h"

/**
 * Provides utility functions for UObject, this class should not be used directly
 */
class UObjectBaseUtility : public UObjectBase
{
	public:
		static bool IsChildOfWorkaround(const UClass* ObjClass, const UClass* TestCls)
		{
			return ObjClass->IsChildOf(TestCls);
		}

		/**Returns true if this object is of the specified type.*/
		bool IsA(UClass* SomeBaseClass) const
		{
			// We have a cyclic dependency between UObjectBaseUtility and UClass,
			// so we use a template to allow inlining of something we haven't yet seen, because it delays compilation until the function is called.

			// 'static_assert' that this thing is actually a UClass pointer or convertible to it.
			// const UClass* SomeBaseClass = SomeBase;
			// (void)SomeBaseClass;
			// checkfSlow(SomeBaseClass, TEXT("IsA(NULL) cannot yield meaningful results"));

			const UClass* ThisClass = GetClass();

			// Stop the compiler doing some unnecessary branching for nullptr checks
			// UE_ASSUME(SomeBaseClass);
			// UE_ASSUME(ThisClass);

			return IsChildOfWorkaround(ThisClass, SomeBaseClass);
		}
};