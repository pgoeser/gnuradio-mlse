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



#include "mlse_vector_slice_vv.h"
#include <gr_io_signature.h>
#include <string.h>
#include <algorithm>

using namespace std;


/*
 * Create a new instance and return
 * a boost shared_ptr.  This is effectively the public constructor.
 */
mlse_vector_slice_vv_sptr 
mlse_make_vector_slice_vv(int itemsize, int input_length, int output_offset, int output_length, bool reverse)
{
  return gnuradio::get_initial_sptr(new mlse_vector_slice_vv(itemsize, input_length, output_offset, output_length, reverse));
}


mlse_vector_slice_vv::mlse_vector_slice_vv(int itemsize, int input_length, int output_offset, int output_length, bool reverse)
  : gr_sync_block ("vector_slice_vv",
		   gr_make_io_signature (1,1, itemsize*input_length),
		   gr_make_io_signature (1,1, itemsize*output_length)),
	d_itemsize(itemsize),
	d_input_length(input_length),
	d_output_length(output_length),
	d_output_offset(output_offset),
	d_reverse(reverse)
{
  // nothing else required
}

// destructor
mlse_vector_slice_vv::~mlse_vector_slice_vv ()
{
  // everything done automatically
}

int 
mlse_vector_slice_vv::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
    // We deal with only one input vector per call
	
	// pointer casting. We don't care about the datatype and work with bytes
	const char* inptr = (char*) input_items[0];
	char* outptr = (char*) output_items[0];

/*	// clear zeros in the beginning of output
	if(d_output_offset < 0){ // (if necessary)
		bzero(outptr, d_itemsize * (-d_output_offset));
	}*/

	// first clear the entire output vector, since that saves us from calculating exactly
	// where to clear
	bzero(outptr, d_output_length * d_itemsize);

	// copy data
	int copy_items;
	if(d_output_offset > 0){ // skip some items
		copy_items = min(d_input_length - d_output_offset, d_output_length);
		memcpy(outptr, inptr + (d_itemsize * d_output_offset), copy_items * d_itemsize);
	} else { 
		copy_items = min(d_input_length, d_output_length + (-d_output_offset));
		memcpy(outptr + (d_itemsize * (-d_output_offset)), inptr, copy_items * d_itemsize);
	}

	// reverse output vector (if necessary)
	if(d_reverse){
		//FIXME: inefficient
		char buf[d_itemsize];
		for(int i=0; i<(d_output_length/2); ++i){
			memcpy(buf, outptr+(i*d_itemsize), d_itemsize);
			memcpy(outptr+(i*d_itemsize), outptr + ((d_output_length-i-1)*d_itemsize), d_itemsize);
			memcpy(outptr + ((d_output_length-i-1)*d_itemsize), buf, d_itemsize);
		}
	}


    // We deal with inputs one at a time
    return 1; 
}
