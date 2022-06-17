#include "pch.h"
#include "sdk.h"

#include "../CoreUObject/UObject/UObjectBaseUtility.h"
#include "../CoreUObject/UObject/UnrealTypePrivate.h"

std::string SDKFormatting::UPropertyTypeToString(UObjectPropertyBase* Property) {
	
	
	UClass* Name = Property->GetClass();

	/*if (Name == IntProperty) {
		return "int";
	}
	*/
	return "";
}

static std::string idfk() {
	return "";
}

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

	result += classInfo.name + " {\n	public:\n";


	if ((Class->GetChildren())) {
		for (UField* Property = (UField*)(Class)->GetChildren(); Property; Property = Property->GetNext()) {
			std::string pType = UPropertyTypeToString((UObjectPropertyBase*)Property);
			std::string pName = Utils::UKismetStringLibrary::Conv_NameToString(((UObjectPropertyBase*)Property)->GetFName()).ToString();

			result += "		" + pType + " " + pName + ";\n";
		}
	}
	
	result += "\n};";
	
	return result;
}