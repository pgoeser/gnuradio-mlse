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

#ifndef INCLUDED_MLSE_PACKETBUILDER_MIDAMBLE_VBB_H
#define INCLUDED_MLSE_PACKETBUILDER_MIDAMBLE_VBB_H

#include <gr_hier_block2.h>
#include "packet_config.h"
#include "mlse_lut_bbv.h"
#include "mlse_vector_slice_vv.h"
#include "mlse_vector_concat_vv.h"
#include <gr_vector_source_b.h>

class mlse_packetbuilder_midamble_vbb;

// sptr
typedef boost::shared_ptr<mlse_packetbuilder_midamble_vbb> mlse_packetbuilder_midamble_vbb_sptr;

mlse_packetbuilder_midamble_vbb_sptr mlse_make_packetbuilder_midamble_vbb(packet_config conf);

/*! \brief construct packets according to a packet_config
 *
 * Together with a modulator, this block is the counterpiece to 
 * mlse_equalizer_midamble_vcb, it can construct packets according to the 
 * given packet_config.
 *
 * Inputs:
 * (0) vector of byte: data to packetize
 * (1) byte: index of the training sequence to use 
 * Outputs:
 * (0) vector of byte: packetized data
 *
 * \param conf  packet_config object that specifies how to construct the packets
 */
class mlse_packetbuilder_midamble_vbb: public gr_hier_block2
{
private:
    friend mlse_packetbuilder_midamble_vbb_sptr mlse_make_packetbuilder_midamble_vbb(packet_config conf);

    mlse_packetbuilder_midamble_vbb(packet_config conf);  	// private constructor

    ///// Members:
    // packet configuration data. Almost everything is in here
    packet_config d_conf;

    // used blocks
    mlse_vector_slice_vv_sptr
			d_payload_front,
			d_payload_rear;
	mlse_vector_concat_vv_sptr
			d_concat_1,
			d_concat_2,
			d_concat_3,
			d_concat_4;
	mlse_lut_bbv_sptr
			d_tsc;
	gr_vector_source_b_sptr
			d_tailbits_front,
			d_tailbits_rear;

public:
    ~mlse_packetbuilder_midamble_vbb();	// public destructor
		   
};

#endif // INCLUDED_MLSE_PACKETBUILDER_MIDAMBLE_VBB_H
