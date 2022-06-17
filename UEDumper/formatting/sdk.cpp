#include "pch.h"
#include "sdk.h"

#include "../CoreUObject/UObject/UObjectBaseUtility.h"
#include "../CoreUObject/UObject/UnrealTypePrivate.h"

static char GetPrefix(UObjectBase* Object) // TODO: Move this to SDKFormatting;
{
	static UClass* UClassClass = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Class");
	static UClass* ActorClass = (UClass*)Utils::StaticFindObject(L"/Script/Engine.Actor");
	static UClass* ObjectClass = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Object");

	return '\0';
}

std::string SDKFormatting::UPropertyTypeToString(UObjectPropertyBase* Property) {

	UClass* ClassPrivate = Property->GetClass();

	static UClass* DoubleProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.DoubleProperty");
	static UClass* FloatProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.FloatProperty");
	static UClass* IntProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.IntProperty");
	static UClass* Int16Prop = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Int16Property");
	static UClass* BoolProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.BoolProperty");
	static UClass* ObjectProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.ObjectProperty");
	static UClass* FunctionProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Function");
	static UClass* StructProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.StructProperty");
	static UClass* ClassProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.ClassProperty");
	static UClass* ArrayProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.ArrayProperty");
	static UClass* ByteProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.ByteProperty");
	static UClass* MulticastDelegateProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.MulticastDelegateProperty");
	static UClass* EnumProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.EnumProperty");
	

	if (ClassPrivate == DoubleProp)
		return "double";
	else if (ClassPrivate == FloatProp)
		return "float";
	else if (ClassPrivate == IntProp)
		return "int";
	else if (ClassPrivate == Int16Prop)
		return "int16_t";
	else if (ClassPrivate == StructProp)
		return /* GetPrefix(Property) + */ Utils::UKismetStringLibrary::Conv_NameToString((*(UStruct**)(__int64(Property) + 0x70))->GetFName()).ToString(); // 0x70 = Struct
	else if (ClassPrivate == ClassProp)
		return /* GetPrefix(Property) + */ Utils::UKismetStringLibrary::Conv_NameToString((*(UStruct**)(__int64(Property) + 120))->GetFName()).ToString(); // 120 = MetaClass
	else if (ClassPrivate == BoolProp)
	{
		// todo: do some bitfield stuff kms
		return "bool";
	} else if (ClassPrivate == ByteProp)
		return "char";
		// return std::format("TEnumAsByte<{}>", Utils::UKismetStringLibrary::Conv_NameToString(Property->GetFName()).ToString());
	else if (ClassPrivate == ArrayProp)
		return std::format("TArray<{}>", UPropertyTypeToString(*(UObjectPropertyBase**)(__int64(Property) + 0x70))); // 0x70 = Inner

	else if (ClassPrivate == FunctionProp) {
		std::string ReturnValueType;
		auto Func = (UStruct*)Property;

		auto FunctionFlags = *(uint32_t*)(__int64(Func) + 0x88);

		std::string ReturnType = "";
		std::vector<std::pair<std::string, std::string>> Params; // Param Type, Param Name

		for (UProperty* Parameter = (UProperty*)Func->GetChildren(); Parameter; Parameter = (UProperty*)Parameter->GetNext())
		{
			auto PropertyFlags = *(uint64_t*)(__int64(Parameter) + 0x38);
			auto ArrayDim = *(uint32_t*)(__int64(Parameter) + 0x30);
			auto ParamType = UPropertyTypeToString((UObjectPropertyBase*)Parameter);

			if (PropertyFlags & 0x400)
			{
				ReturnType = ParamType;
			}
			else if (PropertyFlags & 0x80) // Param Flag
			{
				Params.push_back({
					ParamType,
					Utils::UKismetStringLibrary::Conv_NameToString(Parameter->GetFName()).ToString()
					});
			}
		}

		if (ReturnType == "")
			ReturnType = "void";

		auto FullFunction = std::format("{} {}(", ReturnType, Utils::UKismetStringLibrary::Conv_NameToString(((UObjectPropertyBase*)Property)->GetFName()).ToString());
		for (int i = 0; i < Params.size(); i++)
		{
			auto& Param = Params[i];
			FullFunction += Param.first + ' ' + Param.second;
			if (i != Params.size() - 1)
				FullFunction += ", ";
		}

		return FullFunction;
	} else if (ClassPrivate == ObjectProp) {
		auto PropertyClass = Property->GetPropertyClass();
		auto ArrayDim = *(uint32_t*)(__int64(Property) + 0x30);

		std::string browtf;

		for (int i = 0; i < ArrayDim; i++)
		{
			browtf += "*";
		}

		if (PropertyClass)
		{
			return /* GetPrefix(Property) + */ Utils::UKismetStringLibrary::Conv_NameToString(PropertyClass->GetFName()).ToString() + browtf;
		}
	} else if (ClassPrivate == MulticastDelegateProp) {
		// Get the UFunction
		auto Delegate = (UMulticastDelegateProperty*)Property;
		
		UFunction* Function = Delegate->GetSignatureFunction();

		//printf(Utils::UKismetSystemLibrary::GetPathName((uintptr_t*)Function).ToString().c_str());

		//printf("Function: %p\n", Function);
		//printf("Func: %p\n", Function->GetFunc());
		// TODO: Take a deeper look into it...
		
		UDelegateProperty* DelegateProperty = (UDelegateProperty*)Function;
		//printf("DOES THAT WORK: %p\n", DelegateProperty->GetSignatureFunction());
		
		return Utils::UKismetStringLibrary::Conv_NameToString(Property->GetClass()->GetFName()).ToString();
	}
	else if (ClassPrivate == EnumProp) {
		auto Enum = (UEnumProperty*)Property;
		
		return Utils::UKismetStringLibrary::Conv_NameToString(Enum->GetEnum()->GetFName()).ToString();
	} else {
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
	}
	else {
		classInfo.name = "UObject";
	}

	result += classInfo.name + " {\n	public:\n";


	if ((Class->GetChildren())) {
		for (UField* Property = (UField*)(Class)->GetChildren(); Property; Property = Property->GetNext()) {
			std::string pType = UPropertyTypeToString((UObjectPropertyBase*)Property);
			std::string pName = Utils::UKismetStringLibrary::Conv_NameToString(((UObjectPropertyBase*)Property)->GetFName()).ToString();
			static UClass* FunctionProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Function");
			bool bIsAFunction = (Property->GetClass() == FunctionProp);

			if (!bIsAFunction)
			{
				result += "		" + pType + " " + pName + ";\n";
			}
			else
			{
				result += "		" + pType + ");\n";
			}
		}
	}

	result += "\n};";

	return result;
}