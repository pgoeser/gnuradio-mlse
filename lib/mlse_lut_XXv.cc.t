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
#include <algorithm> // for copy
#include <stdexcept> 


/*
 * Create a new instance and return
 * a boost shared_ptr.  This is effectively the public constructor.
 */
@SPTR_NAME@ 
mlse_make_@BASE_NAME@(std::vector<std::vector<@O_TYPE@> > const &data) throw(std::invalid_argument)
{
  return gnuradio::get_initial_sptr(new @NAME@(data));
}


@NAME@::@NAME@(std::vector<std::vector<@O_TYPE@> > const &data) throw(std::invalid_argument)
  : gr_sync_block ("@BASE_NAME@",
		   gr_make_io_signature (1,1, sizeof(@I_TYPE@)),
		   gr_make_io_signature (1,1, sizeof(@O_TYPE@) * data.at(0).size()))
{
	d_output_length = data.at(0).size(); //store the allocated output size 
	set_data(data);
}

// destructor
@NAME@::~@NAME@ ()
{
  // everything done automatically
}

void @NAME@::set_data(std::vector<std::vector<@O_TYPE@> > const &data) throw(std::invalid_argument){
	// validate data: check all sub-vectors are the same size
	int vector_len = data.at(0).size();
	if(vector_len != d_output_length){
		throw std::invalid_argument("@NAME@: data vectors in set_data() must be the same size as in constructor");
	}
	if(vector_len == 0){
		throw std::invalid_argument("@NAME@: data vectors may not be empty");
	}
	for(int i=0; i<data.size(); ++i){
		if(vector_len != data.at(i).size()){
			throw std::invalid_argument("@NAME@: data vectors must all be the same size");
		}
	}
	d_data = data;
}

std::vector<std::vector<@O_TYPE@> > @NAME@::slice_lut(
		const std::vector<std::vector<@O_TYPE@> > &lut,
		int start,
		int len,
		bool reverse) throw(std::invalid_argument)
{
    assert(start>=0);
	std::vector<std::vector<@O_TYPE@> > r(lut.size(),std::vector<@O_TYPE@>());
	for(int i=0; i<lut.size(); ++i){
        assert(start+len <= lut[i].size());
		if(reverse){
			r[i] = std::vector<@O_TYPE@>(lut[i].rbegin() + start, lut[i].rbegin() + start + len);
		} else { 
			r[i] = std::vector<@O_TYPE@>(lut[i].begin() + start, lut[i].begin() + start + len);
		}
	}
	return r;
}

int 
@NAME@::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
    // We deal with only one input vector per call
	
	// pointer casting. We don't care about the datatype and work with bytes
	@O_TYPE@* outptr = (@O_TYPE@*) output_items[0];

    @I_TYPE@ index = *((@I_TYPE@*) input_items[0]);

    if(index >= d_data.size()){
        index = 0; // out of range. FIXME: better handling than returning fist element
    }

    // copy data
	std::vector<@O_TYPE@>::iterator start, end;
	start = d_data.at(index).begin();
	end = d_data.at(index).end();
	std::copy(start, end, outptr); // behold the power of the STL

    // all done

    // We deal with inputs one at a time
    return 1; 
}
