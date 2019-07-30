#include "devicelinkinfo.h"

DeviceLinkInfo::DeviceLinkInfo(QObject *parent) : QObject(parent),linkState(false)
{
    isReceiving = false;
    waitingCount = 0;
    reConnectCount = 1;
}

void DeviceLinkInfo::init()
{
    isReceiving = false;
    expDataLen = 0;
    rec_buffer.clear();
    msgSendQueue.clear();
    msgPriSendQueue.clear();
}

void DeviceLinkInfo::handlerReceiveMsg()
{
    rec_buffer.clear();
}

void DeviceLinkInfo::handleSendMsg()
{

}

QString DeviceLinkInfo::getParam(int type)
{
    return "";
}

QString DeviceLinkInfo::getComAddress()
{
    QString address="";
    return address;
}

QByteArray DeviceLinkInfo::getSendMsg()
{
    QByteArray ba;
    return ba;
}

void DeviceLinkInfo::addPipe(QString name,QString mcode,QString dcode,QString dname,QString dtype,QString pipecode)
{
    DevicePipeInfo *dpi = new DevicePipeInfo();
    dpi->setPipeName(name);
    dpi->setDCode(dcode);
    dpi->setDName(dname);
    dpi->setDType(dtype);
    dpi->setMCode(mcode);
    dpi->setPipeCode(pipecode);
    pipes.append(dpi);
}

void DeviceLinkInfo::addSendMsg(struct ModbusTCPMapInfo msg)
{
    msgPriSendQueue.append(msg);
}

void DeviceLinkInfo::addrevdata(QByteArray recv)
{
    int slen = recv.size();
    for(int i=0;i<slen;i++)
    {
        rec_buffer.append(recv[i]);
    }
}

bool DeviceLinkInfo::isExpLen()
{
    int len = rec_buffer.size();
    if (expDataLen > 1024)
    {
        if (6 == len) //recording wave
        {
            return true;
        }
        else
        {
            if (len < expDataLen)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
    }
    else
    {
        if (len > 0 && len <= expDataLen)
        {
            return true;
        }
    }
}

float DeviceLinkInfo::bufferTofloat(QByteArray value,BYTEORDER4 bo)
{
    float result = 0;
    if(value.size() < 4)
    {
        return result;
    }
    QByteArray tmp;
    tmp.resize(4);

    switch (bo) {
    case LL_LH_HL_HH:
        tmp[0] = value[0];
        tmp[1] = value[1];
        tmp[2] = value[2];
        tmp[3] = value[3];
        break;
    case HH_HL_LH_LL:
        tmp[0] = value[3];
        tmp[1] = value[2];
        tmp[2] = value[1];
        tmp[3] = value[0];
        break;
    default:
        break;
    }

    memcpy(&result,tmp,4);

    if (!(qIsFinite(result) && !qIsNaN(result)))
    {
        result = 0;
    }

    return result;
}

ushort DeviceLinkInfo::bufferToUShort(QByteArray value,BYTEORDER2 bo)
{
    QByteArray tmp;
    tmp.resize(2);

    switch (bo) {
    case LL_HH:
        tmp[0] = value[0];
        tmp[1] = value[1];
        break;
    case HH_LL:
        tmp[0] = value[1];
        tmp[1] = value[0];
        break;
    default:
        break;
    }

    ushort comdata;
    memcpy(&comdata,tmp,2);

    return comdata;
}

QByteArray DeviceLinkInfo::floatToBuffer(float value,BYTEORDER4 bo)
{
    float tmp = value;
    QByteArray tmpArr;
    int len_tmpVar = sizeof(tmp);
    tmpArr.resize(len_tmpVar);
    memcpy(tmpArr.data(), &tmp, len_tmpVar);

    QByteArray result;
    result.resize(4);

    switch (bo) {
    case LL_LH_HL_HH:
        result[0] = tmpArr[0];
        result[1] = tmpArr[1];
        result[2] = tmpArr[2];
        result[3] = tmpArr[3];
        break;
    case HH_HL_LH_LL:
        result[0] = tmpArr[3];
        result[1] = tmpArr[2];
        result[2] = tmpArr[1];
        result[3] = tmpArr[0];
        break;
    default:
        break;
    }

    return result;
}

QByteArray DeviceLinkInfo::ushortToBuffer(ushort value,BYTEORDER2 bo)
{
	QByteArray ba;
	return ba;
}

QByteArray DeviceLinkInfo::modbusEncomposeMsg(ModbusTCPMapInfo mtInfo)
{
    if (mtInfo.Command == 0x3) //modbus read
    {
        QByteArray ba;
        ba.resize(12);
        ba[0] = 0;
        ba[1] = 0;
        ba[2] = 0;
        ba[3] = 0;
        ba[4] = 0;
        ba[5] = 6;
        ba[6] = mtInfo.Unit;
        ba[7] = mtInfo.Command;
        ba[8] = mtInfo.Addr / 256;
        ba[9] = mtInfo.Addr % 256;
        ba[10] = mtInfo.Length / 256;
        ba[11] = mtInfo.Length % 256;

        return ba;
    }
    else if (mtInfo.Command == 0x10) //modbus write
    {
        int dataLen = mtInfo.Length;
        int regNum = dataLen / 2;
        if (dataLen > 0)
        {
            QByteArray ba;
            ba.resize(13 + dataLen);
            ba[0] = 0;
            ba[1] = 0;
            ba[2] = 0;
            ba[3] = 0;
            ba[4] = 0;
            ba[5] = dataLen + 7;
            ba[6] = mtInfo.Unit;
            ba[7] = mtInfo.Command;
            ba[8] = mtInfo.Addr / 256;
            ba[9] = mtInfo.Addr % 256;
            ba[10] = regNum / 256;
            ba[11] = regNum % 256;
            ba[12] = dataLen;
            for(int i=0;i<dataLen;i++)
            {
                ba[13+i] = mtInfo.data[i];
            }
            return ba;
        }
    }

    QByteArray ba;
    return ba;
}

DeviceLinkInfo::~DeviceLinkInfo()
{
    int len = pipes.count();
    for(int i=0;i<len;i++)
    {
        delete pipes.at(i);
    }
    pipes.clear();
}
