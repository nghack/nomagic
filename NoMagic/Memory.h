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
#pragma once

namespace NoMagic
{
	namespace Wrappers
	{
		enum AllocationType : DWORD
		{
			AllocType_commit = MEM_COMMIT,
			AllocType_reserve = MEM_RESERVE,
			AllocType_reset = MEM_RESET,

			//Flags:
			AllocType_large_pages = MEM_LARGE_PAGES,
			AllocType_physical = MEM_PHYSICAL,
			AllocType_top_down = MEM_TOP_DOWN
		};

		enum FreeType : DWORD
		{
			FreeType_decommit = MEM_DECOMMIT,
			FreeType_release = MEM_RELEASE
		};

		enum PageProtection : DWORD
		{
			PageProt_execute = PAGE_EXECUTE,
			PageProt_execute_read = PAGE_EXECUTE_READ,
			PageProt_execute_readwrite = PAGE_EXECUTE_READWRITE,
			PageProt_execute_writecopy = PAGE_EXECUTE_WRITECOPY,
			PageProt_noaccess = PAGE_NOACCESS,
			PageProt_readonly = PAGE_READONLY,
			PageProt_readwrite = PAGE_READWRITE,
			PageProt_writecopy = PAGE_WRITECOPY,

			//Flags:
			PageProt_guard = PAGE_GUARD,
			PageProt_nocache = PAGE_NOCACHE,
			PageProt_writecombine = PAGE_WRITECOMBINE
		};

		class Memory
		{
		public:
			static DWORD Protect(UINT_PTR address, DWORD numBytes, DWORD newProtect);

			static UINT_PTR FindPattern(UINT_PTR startAddress, UINT_PTR endAddress
				, byteString const& pattern, std::vector<bool> const& mask, Algorithm::IPatternAlgorithm& algorithm);

			static PBYTE DetourFunction(const PBYTE targetFunction, const PBYTE newFunction);
			static BOOL RemoveDetour(const PBYTE origFunction, const PBYTE yourFunction);
			
			#pragma region Allocate
			static UINT_PTR Allocate(const HANDLE process, UINT_PTR address, UINT_PTR baseAddress, DWORD size, AllocationType allocType, PageProtection protection);
			static UINT_PTR Allocate(const HANDLE process, UINT_PTR address, DWORD size, AllocationType allocType, PageProtection protection);
			static UINT_PTR Allocate(const HANDLE process, UINT_PTR address, DWORD size);

			static UINT_PTR Allocate(const Process& process, UINT_PTR address, UINT_PTR baseAddress, DWORD size, AllocationType allocType, PageProtection protection);
			static UINT_PTR Allocate(const Process& process, UINT_PTR address, DWORD size, AllocationType allocType, PageProtection protection);
			static UINT_PTR Allocate(const Process& process, UINT_PTR address, DWORD size);
			#pragma endregion

			#pragma region FreeMemory
			static void FreeMemory(const HANDLE process, UINT_PTR address, UINT_PTR baseAddress, DWORD size, FreeType freeType);
			static void FreeMemory(const HANDLE process, UINT_PTR address, DWORD size, FreeType freeType);
			static void FreeMemory(const HANDLE process, UINT_PTR address, DWORD size);

			static void FreeMemory(const Process& process, UINT_PTR address, UINT_PTR baseAddress, DWORD size, FreeType freeType);
			static void FreeMemory(const Process& process, UINT_PTR address, DWORD size, FreeType freeType);
			static void FreeMemory(const Process& process, UINT_PTR address);
			#pragma endregion

			#pragma region ReadMethods
			static std::string ReadString(const HANDLE process, UINT_PTR address, UINT_PTR baseAddress);
			static std::string ReadString(const HANDLE process, UINT_PTR address);

			static std::string ReadString(const Process& process, UINT_PTR address, UINT_PTR baseAddress);
			static std::string ReadString(const Process& process, UINT_PTR address);
			
			template <typename type>
			static type Read(const HANDLE process, UINT_PTR address, UINT_PTR baseAddress)
			{
				type buffer;
				UINT_PTR addr = address+baseAddress;
				W32_CALL(ReadProcessMemory(process, reinterpret_cast<LPCVOID>(addr), &buffer, sizeof(buffer), nullptr));

				return buffer;
			}
			
			template <typename type>
			static type Read(const HANDLE process, UINT_PTR address)
			{
				MAGIC_CALL( return Read<type>(process, address, 0) );
			}
			
			template <typename type>
			static type Read(const Process& process, UINT_PTR address, UINT_PTR baseAddress)
			{
				type buffer;
				UINT_PTR addr = address+baseAddress;
				W32_CALL(ReadProcessMemory(process.GetHandle(), reinterpret_cast<LPCVOID>(addr), &buffer, sizeof(buffer), nullptr));

				return buffer;
			}
			
			template <typename type>
			static type Read(const Process& process, UINT_PTR address)
			{
				MAGIC_CALL( return Read<type>(process, address, 0) );
			}
			#pragma endregion

			#pragma region WriteMethods
			static DWORD WriteString(const HANDLE process, UINT_PTR address, std::string const& value, UINT_PTR baseAddress);
			static DWORD WriteString(const HANDLE process, UINT_PTR address, std::string const& value);

			static DWORD WriteString(const Process& process, UINT_PTR address, std::string const& value, UINT_PTR baseAddress);
			static DWORD WriteString(const Process& processs, UINT_PTR address, std::string const& value);

			
			template <typename type>
			static SIZE_T Write(const HANDLE process, UINT_PTR address, type const value, UINT_PTR baseAddress)
			{
				SIZE_T numWritten = 0;
				UINT_PTR addr = address+baseAddress;
				W32_CALL(WriteProcessMemory(process, reinterpret_cast<LPVOID>(addr), &value, sizeof(value), &numWritten));

				return numWritten;
			}

			template <typename type>
			static SIZE_T Write(const Process& process, UINT_PTR address, type const value, UINT_PTR baseAddress)
			{
				SIZE_T numWritten = 0;
				UINT_PTR addr = address+baseAddress;
				W32_CALL(WriteProcessMemory(process.GetHandle(), reinterpret_cast<LPVOID>(addr), &value, sizeof(value), &numWritten));

				return numWritten;
			}

			template <typename type>
			static SIZE_T Write(const HANDLE process, UINT_PTR address, type const value)
			{
				MAGIC_CALL( return Write(process, address, value, 0) );
			}

			template <typename type>
			static SIZE_T Write(const Process& process, UINT_PTR address, type const value)
			{
				MAGIC_CALL( return Write(process, address, value, 0) );
			}
			#pragma endregion
		};
	}
}