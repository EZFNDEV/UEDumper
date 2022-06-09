#include "pch.h"
#include "utils.h"

uintptr_t* Utils::StaticFindObject(const wchar_t* ObjectName) {
	return _StaticFindObject(nullptr, nullptr, ObjectName, false);
}