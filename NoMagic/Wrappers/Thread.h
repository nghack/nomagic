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
		class Thread
		{
			Thread(const Thread& right);
		private:
			DWORD m_threadId;
			Handle m_handle;
		public:
			Thread();
			Thread(Thread&& right);
			Thread(DWORD threadId);
			Thread(DWORD accesRights, DWORD threadId);
			Thread(HANDLE thread);
			Thread(HANDLE thread, DWORD threadId);
			~Thread(void);

			DWORD GetExitCode() const;

			void Suspend() const;
			static void Suspend(HANDLE handle);

			void Terminate() const;
			static void Terminate(HANDLE handle);

			static Thread CreateRemoteThread(HANDLE process, UINT_PTR startAddress, LPVOID parameter);
			static Thread CreateRemoteThread(HANDLE process, UINT_PTR startAddress);

			static Thread CreateRemoteThread(Process const& process, UINT_PTR startAddress, LPVOID parameter);
			static Thread CreateRemoteThread(Process const& process, UINT_PTR startAddress);

			void WaitForSingleObject() const;
			void WaitForSingleObject(DWORD milliseconds) const;

			void Redirect(UINT_PTR addr);
			static void Redirect(HANDLE thread, UINT_PTR addr);

			const Handle& GetHandle() const;
			const DWORD GetId() const;
		};
	}
}
