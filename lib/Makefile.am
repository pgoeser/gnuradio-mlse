#
# Copyright 2004,2005,2006,2008,2009,2010 Free Software Foundation, Inc.
# Modifications 2011 Paul Goeser
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

include $(top_srcdir)/Makefile.common


# list of programs run by "make check" and "make distcheck"
TESTS = test_all

# ----------------------------------------------------------------
# mlse C++ library: libgnuradio-mlse.so
# ----------------------------------------------------------------

# C/C++ headers get installed in ${prefix}/include/$(modname)
modinclude_HEADERS = \
	channel_model.h \
	mlse_viterbi_vcb.h \
	mlse_viterbi_impl.h \
	mlse_vector_slice_vv.h \
	mlse_vector_concat_vv.h \
	mlse_channelestimate_vcc.h \
	packet_config.h \
	packet_config_gsm.h \
	mlse_equalizer_midamble_vcb.h \
	mlse_packetbuilder_midamble_vbb.h \
	mlse_ber_vbi.h \
	$(GENERATED_H) \
	mlse_derotate_cc.h

lib_LTLIBRARIES = libgnuradio-mlse.la

libgnuradio_mlse_la_SOURCES = \
	mlse_viterbi_vcb.cc \
	mlse_viterbi_impl.cc \
	channel_model.cc \
	mlse_vector_slice_vv.cc \
	mlse_vector_concat_vv.cc \
	mlse_channelestimate_vcc.cc \
	packet_config.cc \
	packet_config_gsm.cc \
	mlse_equalizer_midamble_vcb.cc \
	mlse_packetbuilder_midamble_vbb.cc \
	mlse_ber_vbi.cc \
	$(GENERATED_CC) \
	mlse_derotate_cc.cc

libgnuradio_mlse_la_LIBADD = \
	$(GNURADIO_CORE_LA)

## important flags, they bring a LOT of performance
libgnuradio_mlse_la_CXXFLAGS =  -fcx-limited-range  -Winline
#TODO: can we add -mtune=native to the portable flags?

## native tuning, makes it run only on the current architecture or better.
## doesn't improve performance by much.
#libgnuradio_mlse_la_CXXFLAGS += -mtune=native -march=native -mfpmath=sse  

## probably unnecessary, since -mtune=native and -march=native should cover that
#libgnuradio_mlse_la_CXXFLAGS += -msse -msse2 -msse3 -msse4 -mssse3 -msse4.1 -mfpmath=sse  
## flags that didn't show noticeable improvement
#libgnuradio_mlse_la_CXXFLAGS += -funroll-all-loops -fbranch-probabilities -foptimize-register-move -fpredictive-commoning -fvect-cost-model

libgnuradio_mlse_la_LDFLAGS = $(NO_UNDEFINED) $(LTVERSIONFLAGS)

# ----------------------------------------------------------------
# generated code
# ----------------------------------------------------------------

core_generator = \
	generate_all.py \
	mlse_lut_XXv.h.t \
	mlse_lut_XXv.cc.t \
	mlse_lut_XXv.i.t \
	mlse_randint_X.h.t \
	mlse_randint_X.cc.t \
	mlse_randint_X.i.t

generated_sources = \
	$(GENERATED_H) \
	$(GENERATED_I)		\
	$(GENERATED_CC)	\
	mlse_generated.i

EXTRA_DIST = 			\
	$(core_generator)

# ----------------------------------------------------------------
# mlse C++ QA library: libgnuradio-mlse-qa.so (not installed)
# ----------------------------------------------------------------

noinst_LTLIBRARIES = libgnuradio-mlse-qa.la

libgnuradio_mlse_qa_la_SOURCES = \
	qa_mlse.cc \
	qa_mlse_viterbi_vcb.cc \
	qa_mlse_packetbuilder_midamble_vbb.cc

libgnuradio_mlse_qa_la_LDFLAGS = $(NO_UNDEFINED) -version-info 0:0:0

libgnuradio_mlse_qa_la_LIBADD = \
	libgnuradio-mlse.la \
	$(CPPUNIT_LIBS) \
	$(BOOST_SYSTEM_LIB) \
	$(BOOST_FILESYSTEM_LIB)

# ----------------------------------------------------------------
# headers that don't get installed
# ----------------------------------------------------------------
noinst_HEADERS = \
	qa_mlse.h \
	qa_mlse_viterbi_vcb.h \
	qa_mlse_packetbuilder_midamble_vbb.h

# ----------------------------------------------------------------
# test program
# ----------------------------------------------------------------
noinst_PROGRAMS = \
	test_all

test_all_SOURCES = test_all.cc 
test_all_LDADD   = libgnuradio-mlse-qa.la


# ----------------------------------------------------------------
# code to generate generated code
# ----------------------------------------------------------------


# include the srcdir's Makefile.gen; doing this creates an implicit
# dependency between $(srcdir)/Makefile.in and $(srcdir)/Makefile.gen.
include $(srcdir)/Makefile.gen

# common way for generating local Makefile.gen
makefile_gen_gen_command = PYTHONPATH=$(top_srcdir):$(top_srcdir)/python srcdir=$(srcdir) do_makefile=1 do_sources=0 $(PYTHON) $(srcdir)/generate_all.py
include $(top_srcdir)/Makefile.gen.gen

gen_sources = $(generated_sources)
gen_sources_deps = $(core_generator)
par_gen_command = PYTHONPATH=$(top_srcdir):$(top_srcdir)/python srcdir=$(srcdir) $(PYTHON) $(srcdir)/generate_all.py
include $(top_srcdir)/Makefile.par.gen


BUILT_SOURCES += $(generated_sources)
