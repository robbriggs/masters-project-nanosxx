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



inline AggregateType::AggregateType(BaseType type, unsigned ptr_depth, unsigned size)
	: _type(type), _ptr_depth(ptr_depth), _size(size) {}

inline AggregateType::BaseType AggregateType::type() const
{
	return _type;
}

inline unsigned AggregateType::ptr_depth() const
{
	return _ptr_depth;
}

inline unsigned AggregateType::size() const
{
	return _size;
}

inline bool AggregateType::operator==(const AggregateType::AggregateType& o) const
{
	return type() == o.type() && size() == o.size() && ptr_depth() == o.ptr_depth();
}

// ---

inline Aggregate::Aggregate(void* value, AggregateType::BaseType base_type, unsigned ptr_depth, unsigned size)
   : _type(base_type, ptr_depth, size)
{
	construct(value);
}

inline Aggregate::Aggregate(void *value, AggregateType type)
	: _type(type)
{
	construct(value);
}

inline void Aggregate::construct(void *value)
{
	const AggregateType::BaseType base_type = _type.type();
	const unsigned ptr_depth = _type.ptr_depth();
	const unsigned size = _type.size();

   if (base_type == AggregateType::NONE)
      return;

   if (ptr_depth == 0)
   {
      // Just a primitive value
      switch (base_type)
      {
      case (AggregateType::INT):
         _value._int = *(int*)value;
         break;
      case (AggregateType::FLOAT):
         _value._float = *(float*)value;
         break;
      case (AggregateType::DOUBLE):
         _value._double = *(double*)value;
         break;
      case (AggregateType::CHAR):
         _value._char = *(char*)value;
         break;
      case (AggregateType::NONE):
      case (AggregateType::VOID):
         break;
      }
      _valid_ptr = &_value;
   }else{
      // A pointer
      switch (base_type)
      {
      case (AggregateType::INT):
         _value._int_ptr = new int[size];
         break;
      case (AggregateType::FLOAT):
         _value._float_ptr = new float[size];
         break;
      case (AggregateType::DOUBLE):
         _value._double_ptr = new double[size];
         break;
      case (AggregateType::CHAR):
         _value._char_ptr = new char[size];
         break;
      case (AggregateType::NONE):
      case (AggregateType::VOID):
         break;
      }

      _valid_ptr = &_value;
      for (unsigned i = 1; i < ptr_depth; ++i)
      {
         void* new_ptr = new (void*)();
         *(void**)new_ptr = _valid_ptr;
         _valid_ptr = new_ptr;
      }
   }
}

inline Aggregate::~Aggregate()
{
	const unsigned depth = _type.ptr_depth();
	if (depth != 0)
	{
		const AggregateType::BaseType base_type = _type.type();
		void *current = _valid_ptr;
		void *next = *(void**)_valid_ptr;

		for (unsigned i = 1; i < depth; ++i)
		{
			switch (base_type)
			{
			case (AggregateType::INT):
				delete (int*) current;
				break;
			case (AggregateType::FLOAT):
				delete (float*) current;
				break;
			case (AggregateType::DOUBLE):
				delete (double*) current;
				break;
			case (AggregateType::CHAR):
				delete (char*) current;
				break;
			case (AggregateType::NONE):
				case (AggregateType::VOID):
				break;
			}
			delete (int*) current;
			current = next;
			next = *(void**)current;
		}
	}
}

template <class T>
static inline bool aggregate_data_match(const unsigned size, const void *mine_void, const void *theirs_void)
{
	const T *mine = static_cast<const T*>(mine_void);
	const T *theirs = static_cast<const T*>(theirs_void);

	for (unsigned i = 0; i < size; ++i)
	{
		if (mine[i] != theirs[i])
			return false;
	}
	return true;
}

inline bool Aggregate::operator==(const Aggregate& o) const
{
	if (!( _type == o._type ))
		return false;

	const AggregateType::BaseType base_type = _type.type();
	const unsigned size = _type.size();

	switch (base_type)
	{
	case (AggregateType::INT):
		return aggregate_data_match<int>(size, _valid_ptr, o._valid_ptr);
	case (AggregateType::FLOAT):
		return aggregate_data_match<float>(size, _valid_ptr, o._valid_ptr);
	case (AggregateType::DOUBLE):
		return aggregate_data_match<double>(size, _valid_ptr, o._valid_ptr);
	case (AggregateType::CHAR):
		return aggregate_data_match<char>(size, _valid_ptr, o._valid_ptr);
	case (AggregateType::NONE):
	case (AggregateType::VOID):
		return false; // Shouldn't get here
	}
}
      

#endif
