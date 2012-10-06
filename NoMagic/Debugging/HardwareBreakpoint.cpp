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
		HardwareBreakpoint::HardwareBreakpoint(HANDLE hThread, UINT_PTR address, HardwareBreakpoint_Type_Code type, HardwareBreakpoint_Size size) :
				Breakpoint(address),
				m_thread(hThread)
		{
			CONTEXT con = {CONTEXT_DEBUG_REGISTERS};
			SuspendThread(hThread);
			GetThreadContext(hThread, &con);

			if(CheckBreakpoint(con, address))
			{
				set_bit(con.Dr7, type, 16 + m_register * 4);
				set_bit(con.Dr7, size, 18 + m_register * 4);
				set_bit(con.Dr7, 1,  m_register * 2);

				con.ContextFlags = CONTEXT_DEBUG_REGISTERS;
			}

			SetThreadContext(hThread, &con);
			ResumeThread(hThread);
		}

		HardwareBreakpoint::~HardwareBreakpoint(void)
		{
		}

		bool HardwareBreakpoint::CheckBreakpoint(CONTEXT& con, UINT_PTR address)
		{
			if(con.Dr0 == 0)
			{
				m_register = 0;
				con.Dr0 = address;
			}
			else if(con.Dr1 == 0)
			{
				m_register = 1;
				con.Dr1 = address;
			}
			else if(con.Dr2 == 0)
			{
				m_register = 2;
				con.Dr2 = address;
			}
			else if(con.Dr3 == 0)
			{
				m_register = 3;
				con.Dr3 = address;
			}
			else
				return false;

			return true;
		}

		void HardwareBreakpoint::Remove()
		{
				CONTEXT con = {CONTEXT_DEBUG_REGISTERS};
				SuspendThread(m_thread);
				GetThreadContext(m_thread, &con);

				if(m_register == 0)
					con.Dr0 = 0;
				else if(m_register == 1)
					con.Dr1 = 0;
				else if(m_register == 2)
					con.Dr2 = 0;
				else if(m_register == 3)
					con.Dr3 = 0;

				set_bit(con.Dr7, 0, 16 + m_register*4);
				set_bit(con.Dr7, 0, 18 + m_register*4);
				set_bit(con.Dr7, 0, m_register*2);

				SetThreadContext(m_thread, &con);
				ResumeThread(m_thread);
		}
	}
}