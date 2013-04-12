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

#ifndef INCLUDED_PACKET_CONFIG_GSM_H
#define INCLUDED_PACKET_CONFIG_GSM_H

#include "packet_config.h"

/*! \brief Packet config data for GSM
 *
 * Creates a packet_config object with the right config data for GSM data packets
 */
packet_config make_packet_config_gsm() throw(std::out_of_range);





#endif //INCLUDED_PACKET_CONFIG_GSM_H
