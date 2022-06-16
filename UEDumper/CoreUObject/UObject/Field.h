#pragma once

/**
  * Object representing a type of an FField struct.
  * Mimics a subset of UObject reflection functions.
  */
class FFieldClass {
	public:
		FName GetFName() {
			return *(FName*)((__int64)this + Offsets::FFieldClass::Name);
		}

		FString GetName()
		{
			return Utils::UKismetStringLibrary::Conv_NameToString(GetFName().GetComparisonIndex().Value);
		}
};

/**
 * Base class of reflection data objects.
 */

class FField {
	public:
		FFieldClass* GetClassPrivate() {
			return 0;
		}
};