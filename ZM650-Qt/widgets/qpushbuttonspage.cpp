/*
 * qpushbuttonspage.cpp
 *
 * 单列PushButton，用于页面底部的操作按钮
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qpushbuttonspage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QVariant>
#include <qmath.h>
#include <QDebug>
#include <QPushButton>
#include "data/utils.h"
#include "data/uistyles.h"

QPushButtonsPage::QPushButtonsPage(QWidget* parent) : QWidget(parent),
    currentPage(0),
    maxPage(0),
    countEachPage(0),
    btn_next(nullptr),
    btn_prev(nullptr)
{
    // map signals handler
    connect(&maplist.signalMapper, SIGNAL(mapped(QWidget*)),
            this, SLOT(_buttonClicked(QWidget*)));
}

void QPushButtonsPage::initLayout(int countEachPage, bool isVertical)
{
    QSizePolicy sizePolicyButton(QSizePolicy::Preferred, QSizePolicy::Preferred);
    QLayout* layout = nullptr;

    // remove esixting layout;
    deleteLayout();

    // create layout
    if(true == isVertical)
        layout = new QVBoxLayout;
    else
        layout = new QHBoxLayout;

    // set margin
    UIStyles::setMarginAndSpacing(layout, 2, 1);

    // create "Prev" button
    if(nullptr == btn_prev)
    {
        //btn_prev = new QPushButton(this);
        btn_prev = new QMyPushButton_U0(this);
        //btn_prev->setImage(":/images/mainIco/gostart.png",":/images/mainIco/gostart_press_.png");
        btn_prev->setPressOrUp(false);
        //btn_prev->update();
        btn_prev->setText("←");
        btn_prev->setSizePolicy(sizePolicyButton);
        //"QPushButton:hover{background-color:white; color: black;}"
        btn_prev->setStyleSheet(UIStyles::css_maincolorconf_u8);

        connect(btn_prev, SIGNAL(clicked()),
                this, SLOT(_buttonClickedPrev()));

        connect(btn_prev, SIGNAL(clicked()),
                this, SLOT(_clearFocus()));
    }

    layout->addWidget(btn_prev);

    // create "Normal" buttons
    if(0 >= countEachPage)
        this->countEachPage = 5; // 默认5个
    else
        this->countEachPage = countEachPage;

    for(int i = 0; i < this->countEachPage; ++i)
    {
        //QPushButton* btn = new QPushButton;
        QMyPushButton_U0* btn = new QMyPushButton_U0;
        maplist.widgets.append(btn);

        btn->setSizePolicy(sizePolicyButton);
        btn->setProperty("pageIndex", QVariant(i));

        layout->addWidget(btn);

        maplist.signalMapper.setMapping(btn, btn);
        connect(btn, SIGNAL(clicked()),
                &maplist.signalMapper, SLOT(map()));

        connect(btn, SIGNAL(clicked()),
                this, SLOT(_clearFocus()));

        btn->setStyleSheet(UIStyles::css_maincolorconf_u8);
    }

    // create "Next" button
    if(nullptr == btn_next)
    {
        //btn_next = new QPushButton(this);
        btn_next = new QMyPushButton_U0(this);
        btn_next->setText("→");
        btn_next->setSizePolicy(sizePolicyButton);

        btn_next->setStyleSheet(UIStyles::css_maincolorconf_u8);
        connect(btn_next, SIGNAL(clicked()),
                this, SLOT(_buttonClickedNext()));

        connect(btn_next, SIGNAL(clicked()),
                this, SLOT(_clearFocus()));
    }

    layout->addWidget(btn_next);

    // setup the layout
    setLayout(layout);

    calculateMaxPage();

    refreshPageText();
}

void QPushButtonsPage::deleteLayout()
{
    QLayout* const layout = this->layout();

    if(nullptr == layout)
        return;

    foreach(QPushButton* btn, maplist.widgets)
    {
        maplist.signalMapper.removeMappings(btn);
        btn->deleteLater();
    }

    maplist.widgets.clear();

    delete layout;
}

void QPushButtonsPage::calculateMaxPage()
{
    // reset current page, calculate the max page num
    currentPage = 0;

    if(0 == maplist.items.size())
        maxPage = 1;
    else
        maxPage = qCeil(static_cast<qreal>(maplist.items.size()) / this->countEachPage);
}

void QPushButtonsPage::init(const QList<item_t>& items)
{
    // TODO: QMutex
    maplist.items = items;

    calculateMaxPage();

    refreshPageText();
}

QPushButtonsPage::~QPushButtonsPage()
{
    deleteLayout();
}

void QPushButtonsPage::_buttonClicked(QWidget* w)
{
    int pageIndex = qvariant_cast<int>(w->property("pageIndex"));
    int itemIndex = pageIndex + countEachPage * currentPage;

    //debug_printf("clicked %d",itemIndex);
    emit buttonClicked(maplist.items.at(itemIndex).id);
}

void QPushButtonsPage::_buttonClickedNext()
{

//    if(currentPage < maxPage - 1)
//        currentPage=currentPage++;
    ++currentPage;
    currentPage=currentPage%maxPage;
//    qDebug()<<"max"<<maxPage<<"cru"<<currentPage;

    refreshPageText();
}

void QPushButtonsPage::_buttonClickedPrev()
{
//    if(currentPage > 0)
//        --currentPage;
    currentPage=0;

    refreshPageText();
}

void QPushButtonsPage::_clearFocus()
{
    qobject_cast<QWidget*>(sender())->clearFocus();
}

void QPushButtonsPage::refreshPageText()
{
    // 若第一页
    if(nullptr != btn_prev)
        btn_prev->setEnabled(0 != currentPage);

    // 若最后一页
    //    if(nullptr != btn_next)
    //        btn_next->setEnabled(maxPage - 1 != currentPage);

    int startIndex = countEachPage * currentPage;
    int endIndex = startIndex + countEachPage;
    int currentButtonIndex = 0;

    // 更新文字
    for(int i = startIndex; i < endIndex; ++i)
    {
        currentButtonIndex = i % countEachPage;
        QPushButton* const& btn = maplist.widgets.at(currentButtonIndex);

        if(i < maplist.items.size())
        {
            btn->setEnabled(true);
            btn->setText(maplist.items.at(i).text);
        }
        else
        {
            btn->setEnabled(false);
            btn->setText(QString());
        }
    }

}
