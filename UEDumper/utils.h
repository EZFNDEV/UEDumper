#pragma once

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
			KismetStringLibrary = StaticFindObject(L"Engine.KismetStringLibrary");

			_Conv_NameToString = StaticFindObject(L"Engine.KismetStringLibrary.Conv_NameToString");

			return (
				KismetStringLibrary != 0 &&
				_Conv_NameToString != 0
			);
		}

		extern inline uintptr_t Conv_NameToString(uintptr_t inName) {
			struct {
				uintptr_t inName;
				uintptr_t ReturnValue;
			} params;
			
			params.inName = inName;
			
			printf("KismetStringLibrary: %p\n", KismetStringLibrary);
			printf("_Conv_NameToString: %p\n", _Conv_NameToString);
			printf("_ProcessEvent: %p\n", _ProcessEvent);
			
			return 0;
			
			_ProcessEvent(KismetStringLibrary, _Conv_NameToString, &params);

			return params.ReturnValue;
		}
	}
}