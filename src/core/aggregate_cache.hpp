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
#ifndef _NANOS_AGGREGATE_CACHE_H
#define _NANOS_AGGREGATE_CACHE_H

#include "aggregate_cache_decl.hpp"

using namespace nanos;

template <class T>
static unsigned find_cardinality(const typename AggregateCache<T>::ArgumentList &list)
{
	typename AggregateCache<T>::ArgumentList::iterator it;
	unsigned count = 0;
	for (it = list.begin(); it != list.end(); it++)
	{
		if ((*it).get_type() != Aggregate::AGG_TYPE_NONE)
			++count;
	}

	return count;
}

template <class T>
AggregateCache<T>::AggregateCache(size_t size)
	: _size(size), _pairs(size)
{

}

template <class T>
void AggregateCache<T>::add(ArgumentList &key, T &value)
{
	const unsigned cardinality = find_cardinality<T>(key);
	PairsWithSameCardinality &cardinality_list = _pairs[cardinality];

	KeyValuePair new_pair(key, value);
	cardinality_list.push_back(new_pair);
}

template <class T>
inline static bool pair_matches(typename AggregateCache<T>::ArgumentList &key, typename AggregateCache<T>::ArgumentList &lookup)
{
	return (key.get_type() == Aggregate::AGG_TYPE_NONE || key == lookup);
}

template <class T>
bool AggregateCache<T>::lookup(std::vector<Aggregate> &lookup_value, T &result)
{
	typename AllPairs::reverse_iterator all_it;
	typename PairsWithSameCardinality::iterator it;
	for (all_it = _pairs.rbegin(); all_it != _pairs.rend(); all_it++)
	{
		// Iterating over cardinality lists
		PairsWithSameCardinality &cardinality_list = *all_it;
		for (it = cardinality_list.begin(); it != cardinality_list.end; it++)
		{
			KeyValuePair &pair = *it;
			if (pair_matches(pair.first, lookup_value))
			{
				result = pair.second;
				return true;
			}
		}
	}

	return false;
}

#endif
