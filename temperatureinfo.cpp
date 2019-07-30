#include "temperatureinfo.h"

TemperatureInfo::TemperatureInfo(QObject *parent) : QueryInfo(parent)
{

}

TemperatureInfo::TemperatureInfo(const TemperatureInfo& c)
{
    id = c.id;
    dcode = c.dcode;
    mcode = c.mcode;
    pipe = c.pipe;
    rksj = c.rksj;

    info_type = c.info_type;

    temp = c.temp;
}

TemperatureInfo& TemperatureInfo::operator=(const TemperatureInfo &c)
{
    id = c.id;
    dcode = c.dcode;
    mcode = c.mcode;
    pipe = c.pipe;
    rksj = c.rksj;

    info_type = c.info_type;

    temp = c.temp;
}

QString TemperatureInfo::toString()
{
    QString result;
    result +=  QString::number(temp,10,4) + "#";
    result +=  dcode + "#";
    result +=  mcode + "#";
    result +=  pipe + "#";
    result +=  rksj;

    return result;
}
