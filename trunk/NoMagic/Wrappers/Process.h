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
		class Process
		{
			Process(const Process& right);
		protected:
			DWORD m_id;
			Handle m_handle;
			tstring m_name;

		public:
			Process();
			Process(Process&& right);
			explicit Process(DWORD processID);
			explicit Process(PROCESSENTRY32 const& processEntry);
			virtual ~Process(void);

			static std::vector<Process> GetProcesses();
			static std::vector<Process> GetProcessesByName(tstring const& processName);

			static Process GetProcessByWindowName(tstring const& windowName);
			static Process GetProcessById(DWORD PID);
			static Process GetProcessByHWND(const HWND hwnd);

			static Process GetCurrentProcess();

			static void SetDebugPrivileges();

			void OpenProcess();

			std::vector<Thread> GetThreads();
			std::vector<Module> GetModules();

			DWORD GetId() const;
			const Handle& GetHandle() const;
			tstring const& GetName() const;
		};
	}
}