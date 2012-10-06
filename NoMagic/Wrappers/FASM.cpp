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

#ifndef UNICODE

#include "../Wrappers_Include.h"

namespace NoMagic
{
	namespace fasm
	{
		Assembler::Assembler()
		{
			#ifdef _M_AMD64
			throw MagicException(_T("This features is not supported in x64!"));
			#endif
			this->fasmLib = LoadLibrary("FASM.DLL");
			if(this->fasmLib == nullptr)
				throw MagicException("Can not load FASM.DLL!", GetLastError());

			this->fasm_assemble = reinterpret_cast<tfasm_assemble>(GetProcAddress(this->fasmLib, _T("fasm_Assemble")));
			if(this->fasm_assemble == nullptr)
				throw MagicException("GetProcAddress(\"fasm_Assemble\") failed!", GetLastError());

			auto getVersion = reinterpret_cast<int(_stdcall*)()>(GetProcAddress(this->fasmLib, _T("fasm_GetVersion")));
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

		std::vector<BYTE> Assembler::Assemble(const std::string& instructions) const
		{
			auto semicolon = instructions.find(';');
			auto code = instructions;

			if(semicolon == -1)
				throw MagicException("Semicolon is missing!");

			std::vector<BYTE> retVector;

			do
			{
				auto substr = code.substr(0, semicolon);
				int len = 0;
				char* buffer;

				MAGIC_CALL( buffer = toAsm(substr.c_str(), len); )

				for(int i = 0; i < len; ++i)
				{
					auto byte = *(reinterpret_cast<BYTE*>(&buffer[i]));
					retVector.push_back(byte);
				}
				
				delete[] buffer;

				code = code.substr(semicolon+1);
				semicolon = code.find(';');
			} while(semicolon != -1);

			return retVector;
		}

		int Assembler::GetVersion() const
		{
			return this->version;
		}
	}
}

#endif