/*	Ijiziert eine .dll Datei Eurer Wahl in Euer WoW. 

	Zieht einfach injectMe.dll auf die DoIt.exe,
	während Euer 3.3.5a WoW läuft. Das Ergebnis
	könnt Ihr Euch hier anschauen:
	http://www.youtube.com/watch?v=o6MTRefO2ms
*/

#include "NoMagic.h"
#pragma comment(lib, "NoMagic.lib")

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		std::cout << "DoIt.exe C:\Path\To\dll\name.dll" << std::endl;
	}

	std::cout << "Injecting " << argv[1] << std::endl;
	NoMagic::NoMagic nm;
	nm.SetDebugPrivileges();
	nm.OpenProcess(_T("Wow.exe"));
	nm.InjectDll(_T(argv[1]));

	std::cout << "Done!";

	std::cin.get();
}
