/*
 * qlistwidgetdragscroll.h
 *
 * 支持鼠标拖动滚动的QListWidget
 *
 * Created on: 2019年 02月 13日 星期三 18:26:02 CST
 * Author: lixingcong
 */

#ifndef QLISTWIDGETEX_H
#define QLISTWIDGETEX_H

#include <QListWidget>
#include <QMouseEvent>

class QListWidgetDragScroll: public QListWidget
{
    Q_OBJECT

public:
    explicit QListWidgetDragScroll(QWidget* parent = 0);

protected:
    void mousePressEvent(QMouseEvent* event)override;
    void mouseMoveEvent(QMouseEvent*)override;
    void mouseReleaseEvent(QMouseEvent*)override;

public slots:
    void onResetScrollPostion();

protected:
    QPoint pointBeforeMove;
};

#endif // QLISTWIDGETEX_H
