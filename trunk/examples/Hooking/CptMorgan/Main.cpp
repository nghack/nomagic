#include "../../../NoMagic/NoMagic_Include.h"
#if (DEBUG)
#pragma comment(lib, "..\\..\\..\\Debug\\NoMagic_d.lib")
#else
#pragma comment(lib, "..\\..\\..\\Release\\NoMagic.lib")
#endif

//Funktionstyp
typedef int(WINAPI* tMessageBox)(HWND, LPCSTR, LPCSTR, UINT);
//Hier speichern wir die originale Funktion
tMessageBox origMessageBox = nullptr;

//Unsere MessageBox Funktion
int WINAPI MyMessageBox(HWND wnd, LPCSTR text, LPCSTR caption, UINT type)
{
	//Wir rufen die originale Funktion mit geänderten Text auf
	return origMessageBox(wnd, "ARRRRRR", caption, type);
}

int main(int argc, char** argv)
{
	NoMagic::NoMagic nm;

	//Wir speichern die Adresse der Funktion, die wir hooken möchten
	auto msgBoxA = reinterpret_cast<PBYTE>(MessageBoxA);
	//Die Funktion, die anstelle der originalen Funktion aufgerufen werden soll
	auto myMsgBox = reinterpret_cast<PBYTE>(MyMessageBox);

	//Wir schreiben den Hook und merken uns wo die originale Funktion liegt...
	auto origMsgBox = nm.HookFunction(msgBoxA, myMsgBox);
	//... und teilen dies origMessageBox mit
	origMessageBox = reinterpret_cast<tMessageBox>(origMsgBox);

	//Mal schauen, ob es funktioniert hat!
	MessageBoxA(nullptr, "Test 123", nullptr, 0);

	//Lasst uns den Hook wieder entfernen!
	nm.UnhookFunction(origMsgBox, myMsgBox);

	//Nobody is perfect:
	MessageBoxA(nullptr, "Hook weg?", nullptr, 0);


	std::cin.get();
}