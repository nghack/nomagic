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

#ifndef UNICODE

namespace NoMagic
{
	namespace fasm
	{
		/**
		FASM internal stuff
		*/
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
		
		/**
		FASM internal stuff
		*/
		struct fasm_read_buffer
		{
			FASM_STATE state;
			PBYTE buffer[4096];
		};

		class Assembler
		{
		private:
			/**
			FASM internal stuff
			*/
			typedef int (__stdcall* tfasm_assemble)(const char* lpSource, void* lpMemory, DWORD size, DWORD nPasses, void* displayPipe);
			tfasm_assemble fasm_assemble;

			int version;
			HMODULE fasmLib;

			/**
			Assembles the given string
			\param source The Assembly command
			\param outLen Number of returned characters
			\return sequence of assembled bytes
			\brief the returned string must be deleted!
			*/
			char* toAsm(const char* source, int& outLen) const;
		public:
			/**
			Loads fasm_assemble from fasm.dll
			\brief throws an exception if it fails!
			*/
			Assembler();
			~Assembler();

			/**
			Assembles semicolon seperated assembly commands
			\param instructions semicolon seperated assembly commands
			\return vector of assembled bytes
			*/
			std::vector<BYTE> Assemble(const std::string& instructions) const;

			/**
			\return fasm.dll's version
			*/
			int GetVersion() const;
		};
	}
}

#endif