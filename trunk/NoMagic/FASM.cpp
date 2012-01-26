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

#include "NoMagic_Include.h"

namespace NoMagic
{
	namespace fasm
	{
		Assembler::Assembler()
		{
			this->fasmLib = LoadLibrary("FASM.DLL");
			if(this->fasmLib == nullptr)
				throw MagicException("Can not load FASM.DLL!", GetLastError());

			this->fasm_assemble = reinterpret_cast<tfasm_assemble>(GetProcAddress(this->fasmLib, _T("fasm_Assemble")));
			if(this->fasm_assemble == nullptr)
				throw MagicException("GetProcAddress(\"fasm_Assemble\") failed!", GetLastError());

			auto getVersion = GetProcAddress(this->fasmLib, _T("fasm_GetVersion"));
			if(getVersion == nullptr)
				throw MagicException("GetProcAddress(\"fasm_Assemble\") failed!", GetLastError());

			this->version = getVersion();
		}

		Assembler::~Assembler()
		{
			FreeLibrary(this->fasmLib);
		}

		char* Assembler::toAsm(const char* source, int& outLen) const
		{
			fasm_read_buffer buffer = {};
			int fasm_returnValue = this->fasm_assemble(source, &buffer, sizeof(buffer), 100, nullptr);
		
			if(fasm_returnValue != 0)
				throw MagicException("fasm_assemble failed!", fasm_returnValue);
			
			outLen = buffer.state.output_length;

			char* retChar = new char[outLen + 1];
			memset(retChar, '\0', outLen + 1);
			memcpy(retChar, buffer.state.output_data, outLen);

			return retChar;
		}

		std::vector<BYTE> Assembler::Assemble(std::string& instructions) const
		{
			int semicolon = instructions.find(';');

			if(semicolon == -1)
				throw MagicException("Semicolon is missing!");

			std::vector<BYTE> retVector;

			do
			{
				std::string substr = instructions.substr(0, semicolon);
				int len = 0;
				char* buffer;
				try {
					buffer = toAsm(substr.c_str(), len);
				} catch(...) {
					throw;
				}

				for(int i = 0; i < len; ++i)
				{
					BYTE byte = *(reinterpret_cast<BYTE*>(&buffer[i]));
					retVector.push_back(byte);
				}
				
				delete[] buffer;

				instructions = instructions.substr(semicolon+1);
				semicolon = instructions.find(';');
			} while(semicolon != -1);

			return retVector;
		}

		DWORD Assembler::GetVersion() const
		{
			return this->version;
		}
	}
}