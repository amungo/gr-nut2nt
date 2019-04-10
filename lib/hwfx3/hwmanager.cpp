#include <thread>
#include <chrono>
#include <iostream>
#include <string.h>


#include "hwmanager.h"
#include "fx3config.h"
#include "fx3dev.h"
#include "fx3tuner.h"
#include "FileSimDev.h"

#ifdef WIN32
#include <windows.h>
#include "fx3devcyapi.h"
#endif


#define BUF_NUM 16
#define BUF_LEN (2 * 1024 * 1024)

#define BYTES_PER_SAMPLE 1

using namespace std;

HWManager::HWManager(FX3Config *cfg) :
        m_pDev(NULL),
        m_pTuner(NULL),
        m_pCfg(cfg),
        m_ppBuf(NULL),
        m_bufNum(BUF_NUM),
        m_bufLen(BUF_LEN),
        m_bufHead(0),
        m_bufUsed(0),
        m_bufOffset(0) {
    m_Running = false;

    m_sampAvail = m_bufLen / BYTES_PER_SAMPLE;

    m_ppBuf = (unsigned char **) malloc(m_bufNum * sizeof(char *));
    if (m_ppBuf != NULL) {
        for (int i = 0; i < m_bufNum; i++)
            m_ppBuf[i] = (unsigned char *) malloc(m_bufLen);
    }
}

HWManager::~HWManager() {
    if (m_pTuner != NULL) {
        delete m_pTuner;
        m_pTuner = NULL;
    }

    if (m_pDev != NULL) {
        FX3DevIfce *safem_pDev = m_pDev;
        m_pDev = NULL;
        delete safem_pDev;
    }

    if (m_ppBuf != NULL) {
        for (unsigned int i = 0; i < m_bufNum; i++)
            ::free(m_ppBuf[i]);

        ::free(m_ppBuf);
        m_ppBuf = 0;
    }
}

Nut2ntSettings &HWManager::GetSettings() {
    return Fx3Tuner::m_ntSettings;
}

Fx3Tuner *HWManager::GetTuner() {
    return m_pTuner;
}

bool HWManager::SetBufferLength(size_t _len) {
    if (_len >= 1024) {
        m_bufLen = _len;
        fprintf(stdout, "HWManager::SetBufferLength: Len: %d \n", (int) _len);
        return true;
    } else
        return true;
}

bool HWManager::SetBufferNum(size_t _num) {
    if (_num > 0 && _num < 128) {
        m_bufNum = _num;
        fprintf(stdout, "HWManager::SetBufferNum: Len: %d \n", (int) _num);
        return true;
    } else
        return false;
}

bool HWManager::initHardware(DriverType_t drvType, const char *imageFileName, const char *ntcfg, const char *ecp5alg,
                             const char *ecp5data) {
    fprintf(stderr, "HWManager::initHardware( %d, %s, %s, %s, %s )\n", (int32_t) drvType, imageFileName, ntcfg, ecp5alg,
            ecp5data);

    if (m_pDev != NULL) {
        delete m_pDev;
        m_pDev = NULL;
    }

    switch (drvType) {
        case DrvTypeLibUsb:
            m_pDev = new FX3Dev(m_bufLen, m_bufNum);
            break;
        case DrvTypeCypress:
#ifdef _WIN32
            m_pDev = new FX3DevCyAPI();
#endif
            break;
        case DrvTypeFileSim:
            m_pCfg->adc_sample_rate_hz = 53.0;
            m_pDev = new FileSimDev(imageFileName, m_pCfg->adc_sample_rate_hz, 2 * 1024 * 1024);
            break;
        default:
            fprintf(stderr, "Device init error \n");
            return false;
    }

    if(drvType == DrvTypeLibUsb) {
        if (m_pDev->init(imageFileName, 0) != FX3_ERR_OK) {
            fprintf(stderr, "Problems with hardware or driver type");
            return false;
        }

        if (m_pDev->init_fpga(ecp5alg, ecp5data) != FX3_ERR_OK) {
            fprintf(stderr, "Problems with loading Lattice firmware (dev->init_fpga)");
            return false;
        }

        if (ntcfg != nullptr && m_pDev->load1065Ctrlfile(ntcfg, 48) != FX3_ERR_OK) {
            fprintf(stderr, "Problems with loading nt config file (dev->load1065Ctrlfile)");
            return false;
        }

        std::cerr << "Device was inited." << std::endl << std::endl;
        m_pDev->log = false;

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        std::cerr << "Determinating sample rate" << std::endl;

        m_pDev->startRead(nullptr);

        // This is temporary workaround for strange bug of 'odd launch'
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        m_pDev->stopRead();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        m_pDev->startRead(nullptr);


        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        m_pDev->getDebugInfoFromBoard(true);

        double size_mb = 0.0;
        double phy_errs = 0;
        int sleep_ms = 200;
        int iter_cnt = 5;
        double overall_seconds = (sleep_ms * iter_cnt) / 1000.0;
        fx3_dev_debug_info_t info = m_pDev->getDebugInfoFromBoard(true);
        for (int i = 0; i < iter_cnt; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
            info = m_pDev->getDebugInfoFromBoard(true);
            //info.print();
            std::cerr << ".";
            size_mb += info.size_tx_mb_inc;
            phy_errs += info.phy_err_inc;
        }
        std::cerr << std::endl;
        m_pDev->stopRead();
        int64_t CHIP_SR = (int64_t) ((size_mb * 1024.0 * 1024.0) / overall_seconds);

        std::cerr << std::endl;
        std::cerr << "SAMPLE RATE  is ~" << CHIP_SR / 1000000 << " MHz " << std::endl;
        std::cerr << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return true;
}

void HWManager::closeHardware() {
    stopStreams();
    delete m_pTuner;
    m_pTuner = NULL;
    delete m_pDev;
    m_pDev = NULL;
    fprintf(stderr, "Device was closed \n");
}

bool HWManager::startStreams() {
    if (m_pDev != NULL && m_Running == false) {
        m_Running = true;
        m_pDev->startRead(this);
        fprintf(stderr, "Data stream was started \n");
        return true;
    } else
        return false;
}

void HWManager::stopStreams() {
    if (m_pDev != NULL) {
        m_Running = false;
        m_pDev->stopRead();
    }
    fprintf(stderr, "Data stream was stopped \n");
}

bool HWManager::setAttReg(uint32_t reg_val) {
    if (m_pDev != NULL) {
        m_pDev->sendAttCommand5bits(reg_val);
        return true;
    } else
        return false;
}

fx3_dev_debug_info_t HWManager::getDebugInfo() {
    fx3_dev_debug_info_t info;
    info.status = FX3_ERR_NO_DEVICE_FOUND;

    if (m_pDev != NULL) {
        info = m_pDev->getDebugInfoFromBoard(m_pCfg->adc_type == ADC_1ch_16bit);
        //(info.status == FX3_ERR_OK)
    }
    return info;
}

void HWManager::HandleDeviceData(void *buf, size_t len) {
    lock_guard<mutex> lock(m_bufMutex);

    int buf_tail = (m_bufHead + m_bufUsed) % m_bufNum;
    ::memcpy(m_ppBuf[buf_tail], buf, len);

    if (m_bufUsed == m_bufNum) {
        cerr << "\033[1;31m" << "OVERFLOW" << "\033[0m" << endl;
        m_bufHead = (m_bufHead + 1) % m_bufNum;
    } else {
        m_bufUsed++;
    }

    m_bufCond.notify_one();
}

size_t HWManager::ReadData(unsigned char *data, size_t len) {
    unique_lock<mutex> lock(m_bufMutex);

    while (m_bufUsed < 2 && m_Running == true)
        m_bufCond.wait(lock);

    if (m_Running == false)
        return 0;

    unsigned char *pOut = data;
    int noutput_items = len;
    while (noutput_items && m_bufUsed) {
        const int nout = std::min(noutput_items, m_sampAvail);
        const unsigned char *buf = m_ppBuf[m_bufHead] + m_bufOffset;

        ::memcpy(pOut, buf, nout);

        pOut += nout;
        noutput_items -= nout;
        m_sampAvail -= nout;

        if (!m_sampAvail) {
            m_bufHead = (m_bufHead + 1) % m_bufNum;
            m_bufUsed--;
            m_sampAvail = m_bufLen / BYTES_PER_SAMPLE;
            m_bufOffset = 0;
        } else {
            m_bufOffset += nout;
        }
    }

    return (pOut - data);
}



