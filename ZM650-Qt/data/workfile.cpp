/*
 * workfile.cpp
 *
 * 加工点文件的读取写入
 *
 * Created on: 2019年 01月 21日 星期一 14:31:00 CST
 * Author: lixingcong
 */
#include "workfile.h"
#include "data/utils.h"
#include "data/globaldata.h"
#include "modbus/modbusaddress.h"
#include "modbus/modbusqueue.h"
#include <QMutexLocker>

Workfile* Workfile::instance = nullptr;

Workfile::Workfile(): QObject(nullptr),
    _isAlreadReadAll(false)
{
    memset(&teachData, 0, sizeof(teachData));
}

Workfile* Workfile::getInstance()
{
    if(nullptr == instance)
        instance = new Workfile;
    return instance;
}

Workfile::~Workfile()
{
    instance = nullptr;
}

bool Workfile::readAll()
{
    int totalPoint = 0;

    setIsAlreadyReadAll(false);

    // 点数
    if(false == loadTotalPointNumber(&totalPoint))
    {
        emit fifoReadWriteFail();
        return false;
    }

    // 无需读取
    if(0 == totalPoint)
    {
        setIsAlreadyReadAll(true);
        emit fifoReadWriteDone();
        return true;
    }

    //读取所有孔
    return fifoReadWrite(0, totalPoint, true, true);
}

bool Workfile::read(int startPointIndex, int count)
{
    if(startPointIndex < 0)
        return readAll();

    return fifoReadWrite(startPointIndex, count, true);
}

bool Workfile::fifoReadWrite(int startPointIndex, int count, bool isRead, bool isFullyRead)
{
    bool isOk = false;

    QMutexLocker locker(&mutexLoading);
    ModbusQueue::element_t element;

    // 读取部份
    element.data = &teachData[startPointIndex];
    element.len = sizeof(TEACH_DATA) * static_cast<size_t>(count);
    element.mixParam.fifo.code = 2;
    element.mixParam.fifo.offset = startPointIndex * static_cast<int>(sizeof(TEACH_DATA));
    element.operation = isRead ? ModbusQueue::OP_READ_DATA_FIFO : ModbusQueue::OP_WRITE_DATA_FIFO;

    connect(GlobalData::modbusQueue, SIGNAL(notifyFifoProgress(int)),
            this, SLOT(_onNotifyFifoProgress(int)));

    isOk = GlobalData::modbusQueue->sendQuery(&element, false);

    disconnect(GlobalData::modbusQueue, SIGNAL(notifyFifoProgress(int)),
               this, SLOT(_onNotifyFifoProgress(int)));

    if(isOk)
    {
        emit fifoReadWriteDone();
    }
    else
    {
        emit fifoReadWriteFail();
    }

    if(true == isRead && false == isFullyRead) // 读模式下 且 仅读取部份数据
        setIsAlreadyReadAll(false);
    else
        setIsAlreadyReadAll(isOk);

    return isOk;
}

bool Workfile::write(int startPointIndex, int count)
{
    return fifoReadWrite(startPointIndex, count, false);
}

bool Workfile::loadTotalPointNumber(int* totalPoint)
{
    ModbusQueue::element_t element;
    bool isOk = false;

    // 总点数
    quint16 _totalPoint;
    element.data = &_totalPoint;
    element.len = sizeof(_totalPoint);
    element.operation = ModbusQueue::OP_READ_DATA;
    element.startAddress = ModbusAddress::WorkfileTotalPoint;

    isOk = GlobalData::modbusQueue->sendQuery(&element, false);

    if(isOk)
    {
        *totalPoint = static_cast<int>(_totalPoint);
    }

    return isOk;
}

void Workfile::_onNotifyFifoProgress(int percent)
{
    emit loading(percent);
}
