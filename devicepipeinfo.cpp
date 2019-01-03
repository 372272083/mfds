#include "devicepipeinfo.h"

DevicePipeInfo::DevicePipeInfo(QObject *parent) : QObject(parent)
{

}

void DevicePipeInfo::setMCode(QString mcode)
{
    this->mcode = mcode;
}

void DevicePipeInfo::setDCode(QString dcode)
{
    this->dcode = dcode;
}

void DevicePipeInfo::setDName(QString dname)
{
    this->dname = dname;
}

void DevicePipeInfo::setDType(QString dtype)
{
    this->dtype = dtype;
}

void DevicePipeInfo::setPipeCode(QString pipecode)
{
    this->pipecode = pipecode;
}

void DevicePipeInfo::setPipeName(QString name)
{
    this->pipename = name;
}

QString DevicePipeInfo::getMCode()
{
    return mcode;
}

QString DevicePipeInfo::getDCode()
{
    return dcode;
}

QString DevicePipeInfo::getDName()
{
    return dname;
}

QString DevicePipeInfo::getDType()
{
    return dtype;
}

QString DevicePipeInfo::getPipeCode()
{
    return pipecode;
}

QString DevicePipeInfo::getPipeName()
{
    return pipename;
}
