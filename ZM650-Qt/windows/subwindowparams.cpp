/*
 * subwindowparams.cpp
 *
 * 参数界面
 *
 * Created on: 2019年 01月 21日 星期一 15:00:16 CST
 * Author: lixingcong
 */
#include "subwindowparams.h"
#include "data/adapters.h"
#include "data/globaldata.h"
#include "data/utils.h"
#include "widgets/qwidgetshowparams_paramtable.h"
#include "widgets/qwidgetshowparams_ioconfig.h"
#include <SysText.h>
#include "widgets/qpushbuttonspage.h"
#include <TestIO.h>
#include <Parameter.h>
#include "data/uistyles.h"
#include "modbus/modbusaddress.h"
#include <QLabel>

SubWindowParams::SubWindowParams(QWidget* parent) :
    QWidget(parent),
    layout_v(new QVBoxLayout(this)),
    widget(nullptr)
{
    this->setContentsMargins(1,1,1,1);//设置边距
    UIStyles::setMarginAndSpacing(layout_v, 0);

    QList<QPushButtonsPage::item_t> sectionItems;

    for(auto i = 0; i < Adapters::getParamTableSectionCount(); ++i)
    {
        QPushButtonsPage::item_t item;
        item.id = Definitions::BTN_PARAM_SYSTEXT_BEGIN + i;
        item.text = Adapters::getParamTableSection(i).title;
        sectionItems.append(item);
    }

    // 输入 输出
    QPushButtonsPage::item_t item;
    item = {QString(tr("输入IO")), Definitions::BTN_PARAM_INCONFIG};
    sectionItems.append(item);
    item = {QString(tr("输出IO")), Definitions::BTN_PARAM_OUTCONFIG};
    sectionItems.append(item);
    GlobalData::pushButtonPage->init(sectionItems);
    connect(GlobalData::pushButtonPage, SIGNAL(buttonClicked(int)),
            this, SLOT(onChangedSection(int)));

    onChangedSection(Definitions::BTN_PARAM_SYSTEXT_BEGIN); // 第一页配置
}

SubWindowParams::~SubWindowParams()
{
}

void SubWindowParams::onChangedSection(int index)
{

    if(nullptr != widget)
    {
        layout_v->removeWidget(widget);
        widget->deleteLater();
        widget = nullptr;
    }
    switch(index)
    {
    case Definitions::BTN_PARAM_SYSTEXT_BEGIN:
    case Definitions::BTN_PARAM_SYSTEXT_BEGIN+1:
    case Definitions::BTN_PARAM_SYSTEXT_BEGIN+2:
    case Definitions::BTN_PARAM_SYSTEXT_BEGIN+3:
    case Definitions::BTN_PARAM_SYSTEXT_BEGIN+4:
    {
        auto systextIndex = index - Definitions::BTN_PARAM_SYSTEXT_BEGIN;

        GlobalData::labelTopTab->setText(Adapters::getParamTableSection(systextIndex).title);

        //widget = new QWidgetShowParams_ParamTable(&ParaRegTab[Adapters::getParamTableSection(systextIndex).startIndex], this);//mod by yzg ---add startIndex
        widget = new QWidgetShowParams_ParamTable(&ParaRegTab_AxisMove[Adapters::getParamTableSection(systextIndex).startIndex],Adapters::getParamTableSection(systextIndex).startIndex, this);
        widget->init(10, 2, Adapters::getParamTableSection(systextIndex).totalCount);
        connect(widget, SIGNAL(moveAxis(int,int,int, double)), this, SLOT(onMoveAxis(int,int,int, double)));
        connect(widget, SIGNAL(openOffset(int)), this, SLOT(onOpenOffset(int)));
        layout_v->addWidget(widget);
        break;
    }

    case Definitions::BTN_PARAM_INCONFIG:
        GlobalData::labelTopTab->setText(tr("输入IO"));
        widget = new QWidgetShowParams_IOConfig(Input_Fun_Name, g_Sysparam.InConfig, ModbusAddress::InConfig,1);
        #ifdef Arm_Kaiteli
        widget->init(8, 3, IN_KAITELI_END);
		#else
        widget->init(8, 3, MAX_INPUT_FUN);
		#endif
        layout_v->addWidget(widget);
        break;

    case Definitions::BTN_PARAM_OUTCONFIG:
        GlobalData::labelTopTab->setText(tr("输出IO"));
        widget = new QWidgetShowParams_IOConfig(Output_Fun_Name, g_Sysparam.OutConfig, ModbusAddress::OutConfig,0);
		#ifdef Arm_Kaiteli
        widget->init(8, 3, OUT_KAITELI_END);
		#else
		widget->init(8, 3, MAX_OUTPUT_FUN);
		#endif
        layout_v->addWidget(widget);
        break;

    default:
        break;
    }
}

void SubWindowParams::onMoveAxis(int axisNum, int direction, int pressType, double distance)
{
   emit moveAxis(axisNum, direction,pressType,distance);
}

void SubWindowParams::onOpenOffset(int tpye)
{
    emit openOffset(tpye);
}
