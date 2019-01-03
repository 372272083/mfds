#include "chargeinfo.h"

ChargeInfo::ChargeInfo(QObject *parent) : QueryInfo(parent)
{

}

QString ChargeInfo::toString()
{
    QString result;
    result +=  QString::number(u,10,4) + "#";
    result +=  QString::number(i,10,4) + "#";
    result +=  QString::number(f,10,4) + "#";
    result +=  QString::number(factor,10,4) + "#";
    result +=  QString::number(p,10,4) + "#";
    result +=  QString::number(q,10,4) + "#";
    result +=  QString::number(s,10,4) + "#";
    result +=  others + "#";
    result +=  pqs + "#";
    result +=  dcode + "#";
    result +=  mcode + "#";
    result +=  pipe + "#";
    result +=  rksj;

    return result;
}
