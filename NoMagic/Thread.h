#pragma once

namespace NoMagic
{
	namespace Wrappers
	{
		class Thread
		{
		private:
			DWORD m_threadId;
			HANDLE m_handle;
		public:
			Thread();
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

			HANDLE GetHandle();
		};
	}
}
