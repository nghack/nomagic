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

	static UINT_PTR findLoadedModule(Wrappers::Process const& process, tstring const& dllPath)
	{
		auto modules = NoMagic::Wrappers::Module::GetModules(process);

		UINT_PTR addr = 0;

		std::for_each(std::begin(modules), std::end(modules), [&](NoMagic::Wrappers::Module module)
		{
			if(module.GetPath() == dllPath ||
				module.GetName() == dllPath)
				addr = module.GetBaseAddress();
		});

		return addr;
	}

	Wrappers::Module Injector::Inject(Wrappers::Process const& process, tstring const& dllPath, UINT_PTR& outStartAddress)
	{
		Wrappers::Module outModule;
		_using(namespace Wrappers)
		{
			UINT_PTR addr;
			UINT_PTR loadedModule;
			Module lib;

			addr = Memory::Allocate(process, 0, dllPath.size() + 1);
			if(addr == 0)
				throw MagicException(_T("VirtualAllocEx failed!"), GetLastError());
			
			try
			{
				Memory::WriteString(process, addr, dllPath);
				
				auto thread = Thread::CreateRemoteThread(process, reinterpret_cast<UINT_PTR>(LoadLibrary), reinterpret_cast<LPVOID>(addr));
				thread.WaitForSingleObject();

				loadedModule = findLoadedModule(process, dllPath);

				outModule = Module(process, reinterpret_cast<HMODULE>(loadedModule) );
				
				Memory::FreeMemory(process, addr);
				addr = 0;
				
				lib = Module::FromLibrary(Process::GetCurrentProcess(), dllPath);
				UINT_PTR startAddr = lib.GetProcAddress("Start");
				
				UINT_PTR diff = static_cast<UINT_PTR>(loadedModule) - reinterpret_cast<UINT_PTR>(lib.GetHandle());
				outStartAddress = startAddr + diff;

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

			return outModule;
		}_endusing
	}

	void Injector::CallStart(Wrappers::Process const& process, UINT_PTR startAddress)
	{
		if(startAddress == 0)
			throw MagicException(_T("startAddress can not be 0!"));

		_using(namespace Wrappers)
		{
			try
			{
				Thread thread = Thread::CreateRemoteThread(process, startAddress);
				thread.WaitForSingleObject();
			}
			catch(...)
			{
				throw;
			}
		} _endusing
	}

	void Injector::UnloadDll(Wrappers::Process const& process, Wrappers::Module const& dll)
	{
		_using(namespace Wrappers)
		{
			try
			{
				auto lib = Module::FromLibrary(Process::GetCurrentProcess(), dll.GetPath());
				UINT_PTR endAddr = lib.GetProcAddress("End");
				
				auto diff = reinterpret_cast<UINT_PTR>(dll.GetHandle()) - reinterpret_cast<UINT_PTR>(lib.GetHandle());
				endAddr = endAddr + diff;
				
				auto thread = Thread::CreateRemoteThread(process, endAddr, 0);
				thread.WaitForSingleObject();

				auto thread2 = Thread::CreateRemoteThread(process, reinterpret_cast<UINT_PTR>(FreeLibrary), reinterpret_cast<LPVOID>(dll.GetHandle()));
				thread2.WaitForSingleObject();
			}
			catch(...)
			{
				throw;
			}

		} _endusing
	}
}