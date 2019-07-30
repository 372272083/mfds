#include "faultinfo.h"

FaultInfo::FaultInfo(QObject *parent) : QObject(parent)
{

}

FaultInfo::FaultInfo(const FaultInfo& fi)
{
    dcode = fi.dcode;
    channel = fi.channel;
    fault_detail_thd = fi.fault_detail_thd;
    fault_detail_cur = fi.fault_detail_cur;
    fault_detail_cof = fi.fault_detail_cof;
    fault_detail_trigger = fi.fault_detail_trigger;

    QVector<int>::ConstIterator it;
    fault_codes.clear();
    for(it=fi.fault_codes.constBegin();it!=fi.fault_codes.constEnd();it++)
    {
        fault_codes.append(*it);
    }
}

FaultInfo& FaultInfo::operator =(const FaultInfo& fi)
{
    this->dcode = fi.dcode;
    this->channel = fi.channel;
    fault_detail_thd = fi.fault_detail_thd;
    fault_detail_cur = fi.fault_detail_cur;
    fault_detail_cof = fi.fault_detail_cof;
    fault_detail_trigger = fi.fault_detail_trigger;

    QVector<int>::ConstIterator it;
    this->fault_codes.clear();
    for(it=fi.fault_codes.constBegin();it!=fi.fault_codes.constEnd();it++)
    {
        this->fault_codes.append(*it);
    }
    return *this;
}

QString FaultInfo::toString()
{
    QString result = dcode.trimmed() + "-" + channel.trimmed();
    return result;
}
