GR_SWIG_BLOCK_MAGIC(mlse,packetbuilder_midamble_vbb);

class mlse_packetbuilder_midamble_vbb;


mlse_packetbuilder_midamble_vbb_sptr mlse_make_packetbuilder_midamble_vbb(packet_config conf);


class mlse_packetbuilder_midamble_vbb: public gr_hier_block2
{
private:
    friend mlse_packetbuilder_midamble_vbb_sptr mlse_make_packetbuilder_midamble_vbb(packet_config conf);

    mlse_packetbuilder_midamble_vbb(packet_config conf);  	// private constructor

public:
    ~mlse_packetbuilder_midamble_vbb();	// public destructor
		   
};

