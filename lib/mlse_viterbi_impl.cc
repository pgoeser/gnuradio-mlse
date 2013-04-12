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


#include "mlse_viterbi_impl.h"
#include <iostream>
#include <gr_io_signature.h>



// constructor
mlse_viterbi_impl::mlse_viterbi_impl(const std::vector<gr_complex> &modulation_table) :
	d_model(channel_model(modulation_table))
{
}
			

// destructor (empty)
mlse_viterbi_impl::~mlse_viterbi_impl()
{
}

void mlse_viterbi_impl::decode_run(const gr_complex *samples_ptr, int length, int tailsymbols){
	for(int i=0; i<length; i++){
		decode_step(*samples_ptr++);
	}
	for(int i=0; i<tailsymbols; i++){ //TODO!important: stop if we run out of samples
		decode_step(*samples_ptr++, 0); 
		// decode forced 0-symbols 
	}
}

	
void mlse_viterbi_impl::update_cached_vars(){
	d_ld_M = log2(d_M);
	d_supersymbol_count = pow(d_M, d_L-1);
	d_input_shift = d_ld_M * (d_L-2);
}

void mlse_viterbi_impl::decode_init(std::vector<char> initdata){
	// pull the metadata from channel_model and recalculate cached helper vars
	d_L = d_model.length();
	d_M = d_model.modulation_size();
	update_cached_vars();
	// allocate vectors
	//d_state_probs.assign(d_supersymbol_count, -INFINITY); // sets all symbols to 'impossible'
	d_state_probs.assign(d_supersymbol_count, 0); 
	d_temp_state_probs.assign(d_supersymbol_count, -INFINITY);
    // put a sentry trackback vector at the start, so we don't have to check if the
    // trackback is empty on skipping forced symbols
    d_trackback_decisions.assign(1,std::vector<int>(d_supersymbol_count,-1));
    d_previous_was_forced_symbol = true; // mark that extra vector for replacement

	// now feed the inital symbols
	for(int i=0; i<initdata.size(); ++i){
		decode_step(gr_complex(0.,0.), initdata[i]);
	}	
	
}

/*void mlse_viterbi_impl::decode_init( vector<int> initial_symbols ){
	decode_init();
	d_state_metrics.assign(d_supersymbol_count, -INFINITY);
	d_state_metrics[d_ssh.list_to_int(initial_symbols)] = 0.;
}*/


void mlse_viterbi_impl::decode_step(gr_complex sample, int forced_symbol){
	// TODO: look at the temporary arrays.
	// d_temp_state_trackbacks elements are currently copied entirely in every iteration
	// solutions:
	//  - perhaps (probably) vector instead of list is faster
	//  - or: only use pointers to the trackbacks and update them inplace after all
	//    supersymbols are processed (hmm, might not work)
	//  - or: iterate by new_supersym to reduce copies that get overwritten
	//  - or: build a tree of linked-list-like objects in memory, so a copy is only
	//    a new node and a link to the old version
	d_temp_state_probs.assign(d_supersymbol_count, -INFINITY); //clear old data
	// ('clearing' the probs is sufficient)

	// used temporary variables
	float prob;
	int new_supersym;
    
    // variable to store the decisions taken for every supersymbol (contains
    // the bits of the one path that wasn't cut) TODO:improve description
    std::vector<int> decisions(d_supersymbol_count,-1);

	// main loop: for each old supersymbol
	for(int supersym=0; supersym < d_supersymbol_count; supersym++){
		
		// inner loop: for each possible next symbol
		for(int nextsym=0; nextsym < d_M; nextsym++){
			
			// handle forced symbols
			if(forced_symbol >= 0){
				if(nextsym != forced_symbol){
					continue;
				}
			}

			// calculate metric
			prob = d_state_probs[supersym];
			prob += calculate_metric(supersym, nextsym, sample);

			// handle forced symbols
			if(forced_symbol >= 0 && forced_symbol != nextsym){
				// we've been told that the nextsym at the current position _has_ to be
				// forced_symbol. Any chain not including it should be dropped.
				prob = -INFINITY; // drop this chain
			}
			
			// calculate following supersymbol
			new_supersym = next_supersym(supersym, nextsym);

			// check if we don't already have a better candidate
			if(d_temp_state_probs[new_supersym] < prob){
				// store new supersym
				d_temp_state_probs[new_supersym] = prob;
                if(d_previous_was_forced_symbol){ 
                    // if previous was a forced symbol, that trackback gets deleted, so we
                    // use the values from the previous trackback
                    decisions[new_supersym] = d_trackback_decisions.back().at(supersym); 
                } else {
                    // store the previous symbol for later trackbacking
    				decisions[new_supersym] = supersym; 
                }
			} 
			// if we have a better candidate, this one is ignored
		} 

	}

    // store trackback
    if(d_previous_was_forced_symbol){ // remove the last "temp" trackback
        d_trackback_decisions.pop_back();
    }
    d_previous_was_forced_symbol = (forced_symbol >= 0);
    d_trackback_decisions.push_back(decisions); //TODO: is the copy here cheaper than the alternative (2D-vector-access in the loop)?

	// swap the vectors. This is fast!
	d_state_probs.swap(d_temp_state_probs);

	//TODO: metrics adjustment so they don't explode
}


void mlse_viterbi_impl::decode_result(char* out, int len){
	float max_prob = -INFINITY;
	float second_best = -INFINITY;
	int max_prob_index;
    //FIXME: validate len or something

	for(int supersym=0; supersym<d_supersymbol_count; supersym++){
		if(d_state_probs[supersym] > max_prob){
			second_best = max_prob;
			max_prob = d_state_probs[supersym];
			max_prob_index = supersym;
		}
	}
//    std::cout << max_prob << std::endl;

    int cur_index = d_trackback_decisions.back().at(max_prob_index); //FIXME: fails if last symbol wasn't forced
    for(int i = len-1; i>=0; --i){
        out[i] = extract_newsym_from_supersymbol(cur_index);
        cur_index = d_trackback_decisions.at(i).at(cur_index);
        // the +1 is to compensate for the sentry trackback at the start
    }
}




inline float mlse_viterbi_impl::calculate_metric(int supersym, int nextsym, gr_complex const received_sample){
	int channel_state;
	float metric;
    
	// the channel state also includes the current input symbol, which is only
	// not included in the viterbi algorithm because you can immediately 
	// eliminate a lot of them.
	channel_state = supersym + (nextsym<<(d_input_shift + d_ld_M));
	
	gr_complex model_sample = d_model.sample(channel_state);

//	metric = 2.*real(conj(model_sample) * received_sample) - real(conj(model_sample)*model_sample);
    metric = - real((model_sample - received_sample) * conj(model_sample - received_sample)); //euclidean

	return metric;
}


