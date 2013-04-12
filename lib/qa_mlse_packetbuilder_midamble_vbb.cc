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

#include <cppunit/TestAssert.h>

#include "qa_mlse_packetbuilder_midamble_vbb.h"
#include "mlse_packetbuilder_midamble_vbb.h"
#include "packet_config.h"
#include "packet_config_gsm.h"

void
qa_mlse_packetbuilder_midamble_vbb::t1()
{
    // stupidly assert that the object can be constructed.
    // 
    // this catches consistency errors in the configuration of the used blocks, and
    // size mismatches between blocks. Testing for further functionality is much too 
    // complicated compared to the expected usefulness.
    //
    // A top-level test tests the entire system running on gsm packets. We should 
    // probably also test other packet configurations (TODO).
    //
    mlse_packetbuilder_midamble_vbb_sptr pb;
    packet_config conf;
    conf = make_packet_config_gsm();

    pb = mlse_make_packetbuilder_midamble_vbb(conf);
    CPPUNIT_ASSERT(1);
}

void
qa_mlse_packetbuilder_midamble_vbb::t2()
{
  // Insert CPPUNIT tests/asserts here
}
