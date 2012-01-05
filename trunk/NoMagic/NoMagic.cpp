#include "NoMagic_Include.h"

namespace NoMagic
{
	NoMagic::NoMagic(void) : procID(0), baseAddress(0), process(nullptr), moduleSize(0), bInProcess(false) { }

	NoMagic::~NoMagic(void)
	{
		CloseHandle(process);
	}

	void NoMagic::GetBaseAddress(DWORD procID, const TCHAR* name)
	{
		MODULEENTRY32 modEntry;

		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, procID);
		modEntry.dwSize = sizeof(MODULEENTRY32);

		if(Module32First(hSnap, &modEntry))
		{
			do 
			{
				if(_tcsicmp(modEntry.szModule, name) == 0)
				{
					baseAddress = (DWORD)modEntry.modBaseAddr;
					moduleSize = modEntry.modBaseSize;
					return;
				}
			} while (Module32Next(hSnap, &modEntry));

			throw MagicException("Can not find module");
		}
	}

	DWORD NoMagic::OpenProcess(tstring const& name)
	{
		PROCESSENTRY32 pe32;
		HANDLE hSnapshot = NULL;

		ZeroMemory(&pe32, sizeof(PROCESSENTRY32));
		pe32.dwSize = sizeof(PROCESSENTRY32);
		hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		if(Process32First(hSnapshot, &pe32) )
		{
			do 
			{
				if(_tcsicmp(pe32.szExeFile, name.c_str()) == 0)
				{
					try 
					{
						GetBaseAddress(pe32.th32ProcessID, name.c_str());
					}
					catch(...)
					{
						throw;
					}
					procID = pe32.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnapshot, &pe32));
		}

		//0x7FF == all rights
		process = ::OpenProcess(0x7FF, FALSE, procID);
		if(!process)
			throw MagicException("Can not open process!", GetLastError());

		return procID;
	}
	
	UINT_PTR NoMagic::SearchPattern(byteString const& pattern, std::vector<bool>& mask, Algorithm::IPatternAlgorithm& algorithm) const
	{
		if(bInProcess)
		{
			BYTE* begin = reinterpret_cast<BYTE*>(baseAddress + 0x1000);
			BYTE* end = reinterpret_cast<BYTE*>(moduleSize-0x1000);
			UINT_PTR ret = algorithm.Utilize(pattern, mask, begin, end);
			return ret != 0 ? baseAddress + 0x1000 + ret : ret;
		}
		else
		{
			BYTE b = Read<BYTE>(baseAddress+0x1000, false);
			std::vector<BYTE> bytes;
			bytes.resize(moduleSize);
			//Hacky?
			LPVOID addr = reinterpret_cast<LPVOID>(const_cast<BYTE*>(bytes.begin()._Ptr));
			ReadProcessMemory(process, LPVOID(baseAddress+0x1000), addr, SIZE_T(moduleSize-0x1000), 0);

			BYTE* begin = reinterpret_cast<BYTE*>(bytes.begin()._Ptr);
			BYTE* end = reinterpret_cast<BYTE*>(bytes.end()._Ptr);
			UINT_PTR ret = algorithm.Utilize(pattern, mask, begin, end);
			return ret != 0 ? baseAddress + 0x1000 + ret : ret;
		}
	}

	void NoMagic::SetInProcess(bool isInProc)
	{
		this->bInProcess = isInProc;
	}

	void NoMagic::SetDebugPrivileges() const
	{
		TOKEN_PRIVILEGES tp;
		HANDLE hToken;

		if(LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid))
		{
			if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
			{
				if(hToken != INVALID_HANDLE_VALUE)
				{
					tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
					tp.PrivilegeCount = 1;
					AdjustTokenPrivileges(hToken, FALSE, &tp, 0, nullptr, nullptr);
					CloseHandle(hToken);
				}
			}
		}
		else
			throw MagicException("Can not lookup privilege value!", GetLastError());
	}

	PBYTE NoMagic::HookFunction(PBYTE targetFunction, PBYTE newFunction) const
	{
		return DetourFunction(targetFunction, newFunction);
	}

	BOOL NoMagic::UnhookFunction(PBYTE origFunction, PBYTE yourFunction) const
	{
		return DetourRemove(origFunction, yourFunction);
	}

	void NoMagic::LoadDLLIntoProcess(tstring const& path, HMODULE& outLoadedModule) const
	{
		PVOID addr = VirtualAllocEx(process, nullptr, path.size() + 1, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if(nullptr == addr)
			throw MagicException("VirtualAllocEx failed!", GetLastError());

		W32_CALL(WriteProcessMemory(process, addr, reinterpret_cast<LPCVOID>(path.c_str()), path.length() + 1, nullptr));

		HANDLE hThread = CreateRemoteThread(process, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryA), addr, 0, nullptr);
		if(nullptr == hThread)
			throw MagicException("CreateRemoteThread failed!", GetLastError());

		if(-1 == WaitForSingleObject(hThread, INFINITE))
			throw MagicException("WaitForSingleObject failed!", GetLastError());
		DWORD loadedModule = 0;
		W32_CALL(GetExitCodeThread( hThread, &loadedModule ));
		outLoadedModule = reinterpret_cast<HMODULE>(loadedModule);
		W32_CALL(VirtualFreeEx(process, addr, 0, MEM_RELEASE));
	}

	void NoMagic::FindStartFunctionAddress(HMODULE& loadedModule, tstring const& path, UINT_PTR& outStartAddress) const
	{
		HMODULE hmod = LoadLibrary(path.c_str());
		if(hmod == nullptr)
			throw MagicException("LoadLibrary failed!", GetLastError());

		DWORD procAddr = reinterpret_cast<DWORD>(GetProcAddress(hmod, "Start"));
		if(procAddr == NULL)
			throw MagicException("GetProcAddress failed!", GetLastError());

		DWORD diff = reinterpret_cast<DWORD>(loadedModule) - reinterpret_cast<DWORD>(hmod);
		outStartAddress = procAddr + diff;
		
		W32_CALL(FreeLibrary(hmod));
	}

	void NoMagic::CallStart(UINT_PTR startFunctionAddress) const
	{
		PVOID addr = VirtualAllocEx(process, nullptr, sizeof(UINT_PTR), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if(nullptr == addr)
			throw MagicException("VirtualAllocEx failed!", GetLastError());

		SIZE_T written = 0;

		DWORD startAddr = static_cast<DWORD>(startFunctionAddress);

		W32_CALL(WriteProcessMemory(process, addr, reinterpret_cast<LPCVOID>(&startAddr), sizeof(LPCVOID), nullptr)); 

		HANDLE hThread = CreateRemoteThread(process, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(startFunctionAddress), addr, 0, nullptr);
		if(nullptr == hThread)
			throw MagicException("CreateRemoteThread failed!", GetLastError());
		
		if(-1 == WaitForSingleObject(hThread, INFINITE))
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
		HANDLE hThread = CreateRemoteThread( process, nullptr, 0, (LPTHREAD_START_ROUTINE)FreeLibrary, (PVOID)address, 0, nullptr);
		if(nullptr == hThread)
			throw MagicException("CreateRemoteThread failed!", GetLastError());

		if(-1 != WaitForSingleObject(hThread, INFINITE))
			throw MagicException("WaitForSingleObject failed!", GetLastError());

		W32_CALL(CloseHandle(hThread));
	}

	std::string NoMagic::ReadString(UINT_PTR address, bool relative) const
	{
		CHAR buffer[512];

		LPCVOID addr = reinterpret_cast<LPVOID>(relative ? address+baseAddress : address);
		W32_CALL(ReadProcessMemory(process, addr, buffer, 512, nullptr));
		
		return buffer;
	}
	
	void NoMagic::WriteString(UINT_PTR address, std::string value, bool relative) const
	{
		LPVOID addr = reinterpret_cast<LPVOID>(relative ? address+baseAddress : address);
		W32_CALL(WriteProcessMemory(process, addr, value.c_str(), value.length() + 1, nullptr));
	}

	HANDLE NoMagic::GetProcessHandle() const { return process; }
	DWORD NoMagic::GetProcessID() const { return procID; }
	UINT_PTR NoMagic::GetBaseAddress() const { return baseAddress; }
	UINT_PTR NoMagic::GetModuleSize() const { return moduleSize; }
}