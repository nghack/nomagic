#include "../../../NoMagic/NoMagic_Include.h"

#if (DEBUG)
#pragma comment(lib, "..\\..\\..\\Debug\\NoMagic_d.lib")
#else
#pragma comment(lib, "..\\..\\..\\Release\\NoMagic.lib")
#endif


int main(int argc, char** argv)
{
	std::string instructions =	"mov edx,09h;"
								"mov eax,01h;"
								"add eax, edx;";
	try
	{
		NoMagic::fasm::Assembler assembler;
		std::vector<BYTE> bytes;
		bytes = assembler.Assemble(instructions);

		std::for_each(std::begin(bytes), std::end(bytes), [](BYTE byte)
		{
			std::cout << std::hex << static_cast<unsigned int>(byte) << " ";
		});
	}
	catch(NoMagic::MagicException const& e)
	{
		std::cout << e.GetMessage() << std::cout;
		std::cin.get();
		return E_FAIL;
	}

	std::cin.get();

	return S_OK;
}