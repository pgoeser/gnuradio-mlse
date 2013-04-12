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

class @NAME@;

// sptr
typedef boost::shared_ptr<@NAME@> @SPTR_NAME@;

@SPTR_NAME@ mlse_make_@BASE_NAME@(std::vector<std::vector<@O_TYPE@> > const &data) throw(std::invalid_argument);

/*! \brief creates a lookup table useful for protocols using predefined sequences
 *      
 *      To clarify: this is a 1-dimensional lookup table, that returns the vector at a given index.
 *      #TODO: better explanation 
 *
 * 		setup: takes a vector of vectors of output-type as lookup-table.
 *		input: index in the lookup-table
 *		output: the vector in the table at the given index 
 * 		
 *		If invalid indexes are supplied, index 0 will be assumed.
 *
 * 	\param data pointer to the data of the lookup table
 *
 */
class @NAME@: public gr_sync_block
{
private:
	friend @SPTR_NAME@ mlse_make_@BASE_NAME@(std::vector<std::vector<@O_TYPE@> > const &data) throw(std::invalid_argument);

    @NAME@(std::vector<std::vector<@O_TYPE@> > const &data) throw(std::invalid_argument);  	// private constructor

	std::vector<std::vector<@O_TYPE@> > d_data;
	int d_output_length; 

public:
    ~@NAME@();	// public destructor
		   
	/*!
	 * \brief changes the lookup-table on-the-fly.
	 * 
	 *		The length of all subvectors must match the length used in the constructor.
	 *		The amount of subvectors may differ.
	 */
	void set_data(std::vector<std::vector<@O_TYPE@> > const &data) throw(std::invalid_argument);


	/*!
	 * \brief slices a lookup-table
	 *
	 *	 This function slices a lookup-table in the same manner as vector_slice_vv.
	 *   It takes and returns a vector of vectors, and for every subvector it outputs
	 *	 a portion starting at start and with length len, optionally reversed.
	 *
	 * \param lut The table to work on
	 * \param start Position of the first element of each subvector to output
	 * \param len Length of each subvector
	 * \param reverse If true, the output table is reversed, and start and len count from 
	 *			the rear of the vectors (which is a different semantic than in vector_slice_vv)
	 */
	static std::vector<std::vector<@O_TYPE@> > slice_lut(
		const std::vector<std::vector<@O_TYPE@> > &lut,
		int start,
		int len,
		bool reverse=false) throw(std::invalid_argument);


    // Where all the action really happens
    int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
	
};



#endif // @GUARD_NAME@
