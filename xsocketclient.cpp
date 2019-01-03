#include "xsocketclient.h"

XSocketClient::XSocketClient(QString guid) : guid(guid)
{

}

XSocketClient::~XSocketClient()
{
    if(nullptr != m_pClientSocket)
    {
        m_pClientSocket->deleteLater();
    }
}

void XSocketClient::SetAcceptSocket(QTcpSocket *pSocket)
{
    m_pClientSocket = pSocket;
    InitClientSocket(m_pClientSocket);
}

void XSocketClient::ConnectTo(QString ip, quint16 port)
{
    if(nullptr == m_pClientSocket)
    {
        m_pClientSocket = new QTcpSocket(this);
    }
    InitClientSocket(m_pClientSocket);
    m_pClientSocket->connectToHost(QHostAddress(ip), port);
}

void XSocketClient::disConnect()
{
    if(nullptr != m_pClientSocket)
    {
        m_pClientSocket->disconnectFromHost();
        m_pClientSocket->close();
        delete m_pClientSocket;
        m_pClientSocket = nullptr;
    }
}

bool XSocketClient::IsConnected()
{
    bool bre = false;
    if(nullptr != m_pClientSocket)
    {
        bre = (m_pClientSocket->state() == QTcpSocket::ConnectedState);
    }
    return bre;
}

bool XSocketClient::SendData(QByteArray by)
{
    bool bre = false;
    if(nullptr != m_pClientSocket)
    {
        bre = (by.size() == m_pClientSocket->write(by));
    }
    return bre;
}

QTcpSocket *XSocketClient::GetSocket()
{
    return m_pClientSocket;
}

void XSocketClient::OnDataReady()
{
    QTcpSocket *pSocket = qobject_cast<QTcpSocket*>(sender());
    if(nullptr != pSocket)
    {
        QByteArray by = pSocket->readAll();

        int datalen = by.size();
        qDebug() << "receive data lenght: " << datalen;

        emit signalOnReceiveData(by,guid);
        OnData(by);
    }
}

void XSocketClient::OnConnected()
{
    //QTcpSocket *pSocket = qobject_cast<QTcpSocket*>(sender());
    emit signalOnConnected(guid);
}

void XSocketClient::OnDisconnected()
{
    //QTcpSocket *pSocket = qobject_cast<QTcpSocket*>(sender());
    emit signalOnDisconnected(guid);
}

void XSocketClient::InitClientSocket(QTcpSocket *pSocket)
{
    if(nullptr != pSocket)
    {
        connect(pSocket, SIGNAL(readyRead()), SLOT(OnDataReady()));
        connect(pSocket, SIGNAL(connected()), SLOT(OnConnected()));
        connect(pSocket, SIGNAL(disconnected()), SLOT(OnDisconnected()));

        //connect(pSocket, SIGNAL(connected()), SIGNAL(signalOnConnected()));
        //connect(pSocket, SIGNAL(disconnected()), SIGNAL(signalOnDisconnected()));
    }
}
