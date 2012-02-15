#include "../../../NoMagic/NoMagic_Include.h"
#if (DEBUG)
#pragma comment(lib, "..\\..\\..\\Debug\\NoMagic_d.lib")
#else
#pragma comment(lib, "..\\..\\..\\Debug\\NoMagic.lib")
#endif


int main(int argc, char** argv)
{
	NoMagic::NoMagic nm;

	//Wir brauchen Debugrechte!
	nm.SetDebugPrivileges();

	try
	{
		nm.OpenProcess(_T("Wow.exe"));
		//Wir lesen aus, ob die Konsole geöffnet ist (nur für WoW 3.3.5a!)
		auto consoleOpen = nm.Read<BOOL>(0xCA1978);

		//Here's my key, Philosophy. A freak like me, just needs infinity!
		while(true)
		{
			//Wir schalten die Konsole ein bzw. aus ...
			if(!consoleOpen)
				consoleOpen = TRUE;
			else
				consoleOpen = FALSE;
			//... und teilen die Änderung WoW mit
			nm.Write<BOOL>(0xCA1978, consoleOpen);
			std::cin.get();
		}
	}
	catch(NoMagic::MagicException& e)
	{
		std::cout << e.GetMessage() << std::endl;
		std::cin.get();
		return E_FAIL;
	}
	
	return S_OK;
}
