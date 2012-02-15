#include "../../../NoMagic/Wrappers_Include.h"
#if (DEBUG)
#pragma comment(lib, "..\\..\\..\\Debug\\NoMagic_d.lib")
#else
#pragma comment(lib, "..\\..\\..\\Debug\\NoMagic.lib")
#endif



int main()
{
	_using(namespace NoMagic::Wrappers)
	{
		auto currentProcess = Process::GetCurrentProcess();

		auto myModules = Module::GetModules(currentProcess);

		std::for_each(std::begin(myModules), std::end(myModules), [](Module const& module)
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