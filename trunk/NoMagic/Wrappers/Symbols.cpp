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
		Symbols::Symbols(tstring const& dllPath)
		{
			auto dbghelp = dllPath + _T("dbghelp.dll");
			auto symsrvd = dllPath + _T("symsrv.dll");
			mDbghlp = LoadLibrary(dbghelp.c_str());
			mSymsrv = LoadLibrary(symsrvd.c_str());
		}


		Symbols::Symbols(Symbols && right)
		{
			std::swap(mDbghlp, right.mDbghlp);
			std::swap(mSymsrv, right.mSymsrv);
		}


		Symbols::~Symbols(void)
		{
			mSymCleanup(mProcess);

			if(mDbghlp)
				FreeLibrary(mDbghlp);

			if(mSymsrv)
				FreeLibrary(mSymsrv);
		}

		
		void Symbols::Init(HANDLE process, tstring const& server)
		{
			mProcess = process;
			mSymSetOption = reinterpret_cast<tSymSetOptions>(GetProcAddress(mDbghlp, "SymSetOptions"));
			mSymInitialize = reinterpret_cast<tSymInitialize>(GetProcAddress(mDbghlp, "SymInitialize"));
			mSymLoadModuleEx = reinterpret_cast<tSymLoadModuleEx>(GetProcAddress(mDbghlp, "SymLoadModuleEx"));
			mSymFromName = reinterpret_cast<tSymFromName>(GetProcAddress(mDbghlp, "SymFromName"));
			mSymCleanup = reinterpret_cast<tSymCleanup>(GetProcAddress(mDbghlp, "SymCleanup"));

			if(!mSymSetOption || !mSymInitialize || !mSymLoadModuleEx || !mSymFromName || !mSymCleanup)
				throw MagicException(_T("Can not load symbol functions"), GetLastError());

			if(!mSymInitialize(mProcess, server.c_str(), FALSE))
				throw MagicException(_T("Can not initialize symbols"), GetLastError());
		}


		void Symbols::SetOptions(DWORD options)
		{
			mSymSetOption(options);
		}


		void Symbols::FromModule(tstring const& moduleName)
		{
			auto module = GetModuleHandle(moduleName.c_str());
			if( !mSymLoadModuleEx(mProcess, nullptr, moduleName.c_str(), nullptr, (DWORD64)module, 0, nullptr, 0))
				throw MagicException(_T("Can not find module"), GetLastError());
		}


		UINT_PTR Symbols::FindFunction(tstring const& functionName)
		{
			std::vector<char> buffer(sizeof(SYMBOL_INFO) + 255 * sizeof(TCHAR) + 1);

			PSYMBOL_INFO info = reinterpret_cast<PSYMBOL_INFO>(buffer.data());
			info->SizeOfStruct = sizeof(SYMBOL_INFO);
			info->MaxNameLen = sizeof(sizeof(TCHAR) * 255 + 1);

			if(!mSymFromName(mProcess, functionName.c_str(), info))
				throw MagicException(_T("Can not find function"), GetLastError());

			return static_cast<UINT_PTR>(info->Address);
		}
	}
}