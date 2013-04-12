
from gnuradio import gr, digital
import mlse_swig as mlse
import random

# takes raw data, returns a modulated gsm stream of samples
class gsm_modulate_burst(gr.hier_block2):
    def __init__(self, samples_per_symbol = 1):
        gr.hier_block2.__init__(self, 'gsm_modulate_burst',
                gr.io_signature2(2,2,58*2,1),
                gr.io_signature(1, 1, gr.sizeof_gr_complex))
        
        # take care of the packetizer
        conf = mlse.make_packet_config_gsm()
        self.packetbuilder = mlse.packetbuilder_midamble_vbb(conf)
        self.connect(self, self.packetbuilder)
        self.connect((self,1), (self.packetbuilder,1)) # tsc_index

        # append 8 1-bits to the packet for the idle-time between bursts.
        # This is necessary among others to flush the gmsk-modulator which is causal.
        # (We use 1-bits because that's what the gsm-standard says)
        # (we also only use 8 bits, because the specified value of 8.25 only works 
        # properly with samplerates that are a multiple of 4.)
        self.guardbits = gr.vector_source_b((1,)*8,True,8)
        self.guard_cat = mlse.vector_concat_vv(148,8)
        self.connect(self.packetbuilder, self.guard_cat)
        self.connect(self.guardbits, (self.guard_cat,1))

        # we now have a vector of bits, transform that into a stream
        self.tostream = gr.vector_to_stream(1, 148+8)
        # do the precoding:
        self.diffcode = gr.diff_decoder_bb(2);
        self.nrz = gr.map_bb([1,-1])
        self.connect(self.guard_cat, self.tostream, self.diffcode, self.nrz)

        # modulate
        #self.mod = gr.gmskmod_bc(samples_per_symbol, 0.3, 8)
        self.mod = digital.gmskmod_bc(samples_per_symbol, 0.3, 8)

        # skip the first gmsk_length*samplerate/2 samples to make this block
        # acausal.
        self.skiphead = gr.skiphead(gr.sizeof_gr_complex, samples_per_symbol*8/2);
        self.connect(self.nrz, self.mod, self.skiphead, self)


# does single-symbol ML decisions to receive a demodulated derotated gsm burst,
# as comparison to MLSE
class gsm_decode_harddecision(gr.hier_block2):
    def __init__(self):
        gr.hier_block2.__init__(self, 'gsm_decode_harddecision',
                gr.io_signature(1, 1, gr.sizeof_gr_complex * 148 ),
                gr.io_signature(1,1,58*2))

        conf = mlse.make_packet_config_gsm()
        slice1 = conf.make_slicer(conf.PAYLOAD_FRONT)
        slice2 = conf.make_slicer(conf.PAYLOAD_REAR)
        cat = mlse.vector_concat_vv(58*gr.sizeof_gr_complex, 58*gr.sizeof_gr_complex)
        tostream = gr.vector_to_stream(gr.sizeof_gr_complex, 58*2)
        real = gr.complex_to_real()
        sign = gr.binary_slicer_fb()
        demap = gr.map_bb([1,0]) # invert 0 and 1 because in gsm 0 is mapped to +1 and 1 to -1
        tovect = gr.stream_to_vector(1, 58*2)

        self.connect(self, slice1)
        self.connect(self, slice2)
        self.connect(slice1, (cat,0))
        self.connect(slice2, (cat,1))
        self.connect(cat, tostream, real, sign, demap, tovect, self)

        

# adapts a channel (which is a gr-block) which uses non-vector I/O to vector input and output
# delay is the internal delay of the channel-block to compensate
class apply_channel_to_vect(gr.hier_block2):
    def __init__(self, vlen, channel_block, delay):
        gr.hier_block2.__init__(self, 'apply_channel_to_vect',
                gr.io_signature(1, 1, gr.sizeof_gr_complex * vlen),
                gr.io_signature(1, 1, gr.sizeof_gr_complex * vlen))

        self.vlen=vlen
        self.channel_block = channel_block
        self.delay = delay

        pad_len = self.vlen + self.delay*2

        if(delay>0):
            self.padding = gr.vector_source_c((0,)*(delay*2),True,delay*2)
            self.pad_cat = mlse.vector_concat_vv(vlen*gr.sizeof_gr_complex,gr.sizeof_gr_complex*2*delay)
            self.connect(self, self.pad_cat)
            self.connect(self.padding, (self.pad_cat,1))
        else:
            self.pad_cat = self 

        self.tostream = gr.vector_to_stream(gr.sizeof_gr_complex, pad_len)
        self.connect(self.pad_cat, self.tostream)

        # connect channel
        self.connect(self.tostream, self.channel_block)

        self.tovector = gr.stream_to_vector(gr.sizeof_gr_complex, pad_len)
        self.connect(self.channel_block, self.tovector)

        # cut out the proper part 
        self.trim = mlse.vector_slice_vv(
                gr.sizeof_gr_complex, 
                pad_len, 
                delay,
                vlen)
        self.connect(self.tovector, self.trim, self)
        #FIXME channel_model is not a sync block, this block doesn't work as expected
        

# unused
class decimate(gr.hier_block2):
    def __init__(self, n, delay):
        gr.hier_block2.__init__(self, 'decimate',
                gr.io_signature(1,1,1),
                gr.io_signature(1,1,1))
        self.n=n
        self.delay=delay
        self.block_delay = gr.delay(1,delay)
        self.block_1_in_n = gr.keep_one_in_n(1, n)
        self.connect(self, self.block_delay, self.block_1_in_n, self)

                
# a simple channel model, with AWGN and a given impulse response
class my_channel(gr.hier_block2):
    """simple awgn and fir channel model.
    rationale: gr_channel_model_cc has a rather large delay and is not sync"""
    def __init__(self, noise_dB=-20., fir_taps=[1]):
        gr.hier_block2.__init__(self, 'my_channel',
                gr.io_signature(1,1,gr.sizeof_gr_complex),
                gr.io_signature(1,1,gr.sizeof_gr_complex))
        noise_adder = gr.add_cc()
        noise = gr.noise_source_c(gr.GR_GAUSSIAN, 10**(noise_dB/20.), random.randint(0,2**30))
        # normalize taps
        firtabsum = float(sum(map(abs,fir_taps)))
        fir_taps = [ i / firtabsum for i in fir_taps]
        multipath = gr.fir_filter_ccc(1,fir_taps)

        self.connect(noise, (noise_adder,1))
        self.connect(self, noise_adder, multipath, self)

        self.__dict__.update(locals()) # didn't feel like using self all over the place
    def set_noise_dB(self, noise_dB):
        self.noise_dB = noise_dB
        self.noise.set_amplitude(10**(noise_dB/20.))

    def set_taps(self, taps):
        self.fir_taps = taps
        self.multipath.set_taps(taps)
    

