/*
 * qwidgetshowparams_axisenable.cpp
 *
 * 参数界面的IO配置
 *
 * Created on: 2020年 01月 5日 星期一 14:57:40 CST
 * Author: caojing
 */
#include "qwidgetshowparams_axisenable.h"
#include "data/utils.h"
#include "data/globaldata.h"
#include "data/uistyles.h"
#include "modbus/modbusaddress.h"
#include <QPushButton>
#include <QTimer>

#define REFRESH_INTERVAL 100

QWidgetShowParams_AxisEnable::QWidgetShowParams_AxisEnable(QWidget* parent):
    QWidgetShowParamsBase(parent),
    timer(new QTimer(this)),
    isForceRefresh(true),
    AixsStatusArray(new unsigned short),
    AixsStatusArrayLast(new unsigned short)
{
    connect(timer, SIGNAL(timeout()),
            this, SLOT(onTimeout()));
    timer->start(REFRESH_INTERVAL);
}

QWidgetShowParams_AxisEnable::~QWidgetShowParams_AxisEnable()
{
    timer->stop();
    Utils::waitForPendingModbus(&pendingQueryCount);
}

void QWidgetShowParams_AxisEnable::onTimeout()
{//时刻读取IO状态
    if(false == mutexReadIO.tryLock()) // it will be unlocked in onModbusQueryCallbackEnd()
        return;
    auto queryItem = new Definitions::ModbusQueryItem;
    queryItem->queryAction = Action_RAXISEBALE;
    auto element = new ModbusQueue::element_t;
    element->data = AixsStatusArray;
    element->len = sizeof(*AixsStatusArray);
    element->operation = ModbusQueue::OP_READ_DATA;
    element->startAddress = ModbusAddress::AXIS_ENABLE;
    element->pointerObject = queryItem;
    sendModbusElement(element);
}

QWidget* QWidgetShowParams_AxisEnable::createFieldWidget(int index)
{//诊断
    QWidget* w = nullptr;

    auto pushButton = new QPushButton(getParamName(index), this);
    //    pushButton->setFocusPolicy(Qt::NoFocus);
    connect(pushButton, SIGNAL(clicked()),
            this, SLOT(onWroteGoodValue()));

    w = pushButton;
    w->setMinimumWidth(GlobalData::widthRate/5-20);
    w->setMaximumWidth(GlobalData::widthRate/5-20);
    w->setMinimumHeight(GlobalData::highRate/4-20);

    return w;
}

QString QWidgetShowParams_AxisEnable::getParamName(int offset) const
{
    return GlobalData::axisName.at(offset)+tr("轴使能");
}

bool QWidgetShowParams_AxisEnable::getDataAuthority(int offset) const
{
    return true;
}

ModbusQueue::element_t* QWidgetShowParams_AxisEnable::createModbusElement(int offset)const
{
    auto queryItem = new Definitions::ModbusQueryItem;
    queryItem->queryAction = Action_WAXISEBALE;
    auto element = new ModbusQueue::element_t;

//    qDebug()<<"AixsStatusArray"<<*AixsStatusArray;
    if(*AixsStatusArray&(0x01<<(offset)))
        *AixsStatusArray = (*AixsStatusArray)&~(0x01<<(offset));
    else
        *AixsStatusArray = (*AixsStatusArray)|(0x01<<(offset));

    queryItem->mixData.u16 =  *AixsStatusArray;
    qDebug()<<"AixsStatusArray"<<*AixsStatusArray;
    element->data = AixsStatusArray;
    element->len = sizeof(*AixsStatusArray);
    element->operation = ModbusQueue::OP_WRITE_DATA;
    element->startAddress = ModbusAddress::AXIS_ENABLE;
    element->pointerObject = queryItem;
    return element;
}

void QWidgetShowParams_AxisEnable::onSendModbusOK(ModbusQueue::element_t* element)
{
    auto queryItem = reinterpret_cast<Definitions::ModbusQueryItem*>(element->pointerObject);

    switch(queryItem->queryAction)
    {
    case Action_RAXISEBALE:
        //debug_printf("read io ok!!!!");
        refreshIOStatus();
        mutexReadIO.unlock();
        break;

    default:
        break;
    }

    Utils::delete_pointer(queryItem);
}

void QWidgetShowParams_AxisEnable::onSendModbusFail(ModbusQueue::element_t* element)
{
    auto queryItem = reinterpret_cast<Definitions::ModbusQueryItem*>(element->pointerObject);
    Utils::delete_pointer(queryItem);
}

void QWidgetShowParams_AxisEnable::onRefreshPageDone()
{
    isForceRefresh = true;
}

void QWidgetShowParams_AxisEnable::refreshIOStatus()
{

    if(false == mutexFieldsCreated.tryLock())
        return;

    if(true == isForceRefresh)
    {
        isForceRefresh = false;
        memset(AixsStatusArrayLast, 0, sizeof(qint16));
    }
    //qDebug()<<"进来 refreshIOStatus AixsStatusArray"<<*AixsStatusArray;
    //    auto startOffset = countPerPage * currentPage;
    for(auto i = 0; i < fieldsCreated.size(); ++i)
    {
        // IO staus did not change at all
        if(*AixsStatusArrayLast == *AixsStatusArray)
            continue;
        if((*AixsStatusArray)&(0x01<<i))
            fieldsCreated.at(i)->setStyleSheet(UIStyles::CSS_IO_HIGHLIGHT);
        else
            fieldsCreated.at(i)->setStyleSheet(QString());
    }
    memcpy(AixsStatusArrayLast, AixsStatusArray, sizeof(qint16));

    //qDebug()<<"出去 refreshIOStatus AixsStatusArray"<<*AixsStatusArray;
    mutexFieldsCreated.unlock();
}

