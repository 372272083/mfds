#include "freqinfo.h"

FreqInfo::FreqInfo(QueryInfo *parent) : QueryInfo(parent)
{

}

FreqInfo& FreqInfo::operator=(const FreqInfo &c)
{
    id = c.id;
    dcode = c.dcode;
    mcode = c.mcode;
    pipe = c.pipe;
    rksj = c.rksj;

    info_type = c.info_type;

    sample_freq = c.sample_freq;
    stype = c.stype;

    return *this;
}

FreqInfo::FreqInfo(const FreqInfo& c)
{
    id = c.id;
    dcode = c.dcode;
    mcode = c.mcode;
    pipe = c.pipe;
    rksj = c.rksj;

    info_type = c.info_type;

    sample_freq = c.sample_freq;
    stype = c.stype;
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
