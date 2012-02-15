#include "../../../NoMagic/NoMagic_Include.h"
#include "../../../NoMagic/Injector_Include.h"
#if (DEBUG)
#pragma comment(lib, "..\\..\\..\\Debug\\NoMagic_d.lib")
#else
#pragma comment(lib, "..\\..\\..\\Debug\\NoMagic.lib")
#endif


int main()
{
	try
	{
		//Equivalent code by using the wrapper class NoMagic
		/*NoMagic::NoMagic noMagic;
		
		noMagic.SetDebugPrivileges();
		noMagic.OpenProcess(_T("HackMe.exe"));
		UINT_PTR startAddress = noMagic.InjectDll(_T("injectMe.dll"));
		
		std::cout << "Press Enter to continue..." << std::endl;
		std::cin.get();
		noMagic.UnloadDll(startAddress);*/
		
		using NoMagic::Wrappers::Process;
		using NoMagic::Wrappers::Module;
		using NoMagic::Wrappers::Thread;
		using NoMagic::Injector;

		Process::SetDebugPrivileges();
		Module module;
		auto process = Process::GetProcessesByName(_T("HackMe.exe"))[0];
		process.OpenProcess();

		auto startAddress = Injector::Inject(process, _T("injectMe.dll"), module);
		Injector::CallStart(process, startAddress);

		std::cout << "Press Return to continue..." << std::endl;
		std::cin.get();
		Injector::UnloadDll(process, module);
	}
	catch(NoMagic::MagicException const& e)
	{
		std::cout << e.GetMessage() << std::endl;
		std::cout << e.GetError() << std::endl;
		std::cin.get();
	}
}