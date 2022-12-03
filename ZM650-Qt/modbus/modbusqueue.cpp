/*
 * modbusqueue.cpp
 *
 * 一个经过封装的Modbus队列，支持回调函数
 *
 * Created on: 2019年 01月 21日 星期一 14:34:20 CST
 * Author: lixingcong
 */
#include "modbusqueue.h"
#include "data/utils.h"
#include "data/globaldata.h"
#include <QMutexLocker>
#include <QThread>

#define MOBUS_THREAD_SLEEP_INTERVAL 5

Q_DECLARE_METATYPE(ModbusQueue::element_t)

ModbusQueue::ModbusQueue(QObject* parent) : QObject(parent),
    modbusQueueWorkerThread(nullptr),
    modbusQueueWorker(nullptr),
    modbusQueueWorkstep(WS_QUEUE_STOPPED),
    offlineFlag(false)
{
}

ModbusQueue::~ModbusQueue()
{
    //debug_printf("ModbusQueue::~ModbusQueue()");
    stopWorker();
}

bool ModbusQueue::init(const QString& host, quint32 timeout, quint32 retry_times, quint8 mb_ID)
{
    if(WS_QUEUE_STARTED == modbusQueueWorkstep)
    {
        stopWorker();
    }

    // run the worker thread
    modbusQueueWorkerThread = new QThread();
    modbusQueueWorker = new ModbusQueueWorker(this);
    modbusQueueWorker->setInitParams(host, timeout, retry_times, mb_ID);
    modbusQueueWorker->moveToThread(modbusQueueWorkerThread);

    // start and exit
    QObject::connect(modbusQueueWorkerThread, SIGNAL(started()),
                     modbusQueueWorker, SLOT(doWork()));
    QObject::connect(modbusQueueWorker, SIGNAL(doWorkExit()),
                     modbusQueueWorkerThread, SLOT(quit()));

    // stop the worker manually
    QObject::connect(this, SIGNAL(_queryStop()),
                     modbusQueueWorker, SLOT(queryStop()));

    // automatically delete objects(worker and thread, in order) when work is done:
    QObject::connect(modbusQueueWorkerThread, SIGNAL(finished()),
                     modbusQueueWorker, SLOT(deleteLater()));
    QObject::connect(modbusQueueWorker, SIGNAL(destroyed()),
                     modbusQueueWorkerThread, SLOT(deleteLater()));

    // notify the thread was deleted
    QObject::connect(modbusQueueWorkerThread, SIGNAL(destroyed()),
                     this, SLOT(setWorkerFlagStopped()));

    // notify the worker has started doing his own work
    QObject::connect(modbusQueueWorker, SIGNAL(doWorkStart()),
                     this, SLOT(setWorkerFlagStarted()));

    // API signal/slot binding
    QObject::connect(this, SIGNAL(_push_back(ModbusQueue::element_t*)),
                     modbusQueueWorker, SLOT(push_back(ModbusQueue::element_t*)));
    QObject::connect(this, SIGNAL(_push_front(ModbusQueue::element_t*)),
                     modbusQueueWorker, SLOT(push_front(ModbusQueue::element_t*)));
    QObject::connect(this, SIGNAL(_queryResume()),
                     modbusQueueWorker, SLOT(queryResume()));
    QObject::connect(this, SIGNAL(_queryPause()),
                     modbusQueueWorker, SLOT(queryPause()));

#ifdef QT_DEBUG
    QObject::connect(this, SIGNAL(_setQueueSleepInterval(int)),
                     modbusQueueWorker, SLOT(setSleepInterval(int)));
#endif

    // run the thread right now!
    modbusQueueWorkerThread->start();

    // wait for the worker started
    while(WS_QUEUE_STARTED != modbusQueueWorkstep)
    {
        Utils::sleep(MOBUS_THREAD_SLEEP_INTERVAL);
    }

    //debug_printf("worker is started");

    // TODO: init for serial COM
    // TODO: fix protential problem: crash when re-initialize it
    //modbusClient->init(host, timeout, retry_times);

    return true;
}

bool ModbusQueue::sendQuery(element_t* element, bool isNonBlocking, bool isPushBack)
{//默认  X  ture ture
    // it always fails when under the offline mode
    //qDebug()<<"offlineFlag"<<offlineFlag;
    if(true == offlineFlag)
    {
        if(true == isNonBlocking)
        {
            element->callback(false, element);
        }
        else
        {
            return false;
        }
    }

    // in-queue
    if(true == isNonBlocking)
    {
        if(isPushBack)
            emit _push_back(element);
        else
            emit _push_front(element);

        return true;
    }

    return sendQueryBlocking(element, isPushBack);
}

void ModbusQueue::queryResume()
{
    emit _queryResume();
}

void ModbusQueue::queryPause()
{
    emit _queryPause();
}

void ModbusQueue::queryStopThenOffline()
{
    offlineFlag = true;
    GlobalData::online = false;
    stopWorker();
    emit notifyOffline();//设置闪烁
}

void ModbusQueue::setWorkerFlagStarted()
{
    modbusQueueWorkstep = WS_QUEUE_STARTED;
}

void ModbusQueue::setWorkerFlagStopped()
{
    modbusQueueWorkstep = WS_QUEUE_STOPPED;
}

void ModbusQueue::_onUpdateFifoProgress(int percent)
{
    emit notifyFifoProgress(percent);
}

void ModbusQueue::_onUpdateFileProgress(int percent)
{
    emit notifyFileProgress(percent);
}

void ModbusQueue::callbackSendBlocking(bool isOk, void* flag)
{
    //debug_printf("ModbusQueue::callbackSendBlocking");

    element_t* element = reinterpret_cast<element_t*>(flag);
    queryBlocking_t* queryBlocking = reinterpret_cast<queryBlocking_t*>(element->pointerObject);
    queryBlocking->isOk = isOk;
    queryBlocking->mutex.unlock();
}

void ModbusQueue::stopWorker()
{
    if(WS_QUEUE_STOPPED == modbusQueueWorkstep) // 没有在运行
        return;

    // send signal to stop the worker
    emit _queryStop();

    //debug_printf("waiting for worker to stop");
    while(WS_QUEUE_STOPPED != modbusQueueWorkstep)
    {
        Utils::sleep(MOBUS_THREAD_SLEEP_INTERVAL);
    }

    //debug_printf("waiting for worker to ::stopped");

    // worker was deleted(auto deleted by eventloop DELETE_LATER)
    modbusQueueWorker = nullptr;
    modbusQueueWorkerThread = nullptr;
}

bool ModbusQueue::sendQueryBlocking(ModbusQueue::element_t* element, bool isPushBack)
{
    bool isOk = false;

    //qDebug()<<"modbusQueueWorkstep"<<modbusQueueWorkstep;
    if(WS_QUEUE_STOPPED == modbusQueueWorkstep)
        return false;

    queryBlocking_t* queryBlocking = new queryBlocking_t;
    queryBlocking->mutex.lock(); // 上锁，直到callback将其解锁

    element->pointerObject = queryBlocking;
    element->callback = std::tr1::bind(&ModbusQueue::callbackSendBlocking, this,
                                       std::tr1::placeholders::_1,
                                       std::tr1::placeholders::_2);

    // 放入发送队列
    if(isPushBack)
        emit _push_back(element);
    else
        emit _push_front(element);

    //debug_printf("ModbusQueue::sendQueryBlocking: waiting for unlock");
    queryBlocking->mutex.lock(); // 阻塞，直到callback被调用，此处就能执行下一步
    //debug_printf("ModbusQueue::sendQueryBlocking: unlocked");
    queryBlocking->mutex.unlock();

    isOk = queryBlocking->isOk; // 最后结果

    Utils::delete_pointer(queryBlocking);

    return isOk;
}



// --------------------------------------
// ModbusQueueWorker
// --------------------------------------
ModbusQueueWorker::ModbusQueueWorker(ModbusQueue* mbq, QObject* parent):
    QObject(parent),
    workstep(WS_WORKER_STOPPED),
    mbq(mbq),
    sleepInterval(MOBUS_THREAD_SLEEP_INTERVAL)
{

}

ModbusQueueWorker::~ModbusQueueWorker()
{
    //debug_printf("~ModbusQueueWorker()");
}

void ModbusQueueWorker::setInitParams(const QString& host, quint32 timeout, quint32 retry_times, quint8 mb_id)
{
    this->host = host;
    this->timeout = timeout;
    this->retry_times = retry_times;
    this->mb_id = mb_id;
}

void ModbusQueueWorker::doWork()
{
    workstep = WS_WORKER_RUNNING;

    //debug_printf("ModbusQueueWorker::doWork(), running...");

    ModbusQueue::element_t* element;
    bool isOk = false;

    modbusClient = new ModbusClient;
    modbusClient->init(this->host, this->timeout, this->retry_times);

    // progress percent update
    QObject::connect(modbusClient, SIGNAL(updateFifoProgress(int)),
                     mbq, SLOT(_onUpdateFifoProgress(int)));
    QObject::connect(modbusClient, SIGNAL(updateFileProgress(int)),
                     mbq, SLOT(_onUpdateFileProgress(int)));

    emit doWorkStart();

    while(1)
    {
        if(WS_WORKER_STOPPED == workstep)
            break;

        if(WS_WORKER_RUNNING == workstep && true == mutexQueue.tryLock())
        {
            if(false == list.empty())
            {
                element = list[0];
                list.pop_front();
                mutexQueue.unlock();
            }
            else
            {
                mutexQueue.unlock();
                Utils::sleep(sleepInterval);
                continue; // while(1)
            }

            // modbus通讯

            switch(element->operation)
            {
            case ModbusQueue::OP_READ_DATA:
                isOk = modbusClient->readData(mb_id, element->startAddress, element->data, static_cast<quint16>(element->len),
                                              (POLL_RW | POLL_RTU | POLL_WORD));
//                if(element->startAddress==3005)
//                {
//                    qDebug()<<"isOk"<<isOk;
//                }
                break;

            case ModbusQueue::OP_WRITE_DATA:
                isOk = modbusClient->writeData(mb_id, element->startAddress, element->data, static_cast<quint16>(element->len),
                                               (POLL_RW | POLL_RTU | POLL_WORD));
                break;

            case ModbusQueue::OP_READ_DATA_FIFO:
                isOk = modbusClient->readDataFifo(mb_id, element->mixParam.fifo.code, element->data, element->mixParam.fifo.offset,
                                                  static_cast<int>(element->len),
                                                  (POLL_RW | POLL_RTU | POLL_WORD));
                break;

            case ModbusQueue::OP_WRITE_DATA_FIFO:
                isOk = modbusClient->writeDataFifo(mb_id, element->mixParam.fifo.code, element->data, element->mixParam.fifo.offset,
                                                   static_cast<int>(element->len),
                                                   (POLL_RW | POLL_RTU | POLL_WORD));
                break;

            case ModbusQueue::OP_READ_FILE:
//                qDebug()<<"mb_id"<<mb_id<<element->mixParam.file.remote<<element->mixParam.file.remote;
                isOk = modbusClient->readFile(mb_id, element->mixParam.file.remote, *element->mixParam.file.local);
                break;

            case ModbusQueue::OP_WRITE_FILE:
//                qDebug()<<"mb_id"<<mb_id<<element->mixParam.file.remote<<element->mixParam.file.remote;
                isOk = modbusClient->writeFile(mb_id, element->mixParam.file.remote, *element->mixParam.file.local);
                break;

            default:
                isOk = false;
                break;
            }

            // 通信完毕，回调函数，isOk指示着是否成功
            (element->callback)(isOk, element);
        }

        Utils::sleep(sleepInterval);
    }

    queryClear(); // callback false

    Utils::delete_pointer(modbusClient);

    emit doWorkExit();
}

void ModbusQueueWorker::setWorkstep(workstep_t workstep)
{
    //debug_printf("ModbusQueueWorker::setWorkstep()");
    this->workstep = workstep;
}

void ModbusQueueWorker::push_back(ModbusQueue::element_t* element)
{
    QMutexLocker locker(&mutexQueue);
    list.append(element);
}

void ModbusQueueWorker::push_front(ModbusQueue::element_t* element)
{
    QMutexLocker locker(&mutexQueue);
    list.insert(0, element);
}

void ModbusQueueWorker::queryStop()
{
    setWorkstep(WS_WORKER_STOPPED);
}

void ModbusQueueWorker::queryResume()
{
    setWorkstep(WS_WORKER_RUNNING);
}

void ModbusQueueWorker::queryPause()
{
    setWorkstep(WS_WORKER_PAUSED);
}

void ModbusQueueWorker::queryClear()
{
    QMutexLocker locker(&mutexQueue);
    ModbusQueue::element_t* element = nullptr;

    while(false == list.empty())
    {
        element = list[0];
        list.removeFirst();

        (element->callback)(false, element); // all callback
    }
}

void ModbusQueueWorker::setSleepInterval(int interval)
{
    sleepInterval = interval;
}
