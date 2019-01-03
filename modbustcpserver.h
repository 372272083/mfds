#ifndef MODBUSTCPSERVER_H
#define MODBUSTCPSERVER_H

#include <QTcpServer>
#include <QObject>
#include "modbustcpclient.h"

class ModbusTCPServer: public QTcpServer
{
    Q_OBJECT
public:
    ModbusTCPServer(QObject *parent=0,int port=0);
    QList<ModbusTcpClient*> tcpClientSocketList;
    void closeServer();
signals:
    void updateServer(QString,int);
    void updateClientList();
public slots:
    void updateClients(QString,int);
    void slotDisconnected(int);
protected:
//    void incomingConnection(int socketDescriptor);
    void incomingConnection(qintptr socketDescriptor);
};

#endif // MODBUSTCPSERVER_H
