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

#include "STD_Include.h"
#include "Dependencies/Types.h"
#include "Dependencies/Utilities.h"
#include "Algorithm_Include.h"

namespace NoMagic
{
	namespace Debugging
	{
		typedef PVECTORED_EXCEPTION_HANDLER exceptionCallback;

		enum HardwareBreakpoint_Type_Code : unsigned char
		{
			hType_code = 0,
			hType_write = 1,
			hType_readwrite = 3,
		};

		enum HardwareBreakpoint_Size : unsigned char
		{
			hSize_1 = 0,
			hSize_2 = 1,
			hSize_4 = 3,
			hSize_8 = 2,
		};

		class MemoryBreakpoint;
		class HardwareBreakpoint;
		class Breakpoint;
		class ExceptionHandler;
	}
}

#include "Debugging/ExceptionHandler.h"
#include "Debugging/Breakpoint.h"
#include "Debugging/HardwareBreakpoint.h"
#include "Debugging/MemoryBreakpoint.h"