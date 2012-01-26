#include "../../../NoMagic/NoMagic_Include.h"
#if (DEBUG)
#pragma comment(lib, "NoMagic_d.lib")
#else
#pragma comment(lib, "NoMagic.lib")
#endif

int main(int argc, char** argv)
{
	NoMagic::NoMagic nm;
	//Wir geben unserem Programm Lese und Schreibrechte für den Arbeitsspeicher...
	nm.SetDebugPrivileges();
	//... und öffnen den Prozess!

	try
	{
		int process = nm.OpenProcess(_T("Tests.exe"));
	
		//Der Prozess konnte nicht geöffnet werden!
		if(!process)
		{
			std::cout << "Can not open Tests.exe, did you opned it?" << std::endl;
			std::cin.get();
			return -1;
		}
		//Unser Suchmuster
		BYTE* pattern = (BYTE*)	"\x52" //PUSH EDX (Maske x (wichtig))
								"\x50" //PUSH EAX (Maske x (wichtig))
								"\xA3" //MOV OV EAX (Maske x (wichtig))
								"\x00\x00\x00\x00"; //Die Adresse der findMe Variabel (Inhalt unbekannt, desshalb 0000. Maske 4x ? (unwichtig))
	
		//Lasst die Suche beginnen!
		DWORD address = nm.SearchPattern(pattern, NoMagic::PatternMask::Make("xxx????", 'x'));
	
		//Wenn die Suche erfolgreich war, gibt die Funktion einen Wert != 0 zurück
		if(address)
		{
			//Die ersten drei Anweisungen überspringen und nur die Adresse auslesen
			address += 3;
			//Wir lesen die gefundene Adresse aus, um an den Pointer zur Variable zu kommen.
			DWORD variablePtr = nm.Read<DWORD>(address, false);
			//Wir überschreiben die gefundene Variable mit dem Werten 100
			nm.Write(variablePtr, 100, false);
			std::cout << "Pattern found and variable rewritten to 100!" << std::endl;
		}
		//Suche war nicht erfolgreich! :(
		else
		{
			std::cout << "Pattern did not match!\nDid you build in Release Mode?" << std::endl;
		}
	}
	catch(NoMagic::MagicException& e)
	{
		std::cout << e.GetMessage() << std::endl;
		std::cin.get();
		return E_FAIL;
	}

	std::cin.get();
	return S_OK;
}