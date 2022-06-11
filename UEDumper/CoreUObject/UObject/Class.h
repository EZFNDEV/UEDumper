#pragma once
#include "Script.h"

//
// Reflection data for a replicated or Kismet callable function.
//
class UFunction /* : public UStruct*/ // TODO: We REALLY need to do this...
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