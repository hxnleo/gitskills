/*
 * myudpclient.cpp
 *
 * UDP客户端
 *
 * Created on: 2019年 01月 21日 星期一 14:34:20 CST
 * Author: lixingcong
 */
#include "myudpclient.h"
#include <QThread>

MyUDPClient::MyUDPClient(const QHostAddress& host, quint16 port, qint32 timeout, QObject* parent) :
    QObject(parent),
    udpSocket(new QUdpSocket(this)),
    timer(new QTimer(this)),
    e(new QEventLoop(this))
{
    this->host = host;
    this->port = port;
    this->timeout = timeout;

    this->timer->setSingleShot(true); // 单次定时器

    connect(this->udpSocket, SIGNAL(readyRead()),
            this, SLOT(onDataReadyRead()));

    connect(this->timer, SIGNAL(timeout()),
            this->e, SLOT(quit()));
}

MyUDPClient::~MyUDPClient()
{
    close();
}

bool MyUDPClient::send(const void* data, qint32 len)
{
    isRecvOk = false;
    qint64 bytesWritten;

#if QT_VERSION < 0x050000
    this->udpSocket->connectToHost(host, port);
#endif

    bytesWritten = this->udpSocket->writeDatagram(reinterpret_cast<const char*>(data), len, this->host, this->port);
    //qDebug()<<udpSocket->errorString();//打印出错信息
    return (bytesWritten > -1);
}

bool MyUDPClient::recv(void* data, qint32* len, qint32 buffer_len)
{
    this->timer->start(this->timeout);
    this->e->exec(); // quit when timeout or recv ok

    close(); // timeout or recv ok

    if(this->isRecvOk)
    {
        *len = recv_buffer.size();

        if(buffer_len < *len) // overflow
            return false;

        memcpy(data, recv_buffer.constData(), static_cast<size_t>(*len));
        return true;
    }

    return false; // timeout
}

void MyUDPClient::onDataReadyRead()
{
    //qDebug("recving...");
    this->timer->stop();

    this->recv_buffer.resize(static_cast<int>(this->udpSocket->pendingDatagramSize()));
    this->udpSocket->readDatagram(recv_buffer.data(), recv_buffer.size());

    //qDebug()<<recv_buffer;
    isRecvOk = true;

    this->e->quit(); // break the recv loop
}

void MyUDPClient::close()
{
    this->udpSocket->disconnectFromHost();
}
