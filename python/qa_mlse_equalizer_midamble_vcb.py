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
from testapp import Simulation
import math, cmath, random

class qa_mlse_equalizer_midamble_vcb(gr_unittest.TestCase):

    def setUp (self):
        self.simu = Simulation(1000)

    def tearDown (self):
        self.simu = None

    def test_001_no_channel(self):
        self.simu.set_channel([0,0,0,1,0,0,0])
        self.simu.set_snr(100)
        ber=self.simu.execute()
        print "dirac channel, snr:100 ber:%s"%ber
        assert(ber < 0.001)
        self.simu.set_snr(10)
        ber=self.simu.execute()
        print "dirac channel, snr:10 ber:%s"%ber
        assert(ber < 0.01)

    def test_002_ramdom_channel(self):
        chan = [ cmath.rect(1,2*math.pi*random.random()) for i in range(6) ] + [0]
        self.simu.set_channel(chan)
        self.simu.set_snr(100)
        ber=self.simu.execute()
        print "random channel, snr:100 ber:%s"%ber
        assert(ber < 0.001)
        self.simu.set_snr(10)
        ber=self.simu.execute()
        print "random channel, snr:10 ber:%s"%ber
        assert(ber < 0.05)



if __name__ == '__main__':
    gr_unittest.main ()
