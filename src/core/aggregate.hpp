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

inline Aggregate::AGG_TYPE Aggregate::get_type()
{
	return _type;
}

inline int Aggregate::get_int()
{
	return _value._int;
}

inline float Aggregate::get_float()
{
	return _value._float;
}

inline double Aggregate::get_double()
{
	return _value._double;
}

inline char Aggregate::get_char()
{
	return _value._char;
}

inline void *Aggregate::get_void_ptr()
{
	return _value._void_ptr;
}

#endif
