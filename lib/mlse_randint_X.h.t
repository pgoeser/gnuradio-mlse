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



// @WARNING@


#ifndef @GUARD_NAME@
#define @GUARD_NAME@

#include <gr_sync_block.h>
#include <vector>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

class @NAME@;

// sptr
typedef boost::shared_ptr<@NAME@> @SPTR_NAME@;

@SPTR_NAME@ mlse_make_@BASE_NAME@(@O_TYPE@ range);

/*! \brief creates a stream of random numbers in the range [0,range)
 *
 *  The PRNG used is boost::mt19937.
 *  Seeding is done with std::time(0).
 *
 * 	\param range  
 */
class @NAME@: public gr_sync_block
{
private:
	friend @SPTR_NAME@ mlse_make_@BASE_NAME@(@O_TYPE@ range );

    @NAME@(@O_TYPE@ range);  	// private constructor

	boost::mt19937 d_rng;
	boost::uniform_int<> d_dist;
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > d_vargen;
	@O_TYPE@ d_range;

public:
    ~@NAME@();	// public destructor
		   

    // Where all the action really happens
    int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
	
};



#endif // @GUARD_NAME@
