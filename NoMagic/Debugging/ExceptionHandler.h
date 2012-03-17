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
	namespace Debugging
	{
		class ExceptionHandler
		{
		private:
			exceptionCallback m_handler;
			LPTOP_LEVEL_EXCEPTION_FILTER m_ptr;
			
		public:
			ExceptionHandler(exceptionCallback callback);
			~ExceptionHandler(void);

			static ExceptionHandler AddHandler(exceptionCallback callback);
		};
	}
}