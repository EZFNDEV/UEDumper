#pragma once
#include "../pch.h"

// Find string ref function by kem0x (and psychopast i think)

namespace Memory {
	namespace FortKit {
		enum ASM : DWORD
		{
			JMP_REL8 = 0xEB,
			CALL = 0xE8,
			LEA = 0x8D,
			CDQ = 0x99,
			CMOVL = 0x4C,
			CMOVS = 0x48,
			INT3 = 0xCC,
			RETN = 0xC3,
			SKIP
		};


		//wrote everything here, except the pattern scanner i stole it off guidedhacking
		static void* FindStringRef(const std::wstring& string)
		{
			uintptr_t base_address = reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr));

			const auto dosHeader = (PIMAGE_DOS_HEADER)base_address;
			const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)base_address + dosHeader->e_lfanew);

			IMAGE_SECTION_HEADER* textSection = nullptr;
			IMAGE_SECTION_HEADER* rdataSection = nullptr;

			auto sectionsSize = ntHeaders->FileHeader.NumberOfSections;
			auto section = IMAGE_FIRST_SECTION(ntHeaders);

			for (WORD i = 0; i < sectionsSize; section++)
			{
				auto secName = std::string((char*)section->Name);

				if (secName == ".text")
				{
					textSection = section;
				}
				else if (secName == ".rdata")
				{
					rdataSection = section;
				}

				if (textSection && rdataSection)
					break;
			}

			auto textStart = base_address + textSection->VirtualAddress;

			auto rdataStart = base_address + rdataSection->VirtualAddress;
			auto rdataEnd = rdataStart + rdataSection->Misc.VirtualSize;

			const auto scanBytes = reinterpret_cast<std::uint8_t*>(textStart);

			//scan only text section
			for (DWORD i = 0x0; i < textSection->Misc.VirtualSize; i++)
			{
				if (&scanBytes[i])
				{
					if ((scanBytes[i] == ASM::CMOVL || scanBytes[i] == ASM::CMOVS) && scanBytes[i + 1] == ASM::LEA)
					{
						auto stringAdd = reinterpret_cast<uintptr_t>(&scanBytes[i]);
						stringAdd = stringAdd + 7 + *reinterpret_cast<int32_t*>(stringAdd + 3);

						//check if the pointer is actually a valid string by checking if it's inside the rdata section
						if (stringAdd > rdataStart && stringAdd < rdataEnd)
						{
							std::wstring lea((wchar_t*)stringAdd);

							if (lea == string)
							{
								return &scanBytes[i];
							}
						}
					}
				}
			}

			return nullptr;
		}

		static void* FindByString(const std::wstring& string, std::vector<int> opcodesToFind = {}, bool bRelative = false, uint32_t offset = 0, bool forward = false)
		{
			auto ref = FindStringRef(string);

			if (ref)
			{
				//printf("Ref %ls %p\n", string.c_str(), ref);


				const auto scanBytes = static_cast<std::uint8_t*>(ref);

				//scan backwards till we hit a ret (and assume this is the function start)
				for (auto i = 0; forward ? (i < 2048) : (i > -2048); forward ? i++ : i--)
				{
					if (opcodesToFind.size() == 0)
					{
						if (scanBytes[i] == ASM::INT3 || scanBytes[i] == ASM::RETN)
						{
							return &scanBytes[i + 1];
						}
					}
					else
					{
						for (uint8_t byte : opcodesToFind)
						{
							if (scanBytes[i] == byte && byte != ASM::SKIP)
							{
								if (bRelative)
								{
									uintptr_t address = reinterpret_cast<uintptr_t>(&scanBytes[i]);
									address = ((address + offset + 4) + *(int32_t*)(address + offset));
									return (void*)address;
								}
								return &scanBytes[i];
							}
						}
					}
				}
			}

			return nullptr;
		}

		static uintptr_t FindPattern(const char* signature, bool bRelative = false, uint32_t offset = 0)
		{
			uintptr_t base_address = reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr));
			static auto patternToByte = [](const char* pattern)
			{
				auto bytes = std::vector<int>{};
				const auto start = const_cast<char*>(pattern);
				const auto end = const_cast<char*>(pattern) + strlen(pattern);

				for (auto current = start; current < end; ++current)
				{
					if (*current == '?')
					{
						++current;
						if (*current == '?')
							++current;
						bytes.push_back(-1);
					}
					else { bytes.push_back(strtoul(current, &current, 16)); }
				}
				return bytes;
			};

			const auto dosHeader = (PIMAGE_DOS_HEADER)base_address;
			const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)base_address + dosHeader->e_lfanew);

			const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
			auto patternBytes = patternToByte(signature);
			const auto scanBytes = reinterpret_cast<std::uint8_t*>(base_address);

			const auto s = patternBytes.size();
			const auto d = patternBytes.data();

			for (auto i = 0ul; i < sizeOfImage - s; ++i)
			{
				bool found = true;
				for (auto j = 0ul; j < s; ++j)
				{
					if (scanBytes[i + j] != d[j] && d[j] != -1)
					{
						found = false;
						break;
					}
				}

				if (found)
				{
					uintptr_t address = reinterpret_cast<uintptr_t>(&scanBytes[i]);
					if (bRelative)
					{
						address = ((address + offset + 4) + *(int32_t*)(address + offset));
						return address;
					}
					return address;
				}
			}

			return NULL;
		}
	}

	namespace Sexy {
		static uintptr_t PatternScanW(uintptr_t pModuleBaseAddress, const char* sSignature, size_t nSelectResultIndex)
		{
			static auto patternToByte = [](const char* pattern)
			{
				auto       bytes = std::vector<int>{};
				const auto start = const_cast<char*>(pattern);
				const auto end = const_cast<char*>(pattern) + strlen(pattern);

				for (auto current = start; current < end; ++current)
				{
					if (*current == '?')
					{
						++current;
						if (*current == '?')
							++current;
						bytes.push_back(-1);
					}
					else
						bytes.push_back(strtoul(current, &current, 16));
				}
				return bytes;
			};

			const auto dosHeader = (PIMAGE_DOS_HEADER)pModuleBaseAddress;
			const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)pModuleBaseAddress + dosHeader->e_lfanew);

			const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
			auto       patternBytes = patternToByte(sSignature);
			const auto scanBytes = reinterpret_cast<std::uint8_t*>(pModuleBaseAddress);

			const auto s = patternBytes.size();
			const auto d = patternBytes.data();

			size_t nFoundResults = 0;

			for (auto i = 0ul; i < sizeOfImage - s; ++i)
			{
				bool found = true;

				for (auto j = 0ul; j < s; ++j)
				{
					if (scanBytes[i + j] != d[j] && d[j] != -1)
					{
						found = false;
						break;
					}
				}

				if (found)
				{
					if (nSelectResultIndex != 0)
					{
						if (nFoundResults < nSelectResultIndex)
						{
							nFoundResults++;
							found = false;
						}
						else
							return reinterpret_cast<uintptr_t>(&scanBytes[i]);
					}
					else
						return reinterpret_cast<uintptr_t>(&scanBytes[i]);
				}
			}

			return NULL;
		}

		static uintptr_t PatternScan(const char* sSignature, uintptr_t start_at)
		{
			if (start_at != 0) {
				return PatternScanW((uintptr_t)start_at, sSignature, 0);
			}

			static bool bIsSetted = false;

			static MODULEINFO info = { 0 };

			if (!bIsSetted)
			{
				GetModuleInformation(GetCurrentProcess(), GetModuleHandle(0), &info, sizeof(info));
				bIsSetted = true;
			}

			return PatternScanW((uintptr_t)info.lpBaseOfDll, sSignature, 0);
		}
	}
}