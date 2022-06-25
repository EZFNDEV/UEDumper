#include "pch.h"
#include "sdk.h"

#include "../offsets/analyze.h"
#include "../CoreUObject/UObject/UObjectBaseUtility.h"
#include "../CoreUObject/UObject/UnrealTypePrivate.h"
#include <iostream>
#include "formatting.h"

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

static std::string UPropertyTypeToStringHacky(UProperty* Property) {
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
		return "struct FString";
	else if (ClassPrivate == UInt64Prop)
		return "uint64_t";
	else if (ClassPrivate == Int64Prop)
		return "int64_t";
	else if (ClassPrivate == Int8Prop)
		return "int8_t";
	else if (ClassPrivate == TextProp)
		return "struct FText";
	else {
		// SOOOO, now we need to make sure the struct is already in the file

		if (ClassPrivate == ArrayProp) {
			UArrayProperty* ArrayProperty = (UArrayProperty*)Property;

			return std::format("TArray<{}>", UPropertyTypeToStringHacky(ArrayProperty->GetInner()));
		}
		else if (ClassPrivate == EnumProp) {
			UEnumProperty* Enum = (UEnumProperty*)Property;

			return Utils::UKismetStringLibrary::Conv_NameToString(Enum->GetEnum()->GetFName()).ToString();
		}
		else if (ClassPrivate == StructProp) {
			UStructProperty* StructProperty = (UStructProperty*)Property;

			return "struct " + GetPrefix(StructProperty) + Utils::UKismetStringLibrary::Conv_NameToString(StructProperty->GetStruct()->GetFName()).ToString() + ((Property->GetArrayDim() > 1) ? "*" : "");
		}
		else if (ClassPrivate == ClassProp) {
			UClassProperty* ClassProperty = (UClassProperty*)Property;
			UClass* MetaClass = ClassProperty->GetMetaClass();
			// Classes are always pointers?
			return "class " + GetPrefix(MetaClass) + Utils::UKismetStringLibrary::Conv_NameToString(MetaClass->GetFName()).ToString() + "*"/* + ((Property->GetArrayDim() > 1) ? "*" : "")*/;
		}
		else if (ClassPrivate == BoolProp) {
			// todo: do some bitfield stuff kms
			return "bool";
		}
		else if (ClassPrivate == ByteProp)
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
		}
		else if (ClassPrivate == SoftObjectProp) {
			USoftObjectProperty* SoftObjectProperty = (USoftObjectProperty*)Property;

			auto PropertyClass = SoftObjectProperty->GetPropertyClass();
			if (PropertyClass) {
				std::string inner = GetPrefix(PropertyClass) + Utils::UKismetStringLibrary::Conv_NameToString(PropertyClass->GetFName()).ToString() + ((Property->GetArrayDim()) ? "*" : "");
				return std::format("struct TSoftObjectPtr<struct {}>", inner);
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
				return std::format("TLazyObjectPtr<{}>", inner);
			}
			else {
				return "FAILED";
			}
		}
		else if (ClassPrivate == WeakObjectProp) {
			UWeakObjectProperty* WeakObjectProperty = (UWeakObjectProperty*)Property;

			auto PropertyClass = WeakObjectProperty->GetPropertyClass();
			if (PropertyClass) {
				std::string inner = GetPrefix(PropertyClass) + Utils::UKismetStringLibrary::Conv_NameToString(PropertyClass->GetFName()).ToString() + ((Property->GetArrayDim()) ? "*" : "");
				return std::format("TWeakObjectPtr<{}>", inner);
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

			for (UProperty* Parameter = (UProperty*)Function->GetChildren(); Parameter; Parameter = (UProperty*)Parameter->GetNext())
			{
				auto PropertyFlags = Parameter->GetPropertyFlags();
				auto ParamType = UPropertyTypeToStringHacky((UObjectPropertyBase*)Parameter);

				if (PropertyFlags & EPropertyFlags::CPF_ReturnParm)
				{
					ReturnType = ParamType;
				}
				else if (PropertyFlags & EPropertyFlags::CPF_Parm)
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

			//	printf("Function: %p\n", Utils::UKismetSystemLibrary::GetPathName((uintptr_t*)Function).ToString().c_str());

				// if (Function->GetClass() == Delegate) {

					//printf(Utils::UKismetSystemLibrary::GetPathName((uintptr_t*)Function).ToString().c_str());

					//printf("Function: %p\n", Function);
					//printf("Func: %p\n", Function->GetFunc());
					// TODO: Take a deeper look into it...

				UDelegateProperty* DelegateProperty = (UDelegateProperty*)Function;


				//	printf("DelegateProperty: %p\n", DelegateProperty);


				//	printf("Func: %p\n", DelegateProperty->GetSignatureFunction());
				//	printf("Outer: %s\n", Utils::UKismetSystemLibrary::GetPathName((uintptr_t*)DelegateProperty->GetOuter()).ToString().c_str());
				// }


			}



			return Utils::UKismetStringLibrary::Conv_NameToString(Property->GetClass()->GetFName()).ToString();
		}
		else {
			return "__int64" + std::string("/*") + Utils::UKismetStringLibrary::Conv_NameToString(Property->GetClass()->GetFName()).ToString() + "*/";
		}
	}
}


std::string GetFullCPPName() // TODO: Do this
{
	return "";
}

void SDKFormatting::LupusFormatUClass(UClass* Class, Ofstreams* streams) {
	static UClass* FunctionClass = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Function");
	static UClass* BoolProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.BoolProperty");
	static __int64 RealUObject = Utils::UKismetStringLibrary::Conv_StringToName_G(L"Object");

	// Get the proper name
	UStruct* SuperStruct = Class->GetSuperStruct();

	std::string fullName = Utils::UKismetSystemLibrary::GetPathName((uintptr_t*)Class).ToString();

	std::string name = Utils::UKismetSystemLibrary::GetPathName((uintptr_t*)Class).ToString();
	#ifdef INCLUDE_IN_UE
		streams->Classes << "\n\nUCLASS(BlueprintType)\n";
	#endif

	if (Class->GetFName() == RealUObject) {
		streams->Classes << R"(
class UObject {
	public:
		static uintptr_t* FindObject(std::string ClassName) {
			return 0;
		}
};
)";

		return;
	}

	if (SuperStruct) {
		streams->Classes << std::format(
			"class {}{}", GetPrefix(Class) + ((UObjectBaseUtility*)Class)->GetName().ToString(), // TODO: Use split and then get the stuff
			" : public " + GetPrefix(Class->GetSuperStruct()) + ((UObjectBaseUtility*)Class->GetSuperStruct())->GetName().ToString()
		);
	} else {
		streams->Classes << std::format(
			"class {}", GetPrefix(Class) + ((UObjectBaseUtility*)Class)->GetName().ToString()
		);
	}

	streams->Classes << "\n{\n	public:\n";

	int offset = 0;
	int unknownProps = 0;
	if (Class->GetChildren()) {
		offset = ((UProperty*)Class->GetChildren())->GetOffset_Internal();

		if (SuperStruct) {
			if (SuperStruct->GetPropertiesSize() < offset) {
				streams->Classes << std::format("	    char UnknownData{}[0x{:x}];\n", unknownProps, offset - SuperStruct->GetPropertiesSize());
				unknownProps++;
			}
		}

		for (UField* Property = (UField*)Class->GetChildren(); Property; Property = Property ? Property->GetNext() : 0) {
			std::string propName = Utils::UKismetStringLibrary::Conv_NameToString(((UObjectPropertyBase*)Property)->GetFName()).ToString();
			
			if (
				Property->GetClass() != BoolProp &&
				((UProperty*)Property)->GetOffset_Internal() != offset
			) {
				if (((UProperty*)Property)->GetOffset_Internal() < offset) {
					// Logic issue?
					streams->Classes << std::format(
						"	    char UnknownData{}[0x{:x}]; // 0x{:x}\n", unknownProps,
						-(((UProperty*)Property)->GetOffset_Internal() - offset),
						offset
					);
				} else {
					streams->Classes << std::format(
						"	    char UnknownData{}[0x{:x}]; // 0x{:x}\n", unknownProps,
						((UProperty*)Property)->GetOffset_Internal() - offset,
						offset
					);
				}

				offset = ((UProperty*)Property)->GetOffset_Internal();
				unknownProps++;
			}

			std::string propType = UPropertyTypeToStringHacky((UObjectPropertyBase*)Property);
			if (Property->GetClass() == FunctionClass) {
				UFunction* Function = (UFunction*)Property;

				std::string ReturnValueType;

				EFunctionFlags FunctionFlags = Function->GetFunctionFlags();

				std::string ReturnType = "";
				std::vector<std::pair<std::string, std::string>> Params; // Param Type, Param Name

				for (UProperty* Parameter = (UProperty*)Function->GetChildren(); Parameter; Parameter = (UProperty*)Parameter->GetNext())
				{
					auto PropertyFlags = Parameter->GetPropertyFlags();
					auto ArrayDim = Parameter->GetArrayDim();
					auto ParamType = UPropertyTypeToStringHacky((UObjectPropertyBase*)Parameter);

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

				ReturnType = "void"; // Testing compiling lol

				auto FullFunction = std::format("{}(", Utils::UKismetStringLibrary::Conv_NameToString(((UObjectPropertyBase*)Property)->GetFName()).ToString());
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
				ReturnType = ((FunctionFlags & EFunctionFlags::FUNC_Static) ? "static " : "") + ReturnType;

				auto nameDef = ReturnType + ' ' + GetPrefix(Class) + ((UObjectBaseUtility*)Class)->GetName().ToString() + "::" + FullFunction; // 0x2000 = STATIC

				streams->Functions << nameDef + R"()
{)";
				if (Params.size() > 0 || oldRet != "void")
				{
					streams->Functions << R"(
	struct {
)";
					if (Params.size() > 0)
					{
						for (int i = 0; i < Params.size(); i++)
						{
							auto& Param = Params[i];
							streams->Functions << "            " + Param.first + ' ' + Param.second + ";";
							if (i != (((Params.size() + ((oldRet == "void") ? 0 : 1))) - 1))
								streams->Functions <<  "\n";
						}
					}

					if (ReturnType != "void")
						streams->Functions << "            " + oldRet + ' ' + "ReturnValue;";

					streams->Functions << R"(
	} params{ )";
					if (Params.size() > 0)
					{
						for (int i = 0; i < Params.size(); i++)
						{
							auto& Param = Params[i];
							streams->Functions << Param.second;
							if (i < Params.size() - 1)
								streams->Functions << ",";
							streams->Functions << " ";
						}
					}

					streams->Functions << "};\n";
				}

				// TODO: VTable
				// streams->Functions << "\n    static auto fn = UObject::FindObject<UFunction>(this::StaticClass(), \"" + propName + "\");\n";
				//streams->Functions << "\n    static auto fn = UObject::FindObject<UFunction>(\"" + Utils::UKismetSystemLibrary::GetPathName((uintptr_t*)Property).ToString() + "\");\n";
				//streams->Functions << "    ProcessEvent(this, fn, " + std::string(((Params.size() > 0) ? "&params" : "nullptr")) + ");";

				//if (ReturnType != "void")
				//	streams->Functions << "\n\n    return params.ReturnValue; ";

				// END

				streams->Functions << R"(
}
)" + std::string("\n");

				if (Property->GetNext())
					streams->Functions << "\n";

				streams->Classes << std::format("	    {}; // 0x{:x} Size: 0x{:x}\n", propType, ((UProperty*)Property)->GetOffset_Internal(), ((UProperty*)Property)->GetElementSize());
			} else {
			
				//#if defined(ANALYZE) && defined(FILTER_ANALYZE_OBJECT) // Without FILTER_ANALYZE_OBJECT it would probably take hours-days
				//	if (fullName.find("GameState") != -1) {
				//		streams->Classes << std::format("{}\n	    {} {}; // 0x{:x} Size: 0x{:x}\n", Analyze::ReturnReferenceInfo((UProperty*)Property), propType, propName, ((UProperty*)Property)->GetOffset_Internal(), ((UProperty*)Property)->GetElementSize());
				//	}
			
				//#else
					streams->Classes << std::format("	    {} {}; // 0x{:x} Size: 0x{:x}\n", propType, propName, ((UProperty*)Property)->GetOffset_Internal(), ((UProperty*)Property)->GetElementSize());
				//#endif
			}
			
			offset += ((UProperty*)Property)->GetElementSize();
		}
	}

	if (offset != Class->GetPropertiesSize()) {
		streams->Classes << std::format("	    char UnknownData{}[0x{:x}];\n", unknownProps, Class->GetPropertiesSize() - offset);
		unknownProps++;
	}

	if (Class->GetChildProperties()) {

	}

	#ifndef INCLUDE_IN_UE
		streams->Classes << "\n		static class UClass* StaticClass()\n	    {\n			static auto ptr = UObject::FindObject(\"" << fullName << "\");\n			return (class UClass*)ptr;\n		};\n";
	#endif

	streams->Classes << "\n};\n\n";
}

void SDKFormatting::LupusFormatStruct(UClass* Class, Ofstreams* streams, std::unordered_map<__int64, __int64>* packageStructs, bool isEnum, bool isScriptStruct, bool isStruct, std::vector<__int64>* structsAdded, std::unordered_map<__int64, Ofstreams*> allstreams) {
	static UClass* FunctionClass = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.Function");
	static UClass* BoolProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.BoolProperty");

	// We are doing this because we can not yet know,
	// if this struct can be used or needs another struct first
	std::string result = "";

	// NOTE: GetName by Milxnor, but it looks wrong to me
	if (isEnum) {
		auto Enum = (UEnum*)Class;
		auto Names = Enum->GetNames();

		if (Names.Num())
		{
			result += std::format("\n\nenum class {} : uint8_t", ((UObjectBaseUtility*)Enum)->GetName().ToString()) + "\n{\n"; // Todo, check if it's really uint8

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

			result += "\n};";
		}
	} else {
		// Get the proper name
		UStruct* SuperStruct = Class->GetSuperStruct();

		std::string name = Utils::UKismetSystemLibrary::GetPathName((uintptr_t*)Class).ToString();
		if (SuperStruct) {
			result += std::format(
				"struct {}{}", GetPrefix(Class) + ((UObjectBaseUtility*)Class)->GetName().ToString(), // TODO: Use split and then get the stuff
				" : public " + GetPrefix(Class->GetSuperStruct()) + ((UObjectBaseUtility*)Class->GetSuperStruct())->GetName().ToString()
			);

			// TODO: Check if Class->GetSuperStruct() is already defined, otherwise make this pending too...
		}
		else {
			result += std::format(
				"struct {}", GetPrefix(Class) + ((UObjectBaseUtility*)Class)->GetName().ToString()
			);
		}

		result += "\n{\n	public:\n";

		int offset = 0;
		int unknownProps = 0;
		if (Class->GetChildren()) {
			offset = ((UProperty*)Class->GetChildren())->GetOffset_Internal();

			if (SuperStruct) {
				if (SuperStruct->GetPropertiesSize() < offset) {
					result += std::format("	    char UnknownData{}[0x{:x}];\n", unknownProps, offset - SuperStruct->GetPropertiesSize());
					unknownProps++;
				}
			}

			for (UField* Property = (UField*)Class->GetChildren(); Property; Property = Property ? Property->GetNext() : 0) {
				std::string propName = Utils::UKismetStringLibrary::Conv_NameToString(((UObjectPropertyBase*)Property)->GetFName()).ToString();

				if (Property->GetClass() == FunctionClass) {
					// TODO: FUNCTION!!
					// streams->Classes << std::format("	    {} {}; // 0x{:x} Size: 0x{:x}\n", "void", propName, ((UProperty*)Property)->GetOffset_Internal(), ((UProperty*)Property)->GetElementSize());

					offset += ((UProperty*)Property)->GetElementSize();
				}
				else {
					if (
						Property->GetClass() != BoolProp &&
						((UProperty*)Property)->GetOffset_Internal() != offset
						) {
						if (((UProperty*)Property)->GetOffset_Internal() < offset) {
							// Logic issue?
							result += std::format(
								"	    char UnknownData{}[0x{:x}]; // 0x{:x}\n", unknownProps,
								-(((UProperty*)Property)->GetOffset_Internal() - offset),
								offset
							);
						}
						else {
							result += std::format(
								"	    char UnknownData{}[0x{:x}]; // 0x{:x}\n", unknownProps,
								((UProperty*)Property)->GetOffset_Internal() - offset,
								offset
							);
						}

						offset = ((UProperty*)Property)->GetOffset_Internal();
						unknownProps++;
					}

					static UClass* StructProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.StructProperty");
					static UClass* EnumProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.EnumProperty");
					static UClass* ArrayProp = (UClass*)Utils::StaticFindObject(L"/Script/CoreUObject.ArrayProperty");

					if (Property->GetClass() == StructProp) {
						static uint64_t FVectorName = Utils::UKismetStringLibrary::Conv_StringToName(L"Vector").GetComparisonIndex().Value;
						
						// if not already added to the file, we will need to wait
						UStructProperty* StructProp = (UStructProperty*)Property;
						__int64 name = (__int64)StructProp->GetStruct()->GetFName();
						if (name != FVectorName) {
							if (!(std::find(structsAdded->begin(), structsAdded->end(), (__int64)StructProp->GetStruct()->GetFName()) != structsAdded->end())) {
								//printf("Missing: %s\n", Utils::UKismetStringLibrary::Conv_NameToString(StructProp->GetStruct()->GetFName()).ToString().c_str());
								packageStructs->emplace((__int64)StructProp->GetStruct()->GetFName(), (__int64)Class);
								return;
							}
						}
						

					} else if (Property->GetClass() == EnumProp) {
						UEnumProperty* EnumProp = (UEnumProperty*)Property;

						if (!(std::find(structsAdded->begin(), structsAdded->end(), (__int64)EnumProp->GetEnum()->GetFName()) != structsAdded->end())) {
							packageStructs->emplace((__int64)EnumProp->GetEnum()->GetFName(), (__int64)Class);
							return;
						}
					} else if (Property->GetClass() == ArrayProp) {
						UArrayProperty* ArrayProperty = (UArrayProperty*)Property;

						if (!(std::find(structsAdded->begin(), structsAdded->end(), (__int64)ArrayProperty->GetInner()->GetFName()) != structsAdded->end())) {
							packageStructs->emplace((__int64)ArrayProperty->GetInner()->GetFName(), (__int64)Class);
							return;
						}
					}

					std::string propType = UPropertyTypeToStringHacky((UObjectPropertyBase*)Property);

					result += std::format("	    {} {}; // 0x{:x} Size: 0x{:x}\n", propType, propName, ((UProperty*)Property)->GetOffset_Internal(), ((UProperty*)Property)->GetElementSize());

					offset += ((UProperty*)Property)->GetElementSize();
				}
			}
		}

		if (offset != Class->GetPropertiesSize()) {
			result += std::format("	    char UnknownData{}[0x{:x}];\n", unknownProps, Class->GetPropertiesSize() - offset);
			unknownProps++;
		}

		if (Class->GetChildProperties()) {

		}

		result += "\n};\n\n";
	}

	streams->Structs << result;
	structsAdded->emplace_back((__int64)Class->GetFName());

	// Sorting
	// If this is a package that was needed for another one, 
	// we need to add the other one after this
	for (auto const& [key, val] : *packageStructs)
	{
		if (Class->GetFName() == key) {
			Ofstreams* Nestreams = 0;

			for (auto const& [key2, val2] : allstreams)
			{
				if (key2 == (__int64)((UClass*)val)->GetOuter()) {
					Nestreams = val2;
					break;
				}
			}

			if (!Nestreams)
				break;
			
			LupusFormatStruct((UClass*)val, Nestreams, packageStructs, false, true, false, structsAdded, allstreams);
			break;
		}
	}
}

std::string SDKFormatting::GenerateInitFunction() {
	std::string result = "\n	static void Init() {\n		_StaticFindObject = decltype(_StaticFindObject)((uintptr_t)GetModuleHandle(0) + ";

	auto Base = (uintptr_t)GetModuleHandleW(0);
	std::string staticfindobject = std::format("0x{:x}", (Offsets::StaticFindObject - Base));
	std::string processevent = std::format("0x{:x}", (Offsets::ProcessEvent - Base));

	result += staticfindobject;

	result += ");\n		_ProcessEvent = decltype(_ProcessEvent)((uintptr_t)GetModuleHandle(0) + " + processevent + ");\n	};";

	return result;
}

void SDKFormatting::CreateSDKHeader(std::ofstream& header) {
	// Include CoreUObject and Basic
	header << std::format("#pragma once\n");

	header << std::format("#include \"{}\"\n", ("SDK/" + std::string(SHORTNAME) + "_" + "Basic.hpp"));

	header << std::format("#include \"{}\"\n", ("SDK/" + std::string(SHORTNAME) + "_" + "CoreUObject_structs.hpp"));
	header << std::format("#include \"{}\"\n", ("SDK/" + std::string(SHORTNAME) + "_" + "CoreUObject_classes.hpp"));
	header << std::format("#include \"{}\"\n", ("SDK/" + std::string(SHORTNAME) + "_" + "CoreUObject_functions.cpp"));
}

void SDKFormatting::CreateBasics(std::ofstream& header) {
	std::string StaticFindObjectOffset = "";
	std::string ProcessEventOffset = "";

	header << R"(
#pragma once

#include <Windows.h>
#include <string>
#include <locale>
#include <format>
#include <iostream>

#include "../SDK.hpp"

namespace SDK {
	// https://github.com/EpicGames/UnrealEngine/blob/99b6e203a15d04fc7bbbf554c421a985c1ccb8f1/Engine/Source/Runtime/CoreUObject/Private/UObject/UObjectGlobals.cpp#L327
	static inline uintptr_t* (__fastcall* _StaticFindObject) (uintptr_t* ObjectClass, uintptr_t* InObjectPackage, const wchar_t* OrigInName, bool ExactClass) = 0;

	// https://github.com/EpicGames/UnrealEngine/blob/c3caf7b6bf12ae4c8e09b606f10a09776b4d1f38/Engine/Source/Runtime/CoreUObject/Private/UObject/ScriptCore.cpp#L1822
	static inline void* (__fastcall* _ProcessEvent) (uintptr_t* Object, uintptr_t* Function, void* Params) = 0;

)";


	header << GenerateInitFunction() << '\n';
	header << GenerateNameStruct() << '\n';
	header << GenerateTArray() << '\n';
	header << GenerateFString() << '\n';

	header << GenerateOthers() << '\n';

	header << "\n}\n\n\n";

	// Include CoreUObject
	header << std::format("#include \"{}\"\n", ("SDK/" + std::string(SHORTNAME) + "_" + "CoreUObject_structs.hpp"));
	header << std::format("#include \"{}\"\n", ("SDK/" + std::string(SHORTNAME) + "_" + "CoreUObject_classes.hpp"));
	header << std::format("#include \"{}\"\n", ("SDK/" + std::string(SHORTNAME) + "_" + "CoreUObject_functions.cpp"));
}