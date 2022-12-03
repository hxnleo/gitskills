/*
 * myudpclient.h
 *
 * UDP客户端
 *
 * Created on: 2019年 01月 21日 星期一 14:34:20 CST
 * Author: lixingcong
 */
#ifndef MYUDPCLIENT_H
#define MYUDPCLIENT_H

#include <QObject>
#include <QMutex>
#include <QTimer>
#include <QEventLoop>

#if QT_VERSION < 0x050000
    #include <QtNetwork/QUdpSocket>
#else
    #include <QtNetwork>
#endif

class MyUDPClient : public QObject
{
    Q_OBJECT
public:
    explicit MyUDPClient(const QHostAddress& host, quint16 port, qint32 timeout, QObject* parent = nullptr);
    ~MyUDPClient();

    // 发送和接收：以下两个函数，均非线程安全，使用时需要在外部做互斥保护
    bool send(const void* data, qint32 len);
    bool recv(void* data, qint32* len, qint32 buffer_len);

signals:

protected slots:
    void onDataReadyRead();

private:
    QUdpSocket* udpSocket;
    QHostAddress host;
    quint16 port;
    qint32 timeout;
    bool isRecvOk; // 是否成功接收
    QTimer* timer;
    QByteArray recv_buffer;
    QEventLoop* e;

private:
    void close();
};

#endif // MYUDPCLIENT_H
