#include "pch.h"
#include "UnrealString.h"

std::string FString::ToString() {
	// Convert the wchart to std::string
    auto length = std::wcslen(Data);
    std::string str(length, '\0');
    std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);
	
    return str;
}