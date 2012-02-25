#include "../../../NoMagic/Wrappers_Include.h"


#if (DEBUG)
#pragma comment(lib, "..\\..\\..\\Debug\\NoMagic_d.lib")
#else
#pragma comment(lib, "..\\..\\..\\Release\\NoMagic.lib")
#endif

int main()
{
	_using(namespace NoMagic::Wrappers)
	{
		auto currentProcess = Process::GetCurrentProcess();

		auto myModules = Module::GetModules(currentProcess);

		std::for_each(std::begin(myModules), std::end(myModules), [](Module& module)
		{
			module.ReadPEHeader();
			std::cout	<< "Module:\n"
						<< "\tName:\t\t" << module.GetName() << "\n"
						<< "\tPath:\t\t" << module.GetPath() << "\n"
						<< "\tSize:\t\t" << module.GetSize() << "\n"
						<< "\tBase Address:\t" << module.GetBaseAddress() << "\n"
						<< "\tHandle:\t\t" << module.GetHandle() << "\n\n"
						<< "\tSections:\n";

			auto sections = module.GetSections();
			module.IterateSections([&](IMAGE_SECTION_HEADER const& section)
			{
				bool hasCode = (section.Characteristics & IMAGE_SCN_CNT_CODE) != 0;
				std::cout << "\t\t\t0x" << std::hex << module.GetSectionAddress(section) << std::dec;
				std::cout << ": " << section.Name;
				std::cout << "  contains code: " << hasCode << "\n";
			});

			std::cout << "---------------------------------------------" << std::endl;
		});	
 
	} _endusing

	std::cin.get();
	return S_OK;
}