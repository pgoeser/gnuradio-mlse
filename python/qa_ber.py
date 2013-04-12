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

class qa_ber (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001 (self):
        """basic test"""
        src1=gr.vector_source_b((1,0,0,1,1,0),False,3)
        src2=gr.vector_source_b((1,0,0,0,1,1),False,3)
        expected_result = (0,2)
        dst=gr.vector_sink_i()
        ber = mlse_swig.ber_vbi(3);
        self.tb.connect(src1,(ber,0))
        self.tb.connect(src2,(ber,1))
        self.tb.connect(ber,dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual( expected_result, result_data)
        self.assertAlmostEqual( ber.bit_error_rate(), 1/3., 6)
        self.assertAlmostEqual( ber.packet_error_rate(), 0.5, 6)

    # coverage-TODO:
    # - data >1
    # - all getters/setters



if __name__ == '__main__':
    gr_unittest.main ()
