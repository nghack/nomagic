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

#include "../Wrappers_Include.h"


namespace NoMagic
{
	namespace Wrappers
	{
		Handle::Handle() :
			m_handle(nullptr)
		{
		}

		Handle::Handle(const Handle& right)
		{
			throw MagicException(_T("One does not simply copy a handle!"));
		}

		Handle::Handle(HANDLE handle) :
			m_handle(handle)
		{
		}

		Handle::Handle(Handle&& right) :
			m_handle(nullptr)
		{
			std::swap(m_handle, right.m_handle);
		}

		Handle::~Handle()
		{
			CloseHandle(m_handle);
		}

		bool Handle::operator==(const Handle& right) const
		{
			return m_handle == right.m_handle;
		}
		
		bool Handle::operator==(const nullptr_t right) const
		{
			return m_handle == right;
		}

		bool Handle::operator!=(const Handle& right) const
		{
			return m_handle != right.m_handle;
		}
		
		bool Handle::operator!=(const nullptr_t right) const
		{
			return m_handle != right;
		}

		Handle& Handle::operator=(const HANDLE handle)
		{
			m_handle = handle;
			return *this;
		}

		Handle::operator HANDLE() const
		{
			return static_cast<HANDLE>(m_handle);
		}
	}
}