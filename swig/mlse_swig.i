/* -*- c++ -*- */

%include "gnuradio.i"			// the common stuff

%{
#include "mlse_viterbi_vcb.h"
#include "mlse_vector_slice_vv.h"
#include "mlse_vector_concat_vv.h"
#include "mlse_channelestimate_vcc.h"
#include "packet_config.h"
#include "packet_config_gsm.h"
#include "mlse_equalizer_midamble_vcb.h"
#include "mlse_packetbuilder_midamble_vbb.h"
#include "mlse_ber_vbi.h"
#include "mlse_derotate_cc.h"
%}

%include "mlse_viterbi_vcb.i"
%include "mlse_vector_slice_vv.i"
%include "mlse_vector_concat_vv.i"
%include "mlse_channelestimate_vcc.i"
%include "packet_config.i"
%include "packet_config_gsm.i"
%include "mlse_equalizer_midamble_vcb.i"
%include "mlse_packetbuilder_midamble_vbb.i"
%include "mlse_ber_vbi.i"
%include "mlse_derotate_cc.i"

%include "mlse_generated.i"

#if SWIGGUILE
%scheme %{
(load-extension-global "libguile-gnuradio-mlse_swig" "scm_init_gnuradio_mlse_swig_module")
%}

%goops %{
(use-modules (gnuradio gnuradio_core_runtime))
%}
#endif
