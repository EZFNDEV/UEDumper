#pragma once

#include <thread>

namespace CoreUObjectOffsetFinder {
	extern inline uintptr_t* UEngineObject = 0;

	extern inline void Init() {
		// For the next offsets we need some more stuff
		while (!Utils::UKismetStringLibrary::Init()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		while (!Utils::UKismetSystemLibrary::Init()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		// Find the needed objects once
		UEngineObject = Utils::StaticFindObject(L"Engine.Engine");
	}

	namespace _UObjectBase {
		extern inline uint16_t ObjectFlags = 0;
		extern inline uint16_t InternalIndex = 0;
		extern inline uint16_t FindClassPrivate();
		extern inline uint16_t FindNamePrivate();
		extern inline uint16_t FindOuterPrivate();
	}

	namespace _UStruct {
		extern inline uint16_t FindSuperStruct();
		extern inline uint16_t FindChildren();
		extern inline uint16_t FindChildProperties();
	}

	namespace _UField {
		extern inline uint16_t FindNext();
	}

	namespace _UProperty {
		extern inline uint16_t ArrayDim = 0;
		extern inline uint16_t FindElementSize();
		extern inline uint16_t PropertyFlags = 0;
		extern inline uint16_t RepIndex = 0;
		extern inline uint16_t BlueprintReplicationCondition = 0;
		extern inline uint16_t FindOffset_Internal();
		extern inline uint16_t RepNotifyFunc = 0;
		extern inline uint16_t PropertyLinkNext = 0;
		extern inline uint16_t NextRef = 0;
		extern inline uint16_t DestructorLinkNext = 0;
		extern inline uint16_t PostConstructLinkNext = 0;
	}

	namespace _UObjectPropertyBase {
		extern inline uint16_t FindPropertyClass();
	}
}