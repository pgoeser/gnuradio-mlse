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


// @WARNING@

#include "@NAME@.h"
#include <gr_io_signature.h>
#include <time.h> 


/*
 * Create a new instance and return
 * a boost shared_ptr.  This is effectively the public constructor.
 */
@SPTR_NAME@ 
mlse_make_@BASE_NAME@(@O_TYPE@ range)
{
  return gnuradio::get_initial_sptr(new @NAME@(range));
}


@NAME@::@NAME@(@O_TYPE@ range)
  : gr_sync_block ("@BASE_NAME@",
		   gr_make_io_signature (0,0,0), 
		   gr_make_io_signature (1,1, sizeof(@O_TYPE@))),
	d_range(range),
	d_rng(boost::mt19937(time(0))),
	d_dist(boost::uniform_int<>(0,range-1)),
	d_vargen(d_rng, d_dist) 
{
}

// destructor
@NAME@::~@NAME@ ()
{
  // everything done automatically
}


int 
@NAME@::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
	
	// pointer casting. We don't care about the datatype and work with bytes
	@O_TYPE@* outptr = (@O_TYPE@*) output_items[0];

	@O_TYPE@ x;
	for(int i=0; i<noutput_items; ++i){
		x = d_vargen();
		*outptr++ = x;
	}

    return noutput_items; 
}
