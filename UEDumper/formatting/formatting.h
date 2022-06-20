#pragma once

#include "../CoreUObject/UObject/ObjectMacros.h"
#include "../CoreUObject/UObject/UObjectBaseUtility.h"

namespace Formatting {
	extern inline std::string GetUPropertySpecifiers(EPropertyFlags Flags);
	
	extern inline std::string GetPrefix(UObjectBase* Object, bool IsClass) {
		if (IsClass) {
			return ((UClass*)Object)->GetPrefixCPP();
		}

		return "F";
	}
	
	extern inline std::string GetPrefix(UObjectBase* Object) {
		static UClass* UClassClass = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Class");
		
		return GetPrefix(Object, ((UObjectBaseUtility*)Object)->IsA(UClassClass));
	}
}