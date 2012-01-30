#include "../../../NoMagic/NoMagic_Include.h"
#if (DEBUG)
#pragma comment(lib, "NoMagic_d.lib")
NoMagic::byteString bString = "\xC6\x05\x01\x01\x01\x01\x01\xC7\x05\x01\x01\x01\x01\x01\x01\x01\x01\xC7\x45\xF8\x01\x01\x01\x01";
std::vector<bool> patternMask = NoMagic::PatternMask::Make("xx????xxx????????xxx????", 'x');

int findMeOffset = 9;
int keepRunningOffset = 2;
#else
#pragma comment(lib, "NoMagic.lib")
NoMagic::byteString bString = "\x68\x01\x01\x01\x01\xC6\x05\x01\x01\x01\x01\x01";
std::vector<bool> patternMask = NoMagic::PatternMask::Make("x????xx????x", 'x');
int findMeOffset = 1;
int keepRunningOffset = 7;
#endif

extern "C" void __declspec(dllexport) Start()
{
	NoMagic::NoMagic noMagic;

	try
	{
		//Open up the process!
		noMagic.OpenProcess(_T("HackMe.exe"));
		noMagic.SetInProcess(true);

		//Lets find the pattern
		UINT_PTR pattern = noMagic.SearchPattern(bString, patternMask);
		if(pattern != 0)
		{
			//Get the pointer to the variable
			UINT_PTR tmpPtr = *(reinterpret_cast<UINT_PTR*>(pattern+findMeOffset));
			UINT_PTR* findMe = reinterpret_cast<UINT_PTR*>(tmpPtr);

			//Lets set the value to 100!
			*findMe = 100;

			//Now let us end the loop by setting keepRunning to false!
			tmpPtr = *(reinterpret_cast<UINT_PTR*>(pattern+keepRunningOffset));
			bool* keepRunning = reinterpret_cast<bool*>(tmpPtr);
			*keepRunning = false;
		}
		else
		{
			std::cout << "Can not find pattern! Do you use Visual Studio 2010?" << std::endl;
		}
	}
	catch(NoMagic::MagicException& e)
	{
		std::cout << e.GetMessage() << std::endl;
	}
}


BOOL WINAPI DllMain(__in HINSTANCE hInstDll, __in DWORD fdwReason, __in LPVOID lpvReserved)
{
	return TRUE;
}