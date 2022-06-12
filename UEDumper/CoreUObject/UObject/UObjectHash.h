#pragma once
#include "UObjectArray.h"


/***
*
* FUObjectArray replaces the functionality of GObjObjects and UObject::Index
*
* Note the layout of this data structure is mostly to emulate the old behavior and minimize code rework during code restructure.
* Better data structures could be used in the future, for example maybe all that is needed is a TSet<UObject *>
* One has to be a little careful with this, especially with the GC optimization. I have seen spots that assume
* that non-GC objects come before GC ones during iteration.
*
**/

class FUObjectArray {
	friend class UObject;
private:
	/**
	 * Reset the serial number from the game thread to invalidate all weak object pointers to it
	 *
	 * @param Object to reset
	 */
	 // void ResetSerialNumber(UObjectBase* Object);

public:

	enum ESerialNumberConstants
	{
		START_SERIAL_NUMBER = 1000,
	};


	/**
	 * Constructor, initializes to no permanent object pool
	 */
	FUObjectArray() {}

	FUObjectArray(uintptr_t ObjectArrayAddr, bool bUseNewObjectArray)
	{
		if (!bUseNewObjectArray)
			ObjObjectsOld = (FFixedUObjectArray*)ObjectArrayAddr;
		else
			ObjObjectsNew = (FChunkedFixedUObjectArray*)ObjectArrayAddr;

		useNewObjectArray = bUseNewObjectArray;
	}

	 /**
	  * Returns the UObject corresponding to index. Be advised this is only for very low level use.
	  *
	  * @param Index index of object to return
	  * @return Object at this index
	  */
	inline FUObjectItem* IndexToObject(int32_t Index)
	{
		// check(Index >= 0);

		if (useNewObjectArray) {
			if (Index < ObjObjectsNew->Num())
			{
				return ObjObjectsNew->GetObjectPtr(Index);
			}
		}
		else {
			if (ObjObjectsOld->IsValidIndex(Index))
			{
				return ObjObjectsOld->GetObjectPtr(Index);
			}
		}

		return nullptr;
	}

	int32_t Num() {
		if (useNewObjectArray) {
			return ObjObjectsNew->Num();
		}
		else {
			return ObjObjectsOld->Num();
		}

		return 0;
	}

public:
	// EZFN-NOTE: Hacky way to support all versions
	FChunkedFixedUObjectArray* ObjObjectsNew;
	FFixedUObjectArray* ObjObjectsOld;

	bool useNewObjectArray;
};

// Global UObject array instance
extern inline FUObjectArray GUObjectArray = *new FUObjectArray();