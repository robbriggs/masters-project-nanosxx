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
#ifndef _NANOS_H_
#define _NANOS_H_
/*!
 * \file  nanos.h
 * \brief Main Nanos++ header file.
 */

/*! \mainpage  Nanos++ Runtime Library
 *
 * This is the main development page for documenting Nanos++ Runtime Library (Nanos++ RTL).
 * Nanos++ is a runtime library designed to serve as runtime support in parallel environments.
 * It is mainly used to support OmpSs (an extension to the OpenMP programming model) developed
 * at BSC, but it also has modules to support OpenMP and Chapel.
 * <p/><br/>
 * The runtime provides several services to support task parallelism using synchronizations
 * based on data-dependencies. Data parallelism is also supported by means of services mapped
 * on top of its task support. Task are implemented as user-level threads when possible
 * (currently x86, x86-64, ia64, arm, ppc32 and ppc64 are supported). It also provides support
 * for maintaining coherence across different address spaces (such as with GPUs or cluster nodes).
 * <p/><br/>
 * The main purpose of Nanos++ is to be used in research of parallel programming environments.
 * Our aim has been to enable easy development of different parts of the runtime so researchers
 * have a platform that allows them to try different mechanisms. As such it is designed to be
 * extensible by means of plugins: the scheduling policy, the throttling policy, the dependence
 * approach, the barrier implementations, the slicers implementation, the instrumentation layer
 * and the architectural level. This extensibility does not come for free. The runtime overheads
 * are slightly increased, but there should be low enough for results to be meaningful except
 * for cases of extreme-fine grain applications.
 * <p/><br/>
 * \section repository Working with the repository
 *
 * Once you have cloned the code you will need to run autoreconf to generate all the initial files.
 * Enter the mcxx directory which has been created from git clone and run there
 *
 * \code
 * $ autoreconf -f -i -v
 * \endcode
 *
 * This process is somewhat fragile: some warnings will appear in several Makefile.am (due to GNU
 * Make extensions used in Mercurium makefiles) and some m4 warnings might or might not appear
 * depending on your precise environments (although this is now rare in Linux it might happen in
 * some versions of Solaris). However, no errors should happen.
 *
 * It may happen that autoreconf does complain about some Libtool macros not recognized. It usually
 * happens if the Libtool used is not 2.2.6 or it is not installed system-wide. In the latter case,
 * adjust your PATH variable to use a 2.2.6 (or better) Libtool. In either case, run the following
 * command (make sure it comes from a 2.2.6 Libtool installation directory!)
 *
 * \code
 * $ libtoolize --version
 * libtoolize (GNU libtool) 2.2.6
 * Written by Gary V. Vaughan <gary@gnu.org>, 2003
 * $ libtoolize -f -i
 * \endcode
 *
 * and then run again:
 *
 * \code
 * $ autoreconf -f -v -i
 * \endcode
 *
 * This should do. There is an obscure bug with autoreconf not discovering that libtool is being used
 * which seems only triggered in environments where the Libtool being used is not system-wide installed,
 * so it might not be a problem in your environment.
 */

//! \defgroup capi Nanos++ C/C++ API

/*! \page capi_families API Families & Versions
 *  \ingroup capi
 * 
 * - nanos interface family: master
 *   - 5004: adding data alignment parameter to slicer wd
 *   - 5005: translate function support
 *   - 5006: adding new parameter to nanos_wg_wait_completation service
 *   - 5007: enable/disable instrumentation through the API
 *   - 5008: removing slicer data parameters in create sliced wd
 *   - 5009: Adding priority management to runtime (compiler advice)
 *   - 5010: stopping and resuming scheduler services
 *   - 5011: distinguish thread's roles within the team (starring/supporting threads)
 *   - 5012: changing work descriptor creation functions and removing field dd_size @ nanos_device_t which is not needed anymore
 *   - 5013: creating memory allocation/deallocation services
 *   - 5014: Wd's props conmute from const to dyn properties
 *   - 5015: Nanos Delayed start, start and finish.
 *   - 5016: Providing runtime general information (architecture, programming model and scheduler)
 *   - 5017: Some common instrumentation changes:
 *     - Removing unnecessary instrument services
 *     - Using a common event generator service nanos_instrument_events(int num_events, nanos_event_t events[]).
 *     - Also using a new event structure.
 *   - 5018: Added nanos_get_wd_priority service.
 *   - 5019: Instrumenting user functions on different address spaces.
 *   - 5020: Service to enable/disable stealing in the scheduling policy.
 *   - 5021: Including void nanos_free0( void *p ) service.
 *   - 5022: Adding const char* description in task creation.
 *   - 5024: Adding is final attribute in wd's dynamic properties.
 *   - 5025: Changed WD priority from unsigned to int.
 * - nanos interface family: worksharing
 *   - 1000: First implementation of work-sharing services (create and next-item)
 * - nanos interface family: deps_api
 *   - 1000: First implementation of dependencies plugins.
 *   - 1001: Commutative clause support.
 * - nanos interface family: openmp
 *   - 1: First Nanos OpenMP interface: nanos_omp_single ( b ) service
 *   - 2: Including nanos_omp_barrier() service
 *   - 3: Including nanos_omp_set_implicit( uwd ) service
 *   - 4: Including nanos_omp_get_max_threads() service
 *   - 5: Including nanos_omp_find_worksharing( omp_sched_t kind );
 *   - 6:
 *   - 7: Including int nanos_omp_get_num_threads_next_parallel ( int threads_requested )
 */

#include <unistd.h>
#include "nanos-int.h"
#include "nanos_error.h"

#include "nanos_version.h"

//! \addtogroup capi_types Types and Structures
//! \ingroup capi
//! \{

// C++ types hidden as void *
typedef void * nanos_wg_t;
typedef void * nanos_team_t;
typedef void * nanos_sched_t;
typedef void * nanos_slicer_t;
typedef void * nanos_dd_t;
typedef void * nanos_sync_cond_t;
typedef unsigned int nanos_copy_id_t;

typedef struct nanos_const_wd_definition_tag {
   nanos_wd_props_t props;
   size_t data_alignment;
   size_t num_copies;
   size_t num_devices;
   size_t num_dimensions;
#ifdef _MF03
   void *description;
#else
   const char *description;
#endif
} nanos_const_wd_definition_t;

typedef struct {
   int nthreads;
   void *arch;
} nanos_constraint_t;

//! \}

#ifdef __cplusplus

#define _Bool bool

extern "C" {
#endif

NANOS_API_DECL(char *, nanos_get_mode, ( void ));

// Functions related to WD
NANOS_API_DECL(nanos_wd_t, nanos_current_wd, (void));
NANOS_API_DECL(int, nanos_get_wd_id, (nanos_wd_t wd));
NANOS_API_DECL(int, nanos_get_wd_priority, (nanos_wd_t wd));
NANOS_API_DECL(nanos_err_t, nanos_get_wd_description, ( char **description, nanos_wd_t wd ));

// Finder functions
NANOS_API_DECL(nanos_slicer_t, nanos_find_slicer, ( const char * slicer ));
NANOS_API_DECL(nanos_ws_t, nanos_find_worksharing, ( const char * label ) );

NANOS_API_DECL(nanos_err_t, nanos_create_wd_compact, ( nanos_wd_t *wd, nanos_const_wd_definition_t *const_data, nanos_wd_dyn_props_t *dyn_props,
                                                       size_t data_size, void ** data, nanos_wg_t wg, nanos_copy_data_t **copies, nanos_region_dimension_internal_t **dimensions, const unsigned char llvmir_start[], const unsigned char llvm_end[], const unsigned char llvm_function[] ));

NANOS_API_DECL(nanos_err_t, nanos_set_translate_function, ( nanos_wd_t wd, nanos_translate_args_t translate_args ));

NANOS_API_DECL(nanos_err_t, nanos_create_sliced_wd, ( nanos_wd_t *uwd, size_t num_devices, nanos_device_t *devices,
                                     size_t outline_data_size, int outline_data_align,
                                     void **outline_data, nanos_wg_t uwg, nanos_slicer_t slicer,
                                     nanos_wd_props_t *props, nanos_wd_dyn_props_t *dyn_props, size_t num_copies, nanos_copy_data_t **copies, size_t num_dimensions, nanos_region_dimension_internal_t **dimensions ));

NANOS_API_DECL(nanos_err_t, nanos_submit, ( nanos_wd_t wd, size_t num_data_accesses, nanos_data_access_t *data_accesses, nanos_team_t team ));

NANOS_API_DECL(nanos_err_t, nanos_create_wd_and_run_compact, ( nanos_const_wd_definition_t *const_data, nanos_wd_dyn_props_t *dyn_props,
                                                               size_t data_size, void * data, size_t num_data_accesses, nanos_data_access_t *data_accesses,
                                                               nanos_copy_data_t *copies, nanos_region_dimension_internal_t *dimensions, nanos_translate_args_t translate_args ));

NANOS_API_DECL(nanos_err_t, nanos_create_for, ( void ));

NANOS_API_DECL(nanos_err_t, nanos_set_internal_wd_data, ( nanos_wd_t wd, void *data ));
NANOS_API_DECL(nanos_err_t, nanos_get_internal_wd_data, ( nanos_wd_t wd, void **data ));
NANOS_API_DECL(nanos_err_t, nanos_yield, ( void ));

NANOS_API_DECL(nanos_err_t, nanos_slicer_get_specific_data, ( nanos_slicer_t slicer, void ** data ));

NANOS_API_DECL(nanos_err_t, nanos_get_num_ready_tasks, ( unsigned int *ready_tasks ));
NANOS_API_DECL(nanos_err_t, nanos_get_num_total_tasks, ( unsigned int *total_tasks ));
NANOS_API_DECL(nanos_err_t, nanos_get_num_nonready_tasks, ( unsigned int *nonready_tasks ));
NANOS_API_DECL(nanos_err_t, nanos_get_num_running_tasks, ( unsigned int *running_tasks ));
NANOS_API_DECL(nanos_err_t, nanos_get_num_blocked_tasks, ( unsigned int *blocked_tasks ));

NANOS_API_DECL(nanos_err_t, nanos_in_final, ( bool *result ));
NANOS_API_DECL(nanos_err_t, nanos_set_final, ( bool value ));

// Team related functions

NANOS_API_DECL(nanos_err_t, nanos_create_team,(nanos_team_t *team, nanos_sched_t sg, unsigned int *nthreads,
                              nanos_constraint_t * constraints, bool reuse, nanos_thread_t *info));

NANOS_API_DECL(nanos_err_t, nanos_create_team_mapped, (nanos_team_t *team, nanos_sched_t sg, unsigned int *nthreads, unsigned int *mapping));

NANOS_API_DECL(nanos_err_t, nanos_enter_team, ( void ));
NANOS_API_DECL(nanos_err_t, nanos_leave_team, ( void ));
NANOS_API_DECL(nanos_err_t, nanos_end_team, ( nanos_team_t team ));

NANOS_API_DECL(nanos_err_t, nanos_team_barrier, ( void ));

NANOS_API_DECL(nanos_err_t, nanos_single_guard, ( bool *));

NANOS_API_DECL(nanos_err_t, nanos_enter_sync_init, ( bool *b ));
NANOS_API_DECL(nanos_err_t, nanos_wait_sync_init, ( void ));
NANOS_API_DECL(nanos_err_t, nanos_release_sync_init, ( void ));

NANOS_API_DECL(nanos_err_t, nanos_memory_fence, (void));

NANOS_API_DECL(nanos_err_t, nanos_team_get_num_starring_threads, ( int *n ) );
NANOS_API_DECL(nanos_err_t, nanos_team_get_starring_threads, ( int *n, nanos_thread_t *list_of_threads ) );
NANOS_API_DECL(nanos_err_t, nanos_team_get_num_supporting_threads, ( int *n ) );
NANOS_API_DECL(nanos_err_t, nanos_team_get_supporting_threads, ( int *n, nanos_thread_t *list_of_threads) );
NANOS_API_DECL(nanos_err_t, nanos_register_reduction, ( nanos_reduction_t *red) );
NANOS_API_DECL(nanos_err_t, nanos_reduction_get_private_data, ( void **copy, void *original ) );

NANOS_API_DECL(nanos_err_t, nanos_reduction_get, ( nanos_reduction_t **dest, void *original ) );

NANOS_API_DECL(nanos_err_t, nanos_admit_current_thread, (void));
NANOS_API_DECL(nanos_err_t, nanos_expel_current_thread, (void));


// dependence
NANOS_API_DECL(nanos_err_t, nanos_dependence_release_all, ( void ) );
NANOS_API_DECL(nanos_err_t, nanos_dependence_pendant_writes, ( bool *res, void *addr ));

// worksharing
NANOS_API_DECL(nanos_err_t, nanos_worksharing_create ,( nanos_ws_desc_t **wsd, nanos_ws_t ws, nanos_ws_info_t *info, bool *b ) );
NANOS_API_DECL(nanos_err_t, nanos_worksharing_next_item, ( nanos_ws_desc_t *wsd, nanos_ws_item_t *wsi ) );

// sync

NANOS_API_DECL(nanos_err_t, nanos_wg_wait_completion, ( nanos_wg_t wg, bool avoid_flush ));

NANOS_API_DECL(nanos_err_t, nanos_create_int_sync_cond, ( nanos_sync_cond_t *sync_cond, volatile int *p, int condition ));
NANOS_API_DECL(nanos_err_t, nanos_create_bool_sync_cond, ( nanos_sync_cond_t *sync_cond, volatile bool *p, bool condition ));
NANOS_API_DECL(nanos_err_t, nanos_sync_cond_wait, ( nanos_sync_cond_t sync_cond ));
NANOS_API_DECL(nanos_err_t, nanos_sync_cond_signal, ( nanos_sync_cond_t sync_cond ));
NANOS_API_DECL(nanos_err_t, nanos_destroy_sync_cond, ( nanos_sync_cond_t sync_cond ));

NANOS_API_DECL(nanos_err_t, nanos_wait_on, ( size_t num_data_accesses, nanos_data_access_t *data_accesses ));

#define NANOS_INIT_LOCK_FREE { NANOS_LOCK_FREE }
#define NANOS_INIT_LOCK_BUSY { NANOS_LOCK_BUSY }
NANOS_API_DECL(nanos_err_t, nanos_init_lock, ( nanos_lock_t **lock ));
NANOS_API_DECL(nanos_err_t, nanos_init_lock_at, ( nanos_lock_t *lock ));
NANOS_API_DECL(nanos_err_t, nanos_set_lock, (nanos_lock_t *lock));
NANOS_API_DECL(nanos_err_t, nanos_unset_lock, (nanos_lock_t *lock));
NANOS_API_DECL(nanos_err_t, nanos_try_lock, ( nanos_lock_t *lock, bool *result ));
NANOS_API_DECL(nanos_err_t, nanos_destroy_lock, ( nanos_lock_t *lock ));
NANOS_API_DECL(nanos_err_t, nanos_get_lock_address, ( void *addr, nanos_lock_t **lock ));

// Device copies
NANOS_API_DECL(nanos_err_t, nanos_set_copies, (nanos_wd_t wd, int num_copies, nanos_copy_data_t *copies));
NANOS_API_DECL(nanos_err_t, nanos_get_addr, ( nanos_copy_id_t copy_id, void **addr, nanos_wd_t cwd ));

NANOS_API_DECL(nanos_err_t, nanos_copy_value, ( void *dst, nanos_copy_id_t copy_id, nanos_wd_t cwd ));

// system interface
NANOS_API_DECL(const char *, nanos_get_default_architecture, ());
NANOS_API_DECL(const char *, nanos_get_pm, ());
NANOS_API_DECL(nanos_err_t, nanos_get_default_binding, ( bool *res ));

NANOS_API_DECL(nanos_err_t, nanos_delay_start, ());
NANOS_API_DECL(nanos_err_t, nanos_start, ());
NANOS_API_DECL(nanos_err_t, nanos_finish, ());
NANOS_API_DECL(nanos_err_t, nanos_current_socket, ( int socket ));
NANOS_API_DECL(nanos_err_t, nanos_get_num_sockets, ( int *num_sockets ));

// Memory management
NANOS_API_DECL(nanos_err_t, nanos_malloc, ( void **p, size_t size, const char *file, int line ));
NANOS_API_DECL(nanos_err_t, nanos_free, ( void *p ));
NANOS_API_DECL(void, nanos_free0, ( void *p )); 

// error handling
NANOS_API_DECL(void, nanos_handle_error, ( nanos_err_t err ));

// instrumentation interface
NANOS_API_DECL(nanos_err_t, nanos_instrument_register_key, ( nanos_event_key_t *event_key, const char *key, const char *description, bool abort_when_registered ));
NANOS_API_DECL(nanos_err_t, nanos_instrument_register_value, ( nanos_event_value_t *event_value, const char *key, const char *value, const char *description, bool abort_when_registered ));

NANOS_API_DECL(nanos_err_t, nanos_instrument_register_value_with_val, ( nanos_event_value_t val, const char *key, const char *value, const char *description, bool abort_when_registered ));

NANOS_API_DECL(nanos_err_t, nanos_instrument_get_key, (const char *key, nanos_event_key_t *event_key));
NANOS_API_DECL(nanos_err_t, nanos_instrument_get_value, (const char *key, const char *value, nanos_event_value_t *event_value));


NANOS_API_DECL(nanos_err_t, nanos_instrument_events, ( unsigned int num_events, nanos_event_t events[] ));

NANOS_API_DECL(nanos_err_t, nanos_instrument_close_user_fun_event,());

NANOS_API_DECL(nanos_err_t, nanos_instrument_enable,( void ));

NANOS_API_DECL(nanos_err_t, nanos_instrument_disable,( void ));

#ifdef _MF03
    typedef void*  nanos_string_t;
#else
    typedef const char* nanos_string_t;
#endif

NANOS_API_DECL(nanos_err_t, nanos_instrument_begin_burst, (nanos_string_t key, nanos_string_t key_descr, nanos_string_t value, nanos_string_t value_descr));

NANOS_API_DECL(nanos_err_t, nanos_instrument_end_burst, (nanos_string_t key, nanos_string_t value));

NANOS_API_DECL(nanos_err_t, nanos_memcpy, (void *dest, const void *src, size_t n));

// scheduling interface
NANOS_API_DECL(const char *, nanos_get_default_scheduler, ());
NANOS_API_DECL(nanos_err_t, nanos_start_scheduler, ());
NANOS_API_DECL(nanos_err_t, nanos_stop_scheduler, ());
NANOS_API_DECL(nanos_err_t, nanos_scheduler_enabled, ( bool *res ));
NANOS_API_DECL(nanos_err_t, nanos_wait_until_threads_paused, () );
NANOS_API_DECL(nanos_err_t, nanos_wait_until_threads_unpaused, () );
NANOS_API_DECL(nanos_err_t, nanos_scheduler_get_stealing, ( bool *res ));
NANOS_API_DECL(nanos_err_t, nanos_scheduler_set_stealing, ( bool value ));

//funtion which will be called by mercurium
//on first line of user main (in some cases, offload and cluster)
NANOS_API_DECL(void, ompss_nanox_main, ());

// utility macros

#define NANOS_SAFE( call ) \
do {\
   nanos_err_t err = call;\
   if ( err != NANOS_OK ) nanos_handle_error( err );\
} while (0)

void nanos_reduction_int_vop ( int, void *, void * );

#ifdef __cplusplus
}
#endif

#endif
