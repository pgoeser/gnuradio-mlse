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

#include "packet_config.h"

#include <stdexcept>
#include <algorithm> // for swap
#include <assert.h>


// constructor
packet_config::packet_config(std::vector<unsigned int> part_lengths, 
			std::vector<gr_complex> constellation, 		
			std::vector<std::vector<unsigned char> > training_seqs,
			int tsc_matching_length,	
			int tsc_max_channel_length)  throw(std::out_of_range) :
	d_part_lengths(part_lengths),
	d_constellation(constellation),
	d_training_seqs(training_seqs),
	d_tsc_matching_length(tsc_matching_length),
	d_tsc_max_channel_length(tsc_max_channel_length),
	d_sample_type_size(sizeof(gr_complex)),
	d_symbol_type_size(sizeof(unsigned char)),
	d_is_valid(false) // default value
{
	// we currently construct packets in one go,
   	// so reject invalid configurations.
	assert(validate()); //invalid/inconsistent packet configuration data;
}

// default constructor 
packet_config::packet_config() : d_is_valid(false){
	// do almost nothing. don't validate. Object is invalid and can't be used
    // FIXME: d_is_valid is not checked by getters
    d_part_lengths = std::vector<unsigned int>(5,0);
}//*/

// validator: checks consistency of data
bool packet_config::validate() throw(std::out_of_range){
	bool valid = true;
	
	// check part_lengths
	if(d_part_lengths.size() != 5){
		d_is_valid = false;
		return(d_is_valid); // return here, too risky otherwise
	}

	// check that the constellation and tsc alphabet size match
	int constellation_size = d_constellation.size();
	int tsc_len = d_training_seqs.at(0).size();
	for(int i=0; i<d_training_seqs.size(); ++i){
		if(d_training_seqs[i].size() != tsc_len){
			valid = false; // also check all tscs are equal length
		}
		for(int j=0; j<d_training_seqs[i].size(); ++j){
			if(d_training_seqs[i][j] >= constellation_size){
				valid = false;
			}
		}
	}
	// check the tsc length fits the corresponding piece length
	if(tsc_len != d_part_lengths.at(TRAINING_SEQ)){
		valid = false;
	}

	// check tsc matching parameters
	if(d_tsc_matching_length > tsc_len ||
			d_tsc_max_channel_length > d_tsc_matching_length){
		valid = false;
	}

	d_is_valid = valid;
	return(d_is_valid);
}

void packet_config::validate_piece_num(int piece_num) throw(std::out_of_range){
	if(piece_num < 0 || piece_num >= 5){
		throw(std::out_of_range)("packet_config: piece_num must be in range 0-4");
	}
}


// getters
int packet_config::get_piece_start(int piece_num) throw(std::out_of_range){
	validate_piece_num(piece_num);
	int sum=0;
	for(int i=0; i<piece_num; ++i){
		sum += d_part_lengths.at(i);
	}
	return(sum);
}

int packet_config::get_piece_len(int piece_num) throw(std::out_of_range){
	validate_piece_num(piece_num);
	return d_part_lengths.at(piece_num);
}

int packet_config::get_total_len(){
	int sum=0;
	for(int i=0; i<5; ++i){
		sum += d_part_lengths.at(i);
	}
	return(sum);
}


// calculated variables
int packet_config::get_preload_len()
{
	// this returns max channel length minus 1.
	return(d_tsc_max_channel_length - 1);
}

// slicer factory
mlse_vector_slice_vv_sptr packet_config::make_slicer(
		int piece_num,
		int include_before,
		int include_after,
		int piece_count,
		bool reverse)
{
	if(reverse){
		// reverse semantics are the following:
		// - the before and after counts are seen from the reversed directions,
		// 	 meaning, they need to be exchanged
		// - piece_num gives the starting piece of the reversed output,
		//   meaning the other pieces are before that in the original view
		std::swap(include_after,include_before);
		piece_num = piece_num - piece_count + 1;
	}
	int start = get_piece_start(piece_num) - include_before;
	int len = include_before + include_after;
	for(int i = piece_num; i < piece_num + piece_count; ++i){
		len += get_piece_len(i);
	}
	return mlse_make_vector_slice_vv(
			d_sample_type_size, 
			get_total_len(),
			start,
			len,
			reverse);
}

// modulate methods
// 	(I tried with templates. It didn't work out.)
// modulate vector of vectors
std::vector<std::vector<gr_complex> > packet_config::modulate(const std::vector<std::vector<unsigned char> > &x){
	std::vector<std::vector<gr_complex> > r;
	r.resize(x.size());
	for(int i=0; i < x.size(); ++i){
		r[i] = modulate(x.at(i));
	}
	return r;
}

// modulate vector
std::vector<gr_complex> packet_config::modulate(const std::vector<unsigned char> &x){
	std::vector<gr_complex> r;
	r.resize(x.size());
	for(int i=0; i < x.size(); ++i){
		r[i] = modulate(x.at(i));
	}
	return r;
}

// modulate single value
gr_complex inline packet_config::modulate(unsigned char x){
	return d_constellation.at(x); // the at() catches range errors
}


// default lookup table factories
mlse_lut_bcv_sptr packet_config::make_lut_channelestimate(int offset, int len)
{
	std::vector<std::vector<gr_complex> > lut;
	lut = modulate(d_training_seqs);
	lut = mlse_lut_bcv::slice_lut(lut, offset, len);
	return mlse_make_lut_bcv(lut);
}

mlse_lut_bbv_sptr packet_config::make_lut_preload(int preload_len, bool reverse)
{
	std::vector<std::vector<unsigned char> > lut;
	lut = mlse_lut_bbv::slice_lut(d_training_seqs, get_piece_len(TRAINING_SEQ) - preload_len, preload_len, reverse);
	return mlse_make_lut_bbv(lut);
}







