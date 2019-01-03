#include "temperatureinfo.h"

TemperatureInfo::TemperatureInfo(QObject *parent) : QueryInfo(parent)
{

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
