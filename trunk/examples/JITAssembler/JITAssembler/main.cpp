#include "../../../NoMagic/NoMagic_Include.h"

#if (DEBUG)
#pragma comment(lib, "..\\..\\..\\Debug\\NoMagic_d.lib")
#else
#pragma comment(lib, "..\\..\\..\\Debug\\NoMagic.lib")
#endif


int main(int argc, char** argv)
{
	std::string instructions =	"mov edx,09h;"
								"mov eax,01h;"
								"add eax, edx;";

	NoMagic::fasm::Assembler assembler;
	std::vector<BYTE> bytes;
	try
	{
		bytes = assembler.Assemble(instructions);
	}
	catch(NoMagic::MagicException const& e)
	{
		std::cout << e.GetMessage() << std::cout;
		std::cin.get();
		return E_FAIL;
	}

	std::for_each(std::begin(bytes), std::end(bytes), [](BYTE byte)
	{
		std::cout << std::hex << static_cast<unsigned int>(byte) << " ";
	});

	std::cin.get();

	return S_OK;
}