#include "pch.h"
#include "sdk.h"

#include "../CoreUObject/UObject/UObjectBaseUtility.h"
#include "../CoreUObject/UObject/UnrealTypePrivate.h"
#include <iostream>

// Returns a string because if you do a char it breaks stuff
static std::string GetPrefix(UObjectBase* Object) // TODO: Move this to SDKFormatting;
{
	static UClass* UClassClass = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Class");

	if (((UObjectBaseUtility*)Object)->IsA(UClassClass))
	{
		return ((UClass*)Object)->GetPrefixCPP();
	}

	return "F";
}
static std::string GetUPropertySpecifiers(EPropertyFlags Flags) {
#ifdef INCLUDE_IN_UE
	std::string result = "UPROPERTY(";
	// result += (Flags & EPropertyFlags::CPF_Edit) ? "EditAnywhere, " : ""; // TODO: Format like this
	if (Flags & EPropertyFlags::CPF_Edit) {
		result += "EditAnywhere, ";
	}

	if (Flags & EPropertyFlags::CPF_Edit & EPropertyFlags::CPF_DisableEditOnTemplate) {
		result += "EditInstanceOnly, ";
	}

	if ((Flags & EPropertyFlags::CPF_Edit & EPropertyFlags::CPF_DisableEditOnInstance) != 0)
	{
		result += "EditDefaultsOnly, ";
	}

	if ((Flags & EPropertyFlags::CPF_Edit & EPropertyFlags::CPF_EditConst) != 0)
	{
		result += "VisibleAnywhere, ";
	}

	if ((Flags & EPropertyFlags::CPF_Edit & EPropertyFlags::CPF_EditConst & EPropertyFlags::CPF_DisableEditOnTemplate) != 0)
	{
		result += "VisibleInstanceOnly, ";
	}

	if ((Flags & EPropertyFlags::CPF_Edit & EPropertyFlags::CPF_EditConst & EPropertyFlags::CPF_DisableEditOnInstance) != 0)
	{
		result += "VisibleDefaultsOnly, ";
	}

	if ((Flags & EPropertyFlags::CPF_BlueprintVisible) != 0)
	{
		if ((Flags & EPropertyFlags::CPF_BlueprintReadOnly) != 0)
		{
			result += "BlueprintReadOnly, ";
		}
		else {
			result += "BlueprintReadWrite, ";
		}
	}

	if ((Flags & EPropertyFlags::CPF_Config) != 0)
	{
		if ((Flags & EPropertyFlags::CPF_GlobalConfig) != 0)
		{
			result += "GlobalConfig, ";
		}
		else
		{
			result += "Config, ";
		}
	}

	if ((Flags & EPropertyFlags::CPF_Transient) != 0)
	{
		result += "Transient, ";
	}

	if ((Flags & EPropertyFlags::CPF_DuplicateTransient) != 0)
	{
		result += "DuplicateTransient, ";
	}

	if ((Flags & EPropertyFlags::CPF_TextExportTransient) != 0)
	{
		result += "TextExportTransient, ";
	}

	if ((Flags & EPropertyFlags::CPF_NonPIEDuplicateTransient) != 0)
	{
		result += "NonPIETransient, ";
	}

	if ((Flags & EPropertyFlags::CPF_NonPIEDuplicateTransient) != 0)
	{
		result += "NonPIEDuplicateTransient, ";
	}

	if ((Flags & EPropertyFlags::CPF_ExportObject) != 0)
	{
		result += "Export, ";
	}

	//  EditInline -> deprecated

	if ((Flags & EPropertyFlags::CPF_NoClear) != 0)
	{
		result += "NoClear, ";
	}

	if ((Flags & EPropertyFlags::CPF_EditFixedSize) != 0)
	{
		result += "EditFixedSize, ";
	}

	if ((Flags & EPropertyFlags::CPF_Net) != 0)
	{
		result += "Replicated, ";

		if ((Flags & EPropertyFlags::CPF_RepNotify) != 0)
		{
			result += "ReplicatedUsing, ";
		}
	}

	// RepRetry -> deprecated

	if ((Flags & EPropertyFlags::CPF_Edit & EPropertyFlags::CPF_BlueprintVisible & EPropertyFlags::CPF_Interp) != 0)
	{
		result += "Interp, ";
	}

	if ((Flags & EPropertyFlags::CPF_NonTransactional) != 0)
	{
		result += "NonTransactional, ";
	}

	if ((Flags & EPropertyFlags::CPF_PersistentInstance & EPropertyFlags::CPF_ExportObject & EPropertyFlags::CPF_InstancedReference) != 0)
	{
		result += "Instanced, ";
	}

	if ((Flags & EPropertyFlags::CPF_BlueprintAssignable) != 0)
	{
		result += "BlueprintAssignable, ";
	}

	if ((Flags & EPropertyFlags::CPF_BlueprintCallable) != 0)
	{
		result += "BlueprintCallable, ";
	}

	if ((Flags & EPropertyFlags::CPF_BlueprintAuthorityOnly) != 0)
	{
		result += "BlueprintAuthorityOnly, ";
	}

	if ((Flags & EPropertyFlags::CPF_AssetRegistrySearchable) != 0)
	{
		result += "AssetRegistrySearchable, ";
	}

	if ((Flags & EPropertyFlags::CPF_SimpleDisplay) != 0)
	{
		result += "SimpleDisplay, ";
	}

	if ((Flags & EPropertyFlags::CPF_AdvancedDisplay) != 0)
	{
		result += "AdvancedDisplay, ";
	}

	if ((Flags & EPropertyFlags::CPF_SaveGame) != 0)
	{
		result += "SaveGame, ";
	}

	if ((Flags & EPropertyFlags::CPF_SkipSerialization) != 0)
	{
		result += "SkipSerialization, ";
	}

	// if (VariableCategory != EVariableCategory::Member)

	if ((Flags & EPropertyFlags::CPF_ConstParm) != 0)
	{
		result += "Const, ";
	}

	if ((Flags & EPropertyFlags::CPF_OutParm & EPropertyFlags::CPF_ReferenceParm) != 0)
	{
		result += "Ref, ";
	}

	if ((Flags & EPropertyFlags::CPF_RepSkip) != 0)
	{
		result += "NotReplicated, ";
		result += "ReplicatedParameter, ";
	}

	auto idx = result.find_last_of(", ");

	if (idx != std::string::npos)
		result = result.erase(idx - 1, idx);

	return result + ");";

#endif
	return "";
}

std::string SDKFormatting::UPropertyTypeToString(UProperty* Property, const std::string& PropName) {
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
	static UClass* UInt64Prop = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.UInt64Property");
	static UClass* Int64Prop = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Int64Property");
	static UClass* Int8Prop = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Int8Property");
	static UClass* TextProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.TextProperty");
	static UClass* SoftObjectProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.SoftObjectProperty");
	static UClass* SoftClassProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.SoftClassProperty");
	static UClass* WeakObjectProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.WeakObjectProperty");
	static UClass* LazyObjectProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.LazyObjectProperty");

	UClass* ClassPrivate = Property->GetClass();

	// "Easy" props first
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
	else if (ClassPrivate == NameProp)
		return "FName";
	else if (ClassPrivate == StrProp)
		return "FString";
	else if (ClassPrivate == UInt64Prop)
		return "uint64_t";
	else if (ClassPrivate == Int64Prop)
		return "int64_t";
	else if (ClassPrivate == Int8Prop)
		return "int8_t";
	else if (ClassPrivate == TextProp)
		return "FText";

	else if (ClassPrivate == ArrayProp) {
		UArrayProperty* ArrayProperty = (UArrayProperty*)Property;

		return std::format("TArray<{}>", UPropertyTypeToString(ArrayProperty->GetInner(), ""));
	} else if (ClassPrivate == EnumProp) {
		UEnumProperty* Enum = (UEnumProperty*)Property;

		return Utils::UKismetStringLibrary::Conv_NameToString(Enum->GetEnum()->GetFName()).ToString();
	} else if (ClassPrivate == StructProp) {
		UStructProperty* StructProperty = (UStructProperty*)Property;
		
		return "struct " + GetPrefix(StructProperty) + Utils::UKismetStringLibrary::Conv_NameToString(StructProperty->GetStruct()->GetFName()).ToString() + ((Property->GetArrayDim() > 1) ? "*" : "");
	} else if (ClassPrivate == ClassProp) {
		auto MetaClass = *(UStruct**)(__int64(Property) + 120);

		return "class " + GetPrefix(MetaClass) + Utils::UKismetStringLibrary::Conv_NameToString(MetaClass->GetFName()).ToString() + ((Property->GetArrayDim() > 1) ? "*" : "");
	}
	else if (ClassPrivate == BoolProp) {
		// todo: do some bitfield stuff kms
		return "bool";
	} else if (ClassPrivate == ByteProp)
		return "char";	// return std::format("TEnumAsByte<{}>", Utils::UKismetStringLibrary::Conv_NameToString(Property->GetFName()).ToString());

	
	// UObjectPropertyBase
	else if (ClassPrivate == ObjectProp) {
		UObjectPropertyBase* ObjectProperty = (UObjectPropertyBase*)Property;

		auto PropertyClass = ObjectProperty->GetPropertyClass();
		if (PropertyClass) {
			return "class " + GetPrefix(PropertyClass) + Utils::UKismetStringLibrary::Conv_NameToString(PropertyClass->GetFName()).ToString() + ((Property->GetArrayDim()) ? "*" : "");
		}
		else {
			return "MILXNOR?";
		}
	} else if (ClassPrivate == SoftObjectProp) {
		USoftObjectProperty* SoftObjectProperty = (USoftObjectProperty*)Property;

		auto PropertyClass = SoftObjectProperty->GetPropertyClass();
		if (PropertyClass) {
			std::string inner = GetPrefix(PropertyClass) + Utils::UKismetStringLibrary::Conv_NameToString(PropertyClass->GetFName()).ToString() + ((Property->GetArrayDim()) ? "*" : "");
			#ifdef INCLUDE_IN_UE
				return std::format("TSoftObjectPtr<{}>", inner);
			#else
				return inner;
			#endif
		}
		else {
			return "FAILED";
		}
	} /*else if (ClassPrivate == SoftObjectProp) { // TODO: MetaClass
		USoftClassProperty* SoftClassProperty = (USoftClassProperty*)Property;

		auto PropertyClass = SoftClassProperty->GetPropertyClass();
		if (PropertyClass) {
			std::string inner = GetPrefix(PropertyClass) + Utils::UKismetStringLibrary::Conv_NameToString(PropertyClass->GetFName()).ToString() + ((Property->GetArrayDim()) ? "*" : "");
			#ifdef INCLUDE_IN_UE
				return std::format("TSoftClassPtr<{}>", inner);
			#else
				return inner;
			#endif
		}
		else {
			return "FAILED";
		}
	}*/ else if (ClassPrivate == LazyObjectProp) {
		ULazyObjectProperty* LazyObjectProperty = (ULazyObjectProperty*)Property;

		auto PropertyClass = LazyObjectProperty->GetPropertyClass();
		if (PropertyClass) {
			std::string inner = GetPrefix(PropertyClass) + Utils::UKismetStringLibrary::Conv_NameToString(PropertyClass->GetFName()).ToString() + ((Property->GetArrayDim()) ? "*" : "");
			#ifdef INCLUDE_IN_UE
				return std::format("TLazyObjectPtr<{}>", inner);
			#else
				return inner;
			#endif
		}
		else {
			return "FAILED";
		}
	} else if (ClassPrivate == WeakObjectProp) {
		UWeakObjectProperty* WeakObjectProperty = (UWeakObjectProperty*)Property;

		auto PropertyClass = WeakObjectProperty->GetPropertyClass();
		if (PropertyClass) {
			std::string inner = GetPrefix(PropertyClass) + Utils::UKismetStringLibrary::Conv_NameToString(PropertyClass->GetFName()).ToString() + ((Property->GetArrayDim()) ? "*" : "");
			#ifdef INCLUDE_IN_UE
				return std::format("TWeakObjectPtr<{}>", inner);
			#else
				return inner;
			#endif
		}
		else {
			return "FAILED";
		}
	}
	
	// Functions
	
	if (ClassPrivate == FunctionProp) {
		std::string ReturnValueType;
		UFunction* Function = (UFunction*)Property;

		EFunctionFlags FunctionFlags = Function->GetFunctionFlags();

		std::string ReturnType = "";
		std::vector<std::pair<std::string, std::string>> Params; // Param Type, Param Name
		// ^ If we use a phmap::btree it is faster but I'm dumb.

		for (UProperty* Parameter = (UProperty*)Function->GetChildren(); Parameter; Parameter = (UProperty*)Parameter->GetNext())
		{
			auto PropertyFlags = *(uint64_t*)(__int64(Parameter) + 0x38);
			auto ArrayDim = *(uint32_t*)(__int64(Parameter) + 0x30);
			auto ParamType = UPropertyTypeToString((UObjectPropertyBase*)Parameter, "");

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

		auto FullFunction = std::format("{} {}(", ReturnType, PropName);
		for (int i = 0; i < Params.size(); i++)
		{
			auto& Param = Params[i];
			FullFunction += Param.first + ' ' + Param.second;
			if (i != Params.size() - 1)
				FullFunction += ", ";
		}

		return (FunctionFlags & EFunctionFlags::FUNC_Static) ? "static " + FullFunction : FullFunction;
	}
	 else if (ClassPrivate == MulticastDelegateProp) {
		// Get the UFunction
		UMulticastDelegateProperty* Delegate = (UMulticastDelegateProperty*)Property;
		UFunction* Function = Delegate->GetSignatureFunction();

		if (Function) { 
			// Note: "Function" won't be in the SDK dump? (Maybe a bug when filtering out?)
			// for now lets just add them manually

			// Probably, bc SuperStuct is null?
			
			// printf("Function: %p\n", Utils::UKismetSystemLibrary::GetPathName((uintptr_t*)Function).ToString().c_str());
			
			// if (Function->GetClass() == Delegate) {
				
				//printf(Utils::UKismetSystemLibrary::GetPathName((uintptr_t*)Function).ToString().c_str());

				//printf("Function: %p\n", Function);
				//printf("Func: %p\n", Function->GetFunc());
				// TODO: Take a deeper look into it...

				UDelegateProperty* DelegateProperty = (UDelegateProperty*)Function;


				// printf("DelegateProperty: %p\n", DelegateProperty);
				
				
				// printf("Func: %p\n", DelegateProperty->GetSignatureFunction());
				// printf("Outer: %s\n", Utils::UKismetSystemLibrary::GetPathName((uintptr_t*)DelegateProperty->GetOuter()).ToString().c_str());
			// }

			
		}

		
		
		return Utils::UKismetStringLibrary::Conv_NameToString(Property->GetClass()->GetFName()).ToString();
	}
	 else {
		return "__int64" + std::string("/*") + Utils::UKismetStringLibrary::Conv_NameToString(Property->GetClass()->GetFName()).ToString() + "*/";
	}
}

std::string GetFullCPPName() // TODO: Do this
{
	
}

void SDKFormatting::FormatUClass(UClass* Class, Ofstreams* streams) {
	static UClass* FunctionProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Function");
	static UClass* SoftObjectProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.SoftObjectProperty");
	static UClass* SoftClassProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.SoftClassProperty");
	static UClass* DelegateFuncProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.DelegateFunction");
	static UClass* WeakObjProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.WeakObjectProperty");
	static UClass* MulticastDelegateProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.MulticastDelegateProperty");
	static UClass* SetProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.SetProperty");
	static UClass* DelegateProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.DelegateProperty");

	std::string result = "";
	std::string funcResult = "";

	std::string Additional = "";

	UStruct* SuperStruct = Class->GetSuperStruct();

	if (SuperStruct)
		Additional = " : public " + GetPrefix(Class->GetSuperStruct()) + ((UObjectBaseUtility*)Class->GetSuperStruct())->GetName().ToString();
	else
		Additional = " : public UObject";

	auto ClassName = ((UObjectBaseUtility*)Class)->GetName().ToString();

	auto name = std::format("class {}{}", GetPrefix(Class) + ClassName, Additional);

	#ifdef INCLUDE_IN_UE
	result += "\n\nUCLASS(BlueprintType)\n";
	#endif

	result += name + "\n{\npublic:\n";

	if ((Class->GetChildren())) {
		int SuperStructSize = 0;

		if (SuperStruct)
			SuperStructSize = *(uint32_t*)((__int64)SuperStruct + 0x40);

		std::pair<int, int> OldProp; // Offset_Internal, Size
		int padNum = 1;
		

		bool bIsFirstAFunction = (Class->GetChildren()->GetClass() == FunctionProp);
		int OffsetDifference = 0;

		if (!bIsFirstAFunction && SuperStructSize) // make sure it inherits something and it has a member
		{
			OffsetDifference = ((UProperty*)Class->GetChildren())->GetOffset_Internal() - SuperStructSize; // Get the first child's offset and subtract the two

			if (OffsetDifference > 0)
			{
				result += std::format("    char UnknownData{}[0x{:x}];\n", padNum, OffsetDifference);
				padNum++;
			}
		}

		for (UField* Property = (UField*)Class->GetChildren(); Property; Property = Property->GetNext()) {
			std::string pName = Utils::UKismetStringLibrary::Conv_NameToString(((UObjectPropertyBase*)Property)->GetFName()).ToString();
			std::string pType = UPropertyTypeToString((UObjectPropertyBase*)Property, pName);
			bool bIsAFunction = (Property->GetClass() == FunctionProp);

			if (!bIsAFunction)
			{
				auto thisElementSize = *(int32_t*)(__int64(Property) + 0x34);
				bool bUnhandledType = false;

				if ( // Properties we do not have implemented
					Property->GetClass() == SoftClassProp ||
					Property->GetClass() == WeakObjProp ||
					// Property->GetClass() == DelegateFuncProp ||
					Property->GetClass() == DelegateProp ||
					Property->GetClass() == MulticastDelegateProp)
				{
					result += std::format("    char UnknownData{}[0x{:x}]; // UNHANDLED {}\n", padNum, thisElementSize, pType + " " + pName);
					padNum++;
					bUnhandledType = true;
				}

				if (OldProp.first && OldProp.second)
				{
					OffsetDifference = ((UProperty*)Property)->GetOffset_Internal() - (OldProp.first + OldProp.second);
					if (OffsetDifference > 0)
					{
						result += std::format("    char UnknownData{}[0x{:x}];\n", padNum, OffsetDifference);
						padNum++;
					}
				}

				OldProp.first = ((UProperty*)Property)->GetOffset_Internal();
				OldProp.second = thisElementSize;

				auto ArrayDim = ((UProperty*)Property)->GetArrayDim();
				
				if (!bUnhandledType)
				{
#ifdef INCLUDE_IN_UE
					result += "    " + GetUPropertySpecifiers(((UProperty*)Property)->GetPropertyFlags()) + "\n";
#endif

					pName += (ArrayDim > 2) ? std::format("[0x{:x}]", ArrayDim) : ""; // do like CharacterParts[0x8];

					result += std::format("    {} {}; // 0x{:x}\n", pType, pName, ((UProperty*)Property)->GetOffset_Internal());
				}

				if (Property->GetNext())
					result += "\n";
			}
			else
			{
				UFunction* Function = (UFunction*)Property;

				std::string ReturnValueType;

				EFunctionFlags FunctionFlags = Function->GetFunctionFlags();

				std::string ReturnType = "";
				std::vector<std::pair<std::string, std::string>> Params; // Param Type, Param Name

				for (UProperty* Parameter = (UProperty*)Function->GetChildren(); Parameter; Parameter = (UProperty*)Parameter->GetNext())
				{
					auto PropertyFlags = Parameter->GetPropertyFlags();
					auto ArrayDim = Parameter->GetArrayDim();
					auto ParamType = UPropertyTypeToString((UObjectPropertyBase*)Parameter, "");

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

				auto FullFunction = std::format("{}(", pName);
				std::string ParamsCombined = "";

				for (int i = 0; i < Params.size(); i++)
				{
					auto& Param = Params[i];
					ParamsCombined += Param.first + ' ' + Param.second;
					if (i != Params.size() - 1)
						ParamsCombined += ", ";
				}

				FullFunction += ParamsCombined;

				auto oldRet = ReturnType; // ReturnType without static
				const bool bIsStatic = (FunctionFlags & EFunctionFlags::FUNC_Static);
				ReturnType = (bIsStatic ? "static " : "") + ReturnType;
				
				auto nameDef = ReturnType + ' ' + GetPrefix(Class) + ClassName + "::" + FullFunction;

				funcResult += nameDef + R"()
{)";
				if (Params.size() > 0 || oldRet != "void")
				{
					funcResult += R"(
	struct {
)";
					if (Params.size() > 0)
					{
						for (int i = 0; i < Params.size(); i++)
						{
							auto& Param = Params[i];
							funcResult += "            " + Param.first + ' ' + Param.second + ";";
							if (i != (((Params.size() + ((oldRet == "void") ? 0 : 1))) - 1))
								funcResult += "\n";
						}
					}

					if (ReturnType != "void")
						funcResult += "            " + oldRet + ' ' + "ReturnValue;";

					funcResult += R"(
	} params{ )";
					if (Params.size() > 0)
					{
						for (int i = 0; i < Params.size(); i++)
						{
							auto& Param = Params[i];
							funcResult += Param.second;
							if (i < Params.size() - 1)
								funcResult += ",";
							funcResult += " ";
						}
					}

					funcResult += "};\n";
				}

				// TODO: VTable
				// funcResult += "\n    static auto fn = UObject::FindObject<UFunction>(this::StaticClass(), \"" + pName + "\");\n";
				funcResult += "\n    static auto fn = UObject::FindObject<UFunction>(\"" + Utils::UKismetSystemLibrary::GetPathName((uintptr_t*)Function).ToString() + "\");\n";
				funcResult += "    ProcessEventO(" + std::string(bIsStatic ? "this->StaticClass()" : "this") + ", fn, " + std::string(((Params.size() > 0) ? " & params" : "nullptr")) + "); ";
				// ^ todo: use format
				
				if (ReturnType != "void")
					funcResult += "\n\n    return params.ReturnValue; ";

				// END

				funcResult += R"(
}
)" + std::string("\n");





			result += "    " + pType + ");";

			if (Property->GetNext())
				result += "\n";
			}
		}
	}

	#ifndef INCLUDE_IN_UE

		result += "\n" + std::string(R"(
		static UClass* StaticClass()
		{
			static UClass* ptr = UObject::FindObject<UClass>(")" + Utils::UKismetSystemLibrary::GetPathName((uintptr_t*)Class).ToString() + R"(");
			return ptr;
		}
	)"); // i don't know why but format doesnt work
	#endif
		
	result += "\n};";

	streams->Classes << result;
	streams->Functions << funcResult;
}

std::string SDKFormatting::CreateEnum(UEnum* Enum) {
	std::string result;

	auto Names = Enum->GetNames();

	if (Names.Num())
	{
		result += std::format("\n\nenum class {} : uint8_t", ((UObjectBaseUtility*)Enum)->GetName().ToString()) + "\n{\n";

		for (int i = 0; i < Names.Num(); i++)
		{
			// result += std::format("\n	{} = {}", Utils::UKismetStringLibrary::Conv_NameToString(Enum->GetNameByIndex(i)).ToString(), i);

			auto& Pair = Names[i];
			auto Name = Pair.Key;
			auto Value = Pair.Value;

			auto NameStr = Utils::UKismetStringLibrary::Conv_NameToString(Name).ToString();

			auto pos = NameStr.find_last_of(':');
			if (pos != std::string::npos) // theres probably a more modern c++ way of doing this
			{
				NameStr = NameStr.substr(pos + 1);
			}

			result += std::format("    {} = {}", NameStr, Value);

			if (i != Names.Num() - 1)
				result += ",\n";
		}

		result += "\n};\n\n";
	}
	return result;
}

std::string SDKFormatting::CreateStruct(UStruct* Struct) {
	static UClass* CoreUObjectStruct = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Struct");
	static UClass* CoreUObjectScriptStruct = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.ScriptStruct");
	static UClass* SoftObjectProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.SoftObjectProperty");
	static UClass* SoftClassProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.SoftClassProperty");
	static UClass* DelegateFuncProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.DelegateFunction");
	static UClass* WeakObjProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.WeakObjectProperty");
	static UClass* MulticastDelegateProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.MulticastDelegateProperty");
	static UClass* DelegateProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.DelegateProperty");
	
	std::string result;

	std::string Additional = "";

	if (Struct->GetSuperStruct())
		Additional = " : public " + GetPrefix(Struct->GetSuperStruct()) + ((UObjectBaseUtility*)Struct->GetSuperStruct())->GetName().ToString();

	auto name = std::format("\n\nstruct {}{}", GetPrefix(Struct) + ((UObjectBaseUtility*)Struct)->GetName().ToString(), Additional);

	result += name + "\n{\n";

	if ((Struct->GetChildren())) { // TODO: basically copy this whole thing from CreateClass when we are done with CreateClass.
		int SuperStructSize = 0;

		if (Struct->GetSuperStruct())
			SuperStructSize = *(uint32_t*)((__int64)Struct->GetSuperStruct() + 0x40);

		std::pair<int, int> OldProp; // Offset_Internal, Size
		int padNum = 1;

		int OffsetDifference = 0;

		if (SuperStructSize) // make sure it inherits something and it has a member
		{
			OffsetDifference = ((UProperty*)Struct->GetChildren())->GetOffset_Internal() - SuperStructSize; // Get the first child's offset and subtract the two

			if (OffsetDifference > 0)
			{
				result += std::format("    char UnknownData{}[0x{:x}];\n", padNum, OffsetDifference);
				padNum++;
			}
		}

		for (UField* Property = (UField*)Struct->GetChildren(); Property; Property = Property->GetNext()) {
			std::string pName = Utils::UKismetStringLibrary::Conv_NameToString(((UObjectPropertyBase*)Property)->GetFName()).ToString();
			std::string pType = UPropertyTypeToString((UObjectPropertyBase*)Property, pName);

			{
				auto thisElementSize = *(int32_t*)(__int64(Property) + 0x34);
				bool bUnhandledType = false;

				if ( // Properties we do not have implemented
					Property->GetClass() == SoftClassProp ||
					Property->GetClass() == WeakObjProp ||
					// Property->GetClass() == DelegateFuncProp ||
					Property->GetClass() == DelegateProp ||
					Property->GetClass() == MulticastDelegateProp)
				{
					result += std::format("    char UnknownData{}[0x{:x}]; // UNHANDLED {}\n", padNum, thisElementSize, pType + " " + pName);
					padNum++;
					bUnhandledType = true;
				}

				if (OldProp.first && OldProp.second)
				{
					OffsetDifference = ((UProperty*)Property)->GetOffset_Internal() - (OldProp.first + OldProp.second);
					if (OffsetDifference > 0)
					{
						result += std::format("    char UnknownData{}[0x{:x}];\n", padNum, OffsetDifference);
						padNum++;
					}
				}

				OldProp.first = ((UProperty*)Property)->GetOffset_Internal();
				OldProp.second = thisElementSize;

				auto ArrayDim = ((UProperty*)Property)->GetArrayDim();

				if (!bUnhandledType)
				{
#ifdef INCLUDE_IN_UE
					result += "    " + GetUPropertySpecifiers(((UProperty*)Property)->GetPropertyFlags()) + "\n";
#endif

					pName += (ArrayDim > 2) ? std::format("[0x{:x}]", ArrayDim) : ""; // do like CharacterParts[0x8];

					result += std::format("    {} {}; // 0x{:x}\n", pType, pName, ((UProperty*)Property)->GetOffset_Internal());
				}

				if (Property->GetNext())
					result += "\n";
			}
		}
	}
	result += "\n};\n\n";

	return result;
}