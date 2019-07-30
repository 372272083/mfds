#include "vibrateinfo.h"

VibrateInfo::VibrateInfo(QObject *parent) : QueryInfo(parent)
{

}

VibrateInfo::VibrateInfo(const VibrateInfo& c)
{
    id = c.id;
    dcode = c.dcode;
    mcode = c.mcode;
    pipe = c.pipe;
    rksj = c.rksj;

    info_type = c.info_type;

    vibrate_e = c.vibrate_e;
    speed_e = c.speed_e;
    position_e = c.position_e;
}

VibrateInfo& VibrateInfo::operator=(const VibrateInfo &c)
{
    id = c.id;
    dcode = c.dcode;
    mcode = c.mcode;
    pipe = c.pipe;
    rksj = c.rksj;

    info_type = c.info_type;

    vibrate_e = c.vibrate_e;
    speed_e = c.speed_e;
    position_e = c.position_e;

    return *this;
}

QString VibrateInfo::toString()
{
    QString result;
    result +=  QString::number(vibrate_e,10,4) + "#";
    result +=  QString::number(speed_e,10,4) + "#";
    result +=  dcode + "#";
    result +=  mcode + "#";
    result +=  pipe + "#";
    result +=  rksj;

    return result;
}
