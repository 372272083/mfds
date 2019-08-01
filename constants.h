#ifndef CONSTANTS_H
#define CONSTANTS_H

const int BASE_INTERVAL = 1000;
const int MODBUS_HEAD_LEHGHT = 9;
const int WINDOW_LEFT_WIDTH = 250;
const int WINDOW_RIGHT_WIDTH = 250;
const int WINDOW_HEADER_HEIGHT = 60;

const int MAX_QUEUE_NUM = 15;
const float PC_RATIO = 0.01;

enum MSGAddr {
    NONE = 0,
    COM_R = 1,
    SYS_R = 2,
    AD_R = 3,
    STATE_R = 4,
    FREQ_W = 5,
    MODE_V_W = 6,
    MODE_V_R = 7,
    FREQ_W_W = 8,
    COM_W = 10,
    PIT_W = 11,
    SAVE_W = 15,
    MEASURE_R = 20,
    ROTATE_R = 21,
    E_FACTOR_R = 40,
    TIME_SYNC_W = 60,
    ROTATE_SYCN_W = 61,
    ROTATE_W = 62,
    POWER_W = 63,

    TW888_R_0 = 90,
    TW888_R_1 = 91,
    TW888_R_2 = 92,
    TW888_R_3 = 93,
    TW888_R_4 = 94,
    TW888_R_5 = 95,
    TW888_R_6 = 96,

    NI_LINK_STATE = 100,
    NI_WAVE_S = 101,
    NI_WAVE_A = 102,
    NI_FREQ_S = 103,
    NI_FREQ_A = 104,
    NI_FEATURE = 105,
    NI_SAMPLE = 106,
};

enum FFT_WINDOW {
    WINDOW_NONE = 0,
    WINDOW_RECT,
    WINDOW_HANNING,
    WINDOWN_HAMMING,
    WINDOW_BLACKMAN,
};

enum FFT_FilterType{
    FilterNone = 0,
    LowPassFilter,
    HghPassFilter,
    BndPassFilter,
    BndStopFilter,
};

enum ChartType {
    LINE = 1,
    BAR = 2
};

enum BYTEORDER4 {
    LL_LH_HL_HH = 1,
    HH_HL_LH_LL = 2,
    HL_HH_LL_LH = 3,
    LH_LL_HH_HL = 4,
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

    CHARGEINFO_I = 10,
    CHARGEFREQINFO_I = 11,
    CHARGEWAVEINFO_I = 12,
    VIBRATEINFO_S = 13,
    VIBRATEFREQINFO_S = 14,
    VIBRATEWAVEINFO_S = 15,

    ACCFREQFEAT = 20,
    SPDFREQFEAT = 21,
    VOLFREQFEAT = 22,
    CURFREQFEAT = 23,
};

struct ModbusTCPMapInfo
{
    unsigned short Num;
    MSGAddr Unit;
    unsigned short Addr;
    int Length;
    char Command;
    unsigned char *data;
    int ExpectLen;
};

enum TREENODETYPE {
    MOTORROOT = 0,
    MOTOR = 1,
    CHANNEL_PHYISIC = 2,
    CHANNEL_COMPOSE = 3,
    CHANNEL_COMPARE = 4,
    MEASURE = 8,
    MEASURE_SPEED = 16,
    MEASURE_POS = 17,
    WAVE = 9,
    FREQENCY = 10,
    WAVE_A = 11,
    SPEED_WAVE_A = 12,
    WAVE_FREQ_A = 13,
    SPEED_FREQ_A = 14,
    EN_FREQ_A = 15,
    DIS_WAVE_A = 18,
    DEVICEROOT = 20,
    DEVICE = 21,
    UMEASURE = 50,
    IMEASURE = 51,
    UWAVE = 52,
    IWAVE = 53,
    UFREQENCY = 54,
    IFREQENCY = 55,
    WATERFALL_ACC = 56,
    WATERFALL_SPD = 57,
    WATERFALL_VOL = 58,
    WATERFALL_CUR = 59,
    PATH_TRACKING = 60,
    DIAGNOSE_REPORT = 61,

    ACC_POWER = 62,
    SPD_POWER = 63,
    ACC_Cepstrum = 64,
    SPD_Cepstrum = 65,

    ACTPOWER = 66,
    REACTPOWER = 67,
    APPPOWER = 68,
    COSF = 69,

    U_P_SEQUENCE = 70,
    U_N_SEQUENCE = 71,
    U_Z_SEQUENCE = 72,

    I_P_SEQUENCE = 73,
    I_N_SEQUENCE = 74,
    I_Z_SEQUENCE = 75,

    PARK_TRANSFORMATION = 76,

    ACC_FREQ_HX = 150,
    ACC_FREQ_1X = 151,
    ACC_FREQ_2X = 152,
    ACC_FREQ_3X = 153,
    ACC_FREQ_4X = 154,
    ACC_FREQ_5X = 155,
    ACC_FREQ_6X = 156,
    ACC_FREQ_7X = 157,
    ACC_FREQ_8X = 158,
    ACC_FREQ_9X = 159,
    ACC_FREQ_10X = 160,

    SPD_FREQ_HX = 170,
    SPD_FREQ_1X = 171,
    SPD_FREQ_2X = 172,
    SPD_FREQ_3X = 173,
    SPD_FREQ_4X = 174,
    SPD_FREQ_5X = 175,
    SPD_FREQ_6X = 176,
    SPD_FREQ_7X = 177,
    SPD_FREQ_8X = 178,
    SPD_FREQ_9X = 179,
    SPD_FREQ_10X = 180,

    VOL_FREQ_HX = 190,
    VOL_FREQ_1X = 191,
    VOL_FREQ_2X = 192,
    VOL_FREQ_3X = 193,
    VOL_FREQ_4X = 194,
    VOL_FREQ_5X = 195,
    VOL_FREQ_6X = 196,
    VOL_FREQ_7X = 197,
    VOL_FREQ_8X = 198,
    VOL_FREQ_9X = 199,
    VOL_FREQ_10X = 200,

    CUR_FREQ_HX = 210,
    CUR_FREQ_1X = 211,
    CUR_FREQ_2X = 212,
    CUR_FREQ_3X = 213,
    CUR_FREQ_4X = 214,
    CUR_FREQ_5X = 215,
    CUR_FREQ_6X = 216,
    CUR_FREQ_7X = 217,
    CUR_FREQ_8X = 218,
    CUR_FREQ_9X = 219,
    CUR_FREQ_10X = 220,

    TEMP_MEASURE = 300,
};

#endif // CONSTANTS_H
