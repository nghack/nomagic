#include "Wrappers_Include.h"


namespace NoMagic
{
	namespace Wrappers
	{
		Thread::Thread()  : m_threadId(0), m_handle(nullptr)
		{
		}

		Thread::Thread(DWORD threadId) : m_threadId(threadId), m_handle(nullptr)
		{
			SetLastError(ERROR_SUCCESS);
			this->m_handle = ::OpenThread(THREAD_ALL_ACCESS, false, threadId);
			if(GetLastError() != ERROR_SUCCESS)
				throw MagicException("OpenThread failed!", GetLastError());
		}

		Thread::Thread(DWORD accesRights, DWORD threadId) : m_threadId(threadId), m_handle(nullptr)
		{
			SetLastError(ERROR_SUCCESS);
			this->m_handle = ::OpenThread(accesRights, false, threadId);
			if(GetLastError() != ERROR_SUCCESS)
				throw MagicException("OpenThread failed!", GetLastError());
		}

		Thread::Thread(HANDLE thread) : m_threadId(0), m_handle(thread)
		{
			this->m_threadId = ::GetThreadId(thread);
		}

		Thread::Thread(HANDLE thread, DWORD threadId) : m_threadId(threadId), m_handle(thread)
		{
		}

		Thread::~Thread(void)
		{
			//FixMe: Crash!
			/*
			if(m_handle != nullptr)
			{
				CloseHandle(m_handle);
				m_handle = nullptr;
			}*/
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
			HANDLE handle = ::CreateRemoteThread(process, nullptr, 0, 
				reinterpret_cast<LPTHREAD_START_ROUTINE>(startAddress), parameter, 0, &threadId);

			if(handle == nullptr)
				throw MagicException("CreateRemoteThread failed!", GetLastError());

			return Thread(handle, threadId);
		}

		Thread Thread::CreateRemoteThread(HANDLE process, UINT_PTR startAddress)
		{
			return CreateRemoteThread(process, startAddress, nullptr);
		}

		Thread Thread::CreateRemoteThread(Process const& process, UINT_PTR startAddress, LPVOID parameter)
		{
			DWORD threadId = 0;
			HANDLE handle = ::CreateRemoteThread(process.GetHandle(), nullptr, 0, 
				reinterpret_cast<LPTHREAD_START_ROUTINE>(startAddress), parameter, 0, &threadId);

			if(handle == nullptr)
				throw MagicException("CreateRemoteThread failed!", GetLastError());

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
				throw MagicException("WaitForSingleObject failed!", GetLastError());
		}

		void Thread::WaitForSingleObject(DWORD milliseconds) const
		{
			SetLastError(0);
			if( ::WaitForSingleObject(m_handle, milliseconds) != 0)
				throw MagicException("WaitForSingleObject failed!", GetLastError());
		}

		HANDLE Thread::GetHandle()
		{
			return this->m_handle;
		}
	}
}