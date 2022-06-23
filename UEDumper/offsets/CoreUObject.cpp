#include "pch.h"
#include "CoreUObject.h"
#include "../CoreUObject/UObject/UObjectBaseUtility.h"
#include "../CoreUObject/UObject/Object.h"

// Do not use this...
static uint16_t __FindOffset(uintptr_t* A, uintptr_t B, uint16_t max = 0xFF) {
	uintptr_t current = (uintptr_t)A;
	for (uint16_t i = 0; i < max; i += 8) {
		if (IsBadReadPtr((void*)current, 8)) continue;

		if (*(uintptr_t*)current == B) {
			return i;
		}

		current += 8;
	}

	return 0;
}

static uint16_t __FindOffset(uintptr_t* A, uint32_t B, uint16_t max = 0xFF) {
	uintptr_t current = (uintptr_t)A;
	for (uint16_t i = 0; i < max; i += 8) {
		if (IsBadReadPtr((void*)current, 8)) continue;

		if (*(uint32_t*)current == B) {
			return i;
		}

		current += 8;
	}

	return 0;
}

#pragma region UObjectBase

/*

EObjectFlags					ObjectFlags;

int32							InternalIndex;

*/

uint16_t CoreUObjectOffsetFinder::_UObjectBase::FindClassPrivate() {
	return __FindOffset(
		UEngineObject,
		(uintptr_t)Utils::StaticFindObject(L"/Script/CoreUObject.Class")
	);
}

uint16_t CoreUObjectOffsetFinder::_UObjectBase::FindNamePrivate() {
	return __FindOffset(
		UEngineObject,
		(uintptr_t)Utils::UKismetStringLibrary::Conv_StringToName_G(L"Engine")
	);
}

uint16_t CoreUObjectOffsetFinder::_UObjectBase::FindOuterPrivate() {
	return __FindOffset(
		UEngineObject,
		(uintptr_t)Utils::StaticFindObject(L"/Script/Engine")
	);
}

#pragma endregion

#pragma region UStruct

uint16_t CoreUObjectOffsetFinder::_UStruct::FindSuperStruct() {
	return __FindOffset(
		UEngineObject,
		(uintptr_t)Utils::StaticFindObject(L"/Script/CoreUObject.Object")
	);
}

uint16_t CoreUObjectOffsetFinder::_UStruct::FindChildren() {
	uintptr_t current = (uintptr_t)UEngineObject;
	uint64_t TinyFont = Utils::UKismetStringLibrary::Conv_StringToName_G(L"TinyFont");

	for (uint16_t i = 0; i < 0xFF; i += 8) {
		if (IsBadReadPtr((void*)(current), 8)) continue;
		uintptr_t Children = *(uintptr_t*)(current);

		// It's a pointer, now we need to verify it's the children
		for (uint16_t j = 0; j < 0xFF; j += 8) {
			if (IsBadReadPtr((void*)(Children + j), 8)) continue; // break;

			if (*(uint64_t*)(Children + j + Offsets::UObjectBase::NamePrivate) == TinyFont)
				return i;
		}

		current += 8;
	}

	return 0;
}

uint16_t CoreUObjectOffsetFinder::_UStruct::FindChildProperties() {
	// TODO: Figure out what's in it...
	return __FindOffset(
		UEngineObject,
		(uintptr_t)Utils::StaticFindObject(L"/Script/CoreUObject.Object")
	);
}

#pragma endregion

#pragma region UField

uint16_t CoreUObjectOffsetFinder::_UField::FindNext() {
	uint64_t TinyFontName = Utils::UKismetStringLibrary::Conv_StringToName_G(L"TinyFontName");

	if (((UStruct*)UEngineObject)->GetChildren()) {
		UField* FirstProperty = (UField*)((UStruct*)UEngineObject)->GetChildren();

		for (uint16_t i = Offsets::UObjectBase::NamePrivate; i < 0xFF; i += 8) { // bruh FF and uint16, im so smart
			uintptr_t Property = *(uintptr_t*)((__int64)FirstProperty + i);

			if (IsBadReadPtr((void*)Property, 8)) continue;

			if (*(uint64_t*)((__int64)Property + Offsets::UObjectBase::NamePrivate) == TinyFontName)
				return i;
		}
	}
}

#pragma endregion


#pragma region UProperty

/*

// Persistent variables.
	int32			ArrayDim;
	int32			ElementSize;
	EPropertyFlags	PropertyFlags;
	uint16			RepIndex;

	TEnumAsByte<ELifetimeCondition> BlueprintReplicationCondition;

	*/

uint16_t CoreUObjectOffsetFinder::_UProperty::FindElementSize() {
	uint64_t TinyFont = Utils::UKismetStringLibrary::Conv_StringToName_G(L"TinyFont");

	if (((UStruct*)UEngineObject)->GetChildren()) {
		for (UField* Property = (UField*)((UStruct*)UEngineObject)->GetChildren(); Property; Property = Property->GetNext()) {
			if (Property->GetFName() == TinyFont) {
				// Must be the size of 8, it's a UFont pointer

				for (uint16_t i = Offsets::UObjectBase::ClassPrivate; i < 0xFF; i += 4) {
					if (IsBadReadPtr((void*)((__int64)Property + i), 4)) continue;

					if (*(uint32_t*)((__int64)Property + i) == 0x08)
						return i;
				}
			}
		}
	}

	return 0;
}

uint16_t CoreUObjectOffsetFinder::_UProperty::FindOffset_Internal() {
	if (((UStruct*)UEngineObject)->GetChildren()) {
		UField* FirstProperty = (UField*)((UStruct*)UEngineObject)->GetChildren();

		for (uint16_t i = Offsets::UField::Next; i < 0xFF; i += 4) {
			if (IsBadReadPtr((void*)((__int64)FirstProperty + i), 4)) continue;

			// TODO: Verify this, it's kinda tricky...
			if (*(uint32_t*)((__int64)FirstProperty + i) > 0x20 && *(uint32_t*)((__int64)FirstProperty + i) < 0x50) {
				return i;
			}
		}
	}

	return 0;
}

	/*
	FName		RepNotifyFunc;

UProperty* PropertyLinkNext;
UProperty* NextRef;
UProperty* DestructorLinkNext;
UProperty* PostConstructLinkNext;
*/



#pragma endregion

#pragma region UObjectPropertyBase

uint16_t CoreUObjectOffsetFinder::_UObjectPropertyBase::FindPropertyClass() {
	UClass* ObjectProperty = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.ObjectProperty");
	uint64_t Font = Utils::UKismetStringLibrary::Conv_StringToName_G(L"Font");

	if (((UStruct*)UEngineObject)->GetChildren()) {
		for (UField* Property = (UField*)((UStruct*)UEngineObject)->GetChildren(); Property; Property = Property->GetNext()) {
			if (Property->GetClass() == ObjectProperty) {
				for (uint16_t i = Offsets::UStruct::Children; i < 0xFF; i += 8) {
					uintptr_t PropClass = ((__int64)Property + i);
					if (IsBadReadPtr((void*)PropClass, 8)) continue;

					uintptr_t* PropClass2 = *(uintptr_t**)(PropClass);
					if (IsBadReadPtr((void*)((__int64)PropClass2 + Offsets::UObjectBase::NamePrivate), 8)) continue;

					if (*(uintptr_t*)((__int64)PropClass2 + Offsets::UObjectBase::NamePrivate) == Font) {
						return i;
					}
				}
			}
		}
	}
	
	if (((UStruct*)UEngineObject)->GetChildProperties()) {

	}

	return 0;
}

#pragma endregion
