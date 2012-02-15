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
	NoMagic::NoMagic(void) : procID(0), baseAddress(0), process(nullptr), moduleSize(0), bInProcess(false) { }

	NoMagic::~NoMagic(void)
	{
		if(process != nullptr)
			CloseHandle(process);
	}

	void NoMagic::GetBaseAddress(Wrappers::Process const& process)
	{
		using Wrappers::Module;
		try
		{
			auto modules = Module::GetModules(process);

			std::for_each(std::begin(modules), std::end(modules), [&](Module module)
			{
				if(process.GetName() == module.GetName())
				{
					baseAddress = module.GetBaseAddress();
					moduleSize = module.GetSize();
					return;
				}
			});
		}
		catch(...)
		{
			throw;
		}
	}

	DWORD NoMagic::OpenProcess(tstring const& name)
	{
		_using(Wrappers::Process)
		{
			std::vector<Process> procs;
			try
			{
				procs = Process::GetProcessesByName(name);

				if(procs.size() <= 0)
					throw MagicException("Can not find any processes!");

				procs[0].OpenProcess();
				GetBaseAddress(procs[0]);
			}
			catch(...)
			{
				throw;
			}
			this->process = procs[0].GetHandle();
			this->procID = procs[0].GetId();
			return procs[0].GetId();
		} _endusing
	}
	
	UINT_PTR NoMagic::SearchPattern(byteString const& pattern, std::vector<bool>& mask, Algorithm::IPatternAlgorithm& algorithm) const
	{
		if(bInProcess)
		{
			auto ret = Wrappers::Memory::FindPattern(baseAddress+0x1000, baseAddress+moduleSize-0x1000, pattern, mask, algorithm);
			return ret != 0 ? baseAddress + 0x1000 + ret : ret;
		}
		else
		{
			std::vector<BYTE> bytes;
			bytes.resize(moduleSize-0x1000);
			//Hacky? - should be fine, since I do reserve enough size. Report any crashes and I will find a better workarround
			auto addr = reinterpret_cast<LPVOID>(const_cast<BYTE*>(bytes.begin()._Ptr));
			ReadProcessMemory(process, LPVOID(baseAddress+0x1000), addr, SIZE_T(moduleSize-0x1000), 0);

			auto begin = reinterpret_cast<BYTE*>(bytes.begin()._Ptr);
			auto end = reinterpret_cast<BYTE*>(bytes.end()._Ptr);
			auto ret = algorithm.Utilize(pattern, mask, begin, end);
			return ret != 0 ? baseAddress + 0x1000 + ret : ret;
		}
	}

	void NoMagic::SetInProcess(bool isInProc)
	{
		this->bInProcess = isInProc;
	}

	void NoMagic::SetDebugPrivileges() const
	{
		Wrappers::Process::SetDebugPrivileges();
	}

	PBYTE NoMagic::HookFunction(PBYTE targetFunction, PBYTE newFunction) const
	{
		return Wrappers::Memory::DetourFunction(targetFunction, newFunction);
	}

	BOOL NoMagic::UnhookFunction(PBYTE origFunction, PBYTE yourFunction) const
	{
		return Wrappers::Memory::RemoveDetour(origFunction, yourFunction);
	}

	void NoMagic::LoadDLLIntoProcess(tstring const& path, HMODULE& outLoadedModule) const
	{
		_using(Wrappers::Memory)
		{
			auto addr = Memory::Allocate(process, 0, path.size() + 1);
			if(addr == 0)
				throw MagicException("VirtualAllocEx failed!", GetLastError());

			MAGIC_CALL( Memory::WriteString(process, addr, path); )
		
			auto hThread = CreateRemoteThread(process, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryA)
				, reinterpret_cast<LPVOID>(addr), 0, nullptr);
			if(nullptr == hThread)
				throw MagicException("CreateRemoteThread failed!", GetLastError());

			if(0 != WaitForSingleObject(hThread, INFINITE))
				throw MagicException("WaitForSingleObject failed!", GetLastError());
			DWORD loadedModule = 0;
			W32_CALL(GetExitCodeThread( hThread, &loadedModule ));
			outLoadedModule = reinterpret_cast<HMODULE>(loadedModule);

			MAGIC_CALL( Memory::FreeMemory(process, addr, 0); )
		}_endusing
	}

	void NoMagic::FindStartFunctionAddress(HMODULE const& loadedModule, tstring const& path, UINT_PTR& outStartAddress) const
	{
		auto hmod = LoadLibrary(path.c_str());
		if(hmod == nullptr)
			throw MagicException("LoadLibrary failed!", GetLastError());

		auto procAddr = reinterpret_cast<DWORD>(GetProcAddress(hmod, "Start"));
		if(procAddr == NULL)
			throw MagicException("GetProcAddress failed!", GetLastError());

		auto diff = reinterpret_cast<DWORD>(loadedModule) - reinterpret_cast<DWORD>(hmod);
		outStartAddress = procAddr + diff;
		
		W32_CALL(FreeLibrary(hmod));
	}

	void NoMagic::CallStart(UINT_PTR startFunctionAddress) const
	{
		auto addr = VirtualAllocEx(process, nullptr, sizeof(UINT_PTR), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if(nullptr == addr)
			throw MagicException("VirtualAllocEx failed!", GetLastError());

		SIZE_T written = 0;

		auto startAddr = static_cast<DWORD>(startFunctionAddress);

		W32_CALL(WriteProcessMemory(process, addr, reinterpret_cast<LPCVOID>(&startAddr), sizeof(LPCVOID), nullptr)); 

		auto hThread = CreateRemoteThread(process, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(startFunctionAddress), addr, 0, nullptr);
		if(nullptr == hThread)
			throw MagicException("CreateRemoteThread failed!", GetLastError());
		
		if(0 != WaitForSingleObject(hThread, INFINITE))
			throw MagicException("WaitForSingleObject failed!", GetLastError());

		W32_CALL(CloseHandle(hThread));
		
		W32_CALL(VirtualFreeEx(process, addr, 0, MEM_RELEASE));
	}

	UINT_PTR NoMagic::InjectDll(tstring const& path) const
	{
		try
		{
			HMODULE hLibModule = nullptr;
			UINT_PTR startFunctionAddress;
			
			LoadDLLIntoProcess(path, hLibModule);
			FindStartFunctionAddress(hLibModule, path, startFunctionAddress);
			CallStart(startFunctionAddress);
			
			return reinterpret_cast<UINT_PTR>(hLibModule);
		}
		catch(...)
		{
			throw;
		}
	}

	void NoMagic::UnloadDll(UINT_PTR address) const
	{
		auto hThread = CreateRemoteThread( process, nullptr, 0, (LPTHREAD_START_ROUTINE)FreeLibrary, (PVOID)address, 0, nullptr);
		if(nullptr == hThread)
			throw MagicException("CreateRemoteThread failed!", GetLastError());

		if(0 != WaitForSingleObject(hThread, INFINITE))
			throw MagicException("WaitForSingleObject failed!", GetLastError());

		W32_CALL(CloseHandle(hThread));
	}
	
	DWORD NoMagic::Protect(UINT_PTR address, DWORD numBytes, DWORD newProtect) const
	{
		MAGIC_CALL( return Wrappers::Memory::Protect(address, numBytes, newProtect); )
	}

	const DWORD* NoMagic::GetVirtualMethod(LPVOID object, DWORD vTableIndex) const
	{
		auto __vmt = reinterpret_cast<LPDWORD**>(object);
		return (*__vmt)[vTableIndex];
	}

	std::string NoMagic::ReadString(UINT_PTR address, bool relative) const
	{
		if(relative)
			MAGIC_CALL( return Wrappers::Memory::ReadString(process, address, baseAddress); )
		else
			MAGIC_CALL( return Wrappers::Memory::ReadString(process, address); )
	}
	
	void NoMagic::WriteString(UINT_PTR address, std::string value, bool relative) const
	{
		if(relative)
			MAGIC_CALL( Wrappers::Memory::WriteString(process, address, value, baseAddress); )
		else
			MAGIC_CALL( Wrappers::Memory::WriteString(process, address, value); )
	}

	HANDLE NoMagic::GetProcessHandle() const { return process; }
	DWORD NoMagic::GetProcessID() const { return procID; }
	UINT_PTR NoMagic::GetBaseAddress() const { return baseAddress; }
	UINT_PTR NoMagic::GetModuleSize() const { return moduleSize; }
}