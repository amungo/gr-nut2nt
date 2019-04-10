//
// Created by gaudima on 1/24/19.
//

#include "filedev.h"

void FileDev::run() {
    while(_threadRunning.test_and_set()) {
        auto start = std::chrono::high_resolution_clock::now();
        _inStream.read((char *)_data, _bufferSize);
        if(_inStream.gcount() == 0) {
            if(_repeat) {
                _inStream.seekg(std::ios::beg);
                continue;
            } else {
                return;
            }
        }
        auto han = _handler.load();
        if(han) {
            han->HandleDeviceData(_data, _bufferSize);
        }
        auto sleepDuration = std::chrono::high_resolution_clock::now() - start;
        sleepDuration = _sleepTime - sleepDuration;
        if(sleepDuration < std::chrono::nanoseconds(0)) {
            std::cerr << "file IO too slow" << std::endl;
        } else {
            std::this_thread::sleep_for(sleepDuration);
        }
    }
}

fx3_dev_err_t FileDev::init(const char *firmwareFileName, const char *additionalFirmwareFileName) {
    if(_fileName.empty()) {
        return FX3_ERR_NO_DEVICE_FOUND;
    }
    _inStream = std::ifstream(_fileName, std::ios::binary);
    _fileSize = _inStream.tellg();
    _inStream.seekg(std::ios::end);
    _fileSize = _inStream.tellg() - _fileSize;
    _inStream.seekg(std::ios::beg);
    return FX3_ERR_OK;
}

fx3_dev_err_t FileDev::init_fpga(const char *algoFileName, const char *dataFileName) {
    return FX3_ERR_OK;
}

void FileDev::startRead(DeviceDataHandlerIfce *handler) {
    changeHandler(handler);
    _threadRunning.test_and_set();
    _runThread = std::thread(&FileDev::run, this);
}

void FileDev::stopRead() {
    _threadRunning.clear();
    if(_runThread.joinable()) {
        _runThread.join();
    }
}

void FileDev::changeHandler(DeviceDataHandlerIfce *handler) {
    _handler.store(handler);
}

void FileDev::sendAttCommand5bits(uint32_t bits) {

}

fx3_dev_debug_info_t FileDev::getDebugInfoFromBoard(bool ask_speed_only) {
    return {};
}

fx3_dev_err_t FileDev::getReceiverRegValue(uint8_t addr, uint8_t &value) {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileDev::putReceiverRegValue(uint8_t addr, uint8_t value) {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileDev::send16bitSPI_ECP5(uint8_t addr, uint8_t data) {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileDev::read16bitSPI_ECP5(uint8_t addr, uint8_t *data) {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileDev::sendECP5(uint8_t *buf, long len) {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileDev::recvECP5(uint8_t *buf, long len) {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileDev::resetECP5() {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileDev::checkECP5() {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileDev::csonECP5() {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileDev::csoffECP5() {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileDev::send24bitSPI8bit(unsigned int data) {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileDev::device_start() {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileDev::device_stop() {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileDev::device_reset() {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileDev::reset_nt1065() {
    return FX3_ERR_OK;
}

fx3_dev_err_t FileDev::load1065Ctrlfile(const char *fwFileName, int lastaddr) {
    return FX3_ERR_OK;
}

FileDev::FileDev(std::string const& fileName, uint32_t bufferSize, uint32_t sampleRate, bool repeat) : _fileName(fileName),
                                                                                    _sampleRate(sampleRate),
                                                                                    _handler(nullptr),
                                                                                    _bufferSize(bufferSize),
                                                                                    _repeat(repeat) {
    _sleepTime = std::chrono::nanoseconds(1) * 1000000000 * bufferSize / sampleRate;
    _data = new uint8_t[_bufferSize];
}

FileDev::~FileDev() {
    delete[] _data;
}
