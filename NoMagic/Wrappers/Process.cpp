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
		Process::Process(DWORD processId) : m_id(processId), m_handle(nullptr)
		{
			PROCESSENTRY32 pe32;
			HANDLE hSnapshot = NULL;

			ZeroMemory(&pe32, sizeof(PROCESSENTRY32));
			pe32.dwSize = sizeof(PROCESSENTRY32);
			hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, processId);

			W32_CALL( Process32First(hSnapshot, &pe32) );
			do 
			{
				if(pe32.th32ProcessID == processId)
					m_name = pe32.szExeFile;
			} while (Process32Next(hSnapshot, &pe32));
			
			CloseHandle(hSnapshot);
		}

		Process::Process(PROCESSENTRY32 const& processEntry) : m_id(processEntry.th32ProcessID), m_handle(nullptr), m_name(processEntry.szExeFile)
		{
		}

		Process::~Process(void)
		{
			/*if(m_handle != nullptr)
			{
				CloseHandle(m_handle);
				m_handle = nullptr;
			}*/
		}

		std::vector<Process> Process::GetProcesses()
		{
			std::vector<Process> processes;
			PROCESSENTRY32 pe32;
			HANDLE hSnapshot = NULL;

			ZeroMemory(&pe32, sizeof(PROCESSENTRY32));
			pe32.dwSize = sizeof(PROCESSENTRY32);
			hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

			if(Process32First(hSnapshot, &pe32) )
			{
				do 
				{
					processes.push_back( Process(pe32) );
				} while (Process32Next(hSnapshot, &pe32));
			}
			CloseHandle(hSnapshot);
			return processes;
		}

		std::vector<Process> Process::GetProcessesByName(tstring const& processName)
		{
			std::vector<Process> processes;
			PROCESSENTRY32 pe32;
			HANDLE hSnapshot = NULL;

			ZeroMemory(&pe32, sizeof(PROCESSENTRY32));
			pe32.dwSize = sizeof(PROCESSENTRY32);
			hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

			if(Process32First(hSnapshot, &pe32) )
			{
				do 
				{
					if(_tcsicmp(pe32.szExeFile, processName.c_str()) == 0)
					{
						processes.push_back( Process(pe32) );
					}
				} while (Process32Next(hSnapshot, &pe32));
			}
			CloseHandle(hSnapshot);
			return processes;
		}

		Process GetProcessByWindowName(tstring const& windowName)
		{
			auto wnd = ::FindWindow(nullptr, windowName.c_str());

			if(wnd == nullptr)
				throw MagicException("Can not find Window!", GetLastError());

			return Process::GetProcessByHWND(wnd);
		}

		Process Process::GetProcessById(DWORD PID)
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
					if(pe32.th32ProcessID == PID)
					{
						CloseHandle(hSnapshot);
						return Process(pe32);
					}
				} while (Process32Next(hSnapshot, &pe32));
			}
			CloseHandle(hSnapshot);
			throw MagicException("Process not found!");
		}

		Process Process::GetProcessByHWND(HWND hwnd)
		{
			DWORD pid = 0;
			W32_CALL( GetWindowThreadProcessId(hwnd, &pid) );
			return Process(pid);
		}

		Process Process::GetCurrentProcess()
		{
			return Process(GetCurrentProcessId());
		}

		void Process::OpenProcess()
		{
			if(this->m_handle != nullptr)
				CloseHandle(m_handle);

			SetLastError(0);
			this->m_handle = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->m_id);
			
			if(this->m_handle == nullptr || GetLastError() != 0)
				throw MagicException("OpenProcess failed!", GetLastError());
		}

		void Process::SetDebugPrivileges()
		{
			TOKEN_PRIVILEGES tp;
			HANDLE hToken;

			W32_CALL(LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid));

			W32_CALL(OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken));

			if(hToken != INVALID_HANDLE_VALUE)
			{
				tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
				tp.PrivilegeCount = 1;
				AdjustTokenPrivileges(hToken, FALSE, &tp, 0, nullptr, nullptr);
				CloseHandle(hToken);
			}
		}

		std::vector<Thread> Process::GetThreads()
		{
			std::vector<Thread> threads;
			THREADENTRY32 te32;
			HANDLE hSnapshot = NULL;

			ZeroMemory(&te32, sizeof(THREADENTRY32));
			te32.dwSize = sizeof(THREADENTRY32);
			
			hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, m_id);

			if(Thread32First(hSnapshot, &te32))
			{
				do
				{
					if(te32.th32OwnerProcessID == m_id)
						threads.push_back(Thread(te32.th32ThreadID));
				} while(Thread32Next(hSnapshot, &te32));
			}

			return threads;
		}

		DWORD Process::GetId() const { return m_id; }
		const HANDLE Process::GetHandle() const { return m_handle; }
		tstring const& Process::GetName() const { return m_name; }
	}
}