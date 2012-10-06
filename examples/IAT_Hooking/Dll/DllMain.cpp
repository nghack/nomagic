/*	Copyright (C) 2012 Dennis Garske (a.k.a. Nightblizard)
	<http://nightblizard.blogspot.com> <nightblizard@gmail.com>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.


	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.


	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "../../../NoMagic/NoMagic_Include.h"

#if (DEBUG)
#ifdef _M_AMD64
#pragma comment(lib, "..\\..\\..\\x64\\Debug\\NoMagic_d.lib")
#else
#pragma comment(lib, "..\\..\\..\\Debug\\NoMagic_d.lib")
#endif
#else
#pragma comment(lib, "..\\..\\..\\Release\\NoMagic.lib")
#endif

HMODULE gHmod = nullptr;
NoMagic::Wrappers::Module gCurrentMod;
unsigned int gOldData = 0;


void thatPrint(int n)
{
	std::cout << "Trolol" << std::endl;
}

__declspec(dllexport) void detourEat()
{
	gOldData = (unsigned int)NoMagic::Wrappers::Memory::DetourEAT("?print@@YAXH@Z", (PBYTE)thatPrint, gCurrentMod);
}

__declspec(dllexport) void undoDetour()
{
	NoMagic::Wrappers::Memory::RemoveEATDetour("?print@@YAXH@Z", (PBYTE)gOldData, gCurrentMod);
}

__declspec(dllexport) void print(int n)
{
	gCurrentMod = NoMagic::Wrappers::Module(NoMagic::Wrappers::Process::GetCurrentProcess(), gHmod);
	gCurrentMod.ReadPEHeader();

	std::cout << "\"n\" equals " << n << std::endl;
}


BOOL WINAPI DllMain(__in HMODULE hModule, __in DWORD fdwReason, __in LPVOID lpvReserved)
{
	if(fdwReason == DLL_PROCESS_ATTACH)
		gHmod = hModule;

	return TRUE;
}