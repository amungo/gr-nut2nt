//
// Created by gaudima on 1/24/19.
//

#ifndef GR_NUT2NT_FILEDEV_H
#define GR_NUT2NT_FILEDEV_H

#include <string>
#include <chrono>
#include <fstream>
#include <thread>
#include <atomic>
#include <fx3devifce.h>

class FileDev : public FX3DevIfce {
private:
    std::string _fileName;
    std::ifstream _inStream;
    ssize_t _fileSize;
    uint32_t _sampleRate;
    uint32_t _bufferSize;
    std::chrono::nanoseconds _sleepTime;
    std::atomic<DeviceDataHandlerIfce*> _handler;
    std::thread _runThread;
    std::atomic_flag _threadRunning = ATOMIC_FLAG_INIT;
    bool _repeat;
    uint8_t* _data;

    void run();

public:
    FileDev(std::string const& filename,  uint32_t bufferSize, uint32_t sampleRate, bool repeat);

    virtual ~FileDev();

    fx3_dev_err_t init(const char *firmwareFileName, const char *additionalFirmwareFileName) override;

    fx3_dev_err_t init_fpga(const char *algoFileName, const char *dataFileName) override;

    void startRead(DeviceDataHandlerIfce *handler) override;

    void stopRead() override;

    void changeHandler(DeviceDataHandlerIfce *handler) override;

    void sendAttCommand5bits(uint32_t bits) override;

    fx3_dev_debug_info_t getDebugInfoFromBoard(bool ask_speed_only) override;

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
};


#endif //GR_NUT2NT_FILEDEV_H
