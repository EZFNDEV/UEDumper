#pragma once

/*===================================================================================
	FastReferenceCollectorOptions.h: Global TFastReferenceCollector enums and defines
=====================================================================================*/

enum class EFastReferenceCollectorOptions // : uint32_t
{
	None = 0,
	Parallel = 1 << 0,
	AutogenerateTokenStream = 1 << 1,
	ProcessNoOpTokens = 1 << 2,
	WithClusters = 1 << 3,
	ProcessWeakReferences = 1 << 4,
	WithPendingKill = 1 << 5,
};