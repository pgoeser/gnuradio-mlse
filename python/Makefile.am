#
# Copyright 2004,2010 Free Software Foundation, Inc.
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

TESTS = run_tests
EXTRA_DIST = run_tests.in run_testapp.in

CLEANFILES = testapp_errors testapp_exitcode

modpython_PYTHON = \
	__init__.py

noinst_PYTHON = \
	qa_ber.py \
	qa_channelestimate_vcc.py \
	qa_lut.py \
	qa_mlse_derotate_cc.py \
	qa_randint.py \
	qa_vector_concat.py \
	qa_vector_slice.py

# to generate filters.py:
# 	cd ../../matlab
#	matlab filterplot.m
# 	cd -
# 	./mat_to_py.py ../../matlab/filters.mat filters.py
#
