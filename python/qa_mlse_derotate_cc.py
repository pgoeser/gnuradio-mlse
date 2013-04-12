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

class qa_derotate_cc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        """tests sample mode"""
        input_data = (1.,)*40
        expected_result = (-1j, -1, 1j,1)*10
        # set up fg
        src = gr.vector_source_c(input_data)
        derot = mlse_swig.derotate_cc(1,4,1)
        sink = gr.vector_sink_c();
        self.tb.connect(src, derot, sink)
        self.tb.run ()
        # check data
        self.assertComplexTuplesAlmostEqual(expected_result, sink.data())

    #TODO: test vector mode and offsets
        
if __name__ == '__main__':
    gr_unittest.main ()
