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


#include "mlse_ber_vbi.h"
#include <gr_io_signature.h>
#include <string.h>
#include <algorithm>

using namespace std;


/*
 * Create a new instance and return
 * a boost shared_ptr.  This is effectively the public constructor.
 */
mlse_ber_vbi_sptr 
mlse_make_ber_vbi(int size)
{
  return gnuradio::get_initial_sptr(new mlse_ber_vbi(size));
}


mlse_ber_vbi::mlse_ber_vbi(int size)
  : gr_sync_block ("ber_vbi",
		   gr_make_io_signature2 (2,2, size * sizeof(char), size * sizeof(char)),
		   gr_make_io_signature (0,1, sizeof(int))),
    d_size(size),
    d_bit_errors(0),
    d_bits(0),
    d_packet_errors(0),
    d_packets(0)
{
  // nothing else required
}

// destructor
mlse_ber_vbi::~mlse_ber_vbi ()
{
  // everything done automatically
}

void mlse_ber_vbi::reset(){
    d_bits = 0;
    d_bit_errors = 0;
    d_packets = 0;
    d_packet_errors = 0;
}

int 
mlse_ber_vbi::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
    // We deal with only one pair of input vectors per call
    if(noutput_items<1){
        return noutput_items;
    }
	
	// pointer casting
	const unsigned char* inptr1 = (unsigned char*) input_items[0];
	const unsigned char* inptr2 = (unsigned char*) input_items[1];
    
    // count bit errors in this packet
    int packet_bit_errors = 0;
    unsigned char c;
    for(int i=0; i<d_size; ++i){
        c = *inptr1++ ^ *inptr2++;
        if(c!=0){
            packet_bit_errors++;
        }
    }

    // update cumulative variables
    d_bit_errors += packet_bit_errors;
    d_bits += d_size;
    if(packet_bit_errors > 0){
        d_packet_errors += 1;
    }
    d_packets += 1;

    // output bit-errors if an output is connected
    if(output_items.size() >= 1){
        int* outptr = (int*) output_items[0];
        *outptr = packet_bit_errors;
    }

    // We deal with inputs one at a time
    return 1; 
}
