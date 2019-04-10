/* -*- c++ -*- */
/*
 * Copyright 2017 <+YOU OR YOUR COMPANY+>.
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

#ifndef INCLUDED_AMUNGO_NUT2NT_IMPL_H
#define INCLUDED_AMUNGO_NUT2NT_IMPL_H

#include <nut2nt/nut2nt_source.h>

class FX3Config;
class HWManager;

typedef float (*Decode2BCharToFloat)(unsigned char _data);
typedef short (*Decode2BCharToShort)(unsigned char _data);
typedef char (*Decode2BCharToByte)(unsigned char _data);


namespace gr {
    namespace nut2nt {

        class nut2nt_source_impl : public nut2nt_source
        {
        private:
            // Nothing to declare in this block.

        public:
            nut2nt_source_impl(int num_outputs, const std::string& img_file, const std::string& cfg_file, const std::string& ecp5_alg,
                               const std::string &ecp5_data, size_t sizeof_stream_item,
                               size_t buf_len, int buf_num, int rotation_count);
            ~nut2nt_source_impl();

            virtual void init_nut2nt_source();

            // Where all the action really happens
            int work(int noutput_items,
                     gr_vector_const_void_star &input_items,
                     gr_vector_void_star &output_items);

            // 0 - pll "A" for all channels. 1 - pll "A" for channels 1&2, pll "B" for channels 3&4
            virtual int set_lo_src(int _lo_src);
            int get_lo_src(int _lo_src);
            //
            virtual int set_pll_freq(int _pll_idx, unsigned int _freq);
            int get_pll_freq(int _pll_idx);
            // CLK frequency source: "0" - from PLL "A", "1" - from PLL "B"
            virtual int set_clk_src(int _clk_src);
            int get_clk_src();
            // CLK type: "0" - CMOS "1" - LVDS
            virtual int set_clk_type(int _clk_type);
            int get_clk_type();
            // CLK divider ratio (8 - 31)
            virtual int set_clk_divider(int _clk_div);
            int get_clk_divider();

            // Set channel USB/LSB (0 - USB, 1 - LSB)
            virtual int set_gnss(int _chan, int _gnss);
            int get_gnss(int _chan);
            // Set channel IF pass band (15.0 MHz - 31.0 MHz) coded: 22,24,24...77)
            virtual int set_if_pass_band(int _chan, int _band_code);
            int get_if_pass_band(int _chan);
            // Enable/Disable channel
            virtual int set_chan_enable(int _chan, bool _enable);
            bool get_chan_enable(int _chan);

        protected:
            virtual bool start();
            virtual bool stop();

        protected:
            static std::vector<Decode2BCharToFloat> m_DecodeToFloatFunc;
            static std::vector<Decode2BCharToShort> m_DecodeToShortFunc;
            static std::vector<Decode2BCharToByte>  m_DecodeToByteFunc;

        private:
            FX3Config* m_pFX3Config;
            HWManager* m_pHWManager;
            size_t m_dataFormat;
            int rotation_count_;
            bool enable_debug_;
            int debug_counter_;
        };

    } // namespace nut2nt
} // namespace gr

#endif // INCLUDED_AMUNGO_NUT2NT_IMPL_H

