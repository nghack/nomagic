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
		Module::Module() : 
			m_module(nullptr),
			m_size(0),
			m_name(),
			m_path(),
			m_baseAddress(),
			m_dosHeader(nullptr),
			m_ntHeaders(nullptr),
			m_sections(nullptr)
		{
		}

		Module::Module(MODULEENTRY32 const& moduleEntry) : 
			m_module(moduleEntry.hModule),
			m_size(moduleEntry.modBaseSize),
			m_name(moduleEntry.szModule),
			m_path(moduleEntry.szExePath),
			m_baseAddress(reinterpret_cast<UINT_PTR>(moduleEntry.modBaseAddr)),
			m_dosHeader(nullptr),
			m_ntHeaders(nullptr),
			m_sections(nullptr)
		{
		}

		Module::Module(Process const& process, const HMODULE module) : 
			m_module(module),
			m_size(0),
			m_name(),
			m_path(),
			m_baseAddress(),
			m_dosHeader(nullptr),
			m_ntHeaders(nullptr),
			m_sections(nullptr)
		{
			MODULEENTRY32 modEntry;
			modEntry.dwSize = sizeof(MODULEENTRY32);
			
			auto findModule = [&]() -> bool
			{
				auto hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process.GetId());
				W32_CALL( Module32First(hSnap, &modEntry) );

				do 
				{
					if(modEntry.hModule == module)
					{
						m_size = modEntry.modBaseSize;
						m_name = modEntry.szModule;
						m_path = modEntry.szExePath;
						m_baseAddress = reinterpret_cast<UINT_PTR>(modEntry.modBaseAddr);
						CloseHandle(hSnap);
						return true;
					}
				} while (Module32Next(hSnap, &modEntry));
				
				CloseHandle(hSnap);
				return false;
			};

			if(findModule() == false)
				throw MagicException(_T("Can not find module!"));
		}

		Module::~Module(void)
		{
		}

		std::vector<Module> Module::GetModules(const Process& process)
		{
			std::vector<Module> modules;
			MODULEENTRY32 modEntry;
			modEntry.dwSize = sizeof(MODULEENTRY32);
			
			auto hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process.GetId());
			W32_CALL( Module32First(hSnap, &modEntry) );
			do 
			{
				if(modEntry.th32ProcessID == process.GetId())
				{
					modules.push_back(Module(modEntry));
				}
			} while (Module32Next(hSnap, &modEntry));
			
			CloseHandle(hSnap);

			return modules;
		}

		Module Module::FromLibrary(Process const& process, tstring const& libPath)
		{
			auto hmod = LoadLibrary(libPath.c_str());
			if(hmod == nullptr)
				throw MagicException(_T("LoadLibrary failed!"), GetLastError());

			return Module(process, hmod);
		}

		void Module::ReadPEHeader()
		{
			m_dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(m_baseAddress);
			m_ntHeaders =  reinterpret_cast<PIMAGE_NT_HEADERS>(m_baseAddress + m_dosHeader->e_lfanew);
			m_sections = IMAGE_FIRST_SECTION(m_ntHeaders);
		}

		UINT_PTR Module::GetProcAddress(std::string const& name) const
		{
			memcpy(0, 0, 0);
			auto proc = reinterpret_cast<UINT_PTR>(::GetProcAddress(m_module, name.c_str()));
			if(proc == 0)
				throw MagicException(_T("GetProcAddress failed!"), GetLastError());

			return proc;
		}
		
		const HMODULE Module::GetHandle() const
		{
			return m_module;
		}

		DWORD Module::GetSize() const
		{
			return m_size;
		}

		tstring const& Module::GetName() const
		{
			return m_name;
		}

		tstring const& Module::GetPath() const
		{
			return m_path;
		}

		UINT_PTR Module::GetBaseAddress() const
		{
			return m_baseAddress;
		}

		IMAGE_DOS_HEADER Module::GetDOSHeader() const
		{
			if(m_dosHeader == nullptr)
				throw MagicException(_T("dos header == nullptr"));
			return *m_dosHeader;
		}

		IMAGE_NT_HEADERS Module::GetNTHeaders() const
		{
			if(m_ntHeaders == nullptr)
				throw MagicException(_T("dos header == nullptr"));
			return *m_ntHeaders;
		}
		
		std::vector<IMAGE_SECTION_HEADER> Module::GetSections() const
		{
			if(m_sections == nullptr)
				throw MagicException(_T("dos header == nullptr"));

			std::vector<IMAGE_SECTION_HEADER> sections(m_ntHeaders->FileHeader.NumberOfSections);

			for(int i = 0; i < m_ntHeaders->FileHeader.NumberOfSections; ++i)
				sections[i] = m_sections[i];

			return sections;
		}

		UINT_PTR Module::GetSectionAddress(IMAGE_SECTION_HEADER const& section) const
		{
			return	section.VirtualAddress + m_baseAddress;
		}
	}
}