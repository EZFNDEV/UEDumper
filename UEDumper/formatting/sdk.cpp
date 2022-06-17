#include "pch.h"
#include "sdk.h"

#include "../CoreUObject/UObject/UObjectBaseUtility.h"
#include "../CoreUObject/UObject/UnrealTypePrivate.h"

// Returns a string because if you do a char it breaks stuff
static std::string GetPrefix(UObjectBase* Object) // TODO: Move this to SDKFormatting;
{
	static UClass* UClassClass = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Class");
	static UClass* ActorClass = (UClass*)Utils::StaticFindObject(L"/Script/Engine.Actor");
	static UClass* ObjectClass = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Object");

	if (((UObjectBaseUtility*)Object)->IsA(UClassClass))
	{
		if (((UObjectBaseUtility*)Object)->IsA(ActorClass))
		{
			return "A";
		}
		else if (((UObjectBaseUtility*)Object)->IsA(ObjectClass))
		{
			return "U";
		}
	}

	return "F";
}

std::string SDKFormatting::UPropertyTypeToString(UObjectPropertyBase* Property) {

	UClass* ClassPrivate = Property->GetClass();

	// TODO: Use the struct and make the StaticClass return it.
	
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
	static UClass* StrProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.StrProperty");
	static UClass* NameProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.NameProperty");
	static UClass* UInt32Prop = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.UInt32Property");
	
	if (ClassPrivate == DoubleProp)
		return "double";
	else if (ClassPrivate == FloatProp)
		return "float";
	else if (ClassPrivate == IntProp)
		return "int";
	else if (ClassPrivate == Int16Prop)
		return "int16_t";
	else if (ClassPrivate == UInt32Prop)
		return "uint32_t";
	else if (ClassPrivate == StructProp)
	{
		auto ActualStruct = *(UStruct**)(__int64(Property) + 0x70);
		return GetPrefix(ActualStruct) + Utils::UKismetStringLibrary::Conv_NameToString(ActualStruct->GetFName()).ToString();

	}
	else if (ClassPrivate == ClassProp)
	{
		auto MetaClass = *(UStruct**)(__int64(Property) + 120);
		return GetPrefix(MetaClass) + Utils::UKismetStringLibrary::Conv_NameToString(MetaClass->GetFName()).ToString();
	}
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
	}

	else if (ClassPrivate == NameProp)
		return "FName";

	else if (ClassPrivate == StrProp)
		return "FString";

	else if (ClassPrivate == ObjectProp)
	{
		auto PropertyClass = Property->GetPropertyClass();
		auto ArrayDim = *(uint32_t*)(__int64(Property) + 0x30);

		std::string browtf;

		for (uint32_t i = 0; i < ArrayDim; i++)
		{
			browtf += "*";
		}

		if (PropertyClass)
		{
			return GetPrefix(PropertyClass) + Utils::UKismetStringLibrary::Conv_NameToString(PropertyClass->GetFName()).ToString() + browtf;
		}
	}
	
	else if (ClassPrivate == MulticastDelegateProp) {
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
	}
	else
	{
		return Utils::UKismetStringLibrary::Conv_NameToString(Property->GetClass()->GetFName()).ToString();
	}
}

std::string SDKFormatting::CreateClass(UStruct* Class) {
	/*
	// BlueprintGeneratedClass _WaterMeshBlueprintMaster._WaterMeshBlueprintMaster_C
	// 0x00C3 (0x03DB - 0x0318)
	*/

	std::string result = "";

	Class4Writing classInfo;

	std::string Additional = "";

	if (Class->GetSuperStruct())
		Additional = " : public " + GetPrefix(Class->GetSuperStruct()) + ((UObjectBaseUtility*)Class->GetSuperStruct())->GetName().ToString();

	classInfo.name = std::format("\n\nclass {}{}", GetPrefix(Class) + ((UObjectBaseUtility*)Class)->GetName().ToString(), Additional);

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
		for (UField* Property = (UField*)Class->GetChildren(); Property; Property = Property->GetNext()) {
			std::string pType = UPropertyTypeToString((UObjectPropertyBase*)Property);
			std::string pName = Utils::UKismetStringLibrary::Conv_NameToString(((UObjectPropertyBase*)Property)->GetFName()).ToString();
			static UClass* FunctionProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Function");
			bool bIsAFunction = (Property->GetClass() == FunctionProp);

			if (!bIsAFunction)
			{
				result += "		" + pType + " " + pName + ";";
				
				if (Property->GetNext())
					result += "\n";
			}
			else
			{
				result += "		" + pType + ");";
				if (Property->GetNext())
					result += "\n";
			}
		}
	}

	result += "\n};";

	return result;
}

std::string SDKFormatting::CreateStruct(UStruct* Struct) {
	static UClass* CoreUObjectStruct = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Struct");
	static UClass* CoreUObjectScriptStruct = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.ScriptStruct");
	static UClass* CoreUObjectEnum = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Enum");

	std::string result;

	Class4Writing classInfo;

	classInfo.name = "\n\nstruct " + ((UObjectBaseUtility*)Struct)->GetName().ToString();

	// we are checking what it is twice, which is slow.

	/* if (((UObjectBaseUtility*)Struct)->IsA(CoreUObjectEnum))
	{
		auto Enum = (UEnum*)Struct;
		auto Names = (TArray<uint64_t>*)(__int64(Enum) + 0x40);
		if (Names)
		{
			result += std::format("enum class {} : uint8_t", ((UObjectBaseUtility*)Enum)->GetName().ToString());

			for (int i = 0; i < (*Names).Num(); i++)
			{
				// result += std::format("\n	{} = {}", Utils::UKismetStringLibrary::Conv_NameToString(Enum->GetNameByIndex(i)).ToString(), i);
				auto Name = (uint64_t*)((*Names).Data + i * 16); // Most Definetly NOT skidded

				if (false) // Name)
				{
					auto NameStr = Utils::UKismetStringLibrary::Conv_NameToString(*Name).ToString();

					result += "		" + NameStr + ",\n";
				}
			}
		}
	} */

	if (((UObjectBaseUtility*)Struct)->IsA(CoreUObjectStruct) || ((UObjectBaseUtility*)Struct)->IsA(CoreUObjectScriptStruct))
	{
		Class4Writing classInfo;
		
		std::string Additional = "";

		if (Struct->GetSuperStruct())
			Additional = " : public " + GetPrefix(Struct->GetSuperStruct()) + ((UObjectBaseUtility*)Struct->GetSuperStruct())->GetName().ToString();

		classInfo.name = std::format("\n\nstruct {}{}", GetPrefix(Struct) + ((UObjectBaseUtility*)Struct)->GetName().ToString(), Additional);

		result += classInfo.name + "\n{\n";

		if ((Struct->GetChildren())) {
			for (UField* Property = (UField*)Struct->GetChildren(); Property; Property = Property->GetNext()) {
				std::string pType = UPropertyTypeToString((UObjectPropertyBase*)Property);
				std::string pName = Utils::UKismetStringLibrary::Conv_NameToString(((UObjectPropertyBase*)Property)->GetFName()).ToString();

				result += "		" + pType + " " + pName + ";";
				
				if (Property->GetNext())
					result += "\n";
			}
		}
		else 
			return ""; // no members, no point of dumping

		result += "\n};";
	}

	return result;
}