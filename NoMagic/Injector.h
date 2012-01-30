#pragma once

namespace NoMagic
{
	class Injector
	{
	public:
		Injector();
		~Injector(void);

		static UINT_PTR Inject(Wrappers::Process& process, tstring const& dllPath, Wrappers::Module& outModule);
		static void CallStart(Wrappers::Process& process, UINT_PTR startAddress);
		static void UnloadDll(Wrappers::Process& process, Wrappers::Module& dll);
	};
}