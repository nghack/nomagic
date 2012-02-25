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
	namespace fasm
	{
		struct FASM_STATE
		{
			DWORD condition;

			union
			{
				DWORD output_length;
				DWORD error_code;
			};

			union
			{
				const char* output_data;
				const char* error_line;
			};
		};

		struct fasm_read_buffer
		{
			FASM_STATE state;
			PBYTE buffer[4096];
		};

		class Assembler
		{
		private:
			typedef int (__stdcall* tfasm_assemble)(const char* lpSource, void* lpMemory, DWORD size, DWORD nPasses, void* displayPipe);
			tfasm_assemble fasm_assemble;

			DWORD version;
			HMODULE fasmLib;

			char* toAsm(const char* source, int& outLen) const;
		public:
			Assembler();
			~Assembler();

			std::vector<BYTE> Assemble(const std::string& instructions) const;

			DWORD GetVersion() const;
		};
	}
}