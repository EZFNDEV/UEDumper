#pragma once

// template <typename KeyType, typename ValueType>
// using TPair = TTuple<KeyType, ValueType>; // THIS IS PROPER

template <typename KeyType, typename ValueType>
class TPair
{
public:
	KeyType Key;
	ValueType Value;
};