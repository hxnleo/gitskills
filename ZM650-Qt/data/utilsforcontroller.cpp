/*
 * utilsforcontroller.cpp
 *
 * 公用的工具函数，主要是下位机部份
 *
 * Created on: 2019年 01月 21日 星期一 14:31:00 CST
 * Author: lixingcong
 */
#include "utilsforcontroller.h"
#include <public.h>
#include "adapters.h"
#include "modbus/modbusaddress.h"
#include "modbus/modbusqueue.h"
#include "definitions.h"

namespace Utils
{

bool checkWorkstatusIfStop(bool isNeedCreateDialog, QProgressDialog** dialogCreated, QWidget* dialogParent)
{
    bool isStopped = (Run != GlobalData::realTimeInfo.iWSta);

//    if(true == isNeedCreateDialog)
//    {
//        const QString hintText_movingNow = QString::fromUtf8("正在等待轴停止运动");
//        const QString hintText_IKnow = QString::fromUtf8("知道了");

//        *dialogCreated = new QProgressDialog(hintText_movingNow, hintText_IKnow, 0, 0, dialogParent, Qt::FramelessWindowHint);
//        (*dialogCreated)->setAttribute(Qt::WA_DeleteOnClose);
//    }

    return isStopped;
}

bool checkWorkstatusIfPause()
{
    return Pause == GlobalData::realTimeInfo.iWSta;
}

bool readIOStatus(bool isInput, int functionNumber, quint16* result)
{
    ModbusQueue::element_t element;

    auto realIndex = Adapters::getRealIOFromFunctionCode(functionNumber, isInput);

    element.data = result;
    element.len = sizeof(*result);
    element.operation = ModbusQueue::OP_READ_DATA;
    element.startAddress = (isInput ? ModbusAddress::IOStatus_In : ModbusAddress::IOStatus_Out) + realIndex;

    return GlobalData::modbusQueue->sendQuery(&element, false, false);
}

bool writeOutputStatus(int functionNumber, quint16 status)
{
    ModbusQueue::element_t element;

    auto realIndex = Adapters::getRealIOFromFunctionCode(functionNumber, false);

    element.data = &status;
    element.len = sizeof(status);
    element.operation = ModbusQueue::OP_WRITE_DATA;
    element.startAddress = ModbusAddress::IOStatus_Out + realIndex;

    return GlobalData::modbusQueue->sendQuery(&element, false, false);
}

} // namspace Utils
