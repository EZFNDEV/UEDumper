#pragma once

#include "UObjectBaseUtility.h"

/**
 * The base class of all UE objects. The type of an object is defined by its UClass.
 * This provides support functions for creating and using objects, and virtual functions that should be overridden in child classes.
 *
 * @see https://docs.unrealengine.com/ProgrammingAndScripting/ProgrammingWithCPP/UnrealArchitecture/Objects
 */

class UObject : public UObjectBaseUtility {
	public:
		/** Default constructor */
		UObject();

		
};