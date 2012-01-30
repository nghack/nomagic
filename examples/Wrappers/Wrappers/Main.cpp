#include "../../../NoMagic/Wrappers_Include.h"
#if (DEBUG)
#pragma comment(lib, "NoMagic_d.lib")
#else
#pragma comment(lib, "NoMagic.lib")
#endif



int main()
{
	_using(namespace NoMagic::Wrappers)
	{
		Process currentProcess = Process::GetCurrentProcess();

		std::vector<Module> myModules = Module::GetModules(currentProcess);

		std::for_each(myModules.begin(), myModules.end(), [](Module& module)
		{
			std::cout	<< "Module:\n"
						<< "\tName:\t\t" << module.GetName() << "\n"
						<< "\tPath:\t\t" << module.GetPath() << "\n"
						<< "\tSize:\t\t" << module.GetSize() << "\n"
						<< "\tBase Address:\t" << module.GetBaseAddress() << "\n"
						<< "\tHandle:\t\t" << module.GetHandle() << "\n"
						<< std::endl;
		});
	} _endusing

	std::cin.get();
	return S_OK;
}