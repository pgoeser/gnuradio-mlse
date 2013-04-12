#!/usr/bin/env python
#
# Copyright 2011 Paul Goeser
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

from gnuradio import gr, gr_unittest
import mlse_swig

class qa_vector_concat (gr_unittest.TestCase):

	def setUp (self):
		self.tb = gr.top_block ()

	def tearDown (self):
		self.tb = None

	def test_001_vector_concat_vv (self):
		src1=gr.vector_source_f((1,2,3,4,5));
		src2=gr.vector_source_f((11,12,13));
		expected_result = (1,2,3,4,5,11,12,13)
		dst=gr.vector_sink_f()
		reor1=gr.stream_to_vector(gr.sizeof_float, 5)
		reor2=gr.stream_to_vector(gr.sizeof_float, 3)
		reor3=gr.vector_to_stream(gr.sizeof_float, 8)
		cat=mlse_swig.vector_concat_vv(gr.sizeof_float*5, gr.sizeof_float*3)
		self.tb.connect(src1,reor1,(cat,0))
		self.tb.connect(src2,reor2,(cat,1))
		self.tb.connect(cat,reor3,dst)
		self.tb.run()
		self.tb.run ()
		result_data = dst.data ()
		self.assertFloatTuplesAlmostEqual (expected_result, result_data, 6)


if __name__ == '__main__':
	gr_unittest.main ()
