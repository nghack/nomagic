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
#include "Injector_Include.h"

namespace NoMagic
{
	Injector::Injector()
	{
	}

	Injector::~Injector(void)
	{
	}

	UINT_PTR Injector::Inject(Wrappers::Process const& process, tstring const& dllPath, Wrappers::Module& outModule)
	{
		_using(namespace Wrappers)
		{
			UINT_PTR addr;
			Thread thread;
			DWORD loadedModule;
			Module lib;

			addr = Memory::Allocate(process, 0, dllPath.size() + 1);
			if(addr == 0)
				throw MagicException("VirtualAllocEx failed!", GetLastError());
			
			try
			{
				Memory::WriteString(process, addr, dllPath);
				
				thread = Thread::CreateRemoteThread(process, reinterpret_cast<UINT_PTR>(LoadLibrary), reinterpret_cast<LPVOID>(addr));
				thread.WaitForSingleObject();

				//Todo: ptr to dword... -.- x64??
				loadedModule = thread.GetExitCode();

				outModule = Module(process, reinterpret_cast<HMODULE>(loadedModule) );
				
				Memory::FreeMemory(process, addr);
				addr = 0;
				
				lib = Module::FromLibrary(Process::GetCurrentProcess(), dllPath);
				UINT_PTR startAddr = lib.GetProcAddress(_T("Start"));

				UINT_PTR diff = static_cast<UINT_PTR>(loadedModule) - reinterpret_cast<UINT_PTR>(lib.GetHandle());
				addr = startAddr + diff;

				W32_CALL(FreeLibrary(lib.GetHandle()));
			}
			catch(...)
			{
				if(addr != 0)
					Memory::FreeMemory(process, addr);

				if(lib.GetHandle() != nullptr)
					W32_CALL(FreeLibrary(lib.GetHandle()));
				throw;
			}

			return addr;
		}_endusing
	}

	void Injector::CallStart(Wrappers::Process const& process, UINT_PTR startAddress)
	{
		if(startAddress == 0)
			throw MagicException("Are you kidding me? -.-");

		_using(namespace Wrappers)
		{
			UINT_PTR addr = Memory::Allocate(process, 0, sizeof(UINT_PTR));

			if(addr == 0)
				throw MagicException("Allocation failed!", GetLastError());
			
			try
			{
				Memory::Write(process, addr, startAddress);
				Thread thread = Thread::CreateRemoteThread(process, startAddress);
				thread.WaitForSingleObject();
			}
			catch(...)
			{
				Memory::FreeMemory(process, addr);
			}
			
			Memory::FreeMemory(process, addr);
		} _endusing
	}

	void Injector::UnloadDll(Wrappers::Process const& process, Wrappers::Module const& dll)
	{
		_using(namespace Wrappers)
		{
			try
			{
				Thread thread = Thread::CreateRemoteThread(process, reinterpret_cast<UINT_PTR>(FreeLibrary), reinterpret_cast<LPVOID>(dll.GetHandle()));
				thread.WaitForSingleObject();
			}
			catch(...)
			{
				throw;
			}

		} _endusing
	}
}