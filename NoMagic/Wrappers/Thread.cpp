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
		Thread::Thread() : 
			m_threadId(0), 
			m_handle(nullptr)
		{
		}

		Thread::Thread(const Thread& right)
		{
			throw MagicException(_T("One does not simply copy a thread!"));
		}

		Thread::Thread(Thread&& right) : 
			m_threadId(0), 
			m_handle(nullptr)
		{
			std::swap(m_handle, right.m_handle);
			std::swap(m_threadId, right.m_threadId);
		}

		Thread::Thread(DWORD threadId) : 
			m_threadId(threadId),
			m_handle(nullptr)
		{
			SetLastError(ERROR_SUCCESS);
			this->m_handle = ::OpenThread(THREAD_ALL_ACCESS, false, threadId);
			if(GetLastError() != ERROR_SUCCESS)
				throw MagicException(_T("OpenThread failed!"), GetLastError());
		}

		Thread::Thread(DWORD accesRights, DWORD threadId) : 
			m_threadId(threadId), 
			m_handle(nullptr)
		{
			SetLastError(ERROR_SUCCESS);
			this->m_handle = ::OpenThread(accesRights, false, threadId);
			if(GetLastError() != ERROR_SUCCESS)
				throw MagicException(_T("OpenThread failed!"), GetLastError());
		}

		Thread::Thread(HANDLE thread) : 
			m_threadId(0), 
			m_handle(thread)
		{
			this->m_threadId = ::GetThreadId(thread);
		}

		Thread::Thread(HANDLE thread, DWORD threadId) : 
			m_threadId(threadId), 
			m_handle(thread)
		{
		}

		Thread::~Thread(void)
		{
		}

		DWORD Thread::GetExitCode() const
		{
			DWORD exitCode = 0;
			W32_CALL( ::GetExitCodeThread(m_handle, &exitCode) );
			return exitCode;
		}

		void Thread::Suspend() const
		{
			::SuspendThread(m_handle);
		}

		void Thread::Suspend(HANDLE handle)
		{
			::SuspendThread(handle);
		}

		void Thread::Terminate() const
		{
			::TerminateThread(m_handle, 0);
		}

		void Thread::Terminate(HANDLE handle)
		{
			::TerminateThread(handle, 0);
		}

		Thread Thread::CreateRemoteThread(HANDLE process, UINT_PTR startAddress, LPVOID parameter)
		{
			DWORD threadId = 0;
			auto handle = ::CreateRemoteThread(process, nullptr, 0, 
				reinterpret_cast<LPTHREAD_START_ROUTINE>(startAddress), parameter, 0, &threadId);

			if(handle == nullptr)
				throw MagicException(_T("CreateRemoteThread failed!"), GetLastError());

			return Thread(handle, threadId);
		}

		Thread Thread::CreateRemoteThread(HANDLE process, UINT_PTR startAddress)
		{
			return CreateRemoteThread(process, startAddress, nullptr);
		}

		Thread Thread::CreateRemoteThread(Process const& process, UINT_PTR startAddress, LPVOID parameter)
		{
			DWORD threadId = 0;
			auto handle = ::CreateRemoteThread(process.GetHandle(), nullptr, 0, 
				reinterpret_cast<LPTHREAD_START_ROUTINE>(startAddress), parameter, 0, &threadId);

			if(handle == nullptr)
				throw MagicException(_T("CreateRemoteThread failed!"), GetLastError());

			return Thread(handle, threadId);
		}

		Thread Thread::CreateRemoteThread(Process const& process, UINT_PTR startAddress)
		{
			return CreateRemoteThread(process, startAddress, nullptr);
		}
		
		void Thread::WaitForSingleObject() const
		{
			SetLastError(0);
			if( ::WaitForSingleObject(m_handle, INFINITE) != 0)
				throw MagicException(_T("WaitForSingleObject failed!"), GetLastError());
		}

		void Thread::WaitForSingleObject(DWORD milliseconds) const
		{
			SetLastError(0);
			if( ::WaitForSingleObject(m_handle, milliseconds) != 0)
				throw MagicException(_T("WaitForSingleObject failed!"), GetLastError());
		}

		const Handle& Thread::GetHandle() const
		{
			return this->m_handle;
		}

		const DWORD Thread::GetId() const
		{
			return this->m_threadId;
		}

		void Thread::Redirect(UINT_PTR addr)
		{
			CONTEXT con = {CONTEXT_CONTROL};
			SuspendThread(m_handle);
			GetThreadContext(m_handle, &con);		
#ifdef _M_AMD64
			con.Rip = addr;
#else
			con.Eip = addr;
#endif
			con.ContextFlags = CONTEXT_CONTROL;

			SetThreadContext(m_handle, &con);
			ResumeThread(m_handle);

		}

		void Thread::Redirect(HANDLE thread, UINT_PTR addr)
		{
			CONTEXT con = {CONTEXT_CONTROL};
			SuspendThread(thread);
			GetThreadContext(thread, &con);
#ifdef _M_AMD64
			con.Rip = addr;
#else
			con.Eip = addr;
#endif
			con.ContextFlags = CONTEXT_CONTROL;

			SetThreadContext(thread, &con);
			ResumeThread(thread);
		}
	}
}