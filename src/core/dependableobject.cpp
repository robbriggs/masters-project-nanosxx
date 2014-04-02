
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

#include "dependableobject.hpp"
#include "instrumentation.hpp"
#include "system.hpp"

using namespace nanos;

void DependableObject::finished ( )
{
   if ( --_references == 0) {
      DependableObject& depObj = *this;

      // This step guarantees that any Object that wants to add depObj as a successor has done it
      // before we continue or, alternatively, won't do it.
      //DependableObject::RegionContainer const &outs = depObj.getWrittenRegions();
      TargetVector const &outs = depObj.getWrittenTargets();
      DependenciesDomain *domain = depObj.getDependenciesDomain();
      if ( domain != 0 && outs.size() > 0 ) {
         SyncRecursiveLockBlock lock1( domain->getInstanceLock() ); // This is needed here to avoid a dead-lock
         SyncLockBlock lock2( depObj.getLock() );
         for ( unsigned int i = 0; i < outs.size(); i++ ) {
            BaseDependency const &target = *outs[i];
            
            domain->deleteLastWriter ( depObj, target );
         }
      }
      
      //  Delete depObj from all trackableObjects it reads 
      if ( domain != 0 ) {
         DependableObject::TargetVector const &reads = depObj.getReadTargets();
         for ( DependableObject::TargetVector::const_iterator it = reads.begin(); it != reads.end(); it++ ) {
            BaseDependency const & target = *(*it);
            
            domain->deleteReader ( depObj, target );
         }
      }

      DependableObject::DependableObjectDependencyDescVector &succ = depObj.getSuccessors();

      for ( DependableObject::DependableObjectDependencyDescVector::iterator it = succ.begin(); it != succ.end(); it++ ) {

         NANOS_INSTRUMENT ( instrument ( *((*it).object()) ); ) 

         const DependableObjectDependencyDesc &desc = *it;
         if (desc.active())
            desc.object()->decreasePredecessors( this );

      }
   }
}

// RESEARCH THIS FUNCTION
DependableObject * DependableObject::releaseImmediateSuccessor ( DependableObjectPredicate &condition )
{
   DependableObject * found = NULL;

   DependableObject::DependableObjectDependencyDescVector &succ = getSuccessors();
   DependableObject::DependableObjectDependencyDescVector incorrectlyErased;

   {
      SyncLockBlock lock( this->getLock() );
      // NOTE: it gets incremented in the erase
      for ( DependableObject::DependableObjectDependencyDescVector::iterator it = succ.begin(); it != succ.end(); ) {
         // Is this an immediate successor?
         DependableObject *object = (*it).object();
         bool active = (*it).active();
         if ( object->numPredecessors() == 1 && condition(*object) && !(object->waits()) ) {
            // remove it
            found = object;
            if (object->isSubmitted()) {
               succ.erase(it++);
               if ( found->numPredecessors() != 1 ) {
                  DependableObjectDependencyDesc new_val(found, active);
                  incorrectlyErased.insert( new_val );
                  found = NULL;
               } else {
                  NANOS_INSTRUMENT ( instrument ( *found ); )
                  DependenciesDomain::decreaseTasksInGraph();
                  break;
               }
            }
         } else {
            it++;
         }
      }
      for ( DependableObject::DependableObjectDependencyDescVector::iterator it = incorrectlyErased.begin(); it != incorrectlyErased.end(); it++) {
         succ.insert(*it);
      }
   }
   return found;
}
