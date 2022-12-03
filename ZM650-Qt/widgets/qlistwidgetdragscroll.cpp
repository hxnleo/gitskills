/*
 * qlistwidgetdragscroll.cpp
 *
 * 支持鼠标拖动滚动的QListWidget
 *
 * Created on: 2019年 02月 13日 星期三 18:26:02 CST
 * Author: lixingcong
 */

#include "qlistwidgetdragscroll.h"
#include <QScrollBar>

QListWidgetDragScroll::QListWidgetDragScroll(QWidget* parent): QListWidget(parent)
{

}

void QListWidgetDragScroll::mousePressEvent(QMouseEvent* event)
{
    // https://stackoverflow.com/questions/36833358/how-qtableview-or-qlistview-is-scrolling-with-hand-drag

    if(event->button() == Qt::LeftButton) //lets map scrolling to right button
        pointBeforeMove = event->pos(); //QPoint member, indicates the start of the scroll

    QListWidget::mousePressEvent(event);
}

void QListWidgetDragScroll::mouseMoveEvent(QMouseEvent* event)
{
    if(!pointBeforeMove.isNull()) //if the scroll was started
    {
        auto pos = event->pos();
        auto direction = pointBeforeMove.y() <
                         pos.y(); //determine direction, true is up (start is below current), false is down (start is above current)
#ifdef __arm__ // 这个宏是是步科ARM屏内置,gcc
        auto singleStep = (direction ? -15 : 15); //fill in the desired value
#else
        auto singleStep = (direction ? -2 : 2);
#endif
        verticalScrollBar()->setValue(verticalScrollBar()->value() + singleStep);
        //scroll by the certain amount in determined direction,
        //you decide how much will be a single step... test and see what you like

        pointBeforeMove = pos;
    }

    QListWidget::mouseMoveEvent(event);
}

void QListWidgetDragScroll::mouseReleaseEvent(QMouseEvent* event)
{
    pointBeforeMove = QPoint(); //resets the scroll drag
    QListWidget::mouseReleaseEvent(event);
}

void QListWidgetDragScroll::onResetScrollPostion()
{
    verticalScrollBar()->setValue(0);
}
