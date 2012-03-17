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

#pragma once

namespace NoMagic
{
	namespace Algorithm
	{
		class BoyerMooreHorspool : public IPatternAlgorithm
		{
		public:
			BoyerMooreHorspool() {}

			/**
			Wendet den Algorithmus an.
			\param pattern Das Muster, nach dem gesucht werden soll.
			\param mask Die Maske, um festzulegen welche Bytes übereinstimmen müssen
			\param begin Pointer zum ersten Byte
			\param end Pointer zum letzten Byte
			\return Adresse, an der das Pattern gefunden wurde oder NULL, wenn nichts gefunden wurde.
			*/
			UINT_PTR Utilize(byteString pattern, std::vector<bool> const& mask, BYTE* begin, BYTE* end)
			{
				auto length = pattern.length() - 1;
				auto last = length - 1;
				auto init = length;
				auto skipTable = new size_t[256];

				for(size_t i = last - 1; i >= 0; --i)
				{
					if(mask[i] == false)
					{
						init = last - i;
						break;
					}
				}

				for(size_t i = 0; i < 256; ++i)
					skipTable[i] = init;

				for(size_t i = 0; i < last; ++i)
				{
					BYTE v = pattern[i];
					size_t newSkip = last - i;
					size_t oldSkip = skipTable[v];
					if(newSkip < oldSkip)
						skipTable[v] = newSkip;
				}

				end -= last;
				size_t counter = 0;
				while(begin < end)
				{
					for(size_t i = last; i >= 0; --i)
					{
						if(mask[i] && begin[i] != pattern[i])
							break;

						if(i == 0)
							return static_cast<INT_PTR>(counter);
					}
					counter += skipTable[begin[last]];
					begin += skipTable[begin[last]];
				}
				return 0;
			}
		};
	}
}