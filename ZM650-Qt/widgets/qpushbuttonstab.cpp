/*
 * qpushbuttonstab.cpp
 *
 * 单列PushButton，用于页面顶部的标签
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qpushbuttonstab.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QVariant>
#include "data/uistyles.h"

QPushButtonsTab::QPushButtonsTab(QWidget* parent) : QWidget(parent),
    isInitialized(false)
{
    // map signals handler
    connect(&maplist.signalMapper, SIGNAL(mapped(QWidget*)),
            this, SLOT(_buttonClicked(QWidget*)));
}

void QPushButtonsTab::init(const QStringList& tabNames)
{
    if(true == isInitialized)
        return;

    maplist.items = tabNames;

    QSizePolicy sizePolicyButton(QSizePolicy::Preferred, QSizePolicy::Preferred);

    auto layout_h = new QHBoxLayout(this);
    UIStyles::setMarginAndSpacing(layout_h, 0, 0);



    for(int i = 0; i < this->maplist.items.size(); ++i)
    {
        QPushButton* btn = new QPushButton(this);
        btn->setFocusPolicy(Qt::NoFocus);

        maplist.widgets.append(btn);

        btn->setSizePolicy(sizePolicyButton);
        btn->setProperty("tabIndex", QVariant(i));
        //btn->setText(maplist.items.at(i).toUtf8());
        btn->setText(QObject::tr(maplist.items.at(i).toUtf8()));

        layout_h->addWidget(btn);

        maplist.signalMapper.setMapping(btn, btn);
        connect(btn, SIGNAL(clicked()),
                &maplist.signalMapper, SLOT(map()));
//"QPushButton:hover{background-color:white; color: black;}"
        btn->setStyleSheet(UIStyles::css_maincolorconf_u5);

    }

    if(maplist.items.size() > 0)
        setTabActive(0);
}

void QPushButtonsTab::setTabActive(int tabIndex)
{
    static int lastActiveTabIndex = -1;

    if(tabIndex < 0 || tabIndex >= maplist.items.size())
        return;

    if(lastActiveTabIndex > -1)
    {
        //maplist.widgets.at(lastActiveTabIndex)->setStyleSheet(QString());
        maplist.widgets.at(lastActiveTabIndex)->setStyleSheet(UIStyles::css_maincolorconf_u5);
    }

    //maplist.widgets.at(tabIndex)->setStyleSheet(UIStyles::CSS_PUSHBUTTON_TAB_ACTIVE);
    //#c3e1ec
    //#13ef47
    //设置选择后的色彩
    maplist.widgets.at(tabIndex)->setStyleSheet(UIStyles::css_maincolorconf_u6);


    lastActiveTabIndex = tabIndex;
}

void QPushButtonsTab::_buttonClicked(QWidget* w)
{
    //转换成窗口对应的数值
    int tabIndex = qvariant_cast<int>(w->property("tabIndex"));

    if(tabIndex < 0 || tabIndex >= maplist.items.size())
        return;

    setTabActive(tabIndex);
    emit tabClicked(tabIndex);
}
