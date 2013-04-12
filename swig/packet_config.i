

class packet_config {

public:
	packet_config(std::vector<unsigned int> part_lengths, 
			std::vector<gr_complex> constellation, 		
			std::vector<std::vector<unsigned char> > training_seqs,
			int tsc_matching_length,		// those two are a result of the mathematical
			int tsc_max_channel_length) throw(std::out_of_range);	// properties of the TSCs. Perhaps move somewhere else...

	bool validate() throw(std::out_of_range); // checks if the config data is complete and consistent

	// getters
	int get_piece_start(int piece_num) throw(std::out_of_range);
	int get_piece_len(int piece_num) throw(std::out_of_range);
	int get_total_len();

	std::vector<gr_complex> get_constellation();
	std::vector<std::vector<unsigned char> > get_training_seqs();
	int get_tsc_matching_length();
	int get_tsc_max_channel_length();

	// calculated variables
	int get_preload_len();

	mlse_vector_slice_vv_sptr make_slicer(
			int piece_num,
		   	int include_before = 0,
			int include_after = 0,
			int piece_count = 1,
			bool reverse = false);

	// modulation functions
	std::vector<std::vector<gr_complex> > modulate(const std::vector<std::vector<unsigned char> > &x);
//	std::vector<gr_complex> modulate(const std::vector<unsigned char> &x);
//	gr_complex modulate(unsigned char x);



// default lookup table factories
mlse_lut_bcv_sptr make_lut_channelestimate(int offset, int len);
mlse_lut_bbv_sptr make_lut_preload(int preload_len, bool reverse);

	// piece names
/*	const static int PIECE_TAILBITS_FRONT = 0;
	const static int PIECE_PAYLOAD_FRONT = 1;
	const static int PIECE_TRAINING_SEQ = 2;
	const static int PIECE_PAYLOAD_REAR = 3;
	const static int PIECE_TAILBITS_REAR = 4;
*/	enum piece { //TODO: use this
			TAILBITS_FRONT = 0,
			PAYLOAD_FRONT = 1,
			TRAINING_SEQ = 2,
			PAYLOAD_REAR = 3,
			TAILBITS_REAR = 4 };

};


