from gnuradio import gr, digital
import mlse_swig as mlse

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
        self.mod = digital.gmskmod_bc(samples_per_symbol, 0.3, 8)

        # skip the first gmsk_length*samplerate/2 samples to make this block
        # acausal.
#        self.skiphead = gr.skiphead(gr.sizeof_gr_complex, int(samples_per_symbol*4.5));
#        self.connect(self.nrz, self.mod, self.skiphead, self)
        self.connect(self.nrz, self.mod, self)  # workaround: we need a negative delay later,
                                                # so let's not insert the positive delay here

