/*
 * modbusqueue.h
 *
 * 一个经过封装的Modbus队列，支持回调函数
 *
 * Created on: 2019年 01月 21日 星期一 14:34:20 CST
 * Author: lixingcong
 */
#ifndef MODBUSQUEUE_H
#define MODBUSQUEUE_H

#include <QObject>
#include <tr1/functional> // C++11 or upper
#include <QMutex>
#include <QList>
#include "modbusclient.h"

class ModbusQueueWorker;
class ModbusQueue;

// C++中的callback函数与tr1用法：https://gist.github.com/lixingcong/35935f6b3e93a2b7e6cb8787c41fae75

typedef std::tr1::function< void (bool, void*) > MB_CALLBACK_FUNC;

class ModbusQueue : public QObject
{
    Q_OBJECT

    // 友元类
    friend class ModbusQueueWorker;
public:

    enum operation_t        // 对modbus操作类型
    {
        OP_READ_DATA,       // 读固定地址数据
        OP_WRITE_DATA,
        OP_READ_DATA_FIFO,  // 读大段数据
        OP_WRITE_DATA_FIFO,
        OP_READ_FILE,       // 从远端读取文件
        OP_WRITE_FILE,
        OP_MAXIS
    };

    struct element_t   // 对modbus操作的队列元素
    {
        // ------- basic read/write flags ---------
        operation_t operation; // modbus操作
        quint16 startAddress; // 起始地址
        void* data; // 数据（指针），modbus操作为写的时候，会直接修改指针指向的Buffer
        size_t len; // 长度，单位：字节

        // ------- other flags ---------------------
        void* pointerObject;       // (仅用于异步发送)其他指针，用于传递给回调函数
        MB_CALLBACK_FUNC
        callback; // (仅用于异步发送)回调函数，当modbus操作完成时，执行回调，callback函数不能占用太多的CPU时间

        // ------- mixed params --------------------
        union
        {
            struct
            {
                char* remote; // 仅用于传输文件
                QString* local; // 仅用于传输文件
            } file;

            struct
            {
                qint32 code;   // (仅用于读/写大段数据FIFO)
                qint32 offset; // (仅用于读/写大段数据FIFO)
            } fifo;
        } mixParam;
    };

public:
    explicit ModbusQueue(QObject* parent = nullptr);
    ~ModbusQueue();

    bool init(const QString& host, quint32 timeout, quint32 retry_times, quint8 mb_ID = MODBUS_UDP_ID);
    bool sendQuery(element_t* element, bool isNonBlocking = true, bool isPushBack = true);

    void queryResume(); // 恢复队列
    void queryPause(); // 暂停队列
    void queryStopThenOffline(); // 清除队列中所有查询请求 离线模式（无法切回在线模式）

    bool isOffline()const
    {
        return offlineFlag;
    }

#ifdef QT_DEBUG
    void setQueueSleepInterval(int interval)
    {
        emit _setQueueSleepInterval(interval);
    }
signals:
    void _setQueueSleepInterval(int);
#endif

signals:
    void notifyOffline();
    void notifyFifoProgress(int percent);
    void notifyFileProgress(int percent);
    void _queryStop();
    void _queryResume();
    void _queryPause();
    void _push_back(ModbusQueue::element_t* element);
    void _push_front(ModbusQueue::element_t* element);

protected slots:
    void setWorkerFlagStarted();
    void setWorkerFlagStopped();
    void _onUpdateFifoProgress(int percent);
    void _onUpdateFileProgress(int percent);

protected: // structures
    struct queryBlocking_t
    {
        bool isOk;
        QMutex mutex;
    };

    enum workstep_t
    {
        WS_QUEUE_STOPPED,
        WS_QUEUE_STARTING,
        WS_QUEUE_STARTED,
    };

protected: // members
    // 发送队列任务
    QThread* modbusQueueWorkerThread;
    ModbusQueueWorker* modbusQueueWorker;
    workstep_t modbusQueueWorkstep;
    bool offlineFlag;

protected: // functions
    void stopWorker();

    bool sendQueryBlocking(element_t* element, bool isPushBack); // 阻塞发送
    void callbackSendBlocking(bool isOk, void* flag); // 阻塞发送callback
};

class ModbusQueueWorker : public QObject
{
    Q_OBJECT

public:
    explicit ModbusQueueWorker(ModbusQueue* mbq, QObject* parent = nullptr);
    ~ModbusQueueWorker();
    void setInitParams(const QString& host, quint32 timeout, quint32 retry_times, quint8 mb_id);
signals:
    void doWorkExit();
    void doWorkStart();

public slots:
    void doWork();
    void push_back(ModbusQueue::element_t* element);
    void push_front(ModbusQueue::element_t* element);
    void queryStop(); // 停止队列
    void queryResume(); // 恢复队列
    void queryPause(); // 暂停队列
    void queryClear(); // 清除队列中所有查询请求
    void setSleepInterval(int);

private: // members
    enum workstep_t
    {
        WS_WORKER_STOPPED,
        WS_WORKER_RUNNING,
        WS_WORKER_PAUSED
    };

    workstep_t workstep;
    ModbusQueue* mbq;
    ModbusClient* modbusClient;
    QString host;
    quint32 timeout;
    quint32 retry_times;
    quint8 mb_id;
    qint32 sleepInterval;

    QMutex mutexQueue; // 队列读写锁
    QList<ModbusQueue::element_t*> list;

private: // functions
    void setWorkstep(workstep_t workstep);
};

#endif // MODBUSQUEUE_H
