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
	typedef std::basic_string<TCHAR> tstring;
	typedef std::iterator<std::input_iterator_tag, BYTE> byteIterator;
	typedef std::vector<BYTE>::iterator byteVectorIterator;

	#define _using(_namespace) { using _namespace;
	#define _endusing }

	class MagicException
	{
		std::string msg;
		int error;
	public:
		MagicException(std::string message, int errorCode = 0) : msg(message), error(errorCode) {}
		std::string& GetMessage() { return msg; }
		int GetError() { return error; }
	};

	///We do not handle Exceptions of any kind with this call
	#define MAGIC_CALL(fun) try { fun; } catch(...) { throw; }
	///Throws MagicException if the winapi functions fails.
	#define W32_CALL(fun) if(FALSE == fun) throw ::NoMagic::MagicException(#fun, GetLastError())

	/**
	Ein String gef&uuml;llt mit unsigned chars anstelle von signed chars.
	*/
	class byteString : public std::basic_string<BYTE>
	{
	public:
		byteString(const char* str) : std::basic_string<BYTE>(str, str+strlen(str)+1) {}
		byteString(std::string& str) : std::basic_string<BYTE>(str.begin(), str.end()) {}
		byteString(const BYTE* str) : std::basic_string<BYTE>(str) {}
	};

	/**
	Erleichtert das Erstellen von pattern Masken
	*/
	class PatternMask
	{
		PatternMask() {}
		~PatternMask() {}
	public:
		/**
		Erstellt eine pattern Maske.
		\param mask Die Maske (Bsp. "xxx??xxx")
		\param required Der Buchstabe, der das Byte als ben&ouml;tigt flagged. (Bsp. 'x')
		\return Explain me!
		*/
		static std::vector<bool> Make(std::string const& mask, const char required)
		{
			std::vector<bool> retVector;

			std::for_each(mask.begin(), mask.end(), [&](char chr)
			{
				if(chr == required)
					retVector.push_back(true);
				else
					retVector.push_back(false);
			});

			return retVector;
		}
	};
}