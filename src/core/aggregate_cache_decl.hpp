/*************************************************************************************/
/*      Copyright 2009 Barcelona Supercomputing Center                               */
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

#ifndef _NANOS_AGGREGATE_CACHE_DECL
#define _NANOS_AGGREGATE_CACHE_DECL

// This cache is a mapping of AggregateVector to templated type. The lookup function returns the
// value who's key is the 'best' 'match' to the lookup value. A match is defined as when all
// non-null elements are the equal and the lookup value has no null values where the key does not.
// Best is defined as the key with the largest number of non-null elements.

#include "aggregate.hpp"
#include <vector>

namespace nanos
{

   template <class T>
   class AggregateCache
   {
      typedef typename std::vector<Aggregate> ArgumentList;
      typedef typename std::pair<ArgumentList, T> KeyValuePair;
      typedef typename std::vector<KeyValuePair> PairsWithSameCardinality;
      typedef typename std::vector<PairsWithSameCardinality> AllPairs;

   private:
      size_t   _size;
      AllPairs _pairs;

   public:
      AggregateCache(size_t size);

      void add(ArgumentList &key, T &value);
      bool lookup(ArgumentList &lookup_value, T &result);
   };

};



#endif
