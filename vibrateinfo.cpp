#include "vibrateinfo.h"

VibrateInfo::VibrateInfo(QObject *parent) : QueryInfo(parent)
{

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
