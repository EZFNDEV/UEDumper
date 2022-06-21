#pragma once

#include "../CoreUObject/UObject/UnrealTypePrivate.h"

namespace SDKFormatting {

	std::string UPropertyTypeToString(UProperty* Property);
	std::string CreateStruct(UStruct* Struct);

	// Faster
	void FormatUClass(UClass* Struct, Ofstreams* streams);
	// faster again
	void LupusFormatUClass(UClass* Class, Ofstreams* streams);

	void LupusFormatStruct(UClass* Class, Ofstreams* streams, std::unordered_map<__int64, __int64>* packageStructs, bool isEnum, bool isScriptStruct, bool isStruct, std::vector<__int64>* structsAdded, std::unordered_map<__int64, Ofstreams*> allstreams);

	void CreateSDKHeader(std::ofstream& header);
	static std::string GenerateInitFunction();
	static std::string GenerateNameStruct() {
		return R"(
	// https://github.com/EpicGames/UnrealEngine/blob/c3caf7b6bf12ae4c8e09b606f10a09776b4d1f38/Engine/Source/Runtime/Core/Public/UObject/NameTypes.h#L403
	struct FName
	{
		uint32_t ComparisonIndex;
		uint32_t Number;
	};
)";
	}

	static std::string GenerateOthers() {
		return R"(
	struct FScriptMulticastDelegate {
		char Unknown[0x10];
	};
)"; // I think the size is always 0x10, but we need to improve this...
	}
	
	static std::string GenerateTArray() {
return R"(
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
)";
	}
	
	static std::string GenerateFString() {
return R"(
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
)";
	}
}