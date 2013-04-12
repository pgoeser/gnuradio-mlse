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

#include "channel_model.h"

#include <iostream>
using namespace std;

channel_model::channel_model(vector<gr_complex> modulation_table) :
		d_modulation_table(modulation_table)
{
	d_impulse_response = vector<gr_complex>(1,gr_complex(1.0,0.0));
	build_cache();
}


// default constructor. Sets 2-PSK with dirac impulse response to have _something_
channel_model::channel_model(){ 
		d_impulse_response = vector<gr_complex>(1,gr_complex(1.0,0.0));
		d_modulation_table = vector<gr_complex>(2,gr_complex(1.0,0.0));
        d_modulation_table.at(1) = gr_complex(-1.0,1.0);


		build_cache();
}

void channel_model::update_channel(vector<gr_complex> impulse_response)
{
	d_impulse_response = impulse_response;
	build_cache();
}

	
void channel_model::build_cache_recurse(int tap, gr_complex sample, int supersymbol){
	int M = modulation_size(); 
	int ld_M = log2(M); //TODO: inefficient
	int L = length();
	int new_supersym;
	gr_complex a, new_sample;

    //cout << "channel: " << tap << " " << sample << " "<< supersymbol << endl;

	for(int i=0; i<M; ++i){
		a = d_modulation_table.at(i);
		new_sample = sample + a * d_impulse_response[tap];
		new_supersym = supersymbol + (i << (tap*ld_M));
		if(tap < L-1){
			build_cache_recurse(tap+1, new_sample, new_supersym);
		} else {
			d_sample_cache.at(new_supersym) = new_sample;
		}
	}
}


void channel_model::build_cache(){
	d_sample_cache.resize(pow(modulation_size(),length()+1));
	build_cache_recurse(0,gr_complex(0.,0.),0);
}
			

int channel_model::length(){
	return d_impulse_response.size();
}

int channel_model::modulation_size(){
	return d_modulation_table.size();
}


//TODO: unit testing
