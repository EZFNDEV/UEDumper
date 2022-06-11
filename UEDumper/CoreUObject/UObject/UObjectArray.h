#pragma once
#include "ObjectMacros.h"

/**
* Single item in the UObject array.
*/
struct FUObjectItem
{
	// Pointer to the allocated object
	class UObjectBase* Object;
	// Internal flags
	int32_t Flags;
	// UObject Owner Cluster Index
	int32_t ClusterRootIndex;
	// Weak Object Pointer Serial number associated with the object
	int32_t SerialNumber;

	FUObjectItem()
		: Object(nullptr)
		, Flags(0)
		, ClusterRootIndex(0)
		, SerialNumber(0)
	{
	}
	~FUObjectItem()
	{
	}

	// Non-copyable
	FUObjectItem(FUObjectItem&&) = delete;
	FUObjectItem(const FUObjectItem&) = delete;
	FUObjectItem& operator=(FUObjectItem&&) = delete;
	FUObjectItem& operator=(const FUObjectItem&) = delete;

	inline void SetOwnerIndex(int32_t OwnerIndex)
	{
		ClusterRootIndex = OwnerIndex;
	}

	inline int32_t GetOwnerIndex() const
	{
		return ClusterRootIndex;
	}

	/** Encodes the cluster index in the ClusterRootIndex variable */
	inline void SetClusterIndex(int32_t ClusterIndex)
	{
		ClusterRootIndex = -ClusterIndex - 1;
	}

	/** Decodes the cluster index from the ClusterRootIndex variable */
	inline int32_t GetClusterIndex() const
	{
		// checkSlow(ClusterRootIndex < 0);
		return -ClusterRootIndex - 1;
	}

	inline int32_t GetSerialNumber() const
	{
		return SerialNumber;
	}

	/*inline void SetFlags(EInternalObjectFlags FlagsToSet)
	{
		// check((int32(FlagsToSet) & ~int32(EInternalObjectFlags::AllFlags)) == 0);
		ThisThreadAtomicallySetFlag(FlagsToSet);
	}*/

	inline EInternalObjectFlags GetFlags() const
	{
		return EInternalObjectFlags(Flags);
	}

	/*inline void ClearFlags(EInternalObjectFlags FlagsToClear)
	{
		check((int32(FlagsToClear) & ~int32(EInternalObjectFlags::AllFlags)) == 0);
		ThisThreadAtomicallyClearedFlag(FlagsToClear);
	}*/

	/**
	 * Uses atomics to clear the specified flag(s).
	 * @param FlagsToClear
	 * @return True if this call cleared the flag, false if it has been cleared by another thread.
	 */
	 /*FORCEINLINE bool ThisThreadAtomicallyClearedFlag(EInternalObjectFlags FlagToClear)
	 {
		 static_assert(sizeof(int32) == sizeof(Flags), "Flags must be 32-bit for atomics.");
		 bool bIChangedIt = false;
		 while (1)
		 {
			 int32 StartValue = int32(Flags);
			 if (!(StartValue & int32(FlagToClear)))
			 {
				 break;
			 }
			 int32 NewValue = StartValue & ~int32(FlagToClear);
			 if ((int32)FPlatformAtomics::InterlockedCompareExchange((int32*)&Flags, NewValue, StartValue) == StartValue)
			 {
				 bIChangedIt = true;
				 break;
			 }
		 }
		 return bIChangedIt;
	 }

	 FORCEINLINE bool ThisThreadAtomicallySetFlag(EInternalObjectFlags FlagToSet)
	 {
		 static_assert(sizeof(int32) == sizeof(Flags), "Flags must be 32-bit for atomics.");
		 bool bIChangedIt = false;
		 while (1)
		 {
			 int32 StartValue = int32(Flags);
			 if (StartValue & int32(FlagToSet))
			 {
				 break;
			 }
			 int32 NewValue = StartValue | int32(FlagToSet);
			 if ((int32)FPlatformAtomics::InterlockedCompareExchange((int32*)&Flags, NewValue, StartValue) == StartValue)
			 {
				 bIChangedIt = true;
				 break;
			 }
		 }
		 return bIChangedIt;
	 }*/

	inline bool HasAnyFlags(EInternalObjectFlags InFlags) const
	{
		return !!(Flags & int32_t(InFlags));
	}
};

/**
* Simple array type that can be expanded without invalidating existing entries.
* This is critical to thread safe FNames.
* @param ElementType Type of the pointer we are storing in the array
* @param MaxTotalElements absolute maximum number of elements this array can ever hold
* @param ElementsPerChunk how many elements to allocate in a chunk
**/
struct FChunkedFixedUObjectArray
{
	enum
	{
		NumElementsPerChunk = 64 * 1024,
	};

	/** Master table to chunks of pointers **/
	FUObjectItem** Objects;
	/** If requested, a contiguous memory where all objects are allocated **/
	FUObjectItem* PreAllocatedObjects;
	/** Maximum number of elements **/
	int32_t MaxElements;
	/** Number of elements we currently have **/
	int32_t NumElements;
	/** Maximum number of chunks **/
	int32_t MaxChunks;
	/** Number of chunks we currently have **/
	int32_t NumChunks;


	/**
	* Allocates new chunk for the array
	**/
	/*void ExpandChunksToIndex(int32 Index)
	{
		check(Index >= 0 && Index < MaxElements);
		int32 ChunkIndex = Index / NumElementsPerChunk;
		while (ChunkIndex >= NumChunks)
		{
			// add a chunk, and make sure nobody else tries
			FUObjectItem** Chunk = &Objects[NumChunks];
			FUObjectItem* NewChunk = new FUObjectItem[NumElementsPerChunk];
			if (FPlatformAtomics::InterlockedCompareExchangePointer((void**)Chunk, NewChunk, nullptr))
			{
				// someone else beat us to the add, we don't support multiple concurrent adds
				check(0)
			}
			else
			{
				NumChunks++;
				check(NumChunks <= MaxChunks);
			}
		}
		check(ChunkIndex < NumChunks&& Objects[ChunkIndex]); // should have a valid pointer now
	}*/

public:

	/** Constructor : Probably not thread safe **/
	/*FChunkedFixedUObjectArray() TSAN_SAFE
		: Objects(nullptr)
		, PreAllocatedObjects(nullptr)
		, MaxElements(0)
		, NumElements(0)
		, MaxChunks(0)
		, NumChunks(0)
	{
	}

	~FChunkedFixedUObjectArray()
	{
		if (!PreAllocatedObjects)
		{
			for (int32 ChunkIndex = 0; ChunkIndex < MaxChunks; ++ChunkIndex)
			{
				delete[] Objects[ChunkIndex];
			}
		}
		else
		{
			delete[] PreAllocatedObjects;
		}
		delete[] Objects;
	}*/

	/**
	* Expands the array so that Element[Index] is allocated. New pointers are all zero.
	* @param Index The Index of an element we want to be sure is allocated
	**/
	/*void PreAllocate(int32 InMaxElements, bool bPreAllocateChunks) TSAN_SAFE
	{
		check(!Objects);
		MaxChunks = InMaxElements / NumElementsPerChunk + 1;
		MaxElements = MaxChunks * NumElementsPerChunk;
		Objects = new FUObjectItem * [MaxChunks];
		FMemory::Memzero(Objects, sizeof(FUObjectItem*) * MaxChunks);
		if (bPreAllocateChunks)
		{
			// Fully allocate all chunks as contiguous memory
			PreAllocatedObjects = new FUObjectItem[MaxElements];
			for (int32 ChunkIndex = 0; ChunkIndex < MaxChunks; ++ChunkIndex)
			{
				Objects[ChunkIndex] = PreAllocatedObjects + ChunkIndex * NumElementsPerChunk;
			}
			NumChunks = MaxChunks;
		}
	}*/

	/**
	* Return the number of elements in the array
	* Thread safe, but you know, someone might have added more elements before this even returns
	* @return	the number of elements in the array
	**/
	inline int32_t Num() const
	{
		return NumElements;
	}
	/**
	* Return if this index is valid
	* Thread safe, if it is valid now, it is valid forever. Other threads might be adding during this call.
	* @param	Index	Index to test
	* @return	true, if this is a valid
	**/
	inline bool IsValidIndex(int32_t Index) const
	{
		return Index < Num() && Index >= 0;
	}

	/**
	* Return a pointer to the pointer to a given element
	* @param Index The Index of an element we want to retrieve the pointer-to-pointer for
	**/
	inline FUObjectItem const* GetObjectPtr(int32_t Index) const // TSAN_SAFE
	{
		const int32_t ChunkIndex = Index / NumElementsPerChunk;
		const int32_t WithinChunkIndex = Index % NumElementsPerChunk;
		// checkf(IsValidIndex(Index), TEXT("IsValidIndex(%d)"), Index);
		// checkf(ChunkIndex < NumChunks, TEXT("ChunkIndex (%d) < NumChunks (%d)"), ChunkIndex, NumChunks);
		// checkf(Index < MaxElements, TEXT("Index (%d) < MaxElements (%d)"), Index, MaxElements);
		FUObjectItem* Chunk = Objects[ChunkIndex];
		// check(Chunk);
		return Chunk + WithinChunkIndex;
	}
	inline FUObjectItem* GetObjectPtr(int32_t Index) // TSAN_SAFE
	{
		const int32_t ChunkIndex = Index / NumElementsPerChunk;
		const int32_t WithinChunkIndex = Index % NumElementsPerChunk;
		// checkf(IsValidIndex(Index), TEXT("IsValidIndex(%d)"), Index);
		// checkf(ChunkIndex < NumChunks, TEXT("ChunkIndex (%d) < NumChunks (%d)"), ChunkIndex, NumChunks);
		// checkf(Index < MaxElements, TEXT("Index (%d) < MaxElements (%d)"), Index, MaxElements);
		FUObjectItem* Chunk = Objects[ChunkIndex];
		// check(Chunk);
		return Chunk + WithinChunkIndex;
	}

	/**
	* Return a reference to an element
	* @param	Index	Index to return
	* @return	a reference to the pointer to the element
	* Thread safe, if it is valid now, it is valid forever. This might return nullptr, but by then, some other thread might have made it non-nullptr.
	**/
	inline FUObjectItem const& operator[](int32_t Index) const
	{
		FUObjectItem const* ItemPtr = GetObjectPtr(Index);
		// check(ItemPtr);
		return *ItemPtr;
	}
	inline FUObjectItem& operator[](int32_t Index)
	{
		FUObjectItem* ItemPtr = GetObjectPtr(Index);
		// check(ItemPtr);
		return *ItemPtr;
	}

	/*int32 AddRange(int32 NumToAdd) TSAN_SAFE
	{
		int32 Result = NumElements;
		checkf(Result + NumToAdd <= MaxElements, TEXT("Maximum number of UObjects (%d) exceeded, make sure you update MaxObjectsInGame/MaxObjectsInEditor in project settings."), MaxElements);
		ExpandChunksToIndex(Result + NumToAdd - 1);
		NumElements += NumToAdd;
		return Result;
	}

	int32 AddSingle() TSAN_SAFE
	{
		return AddRange(1);
	}*/

	/**
	* Return a naked pointer to the fundamental data structure for debug visualizers.
	**/
	/*FUObjectItem*** GetRootBlockForDebuggerVisualizers()
	{
		return nullptr;
	}*/
};
