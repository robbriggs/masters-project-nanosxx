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

#include <iostream>
#include "simpleallocator.hpp"

using namespace nanos;

SimpleAllocator::SimpleAllocator( uint64_t baseAddress, size_t len ) : _baseAddress( baseAddress )
{
   _freeChunks[ baseAddress ] = len;
}

void SimpleAllocator::init( uint64_t baseAddress, size_t len )
{
   _baseAddress = baseAddress;
   _freeChunks[ baseAddress ] = len;
}

void * SimpleAllocator::allocate( size_t size )
{
   SegmentMap::iterator mapIter = _freeChunks.begin();
   void * retAddr = (void *) 0;

   while( mapIter != _freeChunks.end() && mapIter->second < size )
   {
      mapIter++;
   }
   if ( mapIter != _freeChunks.end() ) {

      uint64_t targetAddr = mapIter->first;
      size_t chunkSize = mapIter->second;

      _freeChunks.erase( mapIter );

      //add the chunk with the new size (previous size - requested size)
      _freeChunks[ targetAddr + size ] = chunkSize - size ;
      _allocatedChunks[ targetAddr ] = size;

      retAddr = ( void * ) targetAddr;
   }
   else {
      std::cerr << "Could not get a chunk of " << size << " bytes" << std::endl;
   }

   return retAddr;
}

void SimpleAllocator::free( void *address )
{
   SegmentMap::iterator mapIter = _allocatedChunks.find( ( uint64_t ) address );
   size_t size = mapIter->second;

   _allocatedChunks.erase( mapIter );

   if ( _freeChunks.size() > 0 ) {
      mapIter = _freeChunks.lower_bound( ( uint64_t ) address );

      //case where address is the highest key, check if it can be merged with the previous chunk
      if ( mapIter == _freeChunks.end() ) {
         mapIter--;
         if ( mapIter->first + mapIter->second == ( uint64_t ) address ) {
            mapIter->second += size;
         }
      }
      //address is not the hightest key, check if it can be merged with the previous and next chunks
      else if ( _freeChunks.key_comp()( ( uint64_t ) address, mapIter->first ) ) {
         size_t totalSize = size;

         //check next chunk
         if ( mapIter->first == ( ( uint64_t ) address ) + size ) {
            totalSize += mapIter->second;
            _freeChunks.erase( mapIter );
            mapIter = _freeChunks.insert( mapIter, SegmentMap::value_type( ( uint64_t ) address, totalSize ) );
         }

         //check previous chunk
         mapIter--;
         if ( mapIter->first + mapIter->second == ( uint64_t ) address ) {
            //if totalSize > size then the next chunk was merged
            if ( totalSize > size ) {
               mapIter->second += totalSize;
               mapIter++;
               _freeChunks.erase( mapIter );
            }
            else {
               mapIter->second += size;
            }
         }
      }
      //duplicate key, error
      else {
         std::cerr << "Duplicate entry in segment map, addr " << address << "." << std::endl;
      }
   }
   else {
      _freeChunks[ ( uint64_t ) address ] = size;
   }
}
