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


#include "mlse_packetbuilder_midamble_vbb.h"
#include <gr_io_signature.h>
#include <gr_reverse.h>
#include <vector>


mlse_packetbuilder_midamble_vbb_sptr 
mlse_make_packetbuilder_midamble_vbb (packet_config conf)
{
  return gnuradio::get_initial_sptr(new mlse_packetbuilder_midamble_vbb(conf));
}

/*
 * Input: 
 *      - sample stream (packet_config::get_total_len() complex)
 *      - TSC index (unsigned char)
 * Outputs: 
 *      - both decoded payloads in one vector, in the order in which they were in the
 *        sample stream (get_piece_len(PAYLOAD_FRONT) + get_piece_len(PAYLOAD_REAR) 
 *        times unsigned char )
 */
mlse_packetbuilder_midamble_vbb::mlse_packetbuilder_midamble_vbb(packet_config conf)
  : gr_hier_block2 ("equalizer_midamble",
		   gr_make_io_signature2 (2,2, 
			   sizeof(char) * (
                   conf.get_piece_len(conf.PAYLOAD_FRONT) + 
                   conf.get_piece_len(conf.PAYLOAD_REAR)), 
			   sizeof(char) ), 
		   gr_make_io_signature (1, 1, sizeof (char) * conf.get_total_len()) ),
    d_conf(conf)
{

	///// Slice the data into the two payloads /////
	int input_len = d_conf.get_piece_len(d_conf.PAYLOAD_FRONT) + d_conf.get_piece_len(d_conf.PAYLOAD_REAR);
	d_payload_front = mlse_make_vector_slice_vv(sizeof(char), input_len, 0, d_conf.get_piece_len(d_conf.PAYLOAD_FRONT));
	d_payload_rear = mlse_make_vector_slice_vv(sizeof(char), input_len, d_conf.get_piece_len(d_conf.PAYLOAD_FRONT), d_conf.get_piece_len(d_conf.PAYLOAD_REAR));
	// connect
	connect(self(),0,d_payload_front,0);
	connect(self(),0,d_payload_rear,0);

	//// Tailbit sources /////
	int front_tailbits = d_conf.get_piece_len(d_conf.TAILBITS_FRONT);
	int rear_tailbits = d_conf.get_piece_len(d_conf.TAILBITS_REAR);
	d_tailbits_front = gr_make_vector_source_b(std::vector<unsigned char>(front_tailbits,0), true, front_tailbits);
	d_tailbits_rear = gr_make_vector_source_b(std::vector<unsigned char>(rear_tailbits,0), true, rear_tailbits);

	///// Training sequence source /////
	d_tsc = d_conf.make_lut_preload(d_conf.get_piece_len(d_conf.TRAINING_SEQ));
	connect(self(),1,d_tsc,0); // connect the tsc selection channel

	
	///// concat can only manage two inputs, so we cascade a bit /////
	// not rolled into a loop so we get static typing.
	// Ugly, but see me care.
	int cumul_len, next_len; 
	cumul_len = d_conf.get_piece_len(d_conf.TAILBITS_FRONT);
	next_len = d_conf.get_piece_len(d_conf.PAYLOAD_FRONT);
	d_concat_1 = mlse_make_vector_concat_vv(cumul_len, next_len);
	connect(d_tailbits_front,0,d_concat_1,0);
	connect(d_payload_front,0,d_concat_1,1);

	cumul_len += next_len;
	next_len = d_conf.get_piece_len(d_conf.TRAINING_SEQ);
	d_concat_2 = mlse_make_vector_concat_vv(cumul_len, next_len);
	connect(d_concat_1,0,d_concat_2,0);
	connect(d_tsc,0,d_concat_2,1);
			
	cumul_len += next_len;
	next_len = d_conf.get_piece_len(d_conf.PAYLOAD_REAR);
	d_concat_3 = mlse_make_vector_concat_vv(cumul_len, next_len);
	connect(d_concat_2,0,d_concat_3,0);
	connect(d_payload_rear,0,d_concat_3,1);
			
	cumul_len += next_len;
	next_len = d_conf.get_piece_len(d_conf.TAILBITS_REAR);
	d_concat_4 = mlse_make_vector_concat_vv(cumul_len, next_len);
	connect(d_concat_3,0,d_concat_4,0);
	connect(d_tailbits_rear,0,d_concat_4,1);

	connect(d_concat_4,0,self(),0);
	// done
			
}

// destructor
mlse_packetbuilder_midamble_vbb::~mlse_packetbuilder_midamble_vbb ()
{
  // everything done automatically
}

