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
#include "../Debugging_Include.h"


namespace NoMagic
{
	namespace Debugging
	{
		ExceptionHandler::ExceptionHandler(exceptionCallback callback) : m_handler(callback)
			, m_ptr(SetUnhandledExceptionFilter(callback))
		{
		}

		ExceptionHandler::~ExceptionHandler(void)
		{
			if(m_ptr != nullptr)
				RemoveVectoredExceptionHandler(m_ptr);
		}
		
		ExceptionHandler ExceptionHandler::AddHandler(exceptionCallback callback)
		{
			return ExceptionHandler(SetUnhandledExceptionFilter(callback));
		}
	}
}