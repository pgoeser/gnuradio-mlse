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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mlse_derotate_cc.h"

#include <gr_io_signature.h>
#include <gr_complex.h>
#include <cmath>


mlse_derotate_cc_sptr
mlse_make_derotate_cc (int vlen, int samples_per_rotation, int offset)
{
	return mlse_derotate_cc_sptr (new mlse_derotate_cc (vlen, samples_per_rotation, offset));
}


mlse_derotate_cc::mlse_derotate_cc (int vlen, int samples_per_rotation, int offset)
	: gr_sync_block ("derotate_cc",
		gr_make_io_signature (1, 1, sizeof(gr_complex) * vlen),
		gr_make_io_signature (1, 1, sizeof(gr_complex) * vlen)),
    d_vlen(vlen),
    d_samples_per_rotation(samples_per_rotation),
    d_phase_accumulator(offset),
    d_offset(offset)
{
}


mlse_derotate_cc::~mlse_derotate_cc ()
{
}

//TODO: void set_rotation(int )

int
mlse_derotate_cc::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
	const gr_complex *in = (const gr_complex *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];

    int acc = d_phase_accumulator;
    gr_complex phase_per_sample = (std::complex<float>) std::polar(1., -M_PI*2./(double)d_samples_per_rotation);
    gr_complex a;
	for(int i=0; i<noutput_items; ++i){ // for each input element (be it vector or sample)
        if(d_vlen > 1){
            acc = d_offset; // reset phase at the start of vectors
        }
        for( int j=0; j<d_vlen; ++j){ // for each vector element (or once, if it's only a sample)
            a = *in++;
            a *= pow(phase_per_sample, acc);
            *out++ = a;
            acc = (acc + 1) % d_samples_per_rotation;
        }
    }
    d_phase_accumulator = acc;

	// Tell runtime system how many output items we produced.
	return noutput_items;
}

