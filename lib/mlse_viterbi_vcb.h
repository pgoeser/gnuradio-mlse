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


#ifndef INCLUDED_MLSE_VITERBI_VCB_H
#define INCLUDED_MLSE_VITERBI_VCB_H

#include <gr_sync_block.h>
#include <gr_complex.h>
#include <vector>

#include "mlse_viterbi_impl.h"


class mlse_viterbi_vcb;

typedef boost::shared_ptr<mlse_viterbi_vcb> mlse_viterbi_vcb_sptr;

mlse_viterbi_vcb_sptr mlse_make_viterbi_vcb(int data_length, int channel_length, int tailsymbols, const std::vector<gr_complex> &modulation_table);


/*! \brief viterbi-decoder for MLSE-Equalization
 *
 * This block wraps viterbi_impl.
 * It applies the viterbi algorithm to given input samples with a given channel.
 *
 * Inputs:
 * (0) vector of gr_complex, length channel_length: Channel impulse response from channel estimator
 * (1) vector of gr_complex, length: channel_length + data_length + tailsymbols - 1: Samples to decode
 * (2) vector of byte, length channel_length - 1: last symbols of the training sequence, to initialize the decoder
 * Output:
 * (0) vector of byte, length data_length: decoded data.
 *
 *
 * \param channel_length length of the channel impulse response
 * \param data_length length of the data to decode
 * \param tailsymbols count of tailsymbols. All tailsymbols are assumed to be 0.
 *
 */
class mlse_viterbi_vcb : public gr_sync_block {
private:
	// configuration
	int d_channel_length;
	int d_data_length;
	int d_tailsymbols;
    
    // implementation
    boost::shared_ptr<mlse_viterbi_impl> d_impl;


	// private constructor
	mlse_viterbi_vcb(int data_length, int channel_length, int tailsymbols, const std::vector<gr_complex> &modulation_table);

	friend mlse_viterbi_vcb_sptr mlse_make_viterbi_vcb(int data_length, int channel_length, int tailsymbols, const std::vector<gr_complex> &modulation_table); // access for sptr-wrapper

public:
	~mlse_viterbi_vcb(); // destructor

	int work (int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
};


#endif // INCLUDED_MLSE_VITERBI_VCB_H
