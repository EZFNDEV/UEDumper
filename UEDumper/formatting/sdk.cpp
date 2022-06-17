#include "pch.h"
#include "sdk.h"

#include "../CoreUObject/UObject/UObjectBaseUtility.h"
#include "../CoreUObject/UObject/UnrealTypePrivate.h"

std::string SDKFormatting::UPropertyTypeToString(UObjectPropertyBase* Property) {
	
	UClass* ClassPrivate = Property->GetClass();

	static UClass* DoubleProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.DoubleProperty");
	static UClass* FloatProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.FloatProperty");
	static UClass* IntProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.IntProperty");
	static UClass* Int16Prop = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Int16Property");
	static UClass* BoolProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.BoolProperty");
	static UClass* ObjectProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.ObjectProperty");
	static UClass* FunctionProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Function");

	if (ClassPrivate == DoubleProp)
		return "double";
	else if (ClassPrivate == FloatProp)
		return "float";
	else if (ClassPrivate == IntProp)
		return "int";
	else if (ClassPrivate == Int16Prop)
		return "int16_t";
	else if (ClassPrivate == BoolProp)
	{
		// todo: do some bitfield stuff kms
		return "bool";
	}
	else if (ClassPrivate == FunctionProp)
	{
		std::string ReturnValueType;
		auto Func = (UStruct*)Property;

		auto FunctionFlags = *(uint32_t*)(__int64(Func) + 0x88);

		for (UProperty* Parameter = (UProperty*)Func->GetChildren(); Parameter; Parameter = (UProperty*)Parameter->GetNext())
		{
			auto PropertyFlags = *(uint64_t*)(__int64(Parameter) + 0x38);

			if (PropertyFlags & 0x400) // FUNC_Native
			{
				return UPropertyTypeToString((UObjectPropertyBase*)Parameter);
			}
		}

		return "void";
	}
	else if (ClassPrivate == ObjectProp)
	{
		auto PropertyClass = Property->GetPropertyClass();
		auto ArrayDim = *(uint32_t*)(__int64(Property) + 0x30);

		std::string browtf;

		for (int i = 0; i < ArrayDim; i++)
		{
			browtf += "*";
		}

		if (PropertyClass)
		{
			return Utils::UKismetStringLibrary::Conv_NameToString(PropertyClass->GetFName()).ToString() + browtf;
		}
	}
	else
	{
		return Utils::UKismetStringLibrary::Conv_NameToString(Property->GetClass()->GetFName()).ToString();
	}
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
			static UClass* FunctionProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Function");

			std::string Additional = (Property->GetClass() == FunctionProp) ? "()" : "";

			result += "		" + pType + " " + pName + Additional + ";\n";
		}
	}
	
	result += "\n};";
	
	return result;
}