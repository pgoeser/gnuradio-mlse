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


#ifndef INCLUDED_MLSE_DEROTATE_CC_H
#define INCLUDED_MLSE_DEROTATE_CC_H

#include <gr_sync_block.h>

class mlse_derotate_cc;
typedef boost::shared_ptr<mlse_derotate_cc> mlse_derotate_cc_sptr;

mlse_derotate_cc_sptr mlse_make_derotate_cc (int vlen=1, int samples_per_rotation=4, int offset=0);

/*!
 * \brief derotates a signal (useful for GMSK)
 *
 *      out[i] = in[i] * exp(-2pi * (i + offset)/samples_per_rotation)
 *      If vlen is >1, the block decodes an entire vector of samples at once
 *      and resets the accumulated rotation at every vector start.
 *
 * \param vlen length of the input vector, or 1 for per-sample operation
 * \param samples_per_rotation number of input samples for a 2pi-rotation
 * \param offset by how many samples the derotation gets shifted before being applied, see formula
 */
class mlse_derotate_cc : public gr_sync_block
{
	friend mlse_derotate_cc_sptr mlse_make_derotate_cc (int vlen, int samples_per_rotation, int offset);

private:
    mlse_derotate_cc (int vlen, int samples_per_rotation, int offset);

    int d_vlen;
    int d_samples_per_rotation;
    int d_phase_accumulator;
    int d_offset;

public:
	~mlse_derotate_cc ();


	int work (int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items);
};

#endif /* INCLUDED_MLSE_DEROTATE_CC_H */

