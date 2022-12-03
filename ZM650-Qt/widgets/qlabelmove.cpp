/*
 * qlabelmove.cpp
 *
 * 带拖动事件的QLabel，用于输入法面板
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qlabelmove.h"
#include <QDebug>

QLabelMove::QLabelMove(QWidget* parent, Qt::WindowFlags f):
    QLabel(parent, f)
{

}

void QLabelMove::mousePressEvent(QMouseEvent* event)
{
    offset = event->pos();
    QLabel::mousePressEvent(event);
}

void QLabelMove::mouseMoveEvent(QMouseEvent* event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        emit wantToMoveWindow(event->pos() - offset);
    }

    QLabel::mouseMoveEvent(event);
}
