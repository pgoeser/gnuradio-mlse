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

#ifndef INCLUDED_PACKET_CONFIG_H
#define INCLUDED_PACKET_CONFIG_H

#include <vector>
#include <gr_types.h>
#include <stdexcept>

#include "mlse_vector_slice_vv.h"
#include "mlse_lut_bbv.h"
#include "mlse_lut_bcv.h"


/*! \brief configuration-data for blocks dealing with bursts that use a training sequence as midamble
 *
 * This contains everything necessary to decode a midamble-burst-system:
 * - length of the parts of the burst (tailbits, payloads, tsc)
 * - constellation used in the modulation
 * - training sequences
 * - parameters for training sequence channel-estimation
 */
class packet_config {

public:
	// constructor
	packet_config(std::vector<unsigned int> part_lengths, 
			std::vector<gr_complex> constellation, 		
			std::vector<std::vector<unsigned char> > training_seqs,
			int tsc_matching_length,		// those two are a result of the mathematical
			int tsc_max_channel_length) throw(std::out_of_range);	// properties of the TSCs. Perhaps move somewhere else...

	packet_config(); // FIXME: handle default constructor more cleanly
	/*
protected:
	// default constructor: only accessible to subclasses, so incomplete packets
	// can't be constructed
	packet_config();
	*/

private:
	// members: configuration data
	std::vector<unsigned int> d_part_lengths;
	std::vector<gr_complex> d_constellation;	
	std::vector<std::vector<unsigned char> > d_training_seqs;
	int d_tsc_matching_length; 
	int d_tsc_max_channel_length;

	// size of the data types
	int d_symbol_type_size; // default: sizeof(unsigned char)
	int d_sample_type_size; // default: sizeof(gr_complex)

	bool d_is_valid; // set by validate

	// check if a piece_num is in range, throw(std::out_of_range) otherwise
	void validate_piece_num(int piece_num) throw(std::out_of_range); 

public:
	bool validate() throw(std::out_of_range); // checks if the config data is complete and consistent

	// getters
	int get_piece_start(int piece_num) throw(std::out_of_range);
	int get_piece_len(int piece_num) throw(std::out_of_range);
	int get_total_len();

	std::vector<gr_complex> get_constellation() const {return d_constellation;}
	std::vector<std::vector<unsigned char> > get_training_seqs() const {return d_training_seqs;}
	int get_tsc_matching_length() const {return d_tsc_matching_length;}
	int get_tsc_max_channel_length() const {return d_tsc_max_channel_length;}

	// calculated variables
	int get_preload_len();

	/*! \brief creates a vector_slice_vv object to slice out some of the samples
	 * 
	 * 		This function is an easy to use wrapper to give you a sample slicer with
	 * 		exactly	the right parameters for optaining one or more of the segments
	 * 		in a packet.
	 *
	 * 	\param piece_num starting piece number. Use one of the enum piece constants.
	 * 	\param include_before amount of samples before the piece start to include
	 * 	\param include_after amount of samples after the (last specified) piece to include
	 * 	\param reverse specify if the output needs to be reversed. Doing so switches the
	 * 			semantics of the other parameters around in such a way that you can for
	 * 			example use the exact same line for PAYLOAD_REAR and PAYLOAD front, 
	 * 			except for the different piece_num and reverse parameter.
	 * 	\param piece_count number of pieces starting with piece_num to slice
	 */
	mlse_vector_slice_vv_sptr make_slicer(
			int piece_num,
		   	int include_before = 0,
			int include_after = 0,
			int piece_count = 1,
			bool reverse = false);

	// modulation functions
	std::vector<std::vector<gr_complex> > modulate(const std::vector<std::vector<unsigned char> > &x);
	std::vector<gr_complex> modulate(const std::vector<unsigned char> &x);
	gr_complex modulate(unsigned char x);


// default lookup table factories
mlse_lut_bcv_sptr make_lut_channelestimate(int offset, int len);
mlse_lut_bbv_sptr make_lut_preload(int preload_len, bool reverse=false);

	// piece names
/*	const static int PIECE_TAILBITS_FRONT = 0;
	const static int PIECE_PAYLOAD_FRONT = 1;
	const static int PIECE_TRAINING_SEQ = 2;
	const static int PIECE_PAYLOAD_REAR = 3;
	const static int PIECE_TAILBITS_REAR = 4;
*/	enum piece { //TODO: use this
			TAILBITS_FRONT = 0,
			PAYLOAD_FRONT = 1,
			TRAINING_SEQ = 2,
			PAYLOAD_REAR = 3,
			TAILBITS_REAR = 4 };

};








#endif //INCLUDED_PACKET_CONFIG_H
