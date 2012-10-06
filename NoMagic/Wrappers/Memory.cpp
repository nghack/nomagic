/*	Copyright (C) 2012 Dennis Garske (a.k.a. Nightblizard)
	<http://nightblizard.blogspot.com> <nightblizard@gmail.com>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.


	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.


	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "../Wrappers_Include.h"

namespace NoMagic
{
	namespace Wrappers
	{
		DWORD Memory::Protect(UINT_PTR address, DWORD numBytes, DWORD newProtect)
		{
			DWORD oldProtect = 0;
			W32_CALL(VirtualProtect(reinterpret_cast<LPVOID>(address), numBytes, newProtect, &oldProtect));
			return oldProtect;
		}

		UINT_PTR Memory::FindPattern(UINT_PTR startAddress, UINT_PTR endAddress
			, byteString const& pattern, std::vector<bool> const& mask, Algorithm::IPatternAlgorithm& algorithm)
		{
			auto begin = reinterpret_cast<BYTE*>(startAddress);
			auto end = reinterpret_cast<BYTE*>(endAddress);
			return algorithm.Utilize(pattern, mask, begin, end);
		}

		UINT_PTR Memory::QuickSearch(byteString const& pattern, std::vector<bool> const& mask, Algorithm::IPatternAlgorithm& algorithm)
		{
			try
			{
				Process proc = Process::GetCurrentProcess();
				auto modules = Module::GetModules(proc);
				for(auto module = std::begin(modules); module != std::end(modules); ++module)
				{
					(*module).ReadPEHeader();
					auto sections = (*module).GetSections();
					UINT_PTR addr = 0;

					std::for_each(std::begin(sections), std::end(sections), [&](IMAGE_SECTION_HEADER const& section)
					{
						if((section.Characteristics & IMAGE_SCN_CNT_CODE) != 0)
						{
							auto begin = reinterpret_cast<BYTE*>((*module).GetSectionAddress(section));
							auto end = begin + section.Misc.VirtualSize;
							addr = algorithm.Utilize(pattern, mask, begin, end);
							if(addr != 0)
							{
								addr += reinterpret_cast<UINT_PTR>(begin);
								return;
							}
						}
					});

					return addr;
				}

				return 0;
			}
			catch(...)
			{
				throw;
			}
		}
		
		const PBYTE Memory::DetourFunction(const PBYTE targetFunction, const PBYTE newFunction)
		{
			#ifdef _M_AMD64
			throw MagicException(_T("This features is not supported in x64!"));
			#else
			return ::DetourFunction(targetFunction, newFunction);
			#endif
		}

		BOOL Memory::RemoveDetour(const PBYTE origFunction, const PBYTE yourFunction)
		{
			#ifdef _M_AMD64
			throw MagicException(_T("This features is not supported in x64!"));
			#else
			return ::DetourRemove(origFunction, yourFunction);
			#endif
		}

		const PBYTE Memory::DetourIAT(std::string const& functionName, const PBYTE newFunction, Module const& module)
		{
			auto ntHeaders = module.GetNTHeaders();
			auto baseAddress = module.GetBaseAddress();
	
			auto dataDir = ntHeaders.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
			auto importTable = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(dataDir.VirtualAddress + baseAddress);
		
			for(; importTable->Characteristics != 0; ++importTable)
			{
				if(importTable->FirstThunk == 0 || importTable->OriginalFirstThunk == 0) //This would be pretty weird! should it throw an exception?
					continue;

				PIMAGE_THUNK_DATA thunk = (PIMAGE_THUNK_DATA)(importTable->FirstThunk + baseAddress);
				PIMAGE_THUNK_DATA origThunk = (PIMAGE_THUNK_DATA)(importTable->OriginalFirstThunk + baseAddress);

				for(; origThunk->u1.Function != 0; ++origThunk, ++thunk)
				{
					if(origThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)
						continue;

					auto import = (PIMAGE_IMPORT_BY_NAME)(origThunk->u1.AddressOfData + baseAddress);

					if(strcmp(functionName.c_str(), (char*)import->Name))
						continue;

					MEMORY_BASIC_INFORMATION thunkMemInfo;
					VirtualQuery(thunk, &thunkMemInfo, sizeof(thunkMemInfo));

					W32_CALL( VirtualProtect(thunkMemInfo.BaseAddress, thunkMemInfo.RegionSize, PAGE_EXECUTE_READWRITE, &thunkMemInfo.Protect) );

					UINT_PTR oldFunction = thunk->u1.Function;
					thunk->u1.Function = (UINT_PTR)newFunction;
				
					DWORD junk = 0;
					W32_CALL( VirtualProtect(thunkMemInfo.BaseAddress, thunkMemInfo.RegionSize, thunkMemInfo.Protect, &junk) );
						
					return (PBYTE)oldFunction;
				}
			}

			return nullptr; //throw?
		}

		const PBYTE Memory::DetourEAT(std::string const& functionName, const PBYTE newFunction, Module const& module)
		{
			auto ntHeaders = module.GetNTHeaders();
			auto baseAddress = module.GetBaseAddress();
	
			auto dataDir = ntHeaders.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
			auto exportTable = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(dataDir.VirtualAddress + baseAddress);

			auto functions = exportTable->NumberOfFunctions;
			auto names = exportTable->NumberOfNames;

			if(names < 1 || functions < 1)
				return nullptr;

			DWORD* functionAddresses = (DWORD*)(exportTable->AddressOfFunctions + baseAddress);
			DWORD* nameAddresses = (DWORD*)(exportTable->AddressOfNames + baseAddress);

			for(unsigned int i = 0; i < functions; ++i, ++functionAddresses, ++nameAddresses)
			{
				auto name = (char*)(*nameAddresses + baseAddress);

				if(strcmp(name, functionName.c_str()))
					continue;

				UINT_PTR function = (UINT_PTR)(*functionAddresses + baseAddress);
				
				UINT_PTR ThatFunction = (UINT_PTR)newFunction;
				
				unsigned int Diff = (unsigned int)(ThatFunction - function - 5);

				auto old = NoMagic::Wrappers::Memory::Protect(function, 5, PAGE_EXECUTE_READWRITE);
				PBYTE oldFunc = (PBYTE)(*((unsigned int*)(function+1)));
				*((unsigned int*)(function+1)) = Diff;
				NoMagic::Wrappers::Memory::Protect(function, 5, old);

				return oldFunc;
			}
			return nullptr;
		}

		const bool Memory::RemoveEATDetour(std::string const& functionName, const PBYTE origFunction, Module const& module)
		{
			auto ntHeaders = module.GetNTHeaders();
			auto baseAddress = module.GetBaseAddress();
	
			auto dataDir = ntHeaders.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
			auto exportTable = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(dataDir.VirtualAddress + baseAddress);

			auto functions = exportTable->NumberOfFunctions;
			auto names = exportTable->NumberOfNames;

			if(names < 1 || functions < 1)
				return false;

			DWORD* functionAddresses = (DWORD*)(exportTable->AddressOfFunctions + baseAddress);
			DWORD* nameAddresses = (DWORD*)(exportTable->AddressOfNames + baseAddress);

			for(unsigned int i = 0; i < functions; ++i, ++functionAddresses, ++nameAddresses)
			{
				auto name = (char*)(*nameAddresses + baseAddress);

				if(strcmp(name, functionName.c_str()))
					continue;
			
				UINT_PTR function = (UINT_PTR)(*functionAddresses + baseAddress);

				auto old = NoMagic::Wrappers::Memory::Protect(function, 5, PAGE_EXECUTE_READWRITE);
				*((unsigned int*)(function+1)) = (unsigned int)(origFunction);
				NoMagic::Wrappers::Memory::Protect(function, 5, old);

				return true;
			}
			return false;
		}

#pragma region Allocate
		UINT_PTR Memory::Allocate(const HANDLE process, UINT_PTR address, UINT_PTR baseAddress, SIZE_T size, 
				AllocationType allocType, PageProtection protection)
		{
			auto addr = address+baseAddress;
			return reinterpret_cast<UINT_PTR>(VirtualAllocEx(process, reinterpret_cast<LPVOID>(address), size, allocType, protection));
		}

		UINT_PTR Memory::Allocate(const HANDLE process, UINT_PTR address, SIZE_T size, AllocationType allocType, PageProtection protection)
		{
			return Allocate(process, address, 0, size, allocType, protection);
		}

		UINT_PTR Memory::Allocate(const HANDLE process, UINT_PTR address, SIZE_T size)
		{
			return Allocate(process, address, 0, size, AllocType_commit, PageProt_execute_readwrite);
		}
		
		UINT_PTR Memory::Allocate(const Process& process, UINT_PTR address, UINT_PTR baseAddress, SIZE_T size, AllocationType allocType, PageProtection protection)
		{
			return Allocate(process.GetHandle(), address, baseAddress, size, allocType, protection);
		}

		UINT_PTR Memory::Allocate(const Process& process, UINT_PTR address, SIZE_T size, AllocationType allocType, PageProtection protection)
		{
			return Allocate(process.GetHandle(), address, 0, size, allocType, protection);
		}

		UINT_PTR Memory::Allocate(const Process& process, UINT_PTR address, SIZE_T size)
		{
			return Allocate(process.GetHandle(), address, 0, size, AllocType_commit, PageProt_execute_readwrite);
		}
#pragma endregion

#pragma region FreeMemory
		void Memory::FreeMemory(const HANDLE process, UINT_PTR address, UINT_PTR baseAddress, DWORD size, FreeType freeType)
		{
			W32_CALL( VirtualFreeEx(process, reinterpret_cast<LPVOID>(address + baseAddress), size, freeType) );
		}

		void Memory::FreeMemory(const HANDLE process, UINT_PTR address, DWORD size)
		{
			MAGIC_CALL( FreeMemory(process, address, 0, size, FreeType_release) );
		}
		
		void Memory::FreeMemory(const HANDLE process, UINT_PTR address, DWORD size, FreeType freeType)
		{
			MAGIC_CALL( FreeMemory(process, address, 0, size, freeType) );
		}
		
		void Memory::FreeMemory(const Process& process, UINT_PTR address, UINT_PTR baseAddress, DWORD size, FreeType freeType)
		{
			W32_CALL( VirtualFreeEx(process.GetHandle(), reinterpret_cast<LPVOID>(address + baseAddress), size, freeType) );
		}

		void Memory::FreeMemory(const Process& process, UINT_PTR address, DWORD size, FreeType freeType)
		{
			W32_CALL( VirtualFreeEx(process.GetHandle(), reinterpret_cast<LPVOID>(address), size, freeType) );
		}

		void Memory::FreeMemory(const Process& process, UINT_PTR address)
		{
			W32_CALL( VirtualFreeEx(process.GetHandle(), reinterpret_cast<LPVOID>(address), 0, FreeType_release) );
		}

#pragma endregion

#pragma region Read
		tstring Memory::ReadString(const HANDLE process, UINT_PTR address, UINT_PTR baseAddress)
		{
			TCHAR buffer[513] = {};
			std::basic_stringstream<TCHAR> sstream;

			UINT_PTR addr = address+baseAddress;

			do
			{
				W32_CALL(ReadProcessMemory(process, reinterpret_cast<LPCVOID>(addr), buffer, 512, nullptr));
				addr += 512;
				sstream << buffer;
			} while(_tcslen(buffer) == 512);

			return sstream.str();
		}

		tstring Memory::ReadString(const HANDLE process, UINT_PTR address)
		{
			MAGIC_CALL( return ReadString(process, address, 0); )
		}
		

		tstring Memory::ReadString(const Process& process, UINT_PTR address, UINT_PTR baseAddress)
		{
			TCHAR buffer[513] = {};
			std::basic_stringstream<TCHAR> sstream;

			UINT_PTR addr = address+baseAddress;

			do
			{
				W32_CALL(ReadProcessMemory(process.GetHandle(), reinterpret_cast<LPCVOID>(addr), buffer, 512, nullptr));
				addr += 512;
				sstream << buffer;
			} while(_tcslen(buffer) == 512);

			return sstream.str();
		}

		tstring Memory::ReadString(const Process& process, UINT_PTR address)
		{
			MAGIC_CALL( return ReadString(process, address, 0); )
		}
#pragma endregion

#pragma region Write

		SIZE_T Memory::WriteString(const HANDLE process, UINT_PTR address, tstring const& value, UINT_PTR baseAddress)
		{
			SIZE_T numWritten = 0;
			auto addr = address+baseAddress;
			W32_CALL(WriteProcessMemory(process, reinterpret_cast<LPVOID>(addr), value.c_str(), (value.length() + 1) * sizeof(TCHAR), &numWritten));
			return numWritten;
		}

		SIZE_T Memory::WriteString(const HANDLE process, UINT_PTR address, tstring const& value)
		{
			MAGIC_CALL( return WriteString(process, address, value, 0); )
		}

		
		SIZE_T Memory::WriteString(const Process& process, UINT_PTR address, tstring const& value, UINT_PTR baseAddress)
		{
			SIZE_T numWritten = 0;
			auto addr = address+baseAddress;
			W32_CALL(WriteProcessMemory(process.GetHandle(), reinterpret_cast<LPVOID>(addr), value.c_str(), (value.length() + 1) * sizeof(TCHAR), &numWritten));
			return numWritten;
		}

		SIZE_T Memory::WriteString(const Process& process, UINT_PTR address, tstring const& value)
		{
			MAGIC_CALL( return WriteString(process, address, value, 0); )
		}
#pragma endregion
	}
}