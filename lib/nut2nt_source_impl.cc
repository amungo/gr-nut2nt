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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdexcept>
#include <iomanip>
//#include <algorithm>
#include <gnuradio/io_signature.h>
#include "nut2nt_source_impl.h"
#include "fx3config.h"
#include "fx3devdrvtype.h"
#include "hwmanager.h"
#include "Chan2bitParser.h"
#include "ntlab_settings.h"
#include "fx3tuner.h"

#include <thread>

static const int MIN_OUT = 1; // minimum number of output streams
static const int MAX_OUT = 4; // maximum number of output streams

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)

static size_t item_size_lut[] = {sizeof(char), sizeof(char), sizeof(short), sizeof(float)};

namespace gr {
    namespace nut2nt {

        std::vector<Decode2BCharToFloat> nut2nt_source_impl::m_DecodeToFloatFunc = {
                decode_2bchar_to_float_ch0,
                decode_2bchar_to_float_ch1,
                decode_2bchar_to_float_ch2,
                decode_2bchar_to_float_ch3
        };

        std::vector<Decode2BCharToShort> nut2nt_source_impl::m_DecodeToShortFunc = {
                decode_2bchar_to_short_ch0,
                decode_2bchar_to_short_ch1,
                decode_2bchar_to_short_ch2,
                decode_2bchar_to_short_ch3
        };


        std::vector<Decode2BCharToByte> nut2nt_source_impl::m_DecodeToByteFunc = {
                decode_2bchar_to_byte_ch0,
                decode_2bchar_to_byte_ch1,
                decode_2bchar_to_byte_ch2,
                decode_2bchar_to_byte_ch3
        };


        nut2nt_source::sptr
        nut2nt_source::make(int num_outputs, const std::string& img_file, const std::string& cfg_file, const std::string& ecp5_alg,
                            const std::string &ecp5_data, size_t sizeof_stream_item,
                            size_t buf_len, int buf_num, int rotation_count) {
            return gnuradio::get_initial_sptr
                    (new nut2nt_source_impl(num_outputs, img_file, cfg_file, ecp5_alg, ecp5_data, sizeof_stream_item,
                                            buf_len, buf_num, rotation_count));
        }

        // The private constructor
        nut2nt_source_impl::nut2nt_source_impl(int num_outputs, const std::string &img_file, const std::string &cfg_file,
                                               const std::string &ecp5_alg,
                                               const std::string &ecp5_data, size_t sizeof_stream_item,
                                               size_t buf_len, int buf_num, int rotation_count)
                : gr::sync_block("nut2nt",
                                 gr::io_signature::make(0, 0, 0),
                                 gr::io_signature::make(num_outputs, num_outputs, item_size_lut[sizeof_stream_item])) {
            std::cout << "nut2nt Constructor" << std::endl
                      << " img_file: " << img_file << std::endl
                      << " algo_file: " << ecp5_alg << std::endl
                      << " data_file: " << ecp5_data << std::endl
                      << " hex_file: " << cfg_file << std::endl
                      << " sizeof_stream_itrm: " << sizeof_stream_item << std::endl
                      << " buf_len: " << buf_len << std::endl
                      << " buf_num: " << buf_num << std::endl
                      << " rotation_count: " << rotation_count << std::endl;
            enable_debug_ = true;
            debug_counter_ = 0;
            rotation_count_ = rotation_count % 4;
            m_pFX3Config = new FX3Config();
            m_pHWManager = new HWManager(m_pFX3Config);

            m_pHWManager->SetBufferLength(buf_len);
            m_pHWManager->SetBufferNum(buf_num);

            switch (sizeof_stream_item) {
                case sizeof(float):
                    m_dataFormat = DATA_FORMAT_FLOAT;
                    break;
                case sizeof(short):
                    m_dataFormat = DATA_FORMAT_SHORT;
                    break;
                case sizeof(char):
                    m_dataFormat = DATA_FORMAT_BYTE;
                    break;
                case 0:
                default:
                    m_dataFormat = DATA_FORMAT_RAW;
                    break;
            }

            Nut2ntSettings &ntSettings = m_pHWManager->GetSettings();
            ntSettings.imgFile = img_file;
            ntSettings.cfgFile = cfg_file;
            ntSettings.ecp5Alg = ecp5_alg;
            ntSettings.ecp5Data = ecp5_data;

        }

        // Our virtual destructor/
        nut2nt_source_impl::~nut2nt_source_impl() {
            if (m_pHWManager != NULL) {
                delete m_pHWManager;
                m_pHWManager = NULL;
            }

            if (m_pFX3Config != NULL) {
                delete m_pFX3Config;
                m_pFX3Config = NULL;
            }
        }

        void nut2nt_source_impl::init_nut2nt_source() {
            Nut2ntSettings &ntSettings = m_pHWManager->GetSettings();

            const char *pFileImg = ntSettings.imgFile.c_str();
            const char *pFileHex = (ntSettings.cfgFile.size() == 0) ? 0 : ntSettings.cfgFile.c_str();
            const char *pEcp5Alg = ntSettings.ecp5Alg.c_str();
            const char *pEcp5Data = ntSettings.ecp5Data.c_str();

            if (!m_pHWManager->initHardware(DrvTypeLibUsb, pFileImg, pFileHex, pEcp5Alg, pEcp5Data))
                throw std::runtime_error("Failed init hardware in HWManager. ");

//            if (!start())
//                throw std::runtime_error("Failed to start streams in HWManager. ");
        }

        bool nut2nt_source_impl::start() {
            fprintf(stderr, "START\n");
            if(!m_pHWManager->startStreams()) {
                throw std::runtime_error("Failed to start streams in HWManager.");
            }
            return true;
        }

        bool nut2nt_source_impl::stop() {
            fprintf(stderr, "STOP\n");
            m_pHWManager->stopStreams();
            return true;
        }

        int nut2nt_source_impl::work(int noutput_items,
                                     gr_vector_const_void_star &input_items,
                                     gr_vector_void_star &output_items) {
            // Do <+signal processing+>

            float **pfOut = (float **) &output_items[0];
            short **psOut = (short **) &output_items[0];
            char **pbOut = (char **) &output_items[0];

            size_t n_out_chan = output_items.size();

            //std::cout << "noutput_items = " << noutput_items << " n_out_chan = " << n_out_chan <<  " data_format = " << m_dataFormat << std::endl;

            // Считываем данные из буфера, которые были через Usb3 получены из платы.
            // Данные в буфере хранятся так как они приходят из платы - в каждом байте
            // упакованы по два бита вещественного значения для каждого из четырех каналов.
            std::vector<unsigned char> samples(noutput_items, 0);
            unsigned char *pSamples = samples.data();
            size_t nRead = m_pHWManager->ReadData(pSamples, noutput_items);


            for (int i = 0; i < nRead; i++) { 
                if (m_dataFormat == DATA_FORMAT_FLOAT) {
                    // Преобразуем в выходной параметр float для каждого канала
                    for (int chan = 0; chan < n_out_chan; chan++) {
                        int out_chan = (chan + rotation_count_) % 4;
                        pfOut[chan][i] = (*m_DecodeToFloatFunc.at(out_chan))(pSamples[i]);
                    }
                } else if (m_dataFormat == DATA_FORMAT_SHORT) {
                    // Преобразуем в выходной параметр short для каждого канала
                    for (int chan = 0; chan < n_out_chan; chan++) {
                        int out_chan = (chan + rotation_count_) % 4;
                        psOut[chan][i] = (*m_DecodeToShortFunc.at(out_chan))(pSamples[i]);
                    }
                } else if (m_dataFormat == DATA_FORMAT_BYTE) {
                    // Преобразуем в выходной параметр byte для каждого канала
                    for (int chan = 0; chan < n_out_chan; chan++) {
                        int out_chan = (chan + rotation_count_) % 4;
                        pbOut[chan][i] = (*m_DecodeToByteFunc.at(out_chan))(pSamples[i]);
                    }
                } else if (m_dataFormat == DATA_FORMAT_RAW) {
                    // Ничего не преобразуем, отправляем упакованные данные.
                    for (int chan = 0; chan < n_out_chan; chan++) {
                        pbOut[chan][i] = pSamples[i];
                    }
                }
            }

            if(enable_debug_) {
                if (debug_counter_ == 10000) {
                    fx3_dev_debug_info_t info = m_pHWManager->getDebugInfo();
                    std::cout << "\033[1;31m"
                              << "Debug:\n"
                              << "   overflows: " << info.overflows << "\n"
                              << "   phy_errs: " << info.phy_errs << "\n"
                              << "   lnk_errs: " << info.lnk_errs << "\n"
                              << "   phy_err_inc: " << info.phy_err_inc << "\n"
                              << "   lnk_err_inc: " << info.lnk_err_inc
                              <<"\033[0m" << std::endl;
                    debug_counter_ = 0;
                }
                debug_counter_++;
            }
            return nRead; // Tell runtime system how many output items we produced.
            //std::this_thread::sleep_for(std::chrono::milliseconds(100));

        }

        int nut2nt_source_impl::set_lo_src(int _lo_src) {
            Nut2ntSettings &ntSettings = m_pHWManager->GetSettings();

            return 0;
        }

        int nut2nt_source_impl::get_lo_src(int _lo_src) {
            return m_pHWManager->GetSettings().loSource;
        }

        //
        int nut2nt_source_impl::set_pll_freq(int _pll_idx, unsigned int _freq) {
            fprintf(stdout, "<<<< set_pll_freq. Pll_id = %d, _freq = %d >>>> \n", _pll_idx, _freq);

            Nut2ntSettings &ntSettings = m_pHWManager->GetSettings();

            Fx3Tuner *pTuner = m_pHWManager->GetTuner();
            if (pTuner != NULL) {
                pTuner->SetFreq(_pll_idx, (float) _freq);
            }

            if (_pll_idx == Nut2ntSettings::CLK_SOURCE_PLLA)
                ntSettings.pllFreqA = _freq;
            else if (_pll_idx == Nut2ntSettings::CLK_SOURCE_PLLA)
                ntSettings.pllFreqB = _freq;

            return 0;
        }

        int nut2nt_source_impl::get_pll_freq(int _pll_idx) {
            return (_pll_idx == 0) ? m_pHWManager->GetSettings().pllFreqA : m_pHWManager->GetSettings().pllFreqB;

        }

        int nut2nt_source_impl::set_clk_src(int _clk_src) {
            Nut2ntSettings &ntSettings = m_pHWManager->GetSettings();
            ntSettings.clkSource = (Nut2ntSettings::CLKSource) _clk_src;

            return 0;
        }

        int nut2nt_source_impl::get_clk_src() {
            return m_pHWManager->GetSettings().clkSource;
        }

        int nut2nt_source_impl::set_clk_type(int _clk_type) {

            return 0;
        }

        int nut2nt_source_impl::get_clk_type() {

            return 0;
        }

        int nut2nt_source_impl::set_clk_divider(int _clk_div) {
            Nut2ntSettings &ntSettings = m_pHWManager->GetSettings();
            ntSettings.clkDivider = _clk_div;

            return 0;
        }

        int nut2nt_source_impl::get_clk_divider() {
            return m_pHWManager->GetSettings().clkDivider;
        }

        //-------------  Channel control  --------------------

        int nut2nt_source_impl::set_gnss(int _chan, int _gnss) {
            fprintf(stdout, "<<<< set_gnss. Chan = %d, _gnss = %d >>>> \n", _chan, _gnss);

            std::vector<ntChannelSettings> &channels = m_pHWManager->GetSettings().channels;
            if (_chan >= 0 && _chan < channels.size()) {
                channels[_chan].gnssType = (ntChannelSettings::GNSSType) _gnss;
            }

            return 0;
        }

        int nut2nt_source_impl::get_gnss(int _chan) {
            std::vector<ntChannelSettings> &channels = m_pHWManager->GetSettings().channels;
            if (_chan >= 0 && _chan < channels.size()) {
                return channels.at(_chan).gnssType;
            }

            return 0;
        }

        int nut2nt_source_impl::set_if_pass_band(int _chan, int _band_code) {
            std::vector<ntChannelSettings> &channels = m_pHWManager->GetSettings().channels;
            if (_chan >= 0 && _chan < channels.size()) {
                auto Iter = std::find(ntChannelSettings::filtersTable.begin(), ntChannelSettings::filtersTable.end(),
                                      _band_code);
                if (Iter != ntChannelSettings::filtersTable.end())
                    channels[_chan].ifBandPass = (ntChannelSettings::IFPassBands) _band_code;
                else {
                    Iter = lower_bound(ntChannelSettings::filtersTable.begin(), ntChannelSettings::filtersTable.end(),
                                       _band_code);
                    if (Iter == ntChannelSettings::filtersTable.end())
                        Iter--;
                    channels[_chan].ifBandPass = (ntChannelSettings::IFPassBands) *Iter;
                }
                return 0;
            }

            return 0;
        }

        int nut2nt_source_impl::get_if_pass_band(int _chan) {
            std::vector<ntChannelSettings> &channels = m_pHWManager->GetSettings().channels;
            if (_chan >= 0 && _chan < channels.size()) {
                return channels.at(_chan).ifBandPass;
            }

            return 0;
        }

        int nut2nt_source_impl::set_chan_enable(int _chan, bool _enable) {
            std::vector<ntChannelSettings> &channels = m_pHWManager->GetSettings().channels;
            if (_chan >= 0 && _chan < channels.size()) {
                if (channels.at(_chan).enabled != _enable) {
                    channels[_chan].enabled = _enable;
                }
            }
            return 0;
        }

        bool nut2nt_source_impl::get_chan_enable(int _chan) {
            std::vector<ntChannelSettings> &channels = m_pHWManager->GetSettings().channels;
            if (_chan >= 0 && _chan < channels.size()) {
                return channels.at(_chan).enabled;
            }
            return false;
        }

    } // namespace Amungo
} // namespace gr

