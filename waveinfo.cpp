#include "waveinfo.h"

WaveInfo::WaveInfo(QueryInfo *parent) : QueryInfo(parent)
{

}

WaveInfo::WaveInfo(const WaveInfo& c)
{
    id = c.id;
    dcode = c.dcode;
    mcode = c.mcode;
    pipe = c.pipe;
    rksj = c.rksj;

    info_type = c.info_type;

    sample_data = c.sample_data;
    stype = c.stype;
}

WaveInfo& WaveInfo::operator=(const WaveInfo &c)
{
    id = c.id;
    dcode = c.dcode;
    mcode = c.mcode;
    pipe = c.pipe;
    rksj = c.rksj;

    info_type = c.info_type;

    sample_data = c.sample_data;
    stype = c.stype;

    return *this;
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
