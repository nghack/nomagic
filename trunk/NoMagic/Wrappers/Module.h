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
		class Module
		{
		private:
			HMODULE m_module;
			DWORD m_size;
			tstring m_name;
			tstring m_path;
			UINT_PTR m_baseAddress;

			IMAGE_DOS_HEADER* m_dosHeader;
			IMAGE_NT_HEADERS* m_ntHeaders;
			IMAGE_SECTION_HEADER* m_sections;
		public:
			Module();
			Module(MODULEENTRY32 const& moduleEntry);
			Module(Process const& process, const HMODULE module);
			~Module(void);

			static std::vector<Module> GetModules(Process const& proc);

			static Module FromLibrary(Process const& process, tstring const& libPath);

			UINT_PTR GetProcAddress(std::string const& name) const;

			const HMODULE GetHandle() const;
			DWORD GetSize() const;
			tstring const& GetName() const;
			tstring const& GetPath() const;
			UINT_PTR GetBaseAddress() const;

			void ReadPEHeader();

			IMAGE_DOS_HEADER GetDOSHeader() const;
			IMAGE_NT_HEADERS GetNTHeaders() const;
			std::vector<IMAGE_SECTION_HEADER> GetSections() const;

			UINT_PTR GetSectionAddress(IMAGE_SECTION_HEADER const& section) const;

			template<typename T>
			T IterateSections(T function) const
			{
				auto sections = GetSections();
				return std::for_each(std::begin(sections), std::end(sections), function);
			}
		};
	}
}