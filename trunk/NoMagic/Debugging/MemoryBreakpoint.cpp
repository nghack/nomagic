#include "../Debugging_Include.h"

namespace NoMagic
{
	namespace Debugging
	{
		MemoryBreakpoint::MemoryBreakpoint(UINT_PTR address) :
			Breakpoint(address),
			m_oldProtect(0)
		{
			MEMORY_BASIC_INFORMATION info;
			VirtualQuery(reinterpret_cast<LPCVOID>(address), &info, sizeof(info));

			W32_CALL(VirtualProtect(reinterpret_cast<LPVOID>(address), info.RegionSize, info.Protect | PAGE_GUARD, &m_oldProtect));
		}


		MemoryBreakpoint::~MemoryBreakpoint(void)
		{
			//This will fire a second exception, so I better don't reset it. Uncomment it, if you need it
			//W32_CALL(VirtualProtect(reinterpret_cast<LPVOID>(m_address), 1, m_oldProtect, nullptr));
		}
	}
}