#ifndef MODBUSTCPCLIENT_H
#define MODBUSTCPCLIENT_H

#include <QTcpSocket>
#include <QObject>

class ModbusTcpClient : public QTcpSocket
{
    Q_OBJECT
public:
    ModbusTcpClient(QObject *parent=0);
signals:
    void updateClients(QString,int);
    void disconnected(int);
protected slots:
    void dataReceived();
    void slotDisconnected();
};

#endif // MODBUSTCPCLIENT_H
