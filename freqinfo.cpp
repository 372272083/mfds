#include "freqinfo.h"

FreqInfo::FreqInfo(QObject *parent) : QueryInfo(parent)
{

}

QString FreqInfo::toString()
{
    QString result;
    result +=  sample_freq + "#";
    result +=  QString::number(stype) + "#";
    result +=  dcode + "#";
    result +=  mcode + "#";
    result +=  pipe + "#";
    result +=  rksj;

    return result;
}
