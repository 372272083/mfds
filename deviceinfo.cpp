#include "deviceinfo.h"

#include <QDateTime>
#include <QDebug>

DeviceInfo::DeviceInfo(TreeNodeInfo *parent) : linkState(false)
{
    isReceiving = false;
    waitingCount = 0;
    reConnectCount = 1;
}

void DeviceInfo::init()
{
    isReceiving = false;
    expDataLen = 0;
    rec_buffer.clear();
    rec_buffer.squeeze();
    msgSendQueue.clear();
    msgPriSendQueue.clear();
}

void DeviceInfo::close()
{

}

void DeviceInfo::handlerReceiveMsg()
{
    rec_buffer.clear();
    rec_buffer.squeeze();
}

void DeviceInfo::handleSendMsg()
{

}

QString DeviceInfo::getParam(int type)
{
    return "";
}

QString DeviceInfo::getComAddress()
{
    QString address="";
    return address;
}

QByteArray DeviceInfo::getSendMsg()
{
    if (isReceiving)
    {
        //qDebug()<<"Send Msg Receiving: " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        QByteArray ba;
        return ba;
    }

    //qDebug()<<"Send Msg Length: " << QString::number(msgPriSendQueue.count());
    isReceiving = true;
    struct ModbusTCPMapInfo cmd;
    if (msgPriSendQueue.count() > 0)
    {
        //qDebug()<< "pri before";
        cmd = msgPriSendQueue.dequeue();
        //qDebug()<< "pri after";
    }
    else if (msgSendQueue.count() > 0)
    {
        //qDebug()<< "normal before";
        cmd = msgSendQueue.dequeue();
        //qDebug()<< "normal after";
    }
    else
    {
        QByteArray ba;
        isReceiving = false;
        return ba;
    }

    expDataLen = cmd.ExpectLen;

    if (cmd.Command == 0x3 || cmd.Command == 0x4 || cmd.Command == 0x10)
    {
        if (cmd.Unit == TIME_SYNC_W)
        {
            QDateTime dt = QDateTime::currentDateTime();
            unsigned int seconds = dt.toTime_t();
            unsigned int time_h = seconds / 0x10000;
            unsigned int time_l = seconds % 0x10000;
            cmd.data[0] = time_l / 0xFF;
            cmd.data[1] = time_l % 0xFF;
            cmd.data[2] = time_h / 0xFF;
            cmd.data[3] = time_h % 0xFF;
        }
        QByteArray ba = modbusEncomposeMsg(cmd);
        if (cmd.Command == 0x10)
        {
            if (nullptr != cmd.data)
            {
                delete cmd.data;
            }
        }
        //qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss") << ": Device: " << deviceCode << " Send MSG: " << QString::number((int)cmd.Unit);
        //isReceiving = true;
        return ba;
    }
    else if (cmd.Command == 0x0)
    {
        //qDebug() << "command 0x0!";
        QByteArray ba;
        ba.resize(cmd.Length);
        for(int i=0;i<cmd.Length;i++)
        {
            ba[i] = cmd.data[i];
        }
        //qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss") << ": Device: " << deviceCode << " Send MSG: " << QString::number((int)cmd.Unit);
        return ba;
    }
    else
    {
        QByteArray ba;
        isReceiving = false;
        return ba;
    }
}

void DeviceInfo::addPipe(QString name,QString mcode,QString dcode,QString dname,QString dtype,QString pipecode)
{
    /*
    ChannelInfo *dpi = new ChannelInfo();
    dpi->setPipeName(name);
    dpi->setDCode(dcode);
    dpi->setDName(dname);
    dpi->setDType(dtype);
    dpi->setMCode(mcode);
    dpi->setPipeCode(pipecode);
    pipes.append(dpi);
    */
}

void DeviceInfo::addSendMsg(struct ModbusTCPMapInfo msg)
{
    msgPriSendQueue.append(msg);
}

void DeviceInfo::addrevdata(QByteArray recv)
{
    int slen = recv.size();
    for(int i=0;i<slen;i++)
    {
        rec_buffer.append(recv[i]);
    }
}

bool DeviceInfo::isExpLen()
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
        if (len > 0 || len <= expDataLen)
        {
            return true;
        }
    }

    return false;
}

int DeviceInfo::bufferToint(QByteArray value,BYTEORDER4 bo)
{
    int result = 0;
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
    case HL_HH_LL_LH:
        tmp[0] = value[2];
        tmp[1] = value[3];
        tmp[2] = value[0];
        tmp[3] = value[1];
        break;
    case LH_LL_HH_HL:
        tmp[0] = value[1];
        tmp[1] = value[0];
        tmp[2] = value[3];
        tmp[3] = value[2];
        break;
    default:
        break;
    }

    memcpy(&result,tmp,4);

    return result;
}

float DeviceInfo::bufferTofloat(QByteArray value,BYTEORDER4 bo)
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
    case HL_HH_LL_LH:
        tmp[0] = value[2];
        tmp[1] = value[3];
        tmp[2] = value[0];
        tmp[3] = value[1];
        break;
    case LH_LL_HH_HL:
        tmp[0] = value[1];
        tmp[1] = value[0];
        tmp[2] = value[3];
        tmp[3] = value[2];
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

ushort DeviceInfo::bufferToUShort(QByteArray value,BYTEORDER2 bo)
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

QByteArray DeviceInfo::floatToBuffer(float value,BYTEORDER4 bo)
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

QByteArray DeviceInfo::ushortToBuffer(ushort value,BYTEORDER2 bo)
{
    QByteArray ba;
    return ba;
}

QByteArray DeviceInfo::modbusEncomposeMsg(ModbusTCPMapInfo mtInfo)
{
    if (mtInfo.Command == 0x3 || mtInfo.Command == 0x4) //modbus read
    {
        QByteArray ba;
        ba.resize(12);
        ba[0] = 0;
        ba[1] = mtInfo.Num;
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

bool DeviceInfo::isHaveSomeState(int queuetype,MSGAddr type)
{
    QQueue<ModbusTCPMapInfo>::ConstIterator it;
    if(queuetype == 0)
    {
        for(it=msgSendQueue.constBegin();it!=msgSendQueue.constEnd();it++)
        {
            ModbusTCPMapInfo mti = *it;
            if(mti.Unit == type)
            {
                return true;
            }
        }
    }
    else
    {
        for(it=msgPriSendQueue.constBegin();it!=msgPriSendQueue.constEnd();it++)
        {
            ModbusTCPMapInfo mti = *it;
            if(mti.Unit == type)
            {
                return true;
            }
        }
    }

    return false;
}

DeviceInfo::~DeviceInfo()
{
    int len = pipes.count();
    for(int i=0;i<len;i++)
    {
        delete pipes.at(i);
    }
    pipes.clear();
}
