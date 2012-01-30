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
#include "Wrappers_Include.h"

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
			, byteString const& pattern, std::vector<bool>& mask, Algorithm::IPatternAlgorithm& algorithm)
		{
			BYTE* begin = reinterpret_cast<BYTE*>(startAddress);
			BYTE* end = reinterpret_cast<BYTE*>(endAddress);
			return algorithm.Utilize(pattern, mask, begin, end);
		}


#pragma region Allocate
		UINT_PTR Memory::Allocate(HANDLE process, UINT_PTR address, UINT_PTR baseAddress, DWORD size, 
				AllocationType allocType, PageProtection protection)
		{
			UINT_PTR addr = address+baseAddress;
			return reinterpret_cast<UINT_PTR>(VirtualAllocEx(process, reinterpret_cast<LPVOID>(address), size, allocType, protection));
		}

		UINT_PTR Memory::Allocate(HANDLE process, UINT_PTR address, DWORD size, AllocationType allocType, PageProtection protection)
		{
			return Allocate(process, address, 0, size, allocType, protection);
		}

		UINT_PTR Memory::Allocate(HANDLE process, UINT_PTR address, DWORD size)
		{
			return Allocate(process, address, 0, size, AllocType_commit, PageProt_execute_readwrite);
		}
		
		UINT_PTR Memory::Allocate(Process& process, UINT_PTR address, UINT_PTR baseAddress, DWORD size, AllocationType allocType, PageProtection protection)
		{
			return Allocate(process.GetHandle(), address, baseAddress, size, allocType, protection);
		}

		UINT_PTR Memory::Allocate(Process& process, UINT_PTR address, DWORD size, AllocationType allocType, PageProtection protection)
		{
			return Allocate(process.GetHandle(), address, 0, size, allocType, protection);
		}

		UINT_PTR Memory::Allocate(Process& process, UINT_PTR address, DWORD size)
		{
			return Allocate(process.GetHandle(), address, 0, size, AllocType_commit, PageProt_execute_readwrite);
		}
#pragma endregion

#pragma region FreeMemory
		void Memory::FreeMemory(HANDLE process, UINT_PTR address, UINT_PTR baseAddress, DWORD size, FreeType freeType)
		{
			W32_CALL( VirtualFreeEx(process, reinterpret_cast<LPVOID>(address + baseAddress), size, freeType) );
		}

		void Memory::FreeMemory(HANDLE process, UINT_PTR address, DWORD size)
		{
			MAGIC_CALL( FreeMemory(process, address, 0, size, FreeType_release) );
		}
		
		void Memory::FreeMemory(HANDLE process, UINT_PTR address, DWORD size, FreeType freeType)
		{
			MAGIC_CALL( FreeMemory(process, address, 0, size, freeType) );
		}
		
		void Memory::FreeMemory(Process& process, UINT_PTR address, UINT_PTR baseAddress, DWORD size, FreeType freeType)
		{
			W32_CALL( VirtualFreeEx(process.GetHandle(), reinterpret_cast<LPVOID>(address + baseAddress), size, freeType) );
		}

		void Memory::FreeMemory(Process& process, UINT_PTR address, DWORD size, FreeType freeType)
		{
			W32_CALL( VirtualFreeEx(process.GetHandle(), reinterpret_cast<LPVOID>(address), size, freeType) );
		}

		void Memory::FreeMemory(Process& process, UINT_PTR address)
		{
			W32_CALL( VirtualFreeEx(process.GetHandle(), reinterpret_cast<LPVOID>(address), 0, FreeType_release) );
		}

#pragma endregion

#pragma region Read
		std::string Memory::ReadString(HANDLE process, UINT_PTR address, UINT_PTR baseAddress)
		{
			CHAR buffer[513] = {};
			std::stringstream sstream;

			UINT_PTR addr = address+baseAddress;

			do
			{
				W32_CALL(ReadProcessMemory(process, reinterpret_cast<LPCVOID>(addr), buffer, 512, nullptr));
				addr += 512;
				sstream << buffer;
			} while(strlen(buffer) == 512);

			return sstream.str();
		}

		std::string Memory::ReadString(HANDLE process, UINT_PTR address)
		{
			MAGIC_CALL( return ReadString(process, address, 0); )
		}
		

		std::string Memory::ReadString(Process& process, UINT_PTR address, UINT_PTR baseAddress)
		{
			CHAR buffer[513] = {};
			std::stringstream sstream;

			UINT_PTR addr = address+baseAddress;

			do
			{
				W32_CALL(ReadProcessMemory(process.GetHandle(), reinterpret_cast<LPCVOID>(addr), buffer, 512, nullptr));
				addr += 512;
				sstream << buffer;
			} while(strlen(buffer) == 512);

			return sstream.str();
		}

		std::string Memory::ReadString(Process& process, UINT_PTR address)
		{
			MAGIC_CALL( return ReadString(process, address, 0); )
		}
#pragma endregion

#pragma region Write

		DWORD Memory::WriteString(HANDLE process, UINT_PTR address, std::string const& value, UINT_PTR baseAddress)
		{
			SIZE_T numWritten = 0;
			UINT_PTR addr = address+baseAddress;
			W32_CALL(WriteProcessMemory(process, reinterpret_cast<LPVOID>(addr), value.c_str(), value.length() + 1, &numWritten));
			return numWritten;
		}

		DWORD Memory::WriteString(HANDLE process, UINT_PTR address, std::string const& value)
		{
			MAGIC_CALL( return WriteString(process, address, value, 0); )
		}

		
		DWORD Memory::WriteString(Process& process, UINT_PTR address, std::string const& value, UINT_PTR baseAddress)
		{
			SIZE_T numWritten = 0;
			UINT_PTR addr = address+baseAddress;
			W32_CALL(WriteProcessMemory(process.GetHandle(), reinterpret_cast<LPVOID>(addr), value.c_str(), value.length() + 1, &numWritten));
			return numWritten;
		}

		DWORD Memory::WriteString(Process& process, UINT_PTR address, std::string const& value)
		{
			MAGIC_CALL( return WriteString(process, address, value, 0); )
		}
#pragma endregion
	}
}