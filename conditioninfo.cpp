#include "conditioninfo.h"

ConditionInfo::ConditionInfo(QueryInfo *parent) : QueryInfo(parent)
{

}

ConditionInfo::ConditionInfo(const ConditionInfo& c)
{
    id = c.id;
    dcode = c.dcode;
    mcode = c.mcode;
    pipe = c.pipe;
    rksj = c.rksj;

    info_type = c.info_type;

    rotate = c.rotate;
    sample = c.sample;
    interval = c.interval;
}

ConditionInfo& ConditionInfo::operator=(const ConditionInfo& c)
{
    id = c.id;
    dcode = c.dcode;
    mcode = c.mcode;
    pipe = c.pipe;
    rksj = c.rksj;

    info_type = c.info_type;

    rotate = c.rotate;
    sample = c.sample;
    interval = c.interval;

    return *this;
}

QString ConditionInfo::toString()
{
    QString result = "";

    return result;
}
