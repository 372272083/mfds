#include "modbustcpserver.h"

ModbusTCPServer::ModbusTCPServer(QObject *parent,int port)
    :QTcpServer(parent)
{
    listen(QHostAddress::Any,port);
}

void ModbusTCPServer::closeServer()
{
    for(int i=0;i<tcpClientSocketList.count();i++)
    {
        ModbusTcpClient *client = tcpClientSocketList.at(i);
        client->disconnectFromHost();
        client->close();
        delete client;
    }
    this->close();
}

void ModbusTCPServer::incomingConnection(qintptr socketDescriptor)
{
    //第链接一个会新创建一个
    ModbusTcpClient *tcpClientSocket=new ModbusTcpClient(this);
    connect(tcpClientSocket,SIGNAL(updateClients(QString,int)),this,SLOT(updateClients(QString,int)));
    connect(tcpClientSocket,SIGNAL(disconnected(int)),this,SLOT(slotDisconnected(int)));

    tcpClientSocket->setSocketDescriptor(socketDescriptor);

    tcpClientSocketList.append(tcpClientSocket);
}

void ModbusTCPServer::updateClients(QString msg,int length)
{

}

void ModbusTCPServer::slotDisconnected(int descriptor)
{
    for(int i=0;i<tcpClientSocketList.count();i++)
    {
        ModbusTcpClient *item = tcpClientSocketList.at(i);
        if(item->socketDescriptor()==descriptor)
        {
            tcpClientSocketList.removeAt(i);
            //delete item;
            //item = nullptr;
            break;
        }
    }
    return;
}
