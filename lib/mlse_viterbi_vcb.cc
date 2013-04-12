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

#include "mlse_viterbi_vcb.h"
#include <iostream>
#include <typeinfo>
#include <gr_io_signature.h>


mlse_viterbi_vcb_sptr mlse_make_viterbi_vcb(int data_length, int channel_length, int tailsymbols, const std::vector<gr_complex> &modulation_table){
	return gnuradio::get_initial_sptr(new mlse_viterbi_vcb(data_length, channel_length,
				tailsymbols, modulation_table));
}


// constructors
mlse_viterbi_vcb::mlse_viterbi_vcb(int data_length, int channel_length, int tailsymbols, const std::vector<gr_complex> &modulation_table) 
	: gr_sync_block("viterbi_vcb",
			gr_make_io_signature3(3,3,
				sizeof(gr_complex)*(channel_length),
				sizeof(gr_complex)*(data_length + tailsymbols), //TODO: figure out real length (which can't be done cleanly)
				sizeof(char)*(channel_length - 1)),
			gr_make_io_signature(1,1,
				sizeof(char)*data_length)),
	d_channel_length(channel_length),
    d_data_length(data_length), 
	d_tailsymbols(tailsymbols),
    d_impl(new mlse_viterbi_impl(modulation_table))
{
}
			

// destructor (empty)
mlse_viterbi_vcb::~mlse_viterbi_vcb()
{
}

int mlse_viterbi_vcb::work (int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items)
{
	// we only handle one item.
	if(noutput_items < 1){
		return 0;
	}
	//TODO: we use a fixed channel length. Make variable.
    
//    assert(d_channel_length % 2 == 1); //TODO: only odd channel lengths supported

	// cast the pointers of the streams
	const gr_complex* chan_ptr = (gr_complex *) (input_items[0]);
	const gr_complex* samples_ptr = (gr_complex *) (input_items[1]);
	const char* initdata_ptr = (char *) (input_items[2]);
	char * data_ptr = (char *) output_items[0];

	// make vector objects from (some) input streams
    std::vector<gr_complex> channel(chan_ptr, chan_ptr + d_channel_length);
    std::vector<char> initdata(initdata_ptr, initdata_ptr + d_channel_length-1);

    // recalculate the lookup-table for the channel model
	d_impl->update_channel(channel); 

	// run the viterbi algorithm
	d_impl->decode_init(initdata);

//    samples_ptr += (d_channel_length-1)/2; //TODO: Inputlänge anpassen, dokumentieren

    d_impl->decode_run(samples_ptr, d_data_length, d_tailsymbols);
    d_impl->decode_result(data_ptr, d_data_length);




	return 1; // we handled one item
}

		


