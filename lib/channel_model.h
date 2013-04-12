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

#ifndef __channel_model_h
#define __channel_model_h

#include <gr_complex.h>
#include <vector>

using std::vector;
/*! \brief simulates a given channel, calculating samples it would return
 *
 * This class precalculates and caches the samples that would be the result of
 * the provided modulation followed by the provided channel impulse response.
 * The viterbi algorithm then compares these samples to the samples that 
 * were actually received.
 *
 * \param modulation_table  a mapping from the symbols that can be sent to the
 *          samples they represent. AKA constellation.
 * 
 */
class channel_model {
	// data
	vector<gr_complex> d_impulse_response;
	vector<gr_complex> d_modulation_table;

	// cache
	vector<gr_complex> d_sample_cache;



	// building cache (done by constructor)
	void build_cache();
	void build_cache_recurse(int tap, gr_complex sample, int supersymbol);

public:	
	// constructors
	channel_model(vector<gr_complex> modulation_table);
	channel_model();

	// setting impulse response
    /*! \brief provides the channel impulse response, recalculating the cache.
     *
     * \param impulse_response  Impulse response of the channel
     */
	void update_channel(vector<gr_complex> impulse_response);

	// getters for parameters
	int length();
	int modulation_size();

	// retrieve a sample of a symbol run through the channel with given state
    /*! \brief retrieves samples for a given channel state (past symbols)
     *
     * This is a lookup into the cache. The function is inlined from viterbi_impl.
     *
     * \param channel_state Last symbols on the channel, encoded as an int,
     *      like in viterbi_impl
     */
	gr_complex sample(int channel_state){
	    return d_sample_cache.at(channel_state);
    }

};


#endif //__channel_model_h
