#include "../../../NoMagic/NoMagic_Include.h"
#include <process.h>


#if (DEBUG)
#pragma comment(lib, "..\\..\\..\\Debug\\NoMagic_d.lib")
#else
#pragma comment(lib, "..\\..\\..\\Release\\NoMagic.lib")
#endif


int stringToInt(std::string const& str)
{
	int ret = -1;
	std::stringstream ss;
	ss << str;
	ss >> ret;
	return ret;
}

void __declspec(naked) redirectToMe()
{
	std::cout << " redirected " << std::endl;
	ExitThread(0);
}

int gInt = 5;

void __cdecl secondThread(void*)
{
	try
	{
		while(true)
		{
			std::cout << "gInt = " << gInt << std::endl;
			gInt += 5;
			Sleep(100);
		}
	}
	catch(...)
	{
		std::cout << "failed" << std::endl;
	}
}

LONG __stdcall Handle(EXCEPTION_POINTERS* ep)
{
	std::cout << "Exception!" << std::endl;

	return EXCEPTION_CONTINUE_EXECUTION;
}

void testRedirect()
{
	using namespace NoMagic::Wrappers;
	HANDLE thread = reinterpret_cast<HANDLE>(_beginthread(&secondThread, 0, nullptr));
	UINT_PTR addr = reinterpret_cast<UINT_PTR>(redirectToMe);

	Sleep(500);

	Thread::Redirect(thread, addr);

	std::cin.get();
	TerminateThread(thread, 0);
}

void testBreakpoints()
{
	using namespace NoMagic::Debugging;

	std::cout << "Hardware breakpoints (1) or memory breakpoints (2)? ";

	std::string line;
	std::getline(std::cin, line);
	int n = stringToInt(line);
	
	HANDLE thread = reinterpret_cast<HANDLE>(_beginthread(&secondThread, 0, nullptr));
	UINT_PTR addr = reinterpret_cast<UINT_PTR>(&gInt);

	SetLastError(0);
	try
	{
	if(n == 1)
		HardwareBreakpoint breakpoint(thread, addr, hType_write, hSize_4);
	else if(n == 2)
		MemoryBreakpoint breakpoint(addr);
	else
		std::cout << "Idiot..." << std::endl;
	}
	catch(NoMagic::MagicException const& e)
	{
		std::cout << e.GetMessage() << std::endl;
	}

	std::cin.get();
	TerminateThread(thread, 0);
}

int main()
{
	using namespace NoMagic::Debugging;
	ExceptionHandler hndler(Handle);

	std::cout << "Test breakpoints (1) or thread redirection (2)? ";

	NoMagic::Wrappers::Process::SetDebugPrivileges();

	std::string line;
	std::getline(std::cin, line);
	int n = stringToInt(line);

	if(n == 1)
		testBreakpoints();
	else if(n == 2)
		testRedirect();
	else
		std::cout << "Idiot..." << std::endl;
}