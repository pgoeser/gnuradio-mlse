GR_SWIG_BLOCK_MAGIC(mlse,equalizer_midamble_vcb);

class mlse_equalizer_midamble_vcb;


mlse_equalizer_midamble_vcb_sptr mlse_make_equalizer_midamble_vcb(packet_config conf);


class mlse_equalizer_midamble_vcb: public gr_hier_block2
{
private:
    friend mlse_equalizer_midamble_vcb_sptr mlse_make_equalizer_midamble_vcb(packet_config conf);

    mlse_equalizer_midamble_vcb(packet_config conf);  	// private constructor

public:
    ~mlse_equalizer_midamble_vcb();	// public destructor
		   
};

