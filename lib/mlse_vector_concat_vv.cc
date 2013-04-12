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


#include "mlse_vector_concat_vv.h"
#include <gr_io_signature.h>
#include <string.h>
#include <algorithm>

using namespace std;


/*
 * Create a new instance and return
 * a boost shared_ptr.  This is effectively the public constructor.
 */
mlse_vector_concat_vv_sptr 
mlse_make_vector_concat_vv(int size1, int size2)
{
  return gnuradio::get_initial_sptr(new mlse_vector_concat_vv(size1, size2));
}


mlse_vector_concat_vv::mlse_vector_concat_vv(int size1, int size2)
  : gr_sync_block ("vector_concat_vv",
		   gr_make_io_signature2 (2,2, size1, size2),
		   gr_make_io_signature (1,1, size1 + size2)),
    d_size1(size1),
    d_size2(size2)
{
  // nothing else required
}

// destructor
mlse_vector_concat_vv::~mlse_vector_concat_vv ()
{
  // everything done automatically
}

int 
mlse_vector_concat_vv::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
    // We deal with only one input vector per call
	
	// pointer casting. We don't care about the datatype and work with bytes
	const char* inptr1 = (char*) input_items[0];
	const char* inptr2 = (char*) input_items[1];
	char* outptr = (char*) output_items[0];

    // copy data
    memcpy(outptr, inptr1, d_size1);
    memcpy(outptr + d_size1, inptr2, d_size2);

    // all done

    // We deal with inputs one at a time
    return 1; 
}
