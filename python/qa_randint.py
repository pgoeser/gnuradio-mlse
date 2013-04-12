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

import itertools

class qa_randint (gr_unittest.TestCase):

	def setUp (self):
		self.tb = gr.top_block ()

	def tearDown (self):
		self.tb = None

	def test_001 (self):
		"""binary test"""
		src = mlse_swig.randint_b(2)
		head = gr.head(gr.sizeof_char, 10000)
		dst=gr.vector_sink_b()
		self.tb.connect(src,head,dst)
		self.tb.run()
		data = dst.data()
		a = set(data)
		self.assertEquals(a, set([0,1]))
		b = sum(data)
		self.assertTrue(4500 < b < 5500) # this can fail by chance


	def test_001 (self):
		"""numbers from 0 to 6, this time in an int source"""
		src = mlse_swig.randint_i(7)
		head = gr.head(gr.sizeof_int, 10000)
		dst=gr.vector_sink_i()
		self.tb.connect(src,head,dst)
		self.tb.run()
		data = dst.data()
		a = set(data)
		self.assertEquals(a, set(range(7)))
		counts = [data.count(i) for i in range(7)]
		countdiff = max(counts) - min(counts)
		print max(counts), min(counts), countdiff
		self.assertTrue(countdiff < 500) # this can fail by chance


	def test_005_lut (self):
		"""tests exception throwing"""
		try:
			src = mlse_swig.randint_b(500);
		except OverflowError:
			return # correct exception thrown (casting 500 to unsigned char) 
		except Exception:
			self.fail() # unknown exception thrown
		self.fail() # no exception thrown even though we gave erroneous data


			



if __name__ == '__main__':
	gr_unittest.main ()
