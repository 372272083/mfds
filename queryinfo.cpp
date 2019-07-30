#include "queryinfo.h"

QueryInfo::QueryInfo(QObject *parent) : QObject(parent)
{

}

QueryInfo::QueryInfo(const QueryInfo& c)
{
    id = c.id;
    dcode = c.dcode;
    mcode = c.mcode;
    pipe = c.pipe;
    rksj = c.rksj;

    info_type = c.info_type;
}

QueryInfo& QueryInfo::operator=(const QueryInfo &c)
{
    id = c.id;
    dcode = c.dcode;
    mcode = c.mcode;
    pipe = c.pipe;
    rksj = c.rksj;

    info_type = c.info_type;
}

QString QueryInfo::toString()
{
	QString result = "";
	return result;
}
