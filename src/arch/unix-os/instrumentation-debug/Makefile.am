#####################################################################################
#      Copyright 2009 Barcelona Supercomputing Center                               #
#                                                                                   #
#      This file is part of the NANOS++ library.                                    #
#                                                                                   #
#      NANOS++ is free software: you can redistribute it and/or modify              #
#      it under the terms of the GNU Lesser General Public License as published by  #
#      the Free Software Foundation, either version 3 of the License, or            #
#      (at your option) any later version.                                          #
#                                                                                   #
#      NANOS++ is distributed in the hope that it will be useful,                   #
#      but WITHOUT ANY WARRANTY; without even the implied warranty of               #
#      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                #
#      GNU Lesser General Public License for more details.                          #
#                                                                                   #
#      You should have received a copy of the GNU Lesser General Public License     #
#      along with NANOS++.  If not, see <http://www.gnu.org/licenses/>.             #
#####################################################################################

include $(top_srcdir)/src/arch/unix-os/version_commons.am
include $(top_srcdir)/src/common.am

if INSTRUMENTATION_DEBUG_VERSION
noinst_LTLIBRARIES=libos.la

nanos_instrdebug_cxxflags=$(common_includes) $(common_instrdebug_cxxflags) 

if BGQ
libos_la_CPPFLAGS=-DBGQ
endif
libos_la_CXXFLAGS=$(nanos_instrdebug_cxxflags)
libos_la_SOURCES=$(os_sources)
libos_la_LIBADD=$(common_libadd)
endif

