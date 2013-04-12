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


#ifndef INCLUDED_MLSE_VECTOR_SLICE_VV_H
#define INCLUDED_MLSE_VECTOR_SLICE_VV_H

#include <gr_sync_block.h>

class mlse_vector_slice_vv;

// sptr
typedef boost::shared_ptr<mlse_vector_slice_vv> mlse_vector_slice_vv_sptr;

mlse_vector_slice_vv_sptr mlse_make_vector_slice_vv(int itemsize, int input_length, int output_offset, int output_length, bool reverse = false);

/*! \brief cuts out a piece of an incoming vector
 *
 * 		This block takes a stream of vectors as input, trims them or extends them with
 * 		zeros as per settings, and outputs them again.
 *
 * 	\param itemsize Size of an item in the vectors
 * 	\param input_length Length of the input vectors
 * 	\param output_offset offset on where in the input vector to start the output vector. If positive, results in the first output_offset items being skipped. May be negative, resulting in prepended zeros.
 * 	\param output_length Length of the output vector. If longer than input_length + output_offset, the remainder is filled with zeros.
 * 	\param reverse If true, the output vectors are reversed after slicing. Defaults to false.
 *
 */
class mlse_vector_slice_vv: public gr_sync_block
{
private:
	friend mlse_vector_slice_vv_sptr mlse_make_vector_slice_vv(int itemsize, int input_length, int output_offset, int output_length, bool reverse);

    mlse_vector_slice_vv(int itemsize, int input_length, int output_offset, int output_length, bool reverse = false);  	// private constructor

	int d_itemsize;
	int d_input_length;
	int d_output_offset;
	int d_output_length;
	bool d_reverse;

    public:
    ~mlse_vector_slice_vv();	// public destructor
		   
    // length of the input vector

    // Where all the action really happens
    int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif // INCLUDED_MLSE_VECTOR_SLICE_VV_H
