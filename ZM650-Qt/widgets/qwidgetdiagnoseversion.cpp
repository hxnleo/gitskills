/*
 * qwidgetdiagnoseversion.cpp
 *
 * 诊断界面的版本信息
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qwidgetdiagnoseversion.h"
#include <public.h>
#include "data/utils.h"
#include "modbus/modbusaddress.h"
#include "modbus/modbusqueue.h"
#include "data/globaldata.h"
#include "data/utils.h"
#include <QGridLayout>
#include <QLabel>
#include <tr1/functional>
#include "data/uistyles.h"

const QString DEFAULT_FIELD_TEXT = "???";

QWidgetDiagnoseVersion::QWidgetDiagnoseVersion(QWidget* parent) : QWidget(parent)
{
    for(int i=0;i<20;i++)
    {
        registered_array.name[i]=0;
    }
    ModbusQueue::element_t element;
    element.data = &registered;
    element.len = sizeof(registered);
    element.operation = ModbusQueue::OP_READ_DATA;
    element.startAddress = 5550;
    GlobalData::modbusQueue->sendQuery(&element, false);


    element.data = &registered_array;
    element.len = sizeof(registered_array);
    element.operation = ModbusQueue::OP_READ_DATA;
    element.startAddress = 5500;
    GlobalData::modbusQueue->sendQuery(&element, false);

    auto gridLayout = new QGridLayout(this);

    connect(this, SIGNAL(_signal_modbusQueryCallback(bool, void*)),
            this, SLOT(onModbusQueryCallbackEnd(bool, void*)), Qt::QueuedConnection);

    QFont font;
    font.setPointSize(10);
    auto addItemToLayout = [&](int row, int col, const QString & title, QLabel*& fieldWidget)
    {
        auto textLabel = new QLabel(title, this);
        textLabel->setFont(font);
        textLabel->setMargin(5);
        fieldWidget = new QLabel(DEFAULT_FIELD_TEXT, this);
        font.setPointSize(9);
        fieldWidget->setFont(font);
        fieldWidget->setAlignment(Qt::AlignCenter);
        fieldWidget->setFixedWidth(GlobalData::widthRate/4);
        fieldWidget->setStyleSheet(UIStyles::CSS_DIAGNOSE_VERSION_LABEL);
        gridLayout->addWidget(textLabel, row, 2 * col);
        gridLayout->addWidget(fieldWidget, row, 2 * col + 1);


        gridLayout->setSpacing(15);
    };
    int rowNum = 0;
    int colNum = 0;

    addItemToLayout(rowNum++, colNum, tr("产品ID"), label_ProductID);
    addItemToLayout(rowNum++, colNum, tr("上位机编译日期"), label_BuildDate[0]);
    addItemToLayout(rowNum++, colNum, tr("控制器编译日期"), label_BuildDate[1]);
    addItemToLayout(rowNum++, colNum, tr("上位机软件版本"), label_SoftwareVersion[0]);
    addItemToLayout(rowNum++, colNum, tr("控制器软件版本"), label_SoftwareVersion[1]);

    rowNum = 0;
    colNum = 1;
    addItemToLayout(rowNum++, colNum, tr("控制器Lib版本"), label_LibVersion);
    addItemToLayout(rowNum++, colNum, tr("控制器Motion版本"), label_MotionVersion);
    addItemToLayout(rowNum++, colNum, tr("控制器FPGA1版本"), label_FpgaVersion1);
    addItemToLayout(rowNum++, colNum, tr("控制器FPGA2版本"), label_FpgaVersion2);
    addItemToLayout(rowNum++, colNum, tr("注册状态"), label_registered);

    // send modbus query
    auto modbusElement = new ModbusQueue::element_t;
    modbusElement->data = new ZSJ_1600E_VER;
    modbusElement->len = sizeof(ZSJ_1600E_VER);
    modbusElement->callback = std::tr1::bind(&QWidgetDiagnoseVersion::_modbusQueryCallback, this,
                                             std::tr1::placeholders::_1, std::tr1::placeholders::_2);
    modbusElement->operation = ModbusQueue::OP_READ_DATA;
    modbusElement->startAddress = ModbusAddress::ControllerVersion;
    GlobalData::modbusQueue->sendQuery(modbusElement);

    // update text
#define SOFTWARE_VERSION_TEXT(config) "V1.0 "#config

#ifdef QT_DEBUG
    const QString versionText = SOFTWARE_VERSION_TEXT(Debug);
#else
    const QString versionText = SOFTWARE_VERSION_TEXT(Release);
#endif

    label_BuildDate[0]->setText(QString(__DATE__) + " " + QString(__TIME__));
    label_SoftwareVersion[0]->setText(versionText);
}

void QWidgetDiagnoseVersion::onModbusQueryCallbackEnd(bool isOk, void* flag)
{
    bool isNeedDelete = true;

    auto element = reinterpret_cast<ModbusQueue::element_t*>(flag);
    auto data = reinterpret_cast<ZSJ_1600E_VER*>(element->data);

    const QChar LEADING_ZERO = QChar('0');

    if(isOk)
    {
        retrytime=0;
        label_BuildDate[1]->setText(QString("%1-%2-%3 %4:%5:%6")
                                    .arg(data->BuildDate.usYear)
                                    .arg(data->BuildDate.ucMonth)
                                    .arg(data->BuildDate.ucDay)
                                    .arg(static_cast<quint16>(data->BuildDate.ucHour), 2, 10, LEADING_ZERO)
                                    .arg(static_cast<quint16>(data->BuildDate.ucMinute), 2, 10, LEADING_ZERO)
                                    .arg(static_cast<quint16>(data->BuildDate.ucSecond), 2, 10, LEADING_ZERO));
        label_ProductID->setText(Utils::usigerchartostring(&(data->ProductName)));
        label_LibVersion->setText(QString("%1").arg(data->FpgaVer.LibVer));
        label_MotionVersion->setText(QString("%1").arg(data->FpgaVer.MotionVer));
        label_FpgaVersion1->setText(QString("%1").arg(data->FpgaVer.FpgaVer1));
        label_FpgaVersion2->setText(QByteArray::number(data->FpgaVer.FpgaVer2,16));
        label_SoftwareVersion[1]->setText(QString(data->SoftVer));
        label_registered->setText(QString(registered_array.name)+"|"+QString("%1").arg(registered));

    }
    else
    {
        if(true == Utils::dialogIsModbusRetry(this, GlobalData::modbusQueue))
        {
            GlobalData::modbusQueue->sendQuery(element, true, false);
            GlobalData::modbusQueue->queryResume();
            isNeedDelete = false;
        }
        else
        {
            GlobalData::modbusQueue->queryStopThenOffline();
        }
    }

    if(true == isNeedDelete)
    {
        Utils::delete_pointer(data);
        Utils::delete_pointer(element);
    }
}
