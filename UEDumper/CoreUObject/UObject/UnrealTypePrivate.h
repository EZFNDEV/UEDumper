#pragma once

#include "Class.h"
#include "ObjectMacros.h"
#include "../../Core/Containers/Set.h"

class UProperty : public UField
{
	public:
		int32_t GetArrayDim() {
			return 0;
		}

		int32_t GetElementSize() {
			return 0;
		}

		EPropertyFlags GetPropertyFlags() {
			return *(EPropertyFlags*)((__int64)this + Offsets::UProperty::PropertyFlags);
		}

		uint16_t GetRepIndex() {
			return 0;
		}

		// In memory variables (generated during Link()).
		int32_t GetOffset_Internal() {
			return 0;
		}

		bool HasAnyPropertyFlags(uint64_t FlagsToCheck) const
		{
			return (*(uint64_t*)((__int64)this + Offsets::UProperty::PropertyFlags) & FlagsToCheck) != 0 || FlagsToCheck == CPF_AllFlags;
		}
};


class UNumericProperty : public UProperty {
	
};

class UByteProperty : public UNumericProperty {
	
};

class UInt8Property : public UNumericProperty {
	
};

class UInt16Property : public UNumericProperty {
	
};

class UIntProperty : public UNumericProperty {
	
};

class UInt64Property : public UNumericProperty {
	
};

class UUInt16Property : public UNumericProperty {
	
};

class UUInt32Property : public UNumericProperty {
	
};

class UUInt64Property : public UNumericProperty {
	
};

class UFloatProperty : public UNumericProperty {
	
};

class UDoubleProperty : public UNumericProperty {
	
};

class UBoolProperty : public UProperty {
	public:
		/** Size of the bitfield/bool property. Equal to ElementSize but used to check if the property has been properly initialized (0-8, where 0 means uninitialized). */
		uint8_t GetFieldSize() {
			return 0; // TODOL
		}

		/** Offset from the memeber variable to the byte of the property (0-7). */
		uint8_t GetByteOffset() {
			return 0;
		}

		/** Mask of the byte with the property value. */
		uint8_t GetByteMask() {
			return 0;
		}
		
		/** Mask of the field with the property value. Either equal to ByteMask or 255 in case of 'bool' type. */
		uint8_t GetFieldMask() {
			return 0;
		}

		/**
		 * If the return value is true this UBoolProperty represents C++ bool type.
		 */
		bool IsNativeBool()
		{
			return GetFieldMask() == 0xff;
		}
};

class UObjectPropertyBase : public UProperty
{
	public:
		UClass* GetPropertyClass() {
			return *(UClass**)((__int64)this + Offsets::UObjectPropertyBase::PropertyClass);
		}
};

class UObjectProperty : public UObjectPropertyBase {
	
};

class UWeakObjectProperty : public UObjectPropertyBase {
	
};

class ULazyObjectProperty : public UObjectPropertyBase {
	
};

class USoftObjectProperty : public UObjectPropertyBase {
	
};

class UClassProperty : public UObjectProperty {
	public: 
		UClass* GetMetaClass() {
			return 0;
		}
};

class USoftClassProperty : public USoftObjectProperty {
	public:
		UClass* GetMetaClass() {
			return 0;
		}
};

class UInterfaceProperty : public UProperty {
	public:
		/** The native interface class that this interface property refers to */
		UClass* GetInterfaceClass() {
			return 0;
		}
};

class UNameProperty : public UProperty {
	
};

class UStrProperty : public UProperty {
	
};

class UArrayProperty : public UProperty {
	public:
		UProperty* GetInner() {
			return 0;
		}
};

class UMapProperty : public UProperty {
	public:
		// Properties representing the key type and value type of the contained pairs
		
		UProperty* GetKeyProp() {
			return 0;
		}
		
		UProperty* GetValueProp() {
			return 0;
		}

		/*FScriptMapLayout GetMapLayout() {
			return 0;
		}*/
};

class USetProperty : public UProperty {
	public:
		// Properties representing the key type and value type of the contained pairs
		UProperty* GetElementProp() {
			return 0;
		}
		
		/*FScriptSetLayout GetSetLayout() {
			return 0;
		}*/
};

class UStructProperty : public UProperty {
	public:
		class UScriptStruct* GetStruct() {
			return 0;
		}
};


class UDelegateProperty : public UProperty {
	public:
		/** Points to the source delegate function (the function declared with the delegate keyword) used in the declaration of this delegate property. */
		UFunction* GetSignatureFunction() {
			return *(UFunction**)((__int64)this + Offsets::UDelegateProperty::SignatureFunction);
		}
};

class UMulticastDelegateProperty : public UProperty {
	public:
		/** Points to the source delegate function (the function declared with the delegate keyword) used in the declaration of this delegate property. */
		UFunction* GetSignatureFunction() {
			return *(UFunction**)((__int64)this + Offsets::UMulticastDelegateProperty::SignatureFunction);
		}
};

class UMulticastInlineDelegateProperty : public UMulticastDelegateProperty {

};

class UMulticastSparseDelegateProperty : public UMulticastDelegateProperty {
	
};

class UEnumProperty : public UProperty {
	public:
		// The property which represents the underlying type of the enum
		UNumericProperty* GetUnderlyingProp() {
			return *(UNumericProperty**)((__int64)this + Offsets::UEnumProperty::UnderlyingProp);
		}
		
		// The enum represented by this property
		class UEnum* GetEnum() {
			return *(UEnum**)((__int64)this + Offsets::UEnumProperty::Enum);
		}
};

class UTextProperty : public UProperty {
	
};