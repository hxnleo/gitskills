/*
 * qwidgetshowparams_ioconfig.cpp
 *
 * 参数界面的IO配置
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qwidgetshowparams_ioconfig.h"
#include "data/utils.h"
#include "data/globaldata.h"
#include "widgets/qpushbuttonparams.h"
#include "widgets/qdialogparams_ioconfig.h"
#include "data/adapters.h"

QWidgetShowParams_IOConfig::QWidgetShowParams_IOConfig(IO_FUN_NAME* funCodeName, IOConfig* ioConfig, quint16 modbusAddr, int status,
        QWidget* parent): QWidgetShowParamsBase(parent),
    adapter(new Adapters::DataAdapter_IOConfigText(funCodeName, ioConfig, modbusAddr))

{
    GlobalData::iostatus = status;//保存当前参数状态
}

QWidgetShowParams_IOConfig::~QWidgetShowParams_IOConfig()
{
    Utils::waitForPendingModbus(&pendingQueryCount);
    Utils::delete_pointer(adapter);
}

QWidget* QWidgetShowParams_IOConfig::createFieldWidget(int index)
{//参数IO
    //auto p=adapter->getBaseAddr()+index;
    QWidget* w = nullptr;

    switch(adapter->getParamWidgetType(index))
    {
    case Adapters::WidgetType_IO_CONFIG:
    {//设置参数IO
        auto pushButton = new QPushButtonParams<QDialogParams_IOConfig, Adapters::DataAdapter_IOConfigText>(adapter, index,
                this);
        connect(pushButton, SIGNAL(wroteValueFromUI()),
                this, SLOT(onWroteGoodValue()));

        w = pushButton;
        break;
    }
    default:
        break;
    }
    w->setMinimumWidth(GlobalData::widthRate/6-40);
    w->setMaximumWidth(GlobalData::widthRate/6-40);
    w->setMinimumHeight(GlobalData::highRate/8-10);
    w->setMaximumHeight(GlobalData::highRate/8-10);
    return w;
}

QString QWidgetShowParams_IOConfig::getParamName(int offset) const
{
    return adapter->getParamName(offset);
}

bool QWidgetShowParams_IOConfig::getDataAuthority(int offset) const
{
    return GlobalData::DataAuthority(adapter->getDataAuthority(offset));
}

ModbusQueue::element_t* QWidgetShowParams_IOConfig::createModbusElement(int offset) const
{
    Utils::printQString(adapter->getParamName(offset));

    auto element = new ModbusQueue::element_t;
    auto funcCode= (adapter->getBaseAddr()+offset)->fun;

    element->data = adapter->getIOConfigBaseAddr() + funcCode;
    element->len = sizeof(IOConfig);
    element->startAddress = adapter->getIOConfigModbusAddr()+funcCode * sizeof(IOConfig)/2;
    element->operation = ModbusQueue::OP_WRITE_DATA;

    return element;
}
