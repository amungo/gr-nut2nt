/* -*- c++ -*- */
/* 
 * Copyright 2019 gr-nut2nt author.
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


#ifndef INCLUDED_NUT2NT_NUT2NT_SOURCE_H
#define INCLUDED_NUT2NT_NUT2NT_SOURCE_H

#include <nut2nt/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
    namespace nut2nt {

        /*!
         * \brief <+description of block+>
         * \ingroup nut2nt
         *
         */
        class NUT2NT_API nut2nt_source : virtual public gr::sync_block {
        public:
            typedef boost::shared_ptr<nut2nt_source> sptr;

            /*!
             * \brief Return a shared_ptr to a new instance of nut2nt::nut2nt_source.
             *
             * To avoid accidental use of raw pointers, nut2nt::nut2nt_source's
             * constructor is in a private implementation
             * class. nut2nt::nut2nt_source::make is the public interface for
             * creating new instances.
             */
            static sptr
            make(int num_outputs, const std::string& img_file, const std::string& cfg_file, const std::string& ecp5_alg,
                 const std::string &ecp5_data, size_t sizeof_stream_item,
                 size_t buf_len, int buf_num, int rotation_count);

            virtual void init_nut2nt_source() = 0;

            // 0 - pll "A" for all channels. 1 - pll "A" for channels 1&2, pll "B" for channels 3&4
            virtual int set_lo_src(int _lo_src) = 0;
            //
            virtual int set_pll_freq(int _pll_idx, unsigned int  _freq) = 0;
            // CLK frequency source: "0" - from PLL "A", "1" - from PLL "B"
            virtual int set_clk_src(int _clk_src) = 0;
            // CLK type: "0" from PLL "A", "1" from PLL "B"
            virtual int set_clk_type(int _clk_type) = 0;
            // CLK divider ratio (8 - 31)
            virtual int set_clk_divider(int _clk_div) = 0;
            // Set channel USB/LSB (0 - USB, 1 - LSB)
            virtual int set_gnss(int _chan, int _gnss) = 0;
            // Set channel IF pass band (15.0 MHz - 31.0 MHz) coded: 22,24,24...77)
            virtual int set_if_pass_band(int _chan, int _band_code) = 0;
            // Set channel Enable/Disable
            virtual int set_chan_enable(int _chan, bool _enable) = 0;

        };

    } // namespace nut2nt
} // namespace gr

#endif /* INCLUDED_NUT2NT_NUT2NT_SOURCE_H */

