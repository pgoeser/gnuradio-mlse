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

class qa_channelestimate_vcc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_channelestimate_vcc (self):
        """this is the estimate of a cleanly modulated GSM TSC, which logically
        estimates the sending filter. expected_result and modulation calculated
        with matlab."""
        tsc = [-1,1,-1,-1,-1,1,1,1,1,-1,1,1,1,-1,1,1]
        tsc_src = gr.vector_source_c(tsc,False,len(tsc));
        samples = \
            [  -9.99989524783540639774e-01 +4.57715230122133446417e-03j,
                9.99989522713466638670e-01 +4.57760453656867304789e-03j,
               -8.58456925386381519694e-01 -5.12885666846092047599e-01j,
               -9.99999999999897748459e-01 -4.52240084170321668262e-07j,
               -8.60795949010275229796e-01 +5.08950227593523707448e-01j,
                8.60795718842493395861e-01 +5.08950616879904171874e-01j,
                9.99989524783540639774e-01 +4.57715230122145676217e-03j,
                9.99989522713466638670e-01 -4.57760453656856462767e-03j,
                8.58456925386544833501e-01 -5.12885666845818821713e-01j,
               -1.00000000000000000000e+00 +3.19314360992690246430e-13j,
                8.58457157333751386119e-01 +5.12885278617406625834e-01j,
                1.00000000000000000000e+00 -1.00276219317748158271e-15j,
                8.58457157333587184134e-01 -5.12885278617681517055e-01j,
               -9.99999999999897748459e-01 +4.52239765902612257729e-07j,
                8.60795949010275340818e-01 +5.08950227593523596425e-01j,
                8.60795718842493062795e-01 -5.08950616879904838008e-01j ]
        samp_src = gr.vector_source_c(samples,False,len(tsc));
        expected_result =  \
            [  -2.77555756156289135106e-17 -2.12816375845339272876e-03j,
               -3.53857681721841677014e-02 -6.93889390390722837765e-18j,
                9.02056207507939689094e-17 +2.55458973742708095322e-01j,
                9.29810599758725708774e-01 -2.49800180540660221595e-16j,
               -1.11022302462515654042e-16 -2.55458973741523820422e-01j,
               -3.53857111477571609215e-02 +6.24500451351650553988e-17j,
                2.04489203348146020289e-14 +1.58700798891242556010e-08j ]
        dst = gr.vector_sink_c(7)
        estimate = mlse_swig.channelestimate_vcc(16,7)
        self.tb.connect(tsc_src,  (estimate,0))
        self.tb.connect(samp_src, (estimate,1))
        self.tb.connect(estimate, dst)
        self.tb.run()
        result_data = dst.data ()
#        for i in range(7):
#            print expected_result[i], result_data[i], abs(expected_result[i]-result_data[i])

        self.assertComplexTuplesAlmostEqual (expected_result, result_data, 6)


if __name__ == '__main__':
    gr_unittest.main ()
