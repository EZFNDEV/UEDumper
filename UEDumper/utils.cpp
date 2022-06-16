#include "pch.h"
#include "utils.h"

uintptr_t* Utils::StaticFindObject(const wchar_t* ObjectName) {
	return _StaticFindObject(nullptr, nullptr, ObjectName, false);
}

struct FString Utils::UKismetStringLibrary::Conv_NameToString(FName* inName) {
	struct {
		uint64_t inName;
		struct FString ReturnValue;
	} params { .inName = inName->GetComparisonIndex().Value };
	
	Utils::_ProcessEvent(Utils::UKismetStringLibrary::KismetStringLibrary, Utils::UKismetStringLibrary::_Conv_NameToString, &params);

	return params.ReturnValue;
}

struct FString Utils::UKismetStringLibrary::Conv_NameToString(uint64_t inName) {
	struct {
		uint64_t inName;
		struct FString ReturnValue;
	} params{ .inName = inName };

	Utils::_ProcessEvent(Utils::UKismetStringLibrary::KismetStringLibrary, Utils::UKismetStringLibrary::_Conv_NameToString, &params);

	return params.ReturnValue;
}

FName Utils::UKismetStringLibrary::Conv_StringToName(struct FString string) {
	struct {
		struct FString string;
		FName ReturnValue;
	} params{ .string = string };

	Utils::_ProcessEvent(Utils::UKismetStringLibrary::KismetStringLibrary, Utils::UKismetStringLibrary::_Conv_StringToName, &params);

	return params.ReturnValue;
}

FString Utils::UKismetSystemLibrary::GetPathName(uintptr_t* Object) {
	struct {
		uintptr_t* Object;
		struct FString ReturnValue;
	} params;

	params.Object = Object;

	Utils::_ProcessEvent(
		Utils::UKismetSystemLibrary::KismetSystemLibrary,
		Utils::UKismetSystemLibrary::_GetPathName,
		&params
	);

	return params.ReturnValue;
}