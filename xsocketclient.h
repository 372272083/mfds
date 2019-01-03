#ifndef XSOCKETCLIENT_H
#define XSOCKETCLIENT_H


#include <QTcpSocket>
#include <QTcpServer>

class XSocketClient : public QObject
{
    Q_OBJECT
public:
    XSocketClient(QString guid);
    ~XSocketClient();

    ///做为服务端接受连接时，设置接受的socket
    void SetAcceptSocket(QTcpSocket *pSocket);

    ///做为客户端主动连接时使用
    void ConnectTo(QString ip, quint16 port);
    void disConnect();

    bool IsConnected();

    bool SendData(QByteArray by);
    QTcpSocket *GetSocket();

signals:
    void signalOnReceiveData(QByteArray by,QString guid);
    void signalOnConnected(QString guid);
    void signalOnDisconnected(QString guid);

protected slots:
    virtual void OnData(QByteArray by){}
    virtual void OnConnected();
    virtual void OnDisconnected();

    void OnDataReady();

private:
    void InitClientSocket(QTcpSocket *pSocket);

private:
    QString guid;
    QTcpSocket *m_pClientSocket = nullptr;

};

#endif // XSOCKETCLIENT_H
