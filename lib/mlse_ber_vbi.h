/* -*- c++ -*- */
/*
 * Copyright 2004 Paul Goeser
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
#ifndef INCLUDED_MLSE_BER_VBI_H
#define INCLUDED_MLSE_BER_VBI_H

#include <gr_sync_block.h>

class mlse_ber_vbi;

// sptr
typedef boost::shared_ptr<mlse_ber_vbi> mlse_ber_vbi_sptr;

mlse_ber_vbi_sptr mlse_make_ber_vbi(int size);

/*! \brief calculates the number of bit errors in incoming packets
 *
 * 		This block takes two stream of byte-vectors as input. It compares them
 * 		and outputs the total number of different bytes between the two vectors
 * 		as an integer. 
 *
 * 		There are also functions to retrieve cumulative bit-errors without going
 * 		via the block flowgraph.
 *
 * 		This block is aimed at comparing packets with binary symbols. Higher-order
 * 		symbols should either be unpacked to one byte per bit beforehand, or the
 * 		block can be used directly to count symbol errors.
 *
 * 	\param size size of the input vectors (in bytes)
 *
 */
class mlse_ber_vbi: public gr_sync_block
{
private:
	friend mlse_ber_vbi_sptr mlse_make_ber_vbi(int size);

    mlse_ber_vbi(int size);  	// private constructor

    int d_size;
	int d_bit_errors;
	int d_packet_errors;
    int d_bits;
    int d_packets;

    public:
    ~mlse_ber_vbi();	// public destructor
		   
    // getters
    int bit_errors() const { return d_bit_errors; }
    int bits() const { return d_bits; }
    int packet_errors() const { return d_packet_errors; }
    int packets() const { return d_packets; }
    double bit_error_rate() const { 
        return (((double)d_bit_errors) / ((double)d_bits)); }
    double packet_error_rate() const { 
        return (((double)d_packet_errors) / ((double)d_packets)); }
   
    void reset();

    // Work function
    int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif // INCLUDED_MLSE_BER_VBI_H
