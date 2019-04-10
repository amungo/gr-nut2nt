/* -*- c++ -*- */

#define NUT2NT_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "nut2nt_swig_doc.i"

%{
#include "nut2nt/nut2nt_source.h"
%}


%include "nut2nt/nut2nt_source.h"
GR_SWIG_BLOCK_MAGIC2(nut2nt, nut2nt_source);
