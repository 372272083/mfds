#include "chargeinfo.h"

ChargeInfo::ChargeInfo(QueryInfo *parent) : QueryInfo(parent)
{

}

ChargeInfo::ChargeInfo(const ChargeInfo& c)
{
    id = c.id;
    dcode = c.dcode;
    mcode = c.mcode;
    pipe = c.pipe;
    rksj = c.rksj;

    info_type = c.info_type;

    u = c.u;
    i = c.i;
    f = c.f;
    factor = c.factor;
    p = c.p;
    q = c.q;
    s = c.s;
    others = c.others;
    pqs = c.pqs;
}

ChargeInfo& ChargeInfo::operator=(const ChargeInfo &c)
{
    id = c.id;
    dcode = c.dcode;
    mcode = c.mcode;
    pipe = c.pipe;
    rksj = c.rksj;

    info_type = c.info_type;

    u = c.u;
    i = c.i;
    f = c.f;
    factor = c.factor;
    p = c.p;
    q = c.q;
    s = c.s;
    others = c.others;
    pqs = c.pqs;

    return *this;
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
