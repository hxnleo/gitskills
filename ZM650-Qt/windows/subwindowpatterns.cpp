/*
 * subwindowpatterns.cpp
 *
 * 花样子窗口
 *
 * Created on: 2019年 01月 26日 星期六 17:02:46 CST
 * Author: lixingcong
 */

#include "subwindowpatterns.h"
#include "data/utils.h"
#include "data/globaldata.h"
#include "data/uistyles.h"
#include "widgets/qpushbuttonspage.h"
#include "data/definitions.h"
#include "widgets/qwidgetshowparams_patterns.h"

SubWindowPatterns::SubWindowPatterns(QWidget* parent) : QWidget(parent),
    layout_v(new QVBoxLayout(this)),
    widget(nullptr)
{
    UIStyles::setMarginAndSpacing(layout_v, 0);

    QList<QPushButtonsPage::item_t> sectionItems =
    {
        {QString("轨迹预览"), Definitions::BTN_SYS_TRACEPATH}
    };

    GlobalData::pushButtonPage->init(sectionItems);

    widget = new QWidgetShowParams_Patterns(this);
    //widget->init(2, 3, widget->getPatternCount()); // 四行，两列
    Utils::sendOffest(&GlobalData::parambite,ModbusAddress::ParamUse,sizeof(GlobalData::parambite));

    widget->init(2, 3, 25); // 四行，两列 7后面仅用与美鑫
    //        widget->init(2, 3, 8); // 四行，两列 7后面仅用与美鑫

    layout_v->addWidget(widget);
}
