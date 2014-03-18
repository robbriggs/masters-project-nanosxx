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

#ifndef _NANOS_AGGREGATE_DECL
#define _NANOS_AGGREGATE_DECL

#include <stdint.h>

namespace nanos
{

   class AggregateType
   {
   public:
      enum BaseType
      {
         NONE,
         INT,
         FLOAT,
         DOUBLE,
         CHAR,
         VOID
      };

   private:
      BaseType _type;
      unsigned _ptr_depth;
      unsigned _size;

   public:
      inline AggregateType(BaseType type, unsigned ptr_depth, unsigned size);
      inline BaseType type() const;
      inline unsigned ptr_depth() const;
      inline unsigned size() const;
      inline bool operator==(const AggregateType& o) const;
   };

   class Aggregate
   {
   private:
      union
      {
         int     _int;
         float   _float;
         double  _double;
         char    _char;

         // Pointer to place actual value is in memory
         int    *_int_ptr;
         float  *_float_ptr;
         double *_double_ptr;
         char   *_char_ptr;
      } _value;

      // Pointer valid inside code
      void* _valid_ptr;

      AggregateType _type;

      Aggregate(void *value, AggregateType::BaseType base_type, unsigned ptr_depth = 0, unsigned size = 0);
      Aggregate(void *value, AggregateType type);
      ~Aggregate();

      bool operator==(const Aggregate& o) const;

   private:
      void construct(void *value);


   };

};



#endif
