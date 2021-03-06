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

include $(top_srcdir)/src/plugins/instrumentation/version_commons.am
include $(top_srcdir)/src/common.am

if DEBUG_VERSION
debug_LTLIBRARIES = \
	libnanox-instrumentation-empty_trace.la \
	libnanox-instrumentation-print_trace.la \
	libnanox-instrumentation-graph.la \
	$(END)

if INSTRUMENTATION_EXTRAE
extrae_mpitrace_cxxflags=$(mpitrace_cxxflags)
extrae_mpitrace_ldflags=$(mpitrace_ldflags)

debug_LTLIBRARIES+=libnanox-instrumentation-extrae.la
endif

if INSTRUMENTATION_AYUDAME
debug_LTLIBRARIES+=libnanox-instrumentation-ayudame.la
endif

nanos_debug_cxxflags=$(common_includes) $(common_debug_cxxflags)

libnanox_instrumentation_empty_trace_la_CXXFLAGS = $(nanos_debug_cxxflags)
libnanox_instrumentation_empty_trace_la_LDFLAGS = $(ld_plugin_flags)
libnanox_instrumentation_empty_trace_la_SOURCES = $(empty_sources)

libnanox_instrumentation_print_trace_la_CXXFLAGS = $(nanos_debug_cxxflags)
libnanox_instrumentation_print_trace_la_LDFLAGS = $(ld_plugin_flags)
libnanox_instrumentation_print_trace_la_SOURCES = $(print_sources)

libnanox_instrumentation_graph_la_CXXFLAGS = $(nanos_debug_cxxflags)
libnanox_instrumentation_graph_la_LDFLAGS = $(ld_plugin_flags)
libnanox_instrumentation_graph_la_SOURCES = $(graph_sources)

if INSTRUMENTATION_EXTRAE
libnanox_instrumentation_extrae_la_CXXFLAGS = $(nanos_debug_cxxflags) $(extrae_mpitrace_cxxflags)
libnanox_instrumentation_extrae_la_LDFLAGS = $(ld_plugin_flags) $(extrae_mpitrace_ldflags)
libnanox_instrumentation_extrae_la_SOURCES = $(extrae_sources)
endif

if INSTRUMENTATION_AYUDAME
libnanox_instrumentation_ayudame_la_CXXFLAGS = $(nanos_debug_cxxflags) $(ayudame_cxxflags)
libnanox_instrumentation_ayudame_la_LDFLAGS = $(ld_plugin_flags) $(ayudame_ldflags)
libnanox_instrumentation_ayudame_la_SOURCES = $(ayudame_sources)
endif

endif

