#include "pch.h"
#include "utils.h"

uintptr_t* Utils::StaticFindObject(const wchar_t* ObjectName) {
	return _StaticFindObject(nullptr, nullptr, ObjectName, false);
}

uintptr_t Utils::UKismetStringLibrary::Conv_NameToString(FName inName) {
	struct {
		FName inName;
		uintptr_t ReturnValue;
	} params;

	params.inName = inName;

	printf("inName: %p\n", inName);
	printf("inName: %p\n", inName.GetComparisonIndex().Value);

	printf("KismetStringLibrary: %p\n", Utils::UKismetStringLibrary::KismetStringLibrary);
	printf("_Conv_NameToString: %p\n", Utils::UKismetStringLibrary::_Conv_NameToString);
	printf("_ProcessEvent: %p\n", Utils::_ProcessEvent);

	Utils::_ProcessEvent(Utils::UKismetStringLibrary::KismetStringLibrary, Utils::UKismetStringLibrary::_Conv_NameToString, &params);

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