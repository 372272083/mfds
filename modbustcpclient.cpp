#include "modbustcpclient.h"

#include "mainwindow.h"

ModbusTcpClient::ModbusTcpClient(QObject *parent)
{
    connect(this,SIGNAL(readyRead()),this,SLOT(dataReceived()));
    connect(this,SIGNAL(disconnected()),this,SLOT(slotDisconnected()));
}

void ModbusTcpClient::dataReceived()
{
    QByteArray errMsg;
    errMsg.resize(9);

    while(bytesAvailable()>0)
    {
        int length = bytesAvailable();
        //char buf[8192];
        QByteArray buf = read(8192);
        //read(buf,length);

        for(int i=0;i<9;i++)
        {
            errMsg[i] = buf[i];
        }
        errMsg[8] = 0x03;

        length = buf.size();
        if (length > 0)
        {
            if (length < 9)
            {
                write(errMsg);
                continue;
            }
            int len = buf[4] * 256 + buf[5] + 6;
            if(len != length)
            {
                write(errMsg);
                continue;
            }
            int msgType = buf[7];
            QString msg = QString::number(msgType);

            if(0x03 == msgType)
            {
                int address=0,datalen=0;
                address = buf[8] * 256 + buf[9];
                address = address * 2;

                datalen = buf[10] * 256 + buf[11];
                datalen = datalen * 2;
                if(datalen > 256)
                {
                    write(errMsg);
                    continue;
                }

                if(address >= 256)
                {
                    write(errMsg);
                    continue;
                }
                int limitValue = address + datalen;
                if (limitValue > 256)
                {
                    limitValue = 256;
                    datalen = 256 - address;
                }

                QByteArray rMsg;
                rMsg.resize(datalen + 9);

                for(int i=0;i<8;i++)
                {
                    rMsg[i] = buf[i];
                }

                rMsg[4] = (datalen + 3) / 256;
                rMsg[5] = (datalen + 3) % 256;

                rMsg[8] = datalen;
                for(int i=0;i<datalen;i++)
                {
                    rMsg[i+9] = 0;
                }
                for(int i=0;i<datalen;i++)
                {
                    rMsg[i+9] = MainWindow::modbus_buffer[address + i];
                }
                write(rMsg);
            }
            else if(0x10 == msgType)
            {
                int address=0,datalen=0;
                address = buf[8] * 256 + buf[9];
                address = address * 2;

                datalen = buf[10] * 256 + buf[11];
                datalen = datalen * 2;

                if (address >= 180)
                {
                    int limitValue = address + datalen;
                    if (limitValue > 256)
                    {
                        limitValue = 256;
                        datalen = 256 - address;
                    }
                    datalen = (datalen / 2) * 2;
                    for(int i=0;i<datalen;i++)
                    {
                        MainWindow::modbus_buffer[address+i] = buf[i+13];
                    }
                    QByteArray rMsg = buf.mid(0,12);
                    rMsg[4] = 0x00;
                    rMsg[5] = 0x06;

                    write(rMsg);
                }
                else
                {
                    write(errMsg);
                }
            }

            emit updateClients(msg,msg.length());
        }
        //write(errMsg);
    }
}

void ModbusTcpClient::slotDisconnected()
{
    emit disconnected(this->socketDescriptor());
}
