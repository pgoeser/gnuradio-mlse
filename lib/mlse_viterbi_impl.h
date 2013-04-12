/* -*- c++ -*- */
/*
 * Copyright 2011 Paul Goeser
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */


#ifndef INCLUDED_MLSE_VITERBI_IMPL_H
#define INCLUDED_MLSE_VITERBI_IMPL_H

#include <gr_complex.h>
#include <vector>
#include "channel_model.h"

/*! \brief Implementation of the Viterbi Algorithm
 *
 *
 *
 *
 * \param modulation_table Modulation table to decode, this is handed directly
 * to channel_model, but is also used to size some internal data structures.
 */
class mlse_viterbi_impl {
private:
	// config from channel_model
	int d_L; // channel length
	int d_M; // modulation symbol alphabet size

	// cached helper variables
	int d_ld_M; // ld(d_M)
	int d_supersymbol_count; // M^(L-1)
	int d_input_shift; // amount to shift the input bits, as the newest input 
			// symbol ends up as the MSBs of the supersymbol-index. ld(M^(L-2))
	void update_cached_vars(); // updates those helper variables


	//state
    std::vector<float> d_state_probs;
    std::vector<vector<int> > d_trackback_decisions;
    bool d_previous_was_forced_symbol;
	
	//temporary state (so it doesn't get allocated and destroyed every iteration
    std::vector<float> d_temp_state_probs;

	channel_model d_model;
	

	// small private helper functions
	int inline supersymbol_count() const {return pow(d_M, d_L-1);} //TODO:remove (we have d_supersymbol_count)
	int inline next_supersym(int supersymbol, int input_symbol) const {
		return (supersymbol >> d_ld_M) + (input_symbol << d_input_shift);
	}
    int inline extract_newsym_from_supersymbol(int supersymbol) const {
        return (supersymbol >> d_input_shift);
    }

	int symbols_to_state(std::vector<int>); //TODO: write?

	void resize_vectors(); //TODO: write?


	inline float calculate_metric(int supersym, int nextsym, gr_complex const received_sample);


public:
    // constructor
	mlse_viterbi_impl(const std::vector<gr_complex> &modulation_table); //
    // destructor
	~mlse_viterbi_impl(); 

    // set a new channel impulse response
	void update_channel(vector<gr_complex> impulse_response){
        d_model.update_channel(impulse_response);}

	void decode_init(std::vector<char> initdata); // initializes the data structures
			// this version sets the initial state of the decoder. All other
			// states get a metric of -inf, the given state gets 0.
			// (reminder: the higher the metric, the more probable the symbol sequence)

    // execute one decoding step, given a sample.
    // If the symbol at the current position is known, it can be passed as 
    // forced_symbol, this is used to pass tailsymbols.
	void decode_step(gr_complex sample, int forced_symbol=-1);

	// FUTURE: decode_guardsymbol // adds samples from the end of a burst, where parts of the channel state have 'no-transmission'-symbols
	
    // returns the trackback from the most likely state. At the end of a 
    // decoding run, this is the final result.
    // Output is stored in out, which is of length len.
    // (only data-symbols will be output, no tailsymbols, up to len of them)
    void decode_result(char* out, int len);

	// decodes 'length' data-symbols followed by 'tailsymbols' tail-symbols.
    // both by calling decode_step
    void decode_run(const gr_complex *samples_ptr, int length, int tailsymbols);

};


#endif // INCLUDED_MLSE_VITERBI_VCB_H
