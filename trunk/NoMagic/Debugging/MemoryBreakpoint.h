#pragma once

namespace NoMagic
{
	namespace Debugging
	{
		class MemoryBreakpoint : public Breakpoint
		{
		private:
			DWORD m_oldProtect;
		public:
			MemoryBreakpoint(UINT_PTR address);
			~MemoryBreakpoint(void);
		};
	}
}