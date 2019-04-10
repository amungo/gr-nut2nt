#include "FileSimDev.h"

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)

#include <iostream>

int fseek_cross( FILE* f, int64_t offset, int origin ) {
#ifdef WIN32
    return _fseeki64( f, offset, origin );
#else
    return fseek( f, offset, origin );
#endif
}

int64_t ftell_cross( FILE* f ) {
#ifdef WIN32
    return _ftelli64( f );
#else
    return ftell( f );
#endif
}



FileSimDev::FileSimDev(const char *sigfname, double real_sr, size_t pts_cnt_per_block) :
    cb_handle( NULL ),
    file( NULL ),
    SR( real_sr ),
    block_pts( pts_cnt_per_block ),
    sleep_ms( ( ( double ) pts_cnt_per_block / real_sr / 1000.0 ) ),
    current_offset8( 0 ),
    file_size8( 0 ),
    buf_file( NULL ),
    running ( false ),
    need_exit( false )

{
    file = fopen( sigfname, "rb" );
    if ( file ) {
        fprintf( stderr, "FileSimDev::FileSimDev() file was opened\n" );
        fseek_cross( file, 0, SEEK_END );
        file_size8 = ftell_cross( file );
        fseek_cross( file, 0, SEEK_SET );
        std::cerr << "FileSimDev::FileSimDev() file size: " << file_size8 << " bytes" << std::endl;
        std::cerr << "FileSimDev::FileSimDev() sleep pause = " << sleep_ms << " u ms" << std::endl;
    } else {
        fprintf( stderr, "__error__ FileSimDev::FileSimDev() file IO error \n" );
    }
    buf_file = new int8_t[ block_pts ];
    thr = std::thread( &FileSimDev::run, this );
}

FileSimDev::~FileSimDev() {
    running = false;
    need_exit = true;
    if ( thr.joinable() ) {
        thr.join();
    }

    if ( file ) {
        fclose( file );
        file = NULL;
    }
    if ( buf_file ) {
        delete [] buf_file;
        buf_file = NULL;
    }
}

void FileSimDev::run() {
    while ( !need_exit ) {
        #ifdef WIN32
        Sleep( sleep_ms );
        #else
        usleep( sleep_ms * 1000 );
        #endif
        if ( running ) {
            if ( file_size8 < current_offset8 + block_pts ) {
                current_offset8 = 0;
                fseek_cross( file, 0, SEEK_SET );
            }
            //fprintf( stderr, "current_offset8 %12lld, %5.0f ms\n", current_offset8, 1000.0 * ((double)current_offset8/1) / SR );
            size_t ret = fread( buf_file, 1, block_pts, file );
            current_offset8 += block_pts;

            if ( cb_handle ) {
                cb_handle->HandleDeviceData( (char*)buf_file, block_pts );
            }
        }
    }
}


fx3_dev_err_t FileSimDev::init(const char*, const char*) {
    if ( file ) {
        return FX3_ERR_OK;
    } else {
        return FX3_ERR_BAD_DEVICE;
    }
}

void FileSimDev::startRead(DeviceDataHandlerIfce *handler) {
    if ( !running ) {
        cb_handle = handler;
        current_offset8 = 0;
        running = true;
    }
}

void FileSimDev::stopRead() {
    running = false;
}

void FileSimDev::sendAttCommand5bits(uint32_t) {

}

fx3_dev_debug_info_t FileSimDev::getDebugInfoFromBoard(bool ask_speed_only) {
    fx3_dev_debug_info_t info;
    info.status = FX3_ERR_REG_WRITE_FAIL;
    return info;
}

fx3_dev_err_t FileSimDev::init_fpga(const char *algoFileName, const char *dataFileName) {
    return FX3_ERR_OK;
}

void FileSimDev::changeHandler(DeviceDataHandlerIfce *handler) {

}

fx3_dev_err_t FileSimDev::getReceiverRegValue(uint8_t addr, uint8_t &value) {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileSimDev::putReceiverRegValue(uint8_t addr, uint8_t value) {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileSimDev::send16bitSPI_ECP5(uint8_t addr, uint8_t data) {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileSimDev::read16bitSPI_ECP5(uint8_t addr, uint8_t *data) {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileSimDev::sendECP5(uint8_t *buf, long len) {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileSimDev::recvECP5(uint8_t *buf, long len) {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileSimDev::resetECP5() {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileSimDev::checkECP5() {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileSimDev::csonECP5() {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileSimDev::csoffECP5() {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileSimDev::send24bitSPI8bit(unsigned int data) {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileSimDev::device_start() {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileSimDev::device_stop() {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileSimDev::device_reset() {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileSimDev::reset_nt1065() {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileSimDev::load1065Ctrlfile(const char *fwFileName, int lastaddr) {
    return FX3_ERR_OK;
}

