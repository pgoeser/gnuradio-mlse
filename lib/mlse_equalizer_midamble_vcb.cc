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


#include "mlse_equalizer_midamble_vcb.h"
#include <gr_io_signature.h>
#include <gr_reverse.h>
#include <vector>


/*
 * This is effectively the public constructor.
 */
mlse_equalizer_midamble_vcb_sptr 
mlse_make_equalizer_midamble_vcb (packet_config conf)
{
  return gnuradio::get_initial_sptr(new mlse_equalizer_midamble_vcb(conf));
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
mlse_equalizer_midamble_vcb::mlse_equalizer_midamble_vcb(packet_config conf)
  : gr_hier_block2 ("equalizer_midamble",
		   gr_make_io_signature2 (2,2, 
                    sizeof (gr_complex) * conf.get_total_len(), 
                    sizeof(char)),
		   gr_make_io_signature2 (2, 2, sizeof(char) * (
                      conf.get_piece_len(conf.PAYLOAD_FRONT) + 
                      conf.get_piece_len(conf.PAYLOAD_REAR)),
                  sizeof(gr_complex) * conf.get_tsc_max_channel_length() ) ), 
    d_conf(conf)
{

    ///// Make channel estimation /////
    // initialize the block doing the actual work
    d_estimate = mlse_make_channelestimate_vcc(
            d_conf.get_tsc_matching_length(), 
            d_conf.get_tsc_max_channel_length());
    // calculate how much samples of the TSC to throw away
    int tsc_excess_samples = d_conf.get_piece_len(d_conf.TRAINING_SEQ) - d_conf.get_tsc_matching_length();
    int tsc_front_cut = tsc_excess_samples / 2;
    int tsc_rear_cut = tsc_excess_samples - tsc_front_cut;
    // make input glue
    d_estimate_slicer = d_conf.make_slicer(d_conf.TRAINING_SEQ, -tsc_front_cut, -tsc_rear_cut);
    d_estimate_lut = d_conf.make_lut_channelestimate(tsc_front_cut, d_conf.get_tsc_matching_length());
    // connect channelestimater
    connect(self(), 0, d_estimate_slicer, 0); // samples->slicer
    connect(d_estimate_slicer, 0, d_estimate, 1); // slicer->estimater
    connect(self(), 1, d_estimate_lut, 0); // index->lut
    connect(d_estimate_lut, 0, d_estimate, 0); // lut->slicer
    
    ///// Make viterbis /////
    // initialize the viterbis
    d_vit_front = mlse_make_viterbi_vcb(
            d_conf.get_piece_len(d_conf.PAYLOAD_FRONT), 
            d_conf.get_tsc_max_channel_length(),
            d_conf.get_piece_len(d_conf.TAILBITS_FRONT), 
            d_conf.get_constellation());
    d_vit_rear = mlse_make_viterbi_vcb(
            d_conf.get_piece_len(d_conf.PAYLOAD_REAR), 
            d_conf.get_tsc_max_channel_length(),
            d_conf.get_piece_len(d_conf.TAILBITS_REAR), 
            d_conf.get_constellation());

    // connect rear viterbi with input glue
    d_vit_rear_slicer = d_conf.make_slicer(
           d_conf.PAYLOAD_REAR, // starting piece
           d_conf.get_tsc_max_channel_length()/2, // shifting the samples by half the channel length (assumes main energy of channel is in the middle)
           -d_conf.get_tsc_max_channel_length()/2, // still shifting
           2); // include 2 pieces (i.e. also tailbits)
    connect(self(), 0, d_vit_rear_slicer, 0);
    connect(d_vit_rear_slicer, 0, d_vit_rear, 1); // samples->slicer->viterbi
    // rear viterbi lookup table
    d_vit_rear_lut = d_conf.make_lut_preload(d_conf.get_preload_len());
    connect(self(), 1, d_vit_rear_lut, 0);
    connect(d_vit_rear_lut, 0, d_vit_rear, 2); // index->lut->viterbi

    // connect front viterbi with input glue
    d_vit_front_slicer = d_conf.make_slicer(
           d_conf.PAYLOAD_FRONT, // starting piece
           (d_conf.get_tsc_max_channel_length()-1)/2, // shifting the samples, the -1 takes care of even channel lengths
           -(d_conf.get_tsc_max_channel_length()-1)/2, // still shifting
           2, // include 2 pieces (i.e. also tailbits)
           true); // reverse output (and the semantics used)
    connect(self(), 0, d_vit_front_slicer, 0);
    connect(d_vit_front_slicer, 0, d_vit_front, 1); // samples->slicer->viterbi
    // front viterbi lookup table
    d_vit_front_lut = d_conf.make_lut_preload(d_conf.get_preload_len(), true);
    connect(self(), 1, d_vit_front_lut, 0);
    connect(d_vit_front_lut, 0, d_vit_front, 2); // index->lut->viterbi

    // connect channelestimate to viterbi
    connect( d_estimate, 0, d_vit_front, 0); // front viterbi
    d_channel_reverse = mlse_make_vector_slice_vv(sizeof(gr_complex), 
            d_conf.get_tsc_max_channel_length(), 0, d_conf.get_tsc_max_channel_length(),
            true); // reverser
    connect( d_estimate, 0, d_channel_reverse, 0); // estimate->reverser
    connect( d_channel_reverse, 0, d_vit_rear, 0); // reverser->viterbi (rear)

    // concatenate outputs of viterbis
    d_data_reverse = mlse_make_vector_slice_vv(sizeof(unsigned char),
            d_conf.get_piece_len(d_conf.PAYLOAD_FRONT), 0, d_conf.get_piece_len(d_conf.PAYLOAD_FRONT),
            true);
    connect(d_vit_front, 0, d_data_reverse, 0);
    d_concat = mlse_make_vector_concat_vv(
            sizeof(unsigned char)*d_conf.get_piece_len(d_conf.PAYLOAD_FRONT), 
            sizeof(unsigned char)*d_conf.get_piece_len(d_conf.PAYLOAD_REAR));
    connect( d_data_reverse, 0, d_concat, 0); // reversed front vit output
    connect( d_vit_rear, 0, d_concat, 1); // rear vit output appended to that
    connect( d_concat, 0, self(), 0); // connect concat output to block output

    connect( d_estimate, 0, self(), 1); // channel estimater output for debugging

}

// destructor
mlse_equalizer_midamble_vcb::~mlse_equalizer_midamble_vcb ()
{
  // everything done automatically
}

