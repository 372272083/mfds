#ifndef CHANNELINFO_H
#define CHANNELINFO_H

#include <QString>

#include "treenodeinfo.h"

class ChannelInfo : public TreeNodeInfo
{
public:
    ChannelInfo();

    int tree_id;
    QString device_code;
    QString channel_code;
    QString motor_code;
    int isactive;
    int channel_type;

    float rms1;
    float rms2;
};

#endif // CHANNELINFO_H
