#pragma once
#include "../../Core/UObject/NameTypes.h"

/**
 * Low level implementation of UObject, should not be used directly in game code
 */
class UObjectBase
{
	public:
		/**
		 * Returns the unique ID of the object...these are reused so it is only unique while the object is alive.
		 * Useful as a tag.
		**/
		/*uint32_t GetUniqueID() const
		{
			return (uint32_t)InternalIndex;
		}*/

		/** Returns the UClass that defines the fields of this object */
		struct UClass* GetClass() const
		{
			return *(struct UClass**)((__int64)this + Offsets::UObjectBase::ClassPrivate);
		}

		/** Returns the UObject this object resides in */
		/*UObject* GetOuter() const
		{
			return OuterPrivate;
		}*/

		/** Returns the logical name of this object */
		FName GetFName() const
		{
			return *(FName*)((__int64)this + Offsets::UObjectBase::NamePrivate);
		}
};