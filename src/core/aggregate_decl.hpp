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

   class Aggregate
   {
      public:
         enum AGG_TYPE
         {
            AGG_TYPE_NONE,
            AGG_TYPE_INT,
            AGG_TYPE_FLOAT,
            AGG_TYPE_DOUBLE,
            AGG_TYPE_CHAR,
            AGG_TYPE_VOID_PTR
         };

         union AGG_VALUE
         {
            int     _int;
            float   _float;
            double  _double;
            char    _char;
            void   *_void_ptr;

            AGG_VALUE();
            AGG_VALUE(int value)    : _int(value) {}
            AGG_VALUE(float value)  : _float(value) {}
            AGG_VALUE(double value) : _double(value) {}
            AGG_VALUE(char value)   : _char(value) {}
            AGG_VALUE(void *value)  : _void_ptr(value) {}
         };

      private:
         AGG_TYPE  _type;
         AGG_VALUE _value;

      public:
         Aggregate()             : _type(AGG_TYPE_NONE) {}
         Aggregate(int value)    : _type(AGG_TYPE_INT), _value(value) {}
         Aggregate(float value)  : _type(AGG_TYPE_FLOAT), _value(value) {}
         Aggregate(double value) : _type(AGG_TYPE_DOUBLE), _value(value) {}
         Aggregate(char value)   : _type(AGG_TYPE_CHAR), _value(value) {}
         Aggregate(void *value)  : _type(AGG_TYPE_VOID_PTR), _value(value) {}

         void set_value(int value);
         void set_value(float value);
         void set_value(double value);
         void set_value(char value);
         void set_value(void *value);

         AGG_TYPE get_type();

         int     get_int();
         float   get_float();
         double  get_double();
         char    get_char();
         void   *get_void_ptr();
   };

};



#endif
