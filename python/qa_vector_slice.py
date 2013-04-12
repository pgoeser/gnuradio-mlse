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

class qa_vector_slice (gr_unittest.TestCase):

	def setUp (self):
		self.tb = gr.top_block ()

	def tearDown (self):
		self.tb = None

	def test_002_vector_slice_vv (self):
		"""tests reversing and cutting off the beginning of the input"""
		src=gr.vector_source_f((1,2,3,4,5,6,7,8,9,10,11,12,13,14,15))
		expected_result = (5,4,3,10,9,8,15,14,13)
		dst=gr.vector_sink_f()
		reor1=gr.stream_to_vector(gr.sizeof_float, 5)
		reor2=gr.vector_to_stream(gr.sizeof_float, 3)
		slicer=mlse_swig.vector_slice_vv(gr.sizeof_float, 5, 2, 3, 1)
		self.tb.connect(src,reor1,slicer,reor2,dst)
		self.tb.run()
		self.tb.run ()
		result_data = dst.data ()
		self.assertFloatTuplesAlmostEqual (expected_result, result_data, 6)

	def test_001_vector_slice_vv (self):
		"""tests negative offsets"""
		src=gr.vector_source_f((1,2,3,4,5,6,7,8,9,10,11,12,13,14,15))
		expected_result = (0,1,2,0,6,7,0,11,12)
		dst=gr.vector_sink_f()
		reor1=gr.stream_to_vector(gr.sizeof_float, 5)
		reor2=gr.vector_to_stream(gr.sizeof_float, 3)
		slicer=mlse_swig.vector_slice_vv(gr.sizeof_float, 5, -1, 3)
		self.tb.connect(src,reor1,slicer,reor2,dst)
		self.tb.run()
		self.tb.run ()
		result_data = dst.data ()
		self.assertFloatTuplesAlmostEqual (expected_result, result_data, 6)


if __name__ == '__main__':
	gr_unittest.main ()
