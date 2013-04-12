#!/usr/bin/env python
# -*- coding: utf-8 -*-
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

from gnuradio import gr
from gnuradio import blks2
import mlse_swig as mlse
import time, random
import gsm


def printvect_c(d):
    print "\n".join(["% 1.3f % 1.3f"%((i).real, (i).imag) for i in d])
    print len(d)

def print_bitvect(v):
    print "".join([ str(int(bool(i))) for i in v])

def main():
    tb = gr.top_block()
    # noise source
    n = mlse.randint_b(2)
    reshape_n = gr.stream_to_vector(1, 58*2)
    tb.connect(n,reshape_n)
    # limit experiment length
    head=gr.head(58*2,1)
    tb.connect(reshape_n, head)
    
    # modulate
    gsm_burstmod = gsm.gsm_modulate_burst()
    tb.connect(head, gsm_burstmod)
    random_tsc_index = mlse.randint_b(8);
    tb.connect(random_tsc_index, (gsm_burstmod,1))

    # apply channel
    channel = gsm.my_channel(-100,[0.2,1,0.2])
    channel_apply = gsm.apply_channel_to_vect(148, channel, 1)
    tb.connect(gsm_burstmod, channel_apply)

    # derotate
    derot = mlse.derotate_cc(148,4,-1) #FIXME: why -1? (not as if it matters)
    tb.connect(channel_apply, derot)

    modsink = gr.vector_sink_c(148)
    datasink = gr.vector_sink_b(58*2)
    tb.connect(derot, modsink)
    tb.connect(head, datasink)


    # mlse-decode
    decode = mlse.equalizer_midamble_vcb(mlse.make_packet_config_gsm())
    tb.connect(derot,decode)
    tb.connect(random_tsc_index, (decode,1))

    # ber
    ber = mlse.ber_vbi(58*2)
    tb.connect(decode, ber)
    tb.connect(head, (ber,1))

    result_sink = gr.vector_sink_b(58*2)
    tb.connect(decode, result_sink)

    chanest_sink = gr.vector_sink_c(7)
    tb.connect((decode,1), chanest_sink)

    tb.run()
    tb.run()
    d=modsink.data()
#    printvect_c(d)
    print ber.bit_error_rate()
    print_bitvect( datasink.data())
    print_bitvect( result_sink.data())
    import operator
    print_bitvect( map(operator.xor, datasink.data(), result_sink.data()))
    printvect_c( chanest_sink.data())
    

def test_viterbi():
    tb=gr.top_block()
    channel = [0,1,0]
    prellen = len(channel) - 1
    data = [random.randint(0,1) for i in xrange(20+prellen)]
    data.extend([0,0,0]) # tailbits
    data.extend([0]*6) # to flush modulator


    # modulate
    datasrc = gr.vector_source_b(data)
    diffcode = mlse.xor_encode_bb();
    nrz = gr.map_bb([1,-1])
    mod = gr.gmskmod_bc(1, 0.3, 12)
    derot = mlse.derotate_cc();
    modsink = gr.vector_sink_c();
    tb.connect(datasrc, diffcode, nrz, mod, derot, modsink)
    tb.run()

    samples = modsink.data()[6:]
    printvect_c(samples)


    preload = data[:prellen]
    rest = data[prellen:][:20]

    tb=gr.top_block()
    conf=mlse.make_packet_config_gsm()
    vit = mlse.viterbi_vcb(20,len(channel),3,[1,-1])#conf.get_constellation())
    chansrc = gr.vector_source_c(channel,vlen=len(channel))
    preloadsrc = gr.vector_source_b(preload,vlen=len(preload))
    samplesrc = gr.vector_source_c(samples,vlen=len(samples))
    sink = gr.vector_sink_b(20)
    tb.connect(chansrc, (vit,0))
    tb.connect(samplesrc, (vit,1))
    tb.connect(preloadsrc, (vit,2))
    tb.connect(vit, sink)
    tb.run()
    print_bitvect(preload)
    print_bitvect(rest)
    print_bitvect(sink.data())


def decimate(l,n,skip=0):
    it=iter(l)
    for i in xrange(skip):
        it.next()
    while True:
        yield it.next()
        for i in xrange(n-1):
            it.next()


def test_mod_cpm():
    tb = gr.top_block()
    precode = mlse.xor_encode_bb()
    nrz = gr.map_bb([1,-1])
    mod = gr.gmskmod_bc(1,0.3,4)
    #src = gr.vector_source_b([0,0,0,0,1,1,1,1,0,0])
    #src = gr.vector_source_b([1,1,0,0,1,0,0,1,1,1,0,0,0,0])
    src = gr.vector_source_b((1,)*1000)
    sink = gr.vector_sink_c()
    derot = mlse.derotate_cc(1,4)
    tb.connect(src, precode, nrz, mod, derot, sink)
    precode_probe = gr.vector_sink_b()
    tb.connect(nrz, precode_probe)
    tb.run()
    d = sink.data()
    from cmath import phase, pi, rect
    real = lambda x: x.real
    import operator
    d_r = d#list(decimate(d,5,2))
    d2 = [ int(round((phase(i)/pi)*100)) for i in d ]
    derotate = [ (-1j)**(i+1) for i in range(len(d_r))]
    d3 = map(operator.mul, d_r, derotate)
#    print "\n".join(map(str,map(real,d3)))
    print precode_probe.data()
#    print "\n".join(map(str,map(phase,d)))
    print "\n".join([str(phase(i)/pi) for i in d])
    print len(d)
    print derotate

def make_noise_gronly():
    global tb
    n = gr.noise_source_f(gr.GR_UNIFORM,1)
    ftob = gr.binary_slicer_fb()
    tb.connect(n,ftob)
    return ftob

def test_freqmod() :
    from cmath import phase, pi, rect
    tb = gr.top_block()
    src = gr.vector_source_f((1,)*1000)
    mod = gr.frequency_modulator_fc(pi)
    sink = gr.vector_sink_c()
    tb.connect(src, mod, sink)
    tb.run()
    d = sink.data()
    print "\n".join(["% 1.3f % 1.3f"%((i*1j).real, (i*1j).imag) for i in d])

def test_channel_model():
    tb = gr.top_block()
    src = gr.vector_source_c([0,0,1,0,0,0,0,0])
#    mod = gr.channel_model()
    mod = gsm.my_channel(-20,[1,1])
    sink = gr.vector_sink_c()
    tb.connect(src, mod, sink)
    tb.run()
    printvect_c( sink.data() )
    print mod.firtabsum


def run():
#    test_mod_cpm()
    main()
#    test_viterbi()
#    test_freqmod()
#    test_channel_model()



if __name__=="__main__":
    run()
