/*
 * qlabelmarquee.cpp
 *
 * 文字滚动的QLabel
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qlabelmarquee.h"
#include <QPainter>
#include <QResizeEvent>

QLabelMarquee::QLabelMarquee(QWidget* parent):
    QLabel(parent),
    forceDisableMarquee(false)
{
    px = 0;
    py = 15;
    speed = 2;
    direction = RightToLeft;
    setAlignment(Qt::AlignVCenter);
    marqueeEnable = true;
    connect(&timer, SIGNAL(timeout()), this, SLOT(refreshLabel()));
    timer.start(20);
}

void QLabelMarquee::refreshLabel()
{
    repaint();
}

QLabelMarquee::~QLabelMarquee()
{

}

void QLabelMarquee::setAlignment(Qt::Alignment al)
{
    m_align = al;
    updateCoordinates();
    QLabel::setAlignment(al);
}

void QLabelMarquee::paintEvent(QPaintEvent* evt)
{
    if(false == marqueeEnable || forceDisableMarquee)
    {
        QLabel::paintEvent(evt);
        return;
    }

    QPainter p(this);

    if(direction == RightToLeft)
    {
        px -= speed;

        if(px <= (-textLength))
            px = width();
    }
    else
    {
        px += speed;

        if(px >= width())
            px = - textLength;
    }

    p.drawText(px, py + fontPointSize, text());
    p.translate(px, 0);
}

void QLabelMarquee::resizeEvent(QResizeEvent* evt)
{
    updateCoordinates();
    QLabel::resizeEvent(evt);
    updateIsForceDisableMarquee();
}

void QLabelMarquee::updateCoordinates()
{
    switch(m_align)
    {
    case Qt::AlignTop:
        py = 10;
        break;

    case Qt::AlignBottom:
        py = height() - 10;
        break;

    case Qt::AlignVCenter:
        py = height() / 2;
        break;
    }

    fontPointSize = font().pointSize() / 2;
}

void QLabelMarquee::updateIsForceDisableMarquee()
{
    textLength = fontMetrics().width(text());
    // 当给QLabel设置setFixedSize属性后，这个forceDisableMarqueee让其更美观
    forceDisableMarquee = textLength <= rect().width();
    //qDebug("isEnable=%d",forceDisableMarquee?1:0);
}

void QLabelMarquee::setSpeed(int s)
{
    speed = s;
}

int QLabelMarquee::getSpeed()
{
    return speed;
}

void QLabelMarquee::setMarqueeEnable(bool enable)
{
    marqueeEnable = enable;

    if(marqueeEnable)
        timer.start();
    else
        timer.stop();

    refreshLabel();
}

void QLabelMarquee::setDirection(int d)
{
    direction = d;

    if(direction == RightToLeft)
        px = width() - textLength;
    else
        px = 0;

    refreshLabel();
}

void QLabelMarquee::setText(const QString& text)
{
    QLabel::setText(text);
    updateIsForceDisableMarquee();
}
