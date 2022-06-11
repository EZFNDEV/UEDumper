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

		// VTable
		extern inline uint16_t ProcessEvent = 0;
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
}