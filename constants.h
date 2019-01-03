#ifndef CONSTANTS_H
#define CONSTANTS_H

const int BASE_INTERVAL = 1000;
const int MODBUS_HEAD_LEHGHT = 9;
const int WINDOW_LEFT_WIDTH = 250;
const int WINDOW_RIGHT_WIDTH = 250;
const int WINDOW_HEADER_HEIGHT = 60;

const int MAX_QUEUE_NUM = 10;
const float PC_RATIO = 0.01;

enum MSGAddr {
    NONE = 0,
    COM_R = 1,
    SYS_R = 2,
    AD_R = 3,
    STATE_R = 4,
    FREQ_W = 5,
    COM_W = 10,
    PIT_W = 11,
    SAVE_W = 15,
    MEASURE_R = 20,
    E_FACTOR_R = 40,
    TIME_SYNC_W = 60,
    ROTATE_SYCN_W = 61,
    ROTATE_W = 62,
    POWER_W = 63,
};

enum ChartType {
    LINE = 1,
    BAR = 2
};

enum BYTEORDER4 {
    LL_LH_HL_HH = 1,
    HH_HL_LH_LL = 2,
};

enum BYTEORDER2 {
    LL_HH = 1,
    HH_LL = 2,
};

enum TABLENAME {
    NONEINFO = 0,
    CHARGEINFO = 1,
    CHARGEFREQINFO = 2,
    CHARGEWAVEINFO = 3,
    VIBRATEINFO = 4,
    VIBRATEFREQINFO = 5,
    VIBRATEWAVEINFO = 6,
    TEMPERATUREINFO = 7,
};

struct ModbusTCPMapInfo
{
    MSGAddr Unit;
    unsigned short Addr;
    int Length;
    char Command;
    char *data;
    int ExpectLen;
};

#endif // CONSTANTS_H
