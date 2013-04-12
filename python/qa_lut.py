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

class qa_lut (gr_unittest.TestCase):

	def setUp (self):
		self.tb = gr.top_block ()

	def tearDown (self):
		self.tb = None

	def test_001_lut (self):
		"""test with integers"""
		src=gr.vector_source_b((1,3,2,0))
		table=[[1,5,3,9],[2,3,8,-1],[3,9,12,3]];
		expected_result = [ table[i] for i in (1,0,2,0) ]
		expected_result = tuple(itertools.chain.from_iterable(expected_result))
		dst=gr.vector_sink_i(len(table[0]))
		lut=mlse_swig.lut_biv(table)
		self.tb.connect(src,lut,dst)
		self.tb.run()
		result_data = dst.data ()
		self.assertEquals(expected_result, result_data)

	def test_002_lut (self):
		""" tests the complex variant"""
		src=gr.vector_source_b((1,0,2,3))
		table=[[1,5,-3],[2,3,8+4j],[3,9,12]];
		expected_result = [ table[i] for i in (1,0,2,0) ]
		expected_result = tuple(itertools.chain.from_iterable(expected_result))
		expected_result = map(complex,expected_result)
		dst=gr.vector_sink_c(3)
		lut=mlse_swig.lut_bcv(table)
		self.tb.connect(src,lut,dst)
		self.tb.run()
		result_data = dst.data ()
		self.assertComplexTuplesAlmostEqual(expected_result, result_data)

	def test_003_lut (self):
		"""tests slicing"""
		src=gr.vector_source_b((1,3,2,0))
		table=[[1,5,3,9],[2,3,8,-1],[3,9,12,3]];
		# FIXME: static method called via instance...
		table_sliced = mlse_swig.lut_biv(table).slice_lut(table, 1, 2)
		# test if the slice was correct
		self.assertEquals(table_sliced, tuple([ tuple(l[1:3]) for l in table ]) )
		expected_result = [ table[i][1:3] for i in (1,0,2,0) ]
		expected_result = tuple(itertools.chain.from_iterable(expected_result))
		dst=gr.vector_sink_i(len(table_sliced[0]))
		lut=mlse_swig.lut_biv(table_sliced)
		self.tb.connect(src,lut,dst)
		self.tb.run()
		result_data = dst.data ()
		self.assertEquals(expected_result, result_data)

	def test_004_lut (self):
		"""tests slicing with reversing"""
		table=[[1,5,3,9],[2,3,8,-1],[3,9,12,3]];
		# FIXME: static method called via instance...
		table_sliced = mlse_swig.lut_biv(table).slice_lut(table, 0, 2, True)
		expected_slice = tuple([ tuple(reversed(l[2:4])) for l in table ])
		# test if the slice was correct
		self.assertEquals(table_sliced, expected_slice)

	def test_005_lut (self):
		"""tests exception throwing"""
		table=[[1,5,3,9],[2,3,8,-1],[3,9,12]]; # last vector of table is too short
		try:
			lut=mlse_swig.lut_bsv(table)
		except ValueError:
			return # correct exception thrown (std::invalid_argument gets translated to ValueError)
		except Exception:
			self.fail() # unknown exception thrown
		self.fail() # no exception thrown even though we gave erroneous data


			



if __name__ == '__main__':
	gr_unittest.main ()
