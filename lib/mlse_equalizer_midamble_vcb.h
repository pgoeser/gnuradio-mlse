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

#ifndef INCLUDED_MLSE_EQUALIZER_MIDAMBLE_VCB_H
#define INCLUDED_MLSE_EQUALIZER_MIDAMBLE_VCB_H

#include <gr_hier_block2.h>
#include "packet_config.h"
#include "mlse_lut_bcv.h"
#include "mlse_lut_bbv.h"
#include "mlse_viterbi_vcb.h"
#include "mlse_channelestimate_vcc.h"
#include "mlse_vector_slice_vv.h"
#include "mlse_vector_concat_vv.h"

class mlse_equalizer_midamble_vcb;

// sptr
typedef boost::shared_ptr<mlse_equalizer_midamble_vcb> mlse_equalizer_midamble_vcb_sptr;

mlse_equalizer_midamble_vcb_sptr mlse_make_equalizer_midamble_vcb(packet_config conf);

/*! \brief Equalizer for bursts using midambles as training sequences.
 *
 * This block takes the configuration parameters for the burst system used.
 * It provides an MLSE-Equalization system using the Viterbi algorithm.
 * Channel estimation is integrated, reception filter and syncronisation must
 * be done externally
 *
 * Inputs:
 * (0) Vector of gr_complex: samples to equalize, length is given by conf
 * (1) Byte: index of the training sequence to use for channel estimation
 *
 * Outputs:
 * (0) Vector of bytes: received data (without training sequence)
 * (1) vector of gr_complex: estimated channel for debug purposes (null-sink this otherwise)
 *
 * \param conf packet_config object containing all the relevant parameters
 */
class mlse_equalizer_midamble_vcb: public gr_hier_block2
{
private:
    friend mlse_equalizer_midamble_vcb_sptr mlse_make_equalizer_midamble_vcb(packet_config conf);

    mlse_equalizer_midamble_vcb(packet_config conf);  	// private constructor

    ///// Members:
    // packet configuration data. Almost everything is in here
    packet_config d_conf;

    // used blocks: glue
    mlse_vector_slice_vv_sptr
            d_vit_front_slicer,   // front burst viterbi sample slicer
            d_vit_rear_slicer,   // rear burst viterbi sample slicer
            d_estimate_slicer,   // channel estimate slicer
            d_channel_reverse,   // channel estimate reverser for the rear viterbi
            d_data_reverse;      // data reverser, to righten the reversed output of the front viterbi
    mlse_lut_bcv_sptr d_estimate_lut;  // lookup table for modulated training sequences for the channel estimater
    mlse_lut_bbv_sptr d_vit_front_lut; // lookup table for the ouput symbol preload of the (front) viterbi 
    mlse_lut_bbv_sptr d_vit_rear_lut; // the same for the rear viterbi 
    mlse_vector_concat_vv_sptr d_concat; // merges together the two payloads


    // used blocks: actual work
    mlse_viterbi_vcb_sptr d_vit_front, d_vit_rear; // viterbi
    mlse_channelestimate_vcc_sptr d_estimate; // channel estimation

public:
    ~mlse_equalizer_midamble_vcb();	// public destructor
		   
};

#endif // INCLUDED_MLSE_EQUALIZER_MIDAMBLE_VCB_H
