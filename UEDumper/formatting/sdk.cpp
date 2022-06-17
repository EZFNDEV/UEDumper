#include "pch.h"
#include "sdk.h"

#include "../CoreUObject/UObject/UObjectBaseUtility.h"
#include "../CoreUObject/UObject/UnrealTypePrivate.h"

std::string SDKFormatting::CreateClass(UStruct* Class) {
	/*
	// BlueprintGeneratedClass _WaterMeshBlueprintMaster._WaterMeshBlueprintMaster_C
	// 0x00C3 (0x03DB - 0x0318)
	*/

	std::string result = "";

	UObjectBaseUtility* FUCKYOU = (UObjectBaseUtility*)Class;

	Class4Writing classInfo;
	
	classInfo.name = "\n\nclass " + FUCKYOU->GetName().ToString();

	auto super = Class->GetSuperStruct();
	if (super && super != Class)
	{
		//c.InheritedSize = offset = super.GetPropertySize();

		//c.NameCppFull += " : public " + MakeValidName(super.GetNameCPP());
	} else {
		classInfo.name = "UObject";
	}

	result += classInfo.name + " {\n";


	// loop all members
	if ((Class->GetChildren())) {
		for (UField* Property = (UField*)(Class)->GetChildren(); Property; Property = Property->GetNext()) {
			result += Utils::UKismetStringLibrary::Conv_NameToString(((UObjectPropertyBase*)Property)->GetFName()).ToString() + "\n";
			//    printf("Property: %p\n", Property);
			 //   printf("Member: %s\n", Utils::UKismetStringLibrary::Conv_NameToString(((UObjectPropertyBase*)Property)->GetFName()).ToString().c_str());
		}
	}

	

	// end
	result += "\n};";
	
	return result;
}