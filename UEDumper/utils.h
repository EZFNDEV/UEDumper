#pragma once

#include "Core/UObject/NameTypes.h"
#include "Core/Containers/UnrealString.h"

namespace Utils {
	// https://github.com/EpicGames/UnrealEngine/blob/99b6e203a15d04fc7bbbf554c421a985c1ccb8f1/Engine/Source/Runtime/CoreUObject/Private/UObject/UObjectGlobals.cpp#L327
	extern inline uintptr_t* (__fastcall* _StaticFindObject) (uintptr_t* ObjectClass, uintptr_t* InObjectPackage, const wchar_t* OrigInName, bool ExactClass) = 0;

	// https://github.com/EpicGames/UnrealEngine/blob/c3caf7b6bf12ae4c8e09b606f10a09776b4d1f38/Engine/Source/Runtime/CoreUObject/Private/UObject/ScriptCore.cpp#L1822
	extern inline void* (__fastcall* _ProcessEvent) (uintptr_t* Object, uintptr_t* Function, void* Params) = 0;
	
	
	extern inline uintptr_t* StaticFindObject(const wchar_t* ObjectName);

	namespace UKismetStringLibrary {
		extern inline uintptr_t* KismetStringLibrary = 0;
		extern inline uintptr_t* _Conv_NameToString = 0;

		extern inline bool Init() {
			if (!KismetStringLibrary) {
				KismetStringLibrary = StaticFindObject(L"Engine.KismetStringLibrary");
			}
			
			if (!_Conv_NameToString) {
				_Conv_NameToString = StaticFindObject(L"Engine.KismetStringLibrary.Conv_NameToString");
			}

			return (
				KismetStringLibrary != 0 &&
				_Conv_NameToString != 0
			);
		}

		extern inline struct FString Conv_NameToString(FName inName);
	}

	namespace UKismetSystemLibrary {
		extern inline uintptr_t* KismetSystemLibrary = 0;
		extern inline uintptr_t* _GetObjectName = 0;
		extern inline uintptr_t* _GetPathName = 0;

		extern inline bool Init() {
			if (!KismetSystemLibrary) {
				KismetSystemLibrary = StaticFindObject(L"Engine.KismetSystemLibrary");
				printf("KismetSystemLibrary: %p\n", KismetSystemLibrary);
			}

			/*if (!_GetObjectName) {
				_GetObjectName = StaticFindObject(L"Engine.KismetSystemLibrary.GetObjectName");
			}*/

			if (!_GetPathName) {
				_GetPathName = StaticFindObject(L"Engine.KismetSystemLibrary.GetPathName");
				printf("_GetPathName: %p\n", _GetPathName);
			}

			return (
				KismetSystemLibrary != 0 &&
				// _GetObjectName != 0 &&
				_GetPathName != 0
			);
		}

		extern inline FString GetObjectName(uintptr_t* Object);
		extern inline FString GetPathName(uintptr_t* Object);
	}
}