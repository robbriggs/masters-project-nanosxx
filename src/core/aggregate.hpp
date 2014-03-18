/*************************************************************************************/
/*      Copyright 2012 Barcelona Supercomputing Center                               */
/*                                                                                   */
/*      This file is part of the NANOS++ library.                                    */
/*                                                                                   */
/*      NANOS++ is free software: you can redistribute it and/or modify              */
/*      it under the terms of the GNU Lesser General Public License as published by  */
/*      the Free Software Foundation, either version 3 of the License, or            */
/*      (at your option) any later version.                                          */
/*                                                                                   */
/*      NANOS++ is distributed in the hope that it will be useful,                   */
/*      but WITHOUT ANY WARRANTY; without even the implied warranty of               */
/*      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                */
/*      GNU Lesser General Public License for more details.                          */
/*                                                                                   */
/*      You should have received a copy of the GNU Lesser General Public License     */
/*      along with NANOS++.  If not, see <http://www.gnu.org/licenses/>.             */
/*************************************************************************************/
#ifndef _NANOS_AGGREGATE_H
#define _NANOS_AGGREGATE_H

#include "aggregate_decl.hpp"

using namespace nanos;

inline void Aggregate::set_value(int value)
{
	_type = AGG_TYPE_INT;
	_value._int = value;
}

inline void Aggregate::set_value(float value)
{
	_type = AGG_TYPE_FLOAT;
	_value._float = value;
}

inline void Aggregate::set_value(double value)
{
	_type = AGG_TYPE_DOUBLE;
	_value._double = value;
}

inline void Aggregate::set_value(char value)
{
	_type = AGG_TYPE_CHAR;
	_value._char = value;
}

inline void Aggregate::set_value(void *value)
{
	_type = AGG_TYPE_VOID_PTR;
	_value._void_ptr = value;
}

inline Aggregate::AGG_TYPE Aggregate::get_type() const
{
	return _type;
}

inline int Aggregate::get_int() const
{
	return _value._int;
}

inline float Aggregate::get_float() const
{
	return _value._float;
}

inline double Aggregate::get_double() const
{
	return _value._double;
}

inline char Aggregate::get_char() const
{
	return _value._char;
}

inline void *Aggregate::get_void_ptr() const
{
	return _value._void_ptr;
}

inline bool Aggregate::operator==(const Aggregate &o) const
{
	if (_type != o._type)
		return false;

	switch (_type)
	{
	case AGG_TYPE_NONE:
		return true;
	case AGG_TYPE_INT:
		return get_int() == o.get_int();
	case AGG_TYPE_FLOAT:
		return get_float() == o.get_float();
	case AGG_TYPE_DOUBLE:
		return get_double() == o.get_double();
	case AGG_TYPE_CHAR:
		return get_char() == o.get_char();
	case AGG_TYPE_VOID_PTR:
		// Oh no...
		return false;
	}
	return false;
}

#endif
