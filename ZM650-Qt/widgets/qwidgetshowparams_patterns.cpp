/*
 * qwidgetshowparams_patterns.cpp
 *
 * 显示所有花样
 *
 * Created on: 2019年 01月 26日 星期六 17:05:50 CST
 * Author: lixingcong
 */

#include "qwidgetshowparams_patterns.h"
#include "data/utils.h"
#include <QProgressDialog>
#include "data/globaldata.h"
#include "modbus/modbusqueue.h"
#include "modbus/modbusaddress.h"
#include <Teach.h>
#include "data/workfile.h"

QWidgetShowParams_Patterns::QWidgetShowParams_Patterns(QWidget* parent): QWidgetShowParamsBase(parent),
    adapter(new Adapters::DataAdapter_Patterns),
    isGenerating(false)
{
    // global signals
    connect(GlobalData::getInstance(), SIGNAL(updateRealtimeInfo()),
            this, SLOT(onUpdateRealtimeInfo()));
}


QWidgetShowParams_Patterns::~QWidgetShowParams_Patterns()
{
    Utils::waitForPendingModbus(&pendingQueryCount);
    Utils::delete_pointer(adapter);
}

QWidget* QWidgetShowParams_Patterns::createFieldWidget(int index)
{
    if(index == 17)
        index = 23;
    else if(index == 18)
        index = 21;
    else if(index == 19)
        index = 24;
    else if(index == 20)
        index = 25;
    else if(index == 21)
        index = 26;
    else if(index == 22)
        index = 27;
    else if(index == 23)
        index = 28;
    else if(index == 24)
        index = 29;

    //    qDebug()<<"index"<<index;
    auto pushButton = new QPushButtonPattern(adapter, index, this);
    connect(pushButton, SIGNAL(wroteValueFromUI()),
            this, SLOT(onWroteGoodValue()));
    pushButton->setMinimumHeight(GlobalData::widthRate/4-60);
    pushButton->setMinimumWidth(GlobalData::widthRate/3-20);
    pushButton->setMaximumWidth(GlobalData::widthRate/3-20);
    return pushButton;
}

QString QWidgetShowParams_Patterns::getParamName(int offset) const
{
    if(offset == 17)
    {
        offset = 23;
        return QString("23-弧形梅花");
    }
    else if(offset == 18)
    {
        offset = 21;
        return QString("21-圆盘颜色");
    }
    else if(offset == 19)
    {
        offset = 24;
        return QString("24-斜孔圆盘");
    }
    else if(offset == 20)
    {
        offset = 25;
        return QString("25-斜孔圆盘(跳色)");
    }
    else if(offset == 21)
    {
        offset = 26;
        return QString("26-圆盘孔(三色)");
    }
    else if(offset == 22)
    {
        offset = 27;
        return QString("27-斜孔圆盘(M:N)");
    }
    else if(offset == 23)
    {
        offset = 28;
        return QString("28-斜孔特定12扇角度圆盘");
    }
    else if(offset == 24)
    {
        offset = 29;
        return QString("29-圆盘斜孔(三色)");
    }

    return adapter->getParamName(offset);
}

bool QWidgetShowParams_Patterns::getDataAuthority(int offset) const
{
    return true;
}

ModbusQueue::element_t* QWidgetShowParams_Patterns::createModbusElement(int offset) const
{
    if(offset == 17)
    {
        offset = 23;
    }
    else if(offset == 18)
    {
        offset = 21;
    }
    else if(offset == 19)
    {
        offset = 24;
    }
    else if(offset == 20)
    {
        offset = 25;
    }
    else if(offset == 21)
    {
        offset = 26;
    }
    else if(offset == 22)
    {
        offset = 27;
    }
    else if(offset == 23)
    {
        offset = 28;
    }
    else if(offset == 24)
    {
        offset = 29;
    }

    //    qDebug()<<"offset"<<offset;
    Utils::printQString(adapter->getParamName(offset) + adapter->getData(offset).toString());
    auto element = new ModbusQueue::element_t;

    auto teachSend = new TEACH_SEND;
    memset(teachSend, 0, sizeof(TEACH_SEND));
    teachSend->Type = TEACH_CAM_PROCESS;

    // 花样号码
    teachSend->SendData.N_CAM_PARA.fCamPara[0] = static_cast<float>(offset);

    // 用户输入值
    auto pattern = adapter->getData(offset).value<Adapters::PatternItem>();
    auto patternParams = &pattern.params;

    for(auto i = 0; i < patternParams->size(); ++i)
    {
        GlobalData::patternarry[offset][i] = QString("%1").arg(patternParams->at(i).value);
        teachSend->SendData.N_CAM_PARA.fCamPara[i + 1] = patternParams->at(i).value;
        debug_printf("f1=%lf", teachSend->SendData.N_CAM_PARA.fCamPara[i + 1]);
    }

    element->data = teachSend;
    element->len = sizeof(TEACH_SEND);
    element->startAddress = ModbusAddress::TeachWorkfileHead;
    element->operation = ModbusQueue::OP_WRITE_DATA;

    return element;
}

void QWidgetShowParams_Patterns::onSendModbusOK(ModbusQueue::element_t* element)
{
    Workfile::getInstance()->setIsAlreadyReadAll(false);

    auto teachSend = reinterpret_cast<TEACH_SEND*>(element->data);
    delete teachSend;
}

void QWidgetShowParams_Patterns::onSendModbusFail(ModbusQueue::element_t* element)
{
    emit dismissProgressDialog();

    auto teachSend = reinterpret_cast<TEACH_SEND*>(element->data);
    delete teachSend;
}

void QWidgetShowParams_Patterns::onWroteGoodValue()
{
    isGenerating = true;

    QWidgetShowParamsBase::onWroteGoodValue();

    if(false == GlobalData::modbusQueue->isOffline())
    {
        auto d = new QProgressDialog(this, Qt::FramelessWindowHint);
        d->setAttribute(Qt::WA_DeleteOnClose);
        d->setLabelText("生成中，请稍候");
        d->setCancelButton(nullptr);
        d->setRange(0, 100);
        connect(this, SIGNAL(dismissProgressDialog()),
                d, SLOT(accept()));
        d->open();
    }
}

void QWidgetShowParams_Patterns::onUpdateRealtimeInfo()
{
    static quint32 lastTeachCounter = 0xffffffff;

    if(lastTeachCounter != GlobalData::realTimeInfo.iTeachAcc)
    {
        if(isGenerating)
        {
            emit dismissProgressDialog();
            isGenerating = false;
        }

        lastTeachCounter = GlobalData::realTimeInfo.iTeachAcc;
    }
}
