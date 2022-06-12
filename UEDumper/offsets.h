#pragma once

namespace Offsets {
	extern inline uintptr_t ProcessEvent = 0;
	extern inline uintptr_t StaticFindObject = 0;
	extern inline uintptr_t GObjects = 0;

	// Not all are used, offets are just examples ofc
	namespace UFunction {
		extern inline uint16_t FunctionFlags = 0;
		extern inline uint16_t NumParms = 0;
		extern inline uint16_t ParmsSize = 0;
		extern inline uint16_t ReturnValueOffset = 0;
		extern inline uint16_t RPCId = 0;
		extern inline uint16_t RPCResponseId = 0;
		extern inline uint16_t FirstPropertyToInit = 0;
		extern inline uint16_t Func = 0xC0;
	}

	namespace UObject {
		extern inline uint16_t InternalIndex = 0x0C;

		// VTable
		extern inline uint16_t ProcessEvent = 0;
	}

	namespace UObjectBase {
		extern inline uint16_t ClassPrivate = 0;
	}

	namespace UClass {
		extern inline uint16_t ChildProperties = 0;
	}

	namespace FField {
		extern inline uint16_t Next = 0;
		extern inline uint16_t Owner = 0;
		extern inline uint16_t NamePrivate = 0;
		extern inline uint16_t FlagsPrivate = 0;
	}

	namespace UStruct {
		extern inline uint16_t Children = 0;
	}

	namespace FName {
		extern inline uint16_t ComparisonIndex = 0;
		extern inline uint16_t DisplayIndex = 0x04;
		extern inline uint16_t Number = 0x04;
	}
}

namespace OffsetsFinder {
	extern uintptr_t FindProcessEvent();
	extern uintptr_t FindStaticFindObject();
	extern uintptr_t FindGObjects();

	extern uint16_t FindUFunctionOffset_Func();

	// VTable Indexes
	extern uint16_t FindUObject_PEVTableIndex();

	// Find by ProcessEvent
	extern uint16_t FindUObjectInternalIndex();

	// UClass stuff
	extern inline uint16_t FindUObjectBase_ClassPrivate();
	extern inline uint16_t FindUClass_ChildProperties();

	// Util
	extern uintptr_t FindRealFunction(uintptr_t* Function);
}