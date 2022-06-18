#pragma once

#include "FastReferenceCollectorOptions.h"

/**
 * FWeakObjectPtr is a weak pointer to a UObject.
 * It can return nullptr later if the object is garbage collected.
 * It has no impact on if the object is garbage collected or not.
 * It can't be directly used across a network.
 *
 * Most often it is used when you explicitly do NOT want to prevent something from being garbage collected.
 */
struct FWeakObjectPtr
{
public:

	template <typename ReferenceProcessorType, typename CollectorType, typename ArrayPoolType, EFastReferenceCollectorOptions Options>
	friend class TFastReferenceCollector;
	friend struct FFieldPath;

private:
	int32_t		ObjectIndex;
	int32_t		ObjectSerialNumber;
};