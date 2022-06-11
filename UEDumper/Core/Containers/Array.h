#pragma once

/**
 * Templated dynamic array
 *
 * A dynamically sized array of typed elements.  Makes the assumption that your elements are relocate-able;
 * i.e. that they can be transparently moved to new memory without a copy constructor.  The main implication
 * is that pointers to elements in the TArray may be invalidated by adding or removing other elements to the array.
 * Removal of elements is O(N) and invalidates the indices of subsequent elements.
 *
 * Caution: as noted below some methods are not safe for element types that require constructors.
 *
 **/

// TODO: Well nothing, fuck it...
template<class T>
struct TArray
{
    friend struct FString;

    public:
        inline TArray()
        {
            Data = nullptr;
            Count = Max = 0;
        };

        inline int Num() const
        {
            return Count;
        };

        inline T& operator[](int i)
        {
            return Data[i];
        };

        inline const T& operator[](int i) const
        {
            return Data[i];
        };

        inline bool IsValidIndex(int i) const
        {
            return i < Num();
        }

    public:
        T* Data;
        uint32_t Count;
        uint32_t Max;
};