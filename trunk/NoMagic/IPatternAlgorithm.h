#pragma once

namespace NoMagic
{
	namespace Algorithm
	{
		class IPatternAlgorithm : public IAlgorithm 
		{
		public:
			IPatternAlgorithm(void) {}
			virtual ~IPatternAlgorithm(void) {}
			
			/**
			Wendet den Algorithmus an.
			\param pattern Das Muster, nach dem gesucht werden soll.
			\param mask Die Maske, um festzulegen welche Bytes übereinstimmen müssen
			\param begin Pointer zum ersten Byte
			\param end Pointer zum letzten Byte
			\return Adresse, an der das Pattern gefunden wurde oder NULL, wenn nichts gefunden wurde.
			*/
			virtual UINT_PTR Utilize(byteString pattern, std::vector<bool>& mask, BYTE* begin, BYTE* end) = 0;
		};
	}
}