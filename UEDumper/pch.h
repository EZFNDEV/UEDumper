// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"

#include "buildSettings.h"

#include <cstdint>
#include <string>
#include <vector>
#include <locale>
#include <string>
#include <chrono>
#include <fstream>
#include <format>
#include <filesystem>

namespace fs = std::filesystem;

struct Stream
{
    std::string FileName; // Bro stream doesnt save this
    std::ofstream stream;

	Stream(const std::string& fileName) { Open(fileName); }

	Stream() {}

	~Stream() { Close(); }

	void Write(const std::string& str)
	{
		stream << str;
	}

	void Open(const std::string& fileName)
	{
		FileName = fileName;
		stream.open(fileName);
	}

	bool IsOpen()
	{
		return stream.is_open();
	}

	void Close()
	{
		if (IsOpen())
			stream.close();
	}
};

/*struct Ofstreams {
    Stream Classes;
    Stream Functions;
    Stream Structs;
    // std::ofstream Parameters;

	Ofstreams() {}
};*/

struct Ofstreams {
	std::ofstream Classes;
	std::ofstream Functions;
	std::ofstream Structs;
	// std::ofstream Parameters;
};

#include "include/memory.h"
#include "offsets.h"
#include "utils.h"

#include "CoreUObject/UObject/Class.h"

#endif //PCH_H
