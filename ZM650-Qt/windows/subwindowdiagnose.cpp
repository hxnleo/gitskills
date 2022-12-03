/*
 * subwindowdiagnose.cpp
 *
 * 诊断界面
 *
 * Created on: 2019年 01月 21日 星期一 15:00:16 CST
 * Author: lixingcong
 */
#include "subwindowdiagnose.h"
#include "subwindowlocalconfig.h"
#include "data/adapters.h"
#include "data/globaldata.h"
#include "data/utils.h"
#include "widgets/qpushbuttonspage.h"
#include "widgets/qwidgetshowparams_iostatus.h"
#include "widgets/qwidgetshowparams_daconfig.h"
#include "widgets/qwidgetshowparams_diagnoseutils.h"
#include "windows/qwidgetshowparams_axisenable.h"
#include "widgets/qwidgetdiagnoseversion.h"
#include <Parameter.h>
#include <TestIO.h>
#include <QLabel>
#include "modbus/modbusaddress.h"
#include "data/definitions.h"
#include "widgets/qwidgetshowparams_zm662daconfig.h"
#define AXISEBALENUM 16
SubWindowDiagnose::SubWindowDiagnose(QWidget* parent) : QWidget(parent),
    layout_v(new QVBoxLayout(this)),
    widget(nullptr)
{
    this->setContentsMargins(1,1,1,1);//设置边距
    // bottom btns
    QList<QPushButtonsPage::item_t> listButtonItems =
    {
        {tr("输入"), Definitions::BTN_DIAGNOSE_INPUT},
        {tr("输出"), Definitions::BTN_DIAGNOSE_OUTPUT},
        {tr("DA"), Definitions::BTN_DIAGNOSE_DA},
        {tr("辅助工具"), Definitions::BTN_DIAGNOSE_OTHERS},
        {tr("版本信息"), Definitions::BTN_DIAGNOSE_INFO},
        {tr("轴使能"), Definitions::BTN_PARAM_AXISENABLE},
        {tr("本地配置"), Definitions::BTN_DIAGNOSE_SCREENCONFIG}
    };

    GlobalData::pushButtonPage->init(listButtonItems);
    connect(GlobalData::pushButtonPage, SIGNAL(buttonClicked(int)),
            this, SLOT(onChangedSection(int)));

    onChangedSection(Definitions::BTN_DIAGNOSE_INPUT);
}

SubWindowDiagnose::~SubWindowDiagnose()
{
    if(nullptr != widget)
    {
        layout_v->removeWidget(widget);
        widget->deleteLater();
        widget = nullptr;
    }
    qDebug()<<"~SubWindowDiagnose";
}

void SubWindowDiagnose::onChangedSection(int index)
{
    if(nullptr != widget)
    {
        layout_v->removeWidget(widget);
        widget->deleteLater();
        widget = nullptr;
    }

    switch(index)
    {
    case Definitions::BTN_DIAGNOSE_INPUT:
    {
        GlobalData::labelTopTab->setText(tr("诊断输入"));
        //        auto widget_new = new QWidgetShowParams_IOStatus(QString::fromUtf8("输入"), g_Sysparam.InConfig, Input_Fun_Name,
        //                MAX_INPUT_FUN, ModbusAddress::IOStatus_In, false, this);
        auto widget_new = new QWidgetShowParams_IOStatus(tr("输入"), g_Sysparam.InConfig, Input_Fun_Name,
                                                         MAX_INPUT_FUN, ModbusAddress::IOStatus_In,1, false, this);
        widget_new->init(5, 7, MAX_INPUT_NUM);
        layout_v->addWidget(widget_new);
        widget = widget_new;
        break;
    }

    case Definitions::BTN_DIAGNOSE_OUTPUT:
    {
        GlobalData::labelTopTab->setText(tr("诊断输出"));
        //        auto widget_new = new QWidgetShowParams_IOStatus(QString::fromUtf8("输出"), g_Sysparam.OutConfig, Output_Fun_Name,
        //                MAX_OUTPUT_FUN, ModbusAddress::IOStatus_Out, true, this);
        auto widget_new = new QWidgetShowParams_IOStatus(tr("输出"), g_Sysparam.OutConfig, Output_Fun_Name,
                                                         MAX_OUTPUT_FUN, ModbusAddress::IOStatus_Out,0, true, this);
        widget_new->init(5, 4, MAX_OUTPUT_NUM);
        layout_v->addWidget(widget_new);
        widget = widget_new;
        break;
    }

    case Definitions::BTN_DIAGNOSE_DA:
    {//DA
        GlobalData::labelTopTab->setText(tr("DA0测试"));
#ifdef Arm_662
        auto widget_new = new QWidgetShowParams_ZM662DAConfig(this);
#else
        auto widget_new = new QWidgetShowParams_DAConfig(MAX_DA_NUM+2,this);
        widget_new->init(3, 7, MAX_DA_NUM);//120
#endif
        layout_v->addWidget(widget_new);
        widget = widget_new;
        break;
    }

    case Definitions::BTN_DIAGNOSE_OTHERS:
    {
        GlobalData::labelTopTab->setText(tr("辅助工具"));
        auto widget_new = new QWidgetShowParams_DiagnoseUtils(this);
        widget_new->init(7, 3, QWidgetShowParams_DiagnoseUtils::Item_MaxCount);
        layout_v->addWidget(widget_new);
        widget = widget_new;
        break;
    }

    case Definitions::BTN_DIAGNOSE_INFO:
    {
        GlobalData::labelTopTab->setText(tr("版本信息"));
        auto widget_new = new QWidgetDiagnoseVersion(this);
        layout_v->addWidget(widget_new);
        widget = widget_new;
        break;
    }

    case Definitions::BTN_PARAM_AXISENABLE:
    {
        GlobalData::labelTopTab->setText(tr("轴使能"));
        auto widget_new = new QWidgetShowParams_AxisEnable(this);

        widget_new->init(4, 5, AXISEBALENUM);
        layout_v->addWidget(widget_new);
        widget = widget_new;
        break;
    }
    case Definitions::BTN_DIAGNOSE_SCREENCONFIG://本地配置
    {
        auto widget_new = new SubWindowLocalConfig(this);
        layout_v->addWidget(widget_new);
        widget = widget_new;
        break;
    }

    default:
        break;
    }
}
