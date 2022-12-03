/*
 * qlabelsystemalarm.cpp
 *
 * 支持多种css样式的QLabel，用于系统报警栏
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qlabelsystemalarm.h"

QLabelSystemAlarm::QLabelSystemAlarm(QWidget* parent): QLabel(parent)
{
#if 0
    connect(this, SIGNAL(clicked()),
            this, SLOT(onClicked()));
#endif
    connect(&timer, SIGNAL(timeout()),
            this, SLOT(onTimeOut()));

    setBlinkFrequency(500);
    isStyle1 = true;
}

void QLabelSystemAlarm::init(const QList<QLabelSystemAlarm::BlinkStyleType>& blinkStyles)
{
    this->blinkStyles = blinkStyles;
    setBlinkStyle(0);
}

void QLabelSystemAlarm::setBlinkFrequency(int changeTime)
{
    timer.setInterval(changeTime);
}

void QLabelSystemAlarm::setBlinkStyle(int styleIndex)
{
    blinkStyleIndex = styleIndex;
    isStyle1 = true;
    changeStyle();
}

#if 0
void QLabelSystemAlarm::onClicked()
{
    // TODO: clear the warning
    if(blinkStyleIndex < blinkStyles.size() - 1)
        blinkStyleIndex++;
    else
        blinkStyleIndex = 0;

    isStyle1 = true;
    changeStyle();
}
#endif

void QLabelSystemAlarm::onTimeOut()
{
    changeStyle();
    isStyle1 = !isStyle1;
}

void QLabelSystemAlarm::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);
    emit clicked();
}

void QLabelSystemAlarm::changeStyle()
{
    if(blinkStyleIndex < 0 || blinkStyleIndex >= blinkStyles.size())
        return;

    auto style = blinkStyles.at(blinkStyleIndex);

    if(true == style.isBlink)
    {
        if(false == timer.isActive())
            timer.start();

        setStyleSheet(isStyle1 ? style.cssStyle1 : style.cssStyle2); // interval style
    }
    else
    {
        timer.stop();
        setStyleSheet(style.cssStyle1); // fixed style
    }
}
