#ifndef FILESIMDEV_H
#define FILESIMDEV_H

#include "fx3devifce.h"
#include <thread>

class FileSimDev : public FX3DevIfce
{
public:
    FileSimDev( const char* sigfname, double real_sr, size_t block_pts );
    ~FileSimDev();

    fx3_dev_err_t init(const char *firmwareFileName, const char *additionalFirmwareFileName);
    void startRead(DeviceDataHandlerIfce *handler);
    void stopRead();
    void sendAttCommand5bits(uint32_t bits);
    fx3_dev_debug_info_t getDebugInfoFromBoard( bool ask_speed_only = false );

protected:
    virtual fx3_dev_err_t ctrlToDevice(   uint8_t cmd, uint16_t value = 0, uint16_t index = 0, void* data = nullptr, size_t data_len = 0 ) { return FX3_ERR_OK; }
    virtual fx3_dev_err_t ctrlFromDevice( uint8_t cmd, uint16_t value = 0, uint16_t index = 0, void* dest = nullptr, size_t data_len = 0 ) { return FX3_ERR_OK; }

public:
    fx3_dev_err_t init_fpga(const char *algoFileName, const char *dataFileName) override;

    void changeHandler(DeviceDataHandlerIfce *handler) override;

    fx3_dev_err_t getReceiverRegValue(uint8_t addr, uint8_t &value) override;

    fx3_dev_err_t putReceiverRegValue(uint8_t addr, uint8_t value) override;

    fx3_dev_err_t send16bitSPI_ECP5(uint8_t addr, uint8_t data) override;

    fx3_dev_err_t read16bitSPI_ECP5(uint8_t addr, uint8_t *data) override;

    fx3_dev_err_t sendECP5(uint8_t *buf, long len) override;

    fx3_dev_err_t recvECP5(uint8_t *buf, long len) override;

    fx3_dev_err_t resetECP5() override;

    fx3_dev_err_t checkECP5() override;

    fx3_dev_err_t csonECP5() override;

    fx3_dev_err_t csoffECP5() override;

    fx3_dev_err_t send24bitSPI8bit(unsigned int data) override;

    fx3_dev_err_t device_start() override;

    fx3_dev_err_t device_stop() override;

    fx3_dev_err_t device_reset() override;

    fx3_dev_err_t reset_nt1065() override;

    fx3_dev_err_t load1065Ctrlfile(const char *fwFileName, int lastaddr) override;

private:
    DeviceDataHandlerIfce *cb_handle;
    FILE* file;

    double SR;
    int64_t block_pts;
    uint32_t sleep_ms;
    int64_t current_offset8;
    int64_t file_size8;
    int8_t*  buf_file;

    bool running;
    bool need_exit;
    std::thread thr;
    void run();

};

#endif // FILESIMDEV_H
