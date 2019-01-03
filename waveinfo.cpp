#include "waveinfo.h"

WaveInfo::WaveInfo(QObject *parent) : QueryInfo(parent)
{

}

QString WaveInfo::toString()
{
    QString result;
    result +=  sample_data + "#";
    result +=  QString::number(stype) + "#";
    result +=  dcode + "#";
    result +=  mcode + "#";
    result +=  pipe + "#";
    result +=  rksj;

    return result;
}
