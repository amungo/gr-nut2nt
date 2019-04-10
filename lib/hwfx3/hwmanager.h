#ifndef HWMANAGER_H
#define HWMANAGER_H

#include "fx3devdrvtype.h"
#include "fx3devifce.h"
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "ntlab_settings.h"


class FX3Config;
class Fx3Tuner;

class HWManager : public DeviceDataHandlerIfce
{
public:
    explicit HWManager(FX3Config* cfg);
    ~HWManager();
    // DeviceDataHandlerIfce interface
    virtual void HandleDeviceData(void* data, size_t len);
    size_t ReadData(unsigned char* data, size_t len);

public:
    bool initHardware( DriverType_t drvType, const char *imageFileName, const char *ntcfg, const char *ecp5alg,
                       const char *ecp5data );
    void closeHardware();
    bool startStreams();
    void stopStreams();
    bool setAttReg( uint32_t reg_val );
    fx3_dev_debug_info_t getDebugInfo();
    bool SetBufferLength(size_t _len);
    bool SetBufferNum(size_t _num);

    Fx3Tuner* GetTuner();
    Nut2ntSettings& GetSettings();

private:
    FX3DevIfce* m_pDev;
    Fx3Tuner* m_pTuner;
    FX3Config* m_pCfg;
    std::atomic<bool> m_Running;

    // Управление буферами
    u_char** m_ppBuf;
    uint32_t m_bufNum;
    uint32_t m_bufLen;
    uint32_t m_bufHead;
    uint32_t m_bufUsed;
    std::mutex m_bufMutex;
    std::condition_variable m_bufCond;

    int32_t m_bufOffset;
    int32_t m_sampAvail;

};

#endif // HWMANAGER_H

