#ifndef _chan2bit_parser_h_
#define _chan2bit_parser_h_

#define CODE_MASK_CH0 ( 0x03 )
#define CODE_MASK_CH1 ( 0x0C )
#define CODE_MASK_CH2 ( 0x30 )
#define CODE_MASK_CH3 ( 0xC0 )

#define CODE_SHIFT_CH0 ( 0 )
#define CODE_SHIFT_CH1 ( 2 )
#define CODE_SHIFT_CH2 ( 4 )
#define CODE_SHIFT_CH3 ( 6 )

static double global_decoder_statistic[4][4] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
template< typename T_IN, typename T_OUT, int ch >
T_OUT decode2b( T_IN incode ) {
    static T_OUT table[ 4 ] = { (T_OUT)1, (T_OUT)3, (T_OUT)-1, (T_OUT)-3};
    const int idx = ( incode >> ( ch * 2 ) ) & 0x03;
    global_decoder_statistic[ch][idx] += 1.0;
    return table[ idx ];
}


inline float decode2bit_to_float_ch0( unsigned short code ) {
    static float decode_samples_f32[4] = {1.0f, 3.0f, -1.0f, -3.0f};
    return decode_samples_f32[ ( code & CODE_MASK_CH0 ) ];
}

inline short decode2bit_to_short_ch0( unsigned short code ) {
    static short decode_samples_i16[4] = {1, 3, -1, -3};
    return decode_samples_i16[ ( code & CODE_MASK_CH0 ) ];
}

inline short decode2bit_to_short_ch1( unsigned short code ) {
    static short decode_samples_i16[4] = {1, 3, -1, -3};
    return decode_samples_i16[ ( code & CODE_MASK_CH1 ) >> CODE_SHIFT_CH1 ];
}

inline short decode2bit_to_short_ch2( unsigned short code ) {
    static short decode_samples_i16[4] = {1, 3, -1, -3};
    return decode_samples_i16[ ( code & CODE_MASK_CH2 ) >> CODE_SHIFT_CH2 ];
}

inline short decode2bit_to_short_ch3( unsigned short code ) {
    static short decode_samples_i16[4] = {1, 3, -1, -3};
    return decode_samples_i16[ ( code & CODE_MASK_CH3 ) >> CODE_SHIFT_CH3 ];
}

//----------------------------------------------------------------------------
inline char decode_2bchar_to_byte_ch0( unsigned char code ) {
    static char decode_samples_b16[4] = {1, 3, -1, -3};
    return decode_samples_b16[ ( code & CODE_MASK_CH0 ) ];
}

inline char decode_2bchar_to_byte_ch1( unsigned char code ) {
    static char decode_samples_b16[4] = {1, 3, -1, -3};
    return decode_samples_b16[ ( code & CODE_MASK_CH1 ) >> CODE_SHIFT_CH1 ];
}

inline char decode_2bchar_to_byte_ch2( unsigned char code ) {
    static char decode_samples_b16[4] = {1, 3, -1, -3};
    return decode_samples_b16[ ( code & CODE_MASK_CH2 ) >> CODE_SHIFT_CH2 ];
}

inline char decode_2bchar_to_byte_ch3( unsigned char code ) {
    static char decode_samples_b16[4] = {1, 3, -1, -3};
    return decode_samples_b16[ ( code & CODE_MASK_CH3 ) >> CODE_SHIFT_CH3 ];
}
//------------------------------------------------------------------------------

inline short decode_2bchar_to_short_ch0( unsigned char code ) {
    static short decode_samples_i16[4] = {1, 3, -1, -3};
    return decode_samples_i16[ ( code & CODE_MASK_CH0 ) ];
}

inline short decode_2bchar_to_short_ch1( unsigned char code ) {
    static short decode_samples_i16[4] = {1, 3, -1, -3};
    return decode_samples_i16[ ( code & CODE_MASK_CH1 ) >> CODE_SHIFT_CH1 ];
}

inline short decode_2bchar_to_short_ch2( unsigned char code ) {
    static short decode_samples_i16[4] = {1, 3, -1, -3};
    return decode_samples_i16[ ( code & CODE_MASK_CH2 ) >> CODE_SHIFT_CH2 ];
}

inline short decode_2bchar_to_short_ch3( unsigned char code ) {
    static short decode_samples_i16[4] = {1, 3, -1, -3};
    return decode_samples_i16[ ( code & CODE_MASK_CH3 ) >> CODE_SHIFT_CH3 ];
}


inline float decode_2bchar_to_float_ch0( unsigned char code ) {
    static float decode_samples_f32[4] = {1.0f, 3.0f, -1.0f, -3.0f};
    return decode_samples_f32[ ( code & CODE_MASK_CH0 ) ];
}

inline float decode_2bchar_to_float_ch1( unsigned char code ) {
    static float decode_samples_f32[4] = {1.0f, 3.0f, -1.0f, -3.0f};
    return decode_samples_f32[ ( code & CODE_MASK_CH1 ) >> CODE_SHIFT_CH1 ];
}

inline float decode_2bchar_to_float_ch2( unsigned char code ) {
    static float decode_samples_f32[4] = {1.0f, 3.0f, -1.0f, -3.0f};
    return decode_samples_f32[ ( code & CODE_MASK_CH2 ) >> CODE_SHIFT_CH2 ];
}

inline float decode_2bchar_to_float_ch3( unsigned char code ) {
    static float decode_samples_f32[4] = {1.0f, 3.0f, -1.0f, -3.0f};
    return decode_samples_f32[ ( code & CODE_MASK_CH3 ) >> CODE_SHIFT_CH3 ];
}



#endif
