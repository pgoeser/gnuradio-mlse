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

#ifndef INCLUDED_MLSE_CHANNELESTIMATE_VCC_H
#define INCLUDED_MLSE_CHANNELESTIMATE_VCC_H

#include <gr_sync_block.h>

class mlse_channelestimate_vcc;

// sptr
typedef boost::shared_ptr<mlse_channelestimate_vcc> mlse_channelestimate_vcc_sptr;

mlse_channelestimate_vcc_sptr mlse_make_channelestimate_vcc(int training_sequence_len, int channel_len);

/*! \brief Estimates a channel impulse response with the aid of a modulated training sequence
 *
 * 		This block takes as first stream the modulated training sequence and as
 * 		second stream the samples of a burst that include the training sequence.
 * 		It outputs an estimate of the channel.
 * 		All inputs and outputs are vectors for one burst.
 * 		The second input stream has the same length than the training sequence.
 * 		Do not include the repeated parts of the training sequence!
 *
 *
 * 	\param training_sequence_len Length of the training sequence (first input stream)
 * 	\param channel_len Length of the channel estimate (output stream)
 *
 */
class mlse_channelestimate_vcc: public gr_sync_block
{
private:
	friend mlse_channelestimate_vcc_sptr mlse_make_channelestimate_vcc(int training_sequence_len, int channel_len);

    mlse_channelestimate_vcc(int training_sequence_len, int channel_len);  	// private constructor

	int d_training_sequence_len;
	int d_channel_len;

    public:
    ~mlse_channelestimate_vcc();	// public destructor
		   
    // length of the input vector

    // Where all the action really happens
    int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif // INCLUDED_MLSE_CHANNELESTIMATE_VCC_H
