/*
 * qlabelmove.h
 *
 * 带拖动事件的QLabel，用于输入法面板
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QLABELMOVE_H
#define QLABELMOVE_H

#include <QLabel>
#include <QMouseEvent>

class QLabelMove : public QLabel
{
    Q_OBJECT
public:
    QLabelMove(QWidget* parent = 0, Qt::WindowFlags f = 0);

signals:
    void wantToMoveWindow(QPoint pos);

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    QPoint offset;
};

#endif // QLABELMOVE_H
