#pragma once
#include "../Containers/UnrealString.h"

/** Maximum size of name, including the null terminator. */
enum { NAME_SIZE = 1024 };

/** Opaque id to a deduplicated name */
struct FNameEntryId
{
	FNameEntryId() : Value(0) {}
public: // We make it public
	uint32_t Value;
};

/**
 * Public name, available to the world.  Names are stored as a combination of
 * an index into a table of unique strings and an instance number.
 * Names are case-insensitive, but case-preserving (when WITH_CASE_PRESERVING_NAME is 1)
 */
class FName {
	public:
		FNameEntryId GetComparisonIndex() {
			return *(FNameEntryId*)((__int64)this + Offsets::FName::ComparisonIndex);
		};

		FNameEntryId GetDisplayIndex() {
			// Wrong actually
			return *(FNameEntryId*)((__int64)this + Offsets::FName::DisplayIndex);
		};

		int32_t GetNumber() {
			return *(int32_t*)((__int64)this + Offsets::FName::Number);
		}

		void SetNumber(int32_t NewNumber) {
			*(int32_t*)((__int64)this + Offsets::FName::Number) = NewNumber;
		}

		/**
		 * Converts an FName to a readable format
		 *
		 * @return String representation of the name
		*/
		FString ToString() const;

		/**
		 * Buffer size required for any null-terminated FName string, i.e. [name] '_' [digits] '\0'
		 */
		static constexpr uint32_t StringBufferSize = NAME_SIZE + 1 + 10; // NAME_SIZE includes null-terminator

		int pad1;
		int pad2;
};