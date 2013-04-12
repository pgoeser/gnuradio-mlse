#
# Copyright 2004,2005,2006,2008,2009,2010 Free Software Foundation, Inc.
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
include $(top_srcdir)/Makefile.swig

noinst_GUILE  = mlse.test

AM_CPPFLAGS += -I$(top_srcdir)/lib 
# quickfix for gruel includes, which got moved into a separate directory for some reason
STD_DEFINES_AND_INCLUDES += -I$(GNURADIO_CORE_INCLUDEDIR)/../gruel/swig

###################################
# SWIG interfaces and libraries

TOP_SWIG_IFILES = \
	mlse_swig.i

# Install so that they end up available as:
#   import mlse
# This ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/$(modname)
mlse_swig_pythondir_category = $(modname)

# additional libraries for linking with the SWIG-generated library
mlse_swig_la_swig_libadd = \
	$(top_builddir)/lib/libgnuradio-mlse.la

# additional SWIG files to be installed
mlse_swiginclude_headers = \
	mlse_derotate_cc.i \
	mlse_ber_vbi.i \
	mlse_channelestimate_vcc.i \
	mlse_derotate_cc.i \
	mlse_equalizer_midamble_vcb.i \
	mlse_packetbuilder_midamble_vbb.i \
	mlse_vector_concat_vv.i \
	mlse_vector_slice_vv.i \
	mlse_viterbi_vcb.i \
	mlse_xor_encode_bb.i \
	packet_config.i \
	packet_config_gsm.i

EXTRA_DIST = $(mlse_swiginclude_headers)

