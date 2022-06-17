#include "pch.h"
#include "Class.h"

std::string UClass::GetPrefixCPP() {
	static uint64_t NAME_Actor = 102;
	
	UClass* TheClass = this;
	bool	bIsActorClass = false;
	bool	bIsDeprecated = false; // TheClass->HasAnyClassFlags(CLASS_Deprecated);
	while (TheClass && !bIsActorClass)
	{
		bIsActorClass = TheClass->GetFName() == NAME_Actor;
		TheClass = TheClass->GetSuperClass();
	}

	if (bIsActorClass)
	{
		if (bIsDeprecated)
		{
			return "ADEPRECATED_";
		}
		else
		{
			return "A";
		}
	}
	else
	{
		if (bIsDeprecated)
		{
			return "UDEPRECATED_";
		}
		else
		{
			return "U";
		}
	}
}