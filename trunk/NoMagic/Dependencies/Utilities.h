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
	template <typename value, typename bit>
	inline bool test_bit(const value _value, const bit _bit) { return ((_value & _bit) != 0); }

	template <typename value, typename bit>
	inline bool test_bit(const value _value, const bit _bit, unsigned int num) { return ((_value & (_bit<<num)) != 0); }


	
	template <typename value, typename bit>
	inline void set_bit(value& _value, const bit _bit) { _value |= _bit; }

	template <typename value, typename bit>
	inline void set_bit(value& _value, const bit _bit, unsigned int num) { _value |= (_bit<<num); }

	

	template <typename value, typename bit>
	inline void clear_bit(value& _value, const bit _bit) { _value &= ~_bit; }

	template <typename value, typename bit>
	inline void clear_bit(value& _value, const bit _bit, unsigned int num) { _value &= ~(_bit<<num); }
	

	template <typename T>
	inline T& get_member(void* ptr, UINT_PTR offset)
	{
		return *(reinterpret_cast<T*>( (UINT_PTR)ptr + offset ));
	}

	template <typename T>
	inline const T& get_member(const void* ptr, UINT_PTR offset)
	{
		return *(reinterpret_cast<const T*>( (UINT_PTR)ptr + offset ));
	}


	template <typename T>
	inline T make_ptr(UINT_PTR addr)
	{
		return reinterpret_cast<T>(addr);
	}


	template <typename T>
	inline void assign(void* ptr, const T& data)
	{
		*reinterpret_cast<T*>(ptr) = data;
	}

	template <typename Fun>
	inline Fun getVFunction(void* ptr, UINT_PTR index, UINT_PTR offset)
	{
		return reinterpret_cast<Fun>( get_member<void**>(ptr, offset)[index] );
	}
}