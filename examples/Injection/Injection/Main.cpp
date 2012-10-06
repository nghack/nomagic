#include "../../../NoMagic/NoMagic_Include.h"
#include "../../../NoMagic/Injector_Include.h"
#if (DEBUG)
#pragma comment(lib, "..\\..\\..\\x64\\Debug\\NoMagic_d.lib")
#else
#pragma comment(lib, "..\\..\\..\\bin\\NoMagic_Win32_Release.lib")
#endif

void __fastcall foo()
{
}

int main()
{
	try
	{		
		using NoMagic::Wrappers::Process;
		using NoMagic::Wrappers::Module;
		using NoMagic::Wrappers::Thread;
		using NoMagic::Injector;

		Process::SetDebugPrivileges();
		Module module;
		auto processes = Process::GetProcessesByName(_T("HackMe.exe"));
		if(processes.size() <= 0)
		{
			std::cout << "Start HackMe.exe first!" << std::endl;
			std::cin.get();
			return E_FAIL;
		}

		auto id = processes[0].GetId();

		auto process = Process::GetProcessById(id);
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