/*
 * qpushbuttonlongpress.cpp
 *
 * 带短按长按事件的QPushButton，用于教导界面单轴移动
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qpushbuttonlongpress.h"

QPushButtonLongPress::QPushButtonLongPress(QWidget *parent, const QString& text): QPushButton(text, parent),
    isLongPress(false)
{
    t.setSingleShot(true);
    t.setInterval(200); // default delay

    connect(&t, SIGNAL(timeout()),
            this, SLOT(onTimeout()));

    connect(this, SIGNAL(pressed()),
            this, SLOT(onPressed()));

    connect(this, SIGNAL(released()),
            this, SLOT(onReleased()));
}

QPushButtonLongPress::~QPushButtonLongPress()
{

}

void QPushButtonLongPress::setKeyPressDelay(int delay)
{
    t.setInterval(delay);
}

void QPushButtonLongPress::onTimeout()
{
    // 长按开始
    isLongPress = true;
    emit keyPress(PressLongBegin);
}

void QPushButtonLongPress::onPressed()
{
    isLongPress = false;
    t.start();
}

void QPushButtonLongPress::onReleased()
{
    t.stop();

    if(true == isLongPress)  // 长按结束
    {
        emit keyPress(PressLongEnd);
    }
    else   // 短按结束
    {
        emit keyPress(PressShort);
    }
}
