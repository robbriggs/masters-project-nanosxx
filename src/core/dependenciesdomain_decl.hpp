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

#ifndef _NANOS_DEPENDENCIES_DOMAIN_DECL
#define _NANOS_DEPENDENCIES_DOMAIN_DECL
#include <stdlib.h>
#include <map>
#include <list>
#include <vector>
#include <string>
#include "atomic_decl.hpp"
#include "dependableobject_decl.hpp"
#include "trackableobject_decl.hpp"
#include "dependency_decl.hpp"
#include "compatibility.hpp"
#include "schedule_fwd.hpp"


namespace nanos
{

  /*! \class DependenciesDomain
   *  \brief Each domain is an independent context in which dependencies between DependableObject are managed
   */
   class DependenciesDomain
   {
      private:
         typedef TR1::unordered_map<void *, TrackableObject*> DepsMap; /**< Maps addresses to Trackable objects */

         static Atomic<int>   _atomicSeed;           /**< ID seed for the domains */
         int                  _id;                   /**< Domain's id */
         unsigned int         _lastDepObjId;         /**< Id to be given to the next submitted DependableObject */
         DepsMap              _addressDependencyMap; /**< Used to track dependencies between DependableObject */
         static Atomic<int>   _tasksInGraph;         /**< Current number of tasks in the graph */
         static Lock          _lock;

        /*! \brief Looks for the dependency's address in the domain and returns the trackableObject associated.
         *  \param dep Dependency to be checked.
         *  \sa Dependency TrackableObject
         */
         TrackableObject* lookupDependency ( const Dependency &dep );
        /*! \brief Assigns the DependableObject depObj an id in this domain and adds it to the domains dependency system.
         *  \param depObj DependableObject to be added to the domain.
         *  \param begin Iterator to the start of the list of dependencies to be associated to the Dependable Object.
         *  \param end Iterator to the end of the mentioned list.
         *  \param callback A function to call when a WD has a successor [Optional].
         *  \sa Dependency DependableObject TrackableObject
         */
         template<typename iterator>
         void submitDependableObjectInternal ( DependableObject &depObj, iterator begin, iterator end, SchedulePolicySuccessorFunctor* callback );

      private:
        /*! \brief DependenciesDomain copy assignment operator (private)
         */
         const DependenciesDomain & operator= ( const DependenciesDomain &depDomain );
      public:

        /*! \brief DependenciesDomain default constructor
         */
         DependenciesDomain ( ) :  _id( _atomicSeed++ ), _lastDepObjId ( 0 ), _addressDependencyMap( ) {}

        /*! \brief DependenciesDomain copy constructor
         */
         DependenciesDomain ( const DependenciesDomain &depDomain )
            : _id( _atomicSeed++ ), _lastDepObjId ( depDomain._lastDepObjId ),
              _addressDependencyMap ( depDomain._addressDependencyMap ) {}

        /*! \brief DependenciesDomain destructor
         */
         ~DependenciesDomain ( );

        /*! \brief get object's id
         */
         int getId ();

        /*! \brief Assigns the DependableObject depObj an id in this domain and adds it to the domains dependency system.
         *  \param depObj DependableObject to be added to the domain.
         *  \param deps List of dependencies to be associated to the Dependable Object.
         *  \param callback A function to call when a WD has a successor [Optional].
         *  \sa Dependency DependableObject TrackableObject
         */
         void submitDependableObject ( DependableObject &depObj, std::vector<Dependency> &deps, SchedulePolicySuccessorFunctor* callback = NULL );

        /*! \brief Assigns the DependableObject depObj an id in this domain and adds it to the domains dependency system.
         *  \param depObj DependableObject to be added to the domain.
         *  \param deps List of dependencies to be associated to the Dependable Object.
         *  \param numDeps Number of dependenices in the list.
         *  \param callback A function to call when a WD has a successor [Optional].
         *  \sa Dependency DependableObject TrackableObject
         */
         void submitDependableObject ( DependableObject &depObj, size_t numDeps, Dependency* deps, SchedulePolicySuccessorFunctor* callback = NULL );

         static void increaseTasksInGraph();

         static void decreaseTasksInGraph();

        /*! \brief Get exclusive access to the object
         */
         static void lock ( );

        /*! \brief Release object's lock
         */
         static void unlock ( );
   };
   
   /*! \class DependenciesManager
    * \brief Interface class of plugins used for dependencies domain.
    */
   class DependenciesManager
   {
      private:
         std::string _name;
      private:
         /*! \brief DependenciesManager default constructor (private)
          */
         DependenciesManager ();
         /*! \brief DependenciesManager copy constructor (private)
          */
         DependenciesManager ( DependenciesManager &sp );
         /*! \brief DependenciesManager copy assignment operator (private)
          */
         DependenciesManager& operator= ( DependenciesManager &sp );
      public:
         /*! \brief DependenciesManager constructor - with std::string &name
          */
         DependenciesManager ( const std::string &name ) : _name(name) {}
         /*! \brief DependenciesManager constructor - with char *name
          */
         DependenciesManager ( const char *name ) : _name(name) {}
         /*! \brief DependenciesManager destructor
          */
         virtual ~DependenciesManager () {};

         const std::string & getName () const;
   };

};

#endif

