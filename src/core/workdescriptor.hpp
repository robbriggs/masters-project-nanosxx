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

#ifndef _NANOS_WORK_DESCRIPTOR_H
#define _NANOS_WORK_DESCRIPTOR_H

#include <stdlib.h>
#include <utility>
#include <vector>
#include <climits>
#include "workdescriptor_decl.hpp"
#include "workgroup.hpp"
#include "dependableobjectwd.hpp"
#include "copydata.hpp"
#include "synchronizedcondition_decl.hpp"
#include "atomic.hpp"
#include "lazy.hpp"
#include "instrumentationcontext.hpp"
#include "directory.hpp"
#include "schedule.hpp"
#include "dependenciesdomain.hpp"
#include "allocator_decl.hpp"
#include "system.hpp"
#include "dependableobjectwr.hpp"

using namespace nanos;

inline WorkDescriptor::WorkDescriptor ( int ndevices, DeviceData **devs, size_t data_size, size_t data_align, void *wdata,
                                 size_t numCopies, CopyData *copies, nanos_translate_args_t translate_args, char *description,
                                 const unsigned char llvmir_start[], const unsigned char llvmir_end[], const unsigned char llvm_function[] )
                               : WorkGroup(), _data_size ( data_size ), _data_align( data_align ),  _data ( wdata ), _totalSize(0),
                                 _wdData ( NULL ), _flags(), _tie ( false ), _tiedTo ( NULL ),
                                 _state( INIT ), _syncCond( NULL ),  _parent ( NULL ), _myQueue ( NULL ), _depth ( 0 ),
                                 _numDevices ( ndevices ), _devices ( devs ), _activeDeviceIdx( ndevices == 1 ? 0 : ndevices ),
                                 _numCopies( numCopies ), _copies( copies ), _paramsSize( 0 ),
                                 _versionGroupId( 0 ), _executionTime( 0.0 ), _estimatedExecTime( 0.0 ),
                                 _doSubmit(NULL), _doWait(), _depsDomain( sys.getDependenciesManager()->createDependenciesDomain() ), 
                                 _directory(NULL), _submitted( false ), _implicit(false), _translateArgs( translate_args ),
                                 _priority( 0 ), _commutativeOwnerMap(NULL), _commutativeOwners(NULL), _wakeUpQueue( UINT_MAX ),
                                 _copiesNotInChunk(false), _description(description), _instrumentationContextData(),
                                 _workRepresentation( llvmir_start, llvmir_end, llvm_function )
                                 {
                                    _flags.is_final = 0;
                                 }

inline WorkDescriptor::WorkDescriptor ( DeviceData *device, size_t data_size, size_t data_align, void *wdata,
                                 size_t numCopies, CopyData *copies, nanos_translate_args_t translate_args, char *description,
                                 const unsigned char *llvmir_start, const unsigned char *llvmir_end, const unsigned char llvm_function[] )
                               : WorkGroup(), _data_size ( data_size ), _data_align ( data_align ), _data ( wdata ), _totalSize(0),
                                 _wdData ( NULL ), _flags(), _tie ( false ), _tiedTo ( NULL ),
                                 _state( INIT ), _syncCond( NULL ), _parent ( NULL ), _myQueue ( NULL ), _depth ( 0 ),
                                 _numDevices ( 1 ), _devices ( NULL ), _activeDeviceIdx( 0 ),
                                 _numCopies( numCopies ), _copies( copies ), _paramsSize( 0 ),
                                 _versionGroupId( 0 ), _executionTime( 0.0 ), _estimatedExecTime( 0.0 ), 
                                 _doSubmit(NULL), _doWait(), _depsDomain( sys.getDependenciesManager()->createDependenciesDomain() ),
                                 _directory(NULL), _submitted( false ), _implicit(false), _translateArgs( translate_args ),
                                 _priority( 0 ),  _commutativeOwnerMap(NULL), _commutativeOwners(NULL),
                                 _wakeUpQueue( UINT_MAX ), _copiesNotInChunk(false), _description(description), _instrumentationContextData(),
                                 _workRepresentation( llvmir_start, llvmir_end, llvm_function )
                                 {
                                     _devices = new DeviceData*[1];
                                     _devices[0] = device;
                                    _flags.is_final = 0;
                                 }

inline WorkDescriptor::WorkDescriptor ( const WorkDescriptor &wd, DeviceData **devs, CopyData * copies, void *data, char *description )
                               : WorkGroup( wd ), _data_size( wd._data_size ), _data_align( wd._data_align ), _data ( data ), _totalSize(0),
                                 _wdData ( NULL ), _flags(), _tie ( wd._tie ), _tiedTo ( wd._tiedTo ),
                                 _state ( INIT ), _syncCond( NULL ), _parent ( wd._parent ), _myQueue ( NULL ), _depth ( wd._depth ),
                                 _numDevices ( wd._numDevices ), _devices ( devs ), _activeDeviceIdx( wd._numDevices == 1 ? 0 : wd._numDevices ),
                                 _numCopies( wd._numCopies ), _copies( wd._numCopies == 0 ? NULL : copies ), _paramsSize( wd._paramsSize ),
                                 _versionGroupId( wd._versionGroupId ), _executionTime( wd._executionTime ),
                                 _estimatedExecTime( wd._estimatedExecTime ), _doSubmit(NULL), _doWait(),
                                 _depsDomain( sys.getDependenciesManager()->createDependenciesDomain() ),
                                 _directory(NULL), _submitted( false ), _implicit( wd._implicit ),_translateArgs( wd._translateArgs ),
                                 _priority( wd._priority ), _commutativeOwnerMap(NULL), _commutativeOwners(NULL),
                                 _wakeUpQueue( wd._wakeUpQueue ), 
                                 _copiesNotInChunk( wd._copiesNotInChunk), _description(description), _instrumentationContextData(),
                                 _workRepresentation( wd._workRepresentation )
                                 {
                                    _flags.is_final = false;
                                    _flags.is_ready = false;
                                 }

/* DeviceData inlined functions */
inline const Device * DeviceData::getDevice () const { return _architecture; }

inline bool DeviceData::isCompatible ( const Device &arch ) { return _architecture == &arch; }

/* WorkDescriptor inlined functions */
inline bool WorkDescriptor::started ( void ) const { return (( _state != INIT ) && (_state != START)); }

inline size_t WorkDescriptor::getDataSize () const { return _data_size; }
inline void WorkDescriptor::setDataSize ( size_t data_size ) { _data_size = data_size; }

inline size_t WorkDescriptor::getDataAlignment () const { return _data_align; }
inline void WorkDescriptor::setDataAlignment ( size_t data_align ) { _data_align = data_align; }

inline void WorkDescriptor::setTotalSize ( size_t size ) { _totalSize = size; }

inline WorkDescriptor * WorkDescriptor::getParent() { return _parent; }
inline void WorkDescriptor::setParent ( WorkDescriptor * p ) { _parent = p; }

inline WDPool * WorkDescriptor::getMyQueue() { return _myQueue; }
inline void WorkDescriptor::setMyQueue ( WDPool * myQ ) { _myQueue = myQ; }

inline bool WorkDescriptor::isEnqueued() { return ( _myQueue != NULL ); }

inline WorkDescriptor & WorkDescriptor::tied () { _tie = true; return *this; }

inline WorkDescriptor & WorkDescriptor::tieTo ( BaseThread &pe ) { _tiedTo = &pe; _tie=false; return *this; }

inline bool WorkDescriptor::isTied() const { return _tiedTo != NULL; }

inline BaseThread* WorkDescriptor::isTiedTo() const { return _tiedTo; }

inline bool WorkDescriptor::shouldBeTied() const { return _tie; }

inline void WorkDescriptor::untie() { _tiedTo = NULL; _tie = false; }

inline void WorkDescriptor::setData ( void *wdata ) { _data = wdata; }

inline void * WorkDescriptor::getData () const { return _data; }

inline void WorkDescriptor::setStart () { _state = WorkDescriptor::START; }

inline bool WorkDescriptor::isIdle () const { return _state == WorkDescriptor::IDLE; }
inline void WorkDescriptor::setIdle () { _state = WorkDescriptor::IDLE; }

inline bool WorkDescriptor::isReady () const { return _flags.is_ready; }

inline void WorkDescriptor::setBlocked () {
   NANOS_INSTRUMENT ( static InstrumentationDictionary *ID = sys.getInstrumentation()->getInstrumentationDictionary(); )
   NANOS_INSTRUMENT ( static nanos_event_key_t Keys  = ID->getEventKey("wd-blocked"); )
   NANOS_INSTRUMENT ( if ( _flags.is_ready ) { )
   NANOS_INSTRUMENT ( nanos_event_value_t Values = (nanos_event_value_t) this; )
   NANOS_INSTRUMENT ( sys.getInstrumentation()->raisePointEvents(1, &Keys, &Values); )
   NANOS_INSTRUMENT ( } )
   _flags.is_ready = false;
}

inline void WorkDescriptor::setReady ()
{
   NANOS_INSTRUMENT ( static InstrumentationDictionary *ID = sys.getInstrumentation()->getInstrumentationDictionary(); )
   NANOS_INSTRUMENT ( static nanos_event_key_t Keys  = ID->getEventKey("wd-ready"); )
   NANOS_INSTRUMENT ( if ( !_flags.is_ready ) { )
   NANOS_INSTRUMENT ( nanos_event_value_t Values = (nanos_event_value_t) this; )
   NANOS_INSTRUMENT ( sys.getInstrumentation()->raisePointEvents(1, &Keys, &Values); )
   NANOS_INSTRUMENT ( } )
   _flags.is_ready = true;
}

inline bool WorkDescriptor::isFinal () const { return _flags.is_final; }
inline void WorkDescriptor::setFinal ( bool value ) { _flags.is_final = value; }

inline GenericSyncCond * WorkDescriptor::getSyncCond() { return _syncCond; }

inline void WorkDescriptor::setSyncCond( GenericSyncCond * syncCond ) { _syncCond = syncCond; }

inline void WorkDescriptor::setDepth ( int l ) { _depth = l; }

inline unsigned WorkDescriptor::getDepth() { return _depth; }

inline DeviceData & WorkDescriptor::getActiveDevice () const { return *_devices[_activeDeviceIdx]; }

inline bool WorkDescriptor::hasActiveDevice() const { return _activeDeviceIdx != _numDevices; }

inline void WorkDescriptor::setActiveDeviceIdx( unsigned int idx ) { _activeDeviceIdx = idx; }
inline unsigned int WorkDescriptor::getActiveDeviceIdx() { return _activeDeviceIdx; }

inline void WorkDescriptor::setInternalData ( void *data, bool ownedByWD ) { 
    union { void* p; intptr_t i; } u = { data };
    // Set the own status
    u.i |= int( ownedByWD );

    _wdData = u.p;
}

inline void * WorkDescriptor::getInternalData () const { 
    union { void* p; intptr_t i; } u = { _wdData };

    // Clear the own status if set
    u.i &= ((~(intptr_t)0) << 1);

    return u.p;
}

inline void WorkDescriptor::setTranslateArgs( nanos_translate_args_t translateArgs ) { _translateArgs = translateArgs; }

inline int WorkDescriptor::getSocket() const
{
   return _socket;
}

inline void WorkDescriptor::setSocket( int socket )
{
   _socket = socket;
}

inline unsigned int WorkDescriptor::getWakeUpQueue() const
{
   return _wakeUpQueue;
}

inline void WorkDescriptor::setWakeUpQueue( unsigned int queue )
{
   _wakeUpQueue = queue;
}

inline unsigned int WorkDescriptor::getNumDevices ( void ) { return _numDevices; }

inline DeviceData ** WorkDescriptor::getDevices ( void ) { return _devices; }

inline bool WorkDescriptor::dequeue ( WorkDescriptor **slice ) { *slice = this; return true; }

inline void WorkDescriptor::clear () { _parent = NULL; }

inline size_t WorkDescriptor::getNumCopies() const { return _numCopies; }

inline CopyData * WorkDescriptor::getCopies() const { return _copies; }

inline size_t WorkDescriptor::getParamsSize() const { return _paramsSize; }

inline unsigned long WorkDescriptor::getVersionGroupId( void ) { return _versionGroupId; }

inline void WorkDescriptor::setVersionGroupId( unsigned long id ) { _versionGroupId = id; }

inline double WorkDescriptor::getExecutionTime() const { return _executionTime; }

inline double WorkDescriptor::getEstimatedExecutionTime() const { return _estimatedExecTime; }

inline void WorkDescriptor::setEstimatedExecutionTime( double time ) { _estimatedExecTime = time; }

inline DOSubmit * WorkDescriptor::getDOSubmit() { return _doSubmit; }

inline void WorkDescriptor::submitWithDependencies( WorkDescriptor &wd, size_t numDeps, DataAccess* deps )
{
   wd._doSubmit = NEW DOSubmit();
   wd._doSubmit->setWD(&wd);

   // Defining call back (cb)
   SchedulePolicySuccessorFunctor cb( *sys.getDefaultSchedulePolicy() );
   
   initCommutativeAccesses( wd, numDeps, deps );
   
   _depsDomain->submitDependableObject( *(wd._doSubmit), numDeps, deps, &cb );
}

inline void WorkDescriptor::waitOn( size_t numDeps, DataAccess* deps )
{
   _doWait->setWD(this);
   _depsDomain->submitDependableObject( *_doWait, numDeps, deps );
}

class DOIsSchedulable : public DependableObjectPredicate
{
   BaseThread &    _thread;

   public:
      DOIsSchedulable(BaseThread &thread) : DependableObjectPredicate(),_thread(thread) { }
      ~DOIsSchedulable() {}

      bool operator() ( DependableObject &obj )
      {       
         WD *wd = (WD *)obj.getRelatedObject();
         // FIXME: The started condition here ensures that doWait objects are not released as
         // they do not work properly if there is no dependenceSatisfied called before
         return (wd != NULL) && Scheduler::checkBasicConstraints(*wd,_thread) && !wd->started() ;
      }
};

inline WorkDescriptor * WorkDescriptor::getImmediateSuccessor ( BaseThread &thread )
{
   if ( _doSubmit == NULL ) return NULL;
   else {
        DOIsSchedulable predicate(thread);
        DependableObject * found = _doSubmit->releaseImmediateSuccessor(predicate);
        return found ? (WD *) found->getRelatedObject() : NULL;
   }
}

inline void WorkDescriptor::workFinished(WorkDescriptor &wd)
{
   if ( wd._doSubmit != NULL ){
      wd._doSubmit->finished();
      delete wd._doSubmit;
      wd._doSubmit = NULL;
   }
}

inline DependenciesDomain & WorkDescriptor::getDependenciesDomain()
{
   return *_depsDomain;
}


inline InstrumentationContextData * WorkDescriptor::getInstrumentationContextData( void ) { return &_instrumentationContextData; }

inline void WorkDescriptor::waitCompletion( bool avoidFlush )
{
   this->WorkGroup::waitCompletion();
   if ( _directory != NULL && !avoidFlush ) _directory->synchronizeHost();
}

inline Directory* WorkDescriptor::getDirectory(bool create)
{
   if ( _directory == NULL && create == false ) return NULL;
   if ( _directory == NULL ) _directory = NEW Directory();

   _directory->setParent( (getParent() != NULL) ? getParent()->getDirectory(false) : NULL );
   return _directory;
}

inline bool WorkDescriptor::isSubmitted() const { return _submitted; }
inline void WorkDescriptor::submitted()  { _submitted = true; }

inline bool WorkDescriptor::isConfigured ( void ) const { return _configured; }
inline void WorkDescriptor::setConfigured ( bool value ) { _configured = value; }

inline void WorkDescriptor::setPriority( WorkDescriptor::PriorityType priority ) { _priority = priority; }
inline WorkDescriptor::PriorityType WorkDescriptor::getPriority() const { return _priority; }

inline void WorkDescriptor::releaseCommutativeAccesses()
{
   if ( _commutativeOwners == NULL ) return;
   const size_t n = _commutativeOwners->size();
   for ( size_t i = 0; i < n; i++ )
      *(*_commutativeOwners)[i] = NULL;
} 

inline void WorkDescriptor::setImplicit( bool b ) { _implicit = b; }
inline bool WorkDescriptor::isImplicit( void ) { return _implicit; } 

inline char * WorkDescriptor::getDescription ( void ) const  { return _description; }

inline void WorkDescriptor::JITCompile()
{
   std::cout << "IN JISTOCMPIAWEF\n";
   if (_workRepresentation.has_ir())
      _workRepresentation.JITCompile(_data);
}

#endif

