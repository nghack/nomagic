#include "../Debugging_Include.h"

namespace NoMagic
{
	namespace Debugging
	{
		MemoryBreakpoint::MemoryBreakpoint(UINT_PTR address) :
			Breakpoint(address)
		{
			W32_CALL(VirtualProtect(reinterpret_cast<LPVOID>(address), 1, PAGE_GUARD, &m_oldProtect));
		}


		MemoryBreakpoint::~MemoryBreakpoint(void)
		{
			//This will fire a second exception, so I better don't reset it.
			//W32_CALL(VirtualProtect(reinterpret_cast<LPVOID>(m_address), 1, m_oldProtect, nullptr));
		}
	}
}