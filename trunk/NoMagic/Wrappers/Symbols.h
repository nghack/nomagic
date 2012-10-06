#pragma once
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
		class Symbols
		{
		public:
			Symbols(tstring const& dllPath);
			Symbols(Symbols && right);
			~Symbols(void);

			void Init(HANDLE process, tstring const& server = _T("SRV*http://msdl.microsoft.com/download/symbols"));
			void FromModule(tstring const& moduleName);
			UINT_PTR FindFunction(tstring const& functionName);

			void SetOptions(DWORD options);

		private:
			typedef DWORD (WINAPI* tSymSetOptions)(DWORD);
			typedef BOOL (WINAPI* tSymInitialize)(HANDLE, TCHAR const*, BOOL);
			typedef DWORD64 (WINAPI* tSymLoadModuleEx)(HANDLE, HANDLE, PCTSTR, PCTSTR, DWORD64, DWORD, PMODLOAD_DATA, DWORD);
			typedef BOOL (WINAPI* tSymFromName)(HANDLE, TCHAR const*, PSYMBOL_INFO);
			typedef BOOL (WINAPI* tSymCleanup)(HANDLE);

			tSymSetOptions mSymSetOption;
			tSymInitialize mSymInitialize;
			tSymLoadModuleEx mSymLoadModuleEx;
			tSymFromName mSymFromName;
			tSymCleanup mSymCleanup;

			HMODULE mDbghlp;
			HMODULE mSymsrv;

			HANDLE mProcess;

			Symbols(Symbols const& right) {}
		};
	}
}
