
#pragma once

#include <Windows.h>
#include <string>
#include <locale>
#include <format>
#include <iostream>

namespace SDK {
	// https://github.com/EpicGames/UnrealEngine/blob/99b6e203a15d04fc7bbbf554c421a985c1ccb8f1/Engine/Source/Runtime/CoreUObject/Private/UObject/UObjectGlobals.cpp#L327
	static inline uintptr_t* (__fastcall* _StaticFindObject) (uintptr_t* ObjectClass, uintptr_t* InObjectPackage, const wchar_t* OrigInName, bool ExactClass) = 0;

	// https://github.com/EpicGames/UnrealEngine/blob/c3caf7b6bf12ae4c8e09b606f10a09776b4d1f38/Engine/Source/Runtime/CoreUObject/Private/UObject/ScriptCore.cpp#L1822
	static inline void* (__fastcall* _ProcessEvent) (uintptr_t* Object, uintptr_t* Function, void* Params) = 0;

	static void Init() {
		_StaticFindObject = decltype(_StaticFindObject)((uintptr_t)GetModuleHandle(0) + 0x14d5030);
		_ProcessEvent = decltype(_ProcessEvent)((uintptr_t)GetModuleHandle(0) + 0x14afde0);
	};

	// https://github.com/EpicGames/UnrealEngine/blob/c3caf7b6bf12ae4c8e09b606f10a09776b4d1f38/Engine/Source/Runtime/Core/Public/UObject/NameTypes.h#L403
	struct FName
	{
		uint32_t ComparisonIndex;
		uint32_t Number;
	};


	template<class T>
	struct TArray
	{
		friend struct FString;

		public:
			inline TArray()
			{
				Data = nullptr;
				Count = Max = 0;
			};

			inline int Num() const
			{
				return Count;
			};

			inline T& operator[](int i)
			{
				return Data[i];
			};

			inline const T& operator[](int i) const
			{
				return Data[i];
			};

			inline bool IsValidIndex(int i) const
			{
				return i < Num();
			}

		public:
			T* Data;
			uint32_t Count;
			uint32_t Max;
	};


	class FString : private TArray<wchar_t>
	{
		public:
			FString() = default;

			FString(const wchar_t* other)
			{
				Max = Count = *other ? std::wcslen(other) + 1 : 0;

				if (Count)
				{
					Data = const_cast<wchar_t*>(other);
				}
			};

			inline bool IsValid() const
			{
				return Data != nullptr;
			}

			inline const wchar_t* c_str() const
			{
				return Data;
			}

			std::string ToString();
	};


	struct FScriptMulticastDelegate {
		char Unknown[0x10];
	};
}

#include "./Packages/FN_CoreUObject_structs.hpp"
#include "./Packages/FN_CoreUObject_classes.hpp"
#include "./Packages/FN_CoreUObject_functions.cpp"