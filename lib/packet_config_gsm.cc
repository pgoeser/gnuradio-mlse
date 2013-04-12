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

#include "packet_config_gsm.h"

//! Number of tailbits
const int GSM_TAILBITS = 3;

//! Number of bits per payload (with stealing flags)
const int GSM_BITS_PER_PAYLOAD = 58;

//! Bits per TSC
const int GSM_TSC_LEN = 26;

//! Amount of TSCs
const int GSM_TSC_COUNT = 8;

//! Bits per shortened TSC
const int GSM_TSC_SHORT_LEN = 16;

//! channel length that we actually deconvolute
const int GSM_CHANNEL_LEN = 6;

unsigned char GSM_TSCS[GSM_TSC_COUNT][GSM_TSC_LEN] = {
		{0,0,1,0,0,1,0,1,1,1,0,0,0,0,1,0,0,0,1,0,0,1,0,1,1,1},
		{0,0,1,0,1,1,0,1,1,1,0,1,1,1,1,0,0,0,1,0,1,1,0,1,1,1},
		{0,1,0,0,0,0,1,1,1,0,1,1,1,0,1,0,0,1,0,0,0,0,1,1,1,0},
		{0,1,0,0,0,1,1,1,1,0,1,1,0,1,0,0,0,1,0,0,0,1,1,1,1,0},
		{0,0,0,1,1,0,1,0,1,1,1,0,0,1,0,0,0,0,0,1,1,0,1,0,1,1},
		{0,1,0,0,1,1,1,0,1,0,1,1,0,0,0,0,0,1,0,0,1,1,1,0,1,0},
		{1,0,1,0,0,1,1,1,1,1,0,1,1,0,0,0,1,0,1,0,0,1,1,1,1,1},
		{1,1,1,0,1,1,1,1,0,0,0,1,0,0,1,0,1,1,1,0,1,1,1,1,0,0}
	};

packet_config make_packet_config_gsm() throw(std::out_of_range){
	std::vector<unsigned int> part_lengths(5,0);
	part_lengths.at(packet_config::TAILBITS_FRONT)=GSM_TAILBITS;
	part_lengths.at(packet_config::PAYLOAD_FRONT) = GSM_BITS_PER_PAYLOAD;
	part_lengths.at(packet_config::TRAINING_SEQ) = GSM_TSC_LEN;
	part_lengths.at(packet_config::PAYLOAD_REAR) = GSM_BITS_PER_PAYLOAD;
	part_lengths.at(packet_config::TAILBITS_REAR) = GSM_TAILBITS;

	std::vector<gr_complex> constellation(2,0);
	constellation.at(0) = 1;
	constellation.at(1) = -1;

	std::vector<std::vector<unsigned char> > training_seqs;
	training_seqs.resize(GSM_TSC_COUNT);
	for(int i=0; i<training_seqs.size(); ++i){
		training_seqs.at(i).assign(GSM_TSCS[i], GSM_TSCS[i]+GSM_TSC_LEN);
	}

	packet_config pkconf(part_lengths, constellation, training_seqs, 
			GSM_TSC_SHORT_LEN, GSM_CHANNEL_LEN);

	return pkconf;
}

