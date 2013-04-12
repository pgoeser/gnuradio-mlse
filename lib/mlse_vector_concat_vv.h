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

#ifndef INCLUDED_MLSE_VECTOR_CONCAT_VV_H
#define INCLUDED_MLSE_VECTOR_CONCAT_VV_H

#include <gr_sync_block.h>

class mlse_vector_concat_vv;

// sptr
typedef boost::shared_ptr<mlse_vector_concat_vv> mlse_vector_concat_vv_sptr;

mlse_vector_concat_vv_sptr mlse_make_vector_concat_vv(int size1, int size2);

/*! \brief appends two incoming vectors into an outgoing one
 *
 * 		This block takes two stream of vectors as input and concatenates them into
 * 		one output vector
 *
 * 	\param size1 size of the first vector (in bytes)
 * 	\param size2 size of the second vector (in bytes)
 *
 */
class mlse_vector_concat_vv: public gr_sync_block
{
private:
	friend mlse_vector_concat_vv_sptr mlse_make_vector_concat_vv(int size1, int size2);

    mlse_vector_concat_vv(int size1, int size2);  	// private constructor

	int d_size1;
	int d_size2;

    public:
    ~mlse_vector_concat_vv();	// public destructor
		   
    // length of the input vector

    // Where all the action really happens
    int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif // INCLUDED_MLSE_VECTOR_CONCAT_VV_H
