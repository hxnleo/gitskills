/*
 * qwidgetshowparams_iostatus.cpp
 *
 * 诊断界面的IO状态
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qwidgetshowparams_iostatus.h"
#include "data/utils.h"
#include "data/globaldata.h"
#include "data/uistyles.h"
#include "modbus/modbusaddress.h"
#include <QPushButton>
#include <QTimer>

#define REFRESH_INTERVAL 100

QWidgetShowParams_IOStatus::QWidgetShowParams_IOStatus(const QString& prefixString, IOConfig* ioConfigBaseAddr,
                                                       IO_FUN_NAME* funcNameAddr, int funcNameCount, unsigned short modbusAddress, int status,bool isAllowClick,  QWidget* parent):
    QWidgetShowParamsBase(parent),
    adapter(new Adapters::DataAdapter_IOStatus(ioConfigBaseAddr, funcNameAddr, funcNameCount)),
    ioConfigBaseAddr(ioConfigBaseAddr),
    status(status),
    isAllowClick(isAllowClick),
    modbusAddress(modbusAddress),
    funcNameCount(funcNameCount),
    timer(new QTimer(this)),
    ioStatusArray(new unsigned short[funcNameCount]), // crash in gcc 4.9.4(MingW64)
    ioStatusArrayLast(new unsigned short[funcNameCount]), // crash in gcc 4.9.4(MingW64)
    prefixString(prefixString),
    isForceRefresh(true)
{
    //获得端口号
    if(!this->status)
    {
        GlobalData::brakenum = ioConfigBaseAddr[OUT_BRAKE].IONum;//刹车端口号
#ifdef Arm_Kaiteli
        GlobalData::motornum = ioConfigBaseAddr[OUT_DRILL].IONum;//变频器端口号
#else
        GlobalData::motornum = ioConfigBaseAddr[OUT_MOTOR_START].IONum;//变频器端口号
#endif
    }

    connect(timer, SIGNAL(timeout()),
            this, SLOT(onTimeout()));

    timer->start(REFRESH_INTERVAL);
}

QWidgetShowParams_IOStatus::~QWidgetShowParams_IOStatus()
{
    timer->stop();

    Utils::waitForPendingModbus(&pendingQueryCount);

    Utils::delete_array(ioStatusArray);
    Utils::delete_array(ioStatusArrayLast);
    Utils::delete_pointer(adapter);
}

void QWidgetShowParams_IOStatus::onTimeout()
{//时刻读取IO状态
    if(false == mutexReadIO.tryLock()) // it will be unlocked in onModbusQueryCallbackEnd()
        return;
    auto queryItem = new Definitions::ModbusQueryItem;
    queryItem->queryAction = Action_ReadIO;
    auto element = new ModbusQueue::element_t;
    element->data = ioStatusArray;
    element->len = sizeof(*ioStatusArray) * funcNameCount;
    element->operation = ModbusQueue::OP_READ_DATA;
    element->startAddress = modbusAddress;
    element->pointerObject = queryItem;
    sendModbusElement(element);
}

QWidget* QWidgetShowParams_IOStatus::createFieldWidget(int index)
{//诊断
    QWidget* w = nullptr;

    switch(adapter->getParamWidgetType(index))
    {
    case Adapters::WidgetType_IO_STATUS:
    {
        auto pushButton = new QPushButton(adapter->getFunctionName(index), this);
        pushButton->setFocusPolicy(Qt::NoFocus);

        if(true == isAllowClick)
        {
            connect(pushButton, SIGNAL(clicked()),
                    this, SLOT(onWroteGoodValue()));
        }

        w = pushButton;
        break;
    }

    default:
        break;
    }
    if(modbusAddress==ModbusAddress::IOStatus_Out)
    {//IO输出
        w->setMinimumWidth(GlobalData::widthRate/5-20);
        w->setMaximumWidth(GlobalData::widthRate/5-20);
        w->setMinimumHeight(GlobalData::highRate/4-20);

    }
    else
    {//输入
        w->setMinimumWidth(GlobalData::widthRate/5-20);
        w->setMaximumWidth(GlobalData::widthRate/5-20);
        w->setMinimumHeight(GlobalData::highRate/7-16);
        GlobalData::getInstance()->widgetEnable(w);
    }

    return w;
}

QString QWidgetShowParams_IOStatus::getParamName(int offset) const
{
    return prefixString + adapter->getParamName(offset);
}

bool QWidgetShowParams_IOStatus::getDataAuthority(int offset) const
{
//    return adapter->getDataAuthority(offset);
    return true;
}

ModbusQueue::element_t* QWidgetShowParams_IOStatus::createModbusElement(int offset)const
{
    Utils::printQString(adapter->getParamName(offset));
    auto queryItem = new Definitions::ModbusQueryItem;
    queryItem->queryAction = Action_WriteIO;
    auto element = new ModbusQueue::element_t;
    //qDebug()<<element->startAddress;

    queryItem->mixData.u16 = 1 - *(ioStatusArray + offset);

    element->data = &queryItem->mixData.u16;
    element->len = sizeof(queryItem->mixData.u16);
    element->operation = ModbusQueue::OP_WRITE_DATA;
    element->startAddress = modbusAddress + offset;
    element->pointerObject = queryItem;

    return element;
}

void QWidgetShowParams_IOStatus::onSendModbusOK(ModbusQueue::element_t* element)
{
    auto queryItem = reinterpret_cast<Definitions::ModbusQueryItem*>(element->pointerObject);

    switch(queryItem->queryAction)
    {
    case Action_ReadIO:
        //debug_printf("read io ok!!!!");
        refreshIOStatus();
        mutexReadIO.unlock();
        break;

    default:
        break;
    }

    Utils::delete_pointer(queryItem);
}

void QWidgetShowParams_IOStatus::onSendModbusFail(ModbusQueue::element_t* element)
{
    auto queryItem = reinterpret_cast<Definitions::ModbusQueryItem*>(element->pointerObject);
    Utils::delete_pointer(queryItem);
}

void QWidgetShowParams_IOStatus::onRefreshPageDone()
{
    isForceRefresh = true;
}

void QWidgetShowParams_IOStatus::refreshIOStatus()
{
    if(false == mutexFieldsCreated.tryLock())
        return;

    if(true == isForceRefresh)
    {
        isForceRefresh = false;
        memset(ioStatusArrayLast, 0, sizeof(unsigned short)*funcNameCount);
    }

    auto startOffset = countPerPage * currentPage;
    for(auto i = 0; i < fieldsCreated.size(); ++i)
    {
        auto ioStatusArrayOffset = startOffset + i;

//        qDebug()<<"输出数据"<<adapter->getData(ioStatusArrayOffset);


        // IO staus did not change at all
        if(*(ioStatusArrayLast + ioStatusArrayOffset) == *(ioStatusArray + ioStatusArrayOffset))
            continue;

        if(1 == *(ioStatusArray + ioStatusArrayOffset))
            fieldsCreated.at(i)->setStyleSheet(UIStyles::CSS_IO_HIGHLIGHT);
        else
            fieldsCreated.at(i)->setStyleSheet(QString());
    }

    memcpy(ioStatusArrayLast, ioStatusArray, sizeof(unsigned short)*funcNameCount);

    mutexFieldsCreated.unlock();
}
