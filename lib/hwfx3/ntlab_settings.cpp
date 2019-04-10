#include "ntlab_settings.h"
#include "fx3tuner.h"

std::vector<ntChannelSettings::IFPassBands> ntChannelSettings::filtersTable = {
    IF_BAND_15_12MHZ,
    IF_BAND_15_69MHZ,
    IF_BAND_16_59MHZ,
    IF_BAND_17_60MHZ,
    IF_BAND_18_33MHZ,
    IF_BAND_19_36MHZ,
    IF_BAND_20_31MHZ,
    IF_BAND_21_13MHZ,
    IF_BAND_21_92MHZ,
    IF_BAND_22_89MHZ,
    IF_BAND_23_82MHZ,
    IF_BAND_24_94MHZ,
    IF_BAND_25_45MHZ,
    IF_BAND_26_50MHZ,
    IF_BAND_27_38MHZ,
    IF_BAND_28_31MHZ,
    IF_BAND_29_02MHZ,
    IF_BAND_29_64MHZ,
    IF_BAND_30_47MHZ,
    IF_BAND_31_19MHZ
};

CLK_LVDSSettings::CLK_LVDSSettings() : ampl(Fx3Tuner::CLK_LVDS_ampl_570mV),
                                        outDC(Fx3Tuner::CLK_LVDS_outDC_2400mV)
{}

CLK_CMOSSettings::CLK_CMOSSettings() : ampl(Fx3Tuner::CLK_CMOS_ampl_2400mV)
{}
