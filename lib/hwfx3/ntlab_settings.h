#ifndef __NTLAB_SETTINGS_H__
#define __NTLAB_SETTINGS_H__

#include <vector>
#include <string>

typedef enum
{
    DATA_FORMAT_FLOAT,
    DATA_FORMAT_SHORT,
    DATA_FORMAT_BYTE,
    DATA_FORMAT_RAW
}DataFormat;

struct ntChannelSettings
{
    typedef enum
    {
        GNSS_TYPE_USB = 0,
        GNSS_TYPE_LSB,
        GNSS_TYPE_SIZE
    }GNSSType;

    typedef enum
    {
        IF_BAND_15_12MHZ = 0x16, // 0010110 [22]
        IF_BAND_15_69MHZ = 0x18, // 0011000 [24]
        IF_BAND_16_59MHZ = 0x1B, // 0011011 [27]
        IF_BAND_17_60MHZ = 0x1E, // 0011110 [30]
        IF_BAND_18_33MHZ = 0x21, // 0100001 [33]
        IF_BAND_19_36MHZ = 0x24, // 0100100 [36]
        IF_BAND_20_31MHZ = 0x27, // 0100111 [39]
        IF_BAND_21_13MHZ = 0x2A, // 0101010 [42]
        IF_BAND_21_92MHZ = 0x2D, // 0101101 [45]
        IF_BAND_22_89MHZ = 0x30, // 0110000 [48]
        IF_BAND_23_82MHZ = 0x33, // 0110011 [51]
        IF_BAND_24_94MHZ = 0x36, // 0110110 [54]
        IF_BAND_25_45MHZ = 0x39, // 0111001 [57]
        IF_BAND_26_50MHZ = 0x3C, // 0111100 [60]
        IF_BAND_27_38MHZ = 0x3F, // 0111111 [63]
        IF_BAND_28_31MHZ = 0x42, // 1000010 [66]
        IF_BAND_29_02MHZ = 0x45, // 1000101 [69]
        IF_BAND_29_64MHZ = 0x48, // 1001000 [72]
        IF_BAND_30_47MHZ = 0x4B, // 1001011 [75]
        IF_BAND_31_19MHZ = 0x4D  // 1001101 [77]
    }IFPassBands;
    static std::vector<IFPassBands> filtersTable;

    ntChannelSettings() : enabled(true),
                        gnssType(GNSS_TYPE_USB),
                        ifBandPass(IF_BAND_15_12MHZ)
    {}

    bool enabled;
    GNSSType gnssType;
    IFPassBands ifBandPass;

};

struct CLK_LVDSSettings
{
    CLK_LVDSSettings();

    uint8_t ampl;
    uint8_t outDC;
};

struct CLK_CMOSSettings
{
    CLK_CMOSSettings();

    uint8_t ampl;
};

struct PLL_Settings
{
    PLL_Settings()
    {}

    bool enable;
};

struct AGC_Settings
{

};

struct Nut2ntSettings
{
    typedef enum
    {
        LO_SOURCE_PLLA = 0,
        LO_SOURCE_PLLA_PLLB,
        LO_SOURCE_SIZE
    }LOSource;

    typedef enum
    {
        CLK_SOURCE_PLLA = 0,
        CLK_SOURCE_PLLB,
        CLK_SOURCE_SIZE
    }CLKSource;

    typedef enum
    {
        CLK_TYPE_LVDS = 0,
        CLK_TYPE_CMOS,
        CLK_TYPE_SIZE
    }CLKType;

    Nut2ntSettings() : loSource(LO_SOURCE_PLLA), clkSource(CLK_SOURCE_PLLA),
                        clkType(CLK_TYPE_CMOS), clkDivider(15),
                        pllFreqA(1590.0), pllFreqB(1590.0)
    {
        channels.resize(4);
    }

    Nut2ntSettings(const Nut2ntSettings& obj)
    {
        imgFile = obj.imgFile;
        cfgFile = obj.cfgFile;
        loSource = obj.loSource;
        clkSource = obj.clkSource;
        clkDivider = obj.clkDivider;
        clkLVDS = obj.clkLVDS;
        clkCMOS = obj.clkCMOS;
        pllFreqA = obj.pllFreqA;
        pllFreqB = obj.pllFreqB;
        channels = obj.channels;
    }

    Nut2ntSettings& operator=(const Nut2ntSettings& _other)
    {
        if(this != &_other)
        {
            imgFile = _other.imgFile;
            cfgFile = _other.cfgFile;
            loSource = _other.loSource;
            clkSource = _other.clkSource;
            clkDivider = _other.clkDivider;
            clkLVDS = _other.clkLVDS;
            clkCMOS = _other.clkCMOS;
            pllFreqA = _other.pllFreqA;
            pllFreqB = _other.pllFreqB;
            channels = _other.channels;
        }
        return *this;
    }

    std::string imgFile;
    std::string cfgFile;
    std::string ecp5Alg;
    std::string ecp5Data;
    LOSource loSource;
    CLKSource clkSource;
    CLKType clkType;
    int clkDivider; // (8 - 31)
    CLK_LVDSSettings clkLVDS;
    CLK_CMOSSettings clkCMOS;

    double pllFreqA; // L1: 1450 - 1650; L2/L3/L5 band 1140 - 1300  MHz
    double pllFreqB; // L1: 1450 - 1650; L2/L3/L5 band 1140 - 1300  MHz
    std::vector<ntChannelSettings> channels;
};

#endif // __NTLAB_SETTINGS_H__

