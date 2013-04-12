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

#include "mlse_channelestimate_vcc.h"
#include <gr_io_signature.h>
#include <string.h>
#include <algorithm>

using namespace std;


/*
 * Create a new instance and return
 * a boost shared_ptr.  This is effectively the public constructor.
 */
mlse_channelestimate_vcc_sptr 
mlse_make_channelestimate_vcc(int training_sequence_len, int channel_len)
{
  return gnuradio::get_initial_sptr(new mlse_channelestimate_vcc(training_sequence_len, channel_len));
}


mlse_channelestimate_vcc::mlse_channelestimate_vcc(int training_sequence_len, int channel_len)
  : gr_sync_block ("channelestimate_vcc",
		   gr_make_io_signature2 (2,2,sizeof(gr_complex) * training_sequence_len
			   						 ,sizeof(gr_complex) * training_sequence_len),
		   gr_make_io_signature (1,1,sizeof(gr_complex) * channel_len)),
    d_training_sequence_len(training_sequence_len),
    d_channel_len(channel_len)
{
  // nothing else required
}

// destructor
mlse_channelestimate_vcc::~mlse_channelestimate_vcc ()
{
  // everything done automatically
}

int 
mlse_channelestimate_vcc::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
    // We deal with only one input vector per call
	
	// pointer casting
	const gr_complex* inptr1 = (gr_complex*) input_items[0];
	const gr_complex* inptr2 = (gr_complex*) input_items[1];
	gr_complex* outptr = (gr_complex*) output_items[0];

	int start_offset = - (int)(d_channel_len/2);

	for(int k=0; k<d_channel_len; ++k){
		int shift = k + start_offset; // start_offset is negative

		// calculate dot product between conjugated shifted training sequence and
		// input samples. The training sequence shifting is cyclical (done with 
		// modulus).
		gr_complex prod = gr_complex(0.,0.);
		for(int i=0; i<d_training_sequence_len; ++i){
			gr_complex di = inptr1[
				(i - shift + d_training_sequence_len) % d_training_sequence_len];
			// Ok, WTF? In C++, the modulo of negative numbers is negative...
			prod += conj(di) * inptr2[i]; //FIXME: in practice di is real, why conj?
		}

		outptr[k] = prod / (float)(d_training_sequence_len);
	}

/*  // hard-coded GSM-sending-filter, was used for testing
	outptr[0] = gr_complex(0,0);
	outptr[1] = gr_complex(-0.02,0);
	outptr[2] = gr_complex(0,0.197);
	outptr[3] = gr_complex(0.96,0);
	outptr[4] = gr_complex(0,-0.197);
	outptr[5] = gr_complex(-0.02,0);
	outptr[6] = gr_complex(0,0);
*/



    // all done

    // We deal with inputs one at a time
    return 1; 
}
