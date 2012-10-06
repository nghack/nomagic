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
#include "../../../NoMagic/Injector_Include.h"

#if (DEBUG)
#ifdef _M_AMD64
#pragma comment(lib, "..\\..\\..\\x64\\Debug\\NoMagic_d.lib")
#pragma comment(lib, "..\\x64\\Debug\\Dll.lib")
#else
#pragma comment(lib, "..\\..\\..\\Debug\\NoMagic_d.lib")
#pragma comment(lib, "..\\Debug\\Dll.lib")
#endif
#else
#pragma comment(lib, "..\\..\\..\\Release\\NoMagic.lib")
#pragma comment(lib, "..\\Release\\Dll.lib")
#endif

__declspec(dllimport) void print(int n);
__declspec(dllimport) void detourEat();
__declspec(dllimport) void undoDetour();


void myPrint(int n)
{
	std::cout << "WASD" << n << std::endl;
}

int main()
{
	using NoMagic::Wrappers::Memory;

	std::cout << "Detouring IAT:\n";

	print(15);
	
	auto process = NoMagic::Wrappers::Process::GetCurrentProcess();
	auto moduleHandle = GetModuleHandle(nullptr);
	auto module = NoMagic::Wrappers::Module(process, moduleHandle);
	module.ReadPEHeader();
	
	//Detour the imported function "print"
	auto origFunc = Memory::DetourIAT("?print@@YAXH@Z", reinterpret_cast<PBYTE>(myPrint), module);

	print(15);

	//Remove the detour
	Memory::DetourIAT("?print@@YAXH@Z", reinterpret_cast<PBYTE>(origFunc), module);

	std::cout << "\nDetouring EAT:\n";

	//Detour the function within the dll
	detourEat();
	print(15);

	//Remove the detour
	undoDetour();
	print(15);
	
	std::getline(std::cin, std::string());
}