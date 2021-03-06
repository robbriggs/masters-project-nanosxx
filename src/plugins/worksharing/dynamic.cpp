#include "nanos-int.h"
#include "loop.hpp"
#include "plugin.hpp"
#include "system.hpp"
#include "worksharing_decl.hpp"

namespace nanos {
namespace ext {

class WorkSharingDynamicFor : public WorkSharing {

     /*! \brief create a loop descriptor
      *  
      *  \return only one thread per loop will get 'true' (single like behaviour)
      */
      bool create( nanos_ws_desc_t **wsd, nanos_ws_info_t *info )
      {
         nanos_ws_info_loop_t *loop_info = (nanos_ws_info_loop_t *) info;
         bool single = false;

         *wsd = myThread->getTeamWorkSharingDescriptor( &single );
         if ( single ) {
            (*wsd)->data = NEW WorkSharingLoopInfo();
            ((WorkSharingLoopInfo *)(*wsd)->data)->lowerBound = loop_info->lower_bound;
            ((WorkSharingLoopInfo *)(*wsd)->data)->upperBound = loop_info->upper_bound;
            ((WorkSharingLoopInfo *)(*wsd)->data)->loopStep   = loop_info->loop_step;
            int chunk_size = std::max(1,loop_info->chunk_size);
            ((WorkSharingLoopInfo *)(*wsd)->data)->chunkSize  = chunk_size;
            int niters = (((loop_info->upper_bound - loop_info->lower_bound) / loop_info->loop_step ) + 1 );
            int chunks = niters / chunk_size;
            if ( niters % chunk_size != 0 ) chunks++;
            ((WorkSharingLoopInfo *)(*wsd)->data)->numOfChunks = chunks;
            ((WorkSharingLoopInfo *)(*wsd)->data)->currentChunk  = 0;

#if 0
fprintf(stderr,"lower=%d, upper=%d, step=%d, chunk=%d,nchunk=%d\n",
            ((WorkSharingLoopInfo *)(*wsd)->data)->lowerBound,
            ((WorkSharingLoopInfo *)(*wsd)->data)->upperBound,
            ((WorkSharingLoopInfo *)(*wsd)->data)->loopStep,
            ((WorkSharingLoopInfo *)(*wsd)->data)->chunkSize,
            ((WorkSharingLoopInfo *)(*wsd)->data)->numOfChunks
            );
#endif

            memoryFence();

            (*wsd)->ws = this; // Once 'ws' field has a value, any other thread can use the structure
         }

         // wait until worksharing descriptor is initialized
         while ( (*wsd)->ws == NULL ) {;}

         return single;
      }

     /*! \brief Get next chunk of iterations
      *
      */
      void nextItem( nanos_ws_desc_t *wsd, nanos_ws_item_t *item )
      {
         nanos_ws_item_loop_t *loop_item = ( nanos_ws_item_loop_t *) item;
         WorkSharingLoopInfo  *loop_data = ( WorkSharingLoopInfo  *) wsd->data;

         int mychunk = loop_data->currentChunk++;
         if ( mychunk > loop_data->numOfChunks)
         {
            loop_item->execute = false;
            return;
         }

         int sign = (( loop_data->loopStep < 0 ) ? -1 : +1);

         loop_item->lower = loop_data->lowerBound + mychunk * loop_data->chunkSize * loop_data->loopStep;
         loop_item->upper = loop_item->lower + loop_data->chunkSize * loop_data->loopStep - sign;
         if ( ( loop_data->upperBound * sign ) < ( loop_item->upper * sign ) ) loop_item->upper = loop_data->upperBound;
         loop_item->last = mychunk == (loop_data->numOfChunks - 1);
         loop_item->execute = (loop_item->lower * sign) <= (loop_item->upper * sign);
      }

      void duplicateWS ( nanos_ws_desc_t *orig, nanos_ws_desc_t **copy) {}
   
};

class WorkSharingDynamicForPlugin : public Plugin {
   public:
      WorkSharingDynamicForPlugin () : Plugin("Worksharing plugin for loops using a dynamic policy",1) {}
      ~WorkSharingDynamicForPlugin () {}

      virtual void config( Config& cfg ) {}

      void init ()
      {
         sys.registerWorkSharing("dynamic_for", NEW WorkSharingDynamicFor() );	
      }
};

} // namespace ext
} // namespace nanos

nanos::ext::WorkSharingDynamicForPlugin NanosXPlugin;
