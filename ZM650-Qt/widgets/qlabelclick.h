/*
 * qlabelclick.h
 *
 * QLabel增加clicked信号
 *
 * Created on: 2019年 02月 15日 星期五 11:44:08 CST
 * Author: lixingcong
 */

#ifndef QLABELCLICK_H
#define QLABELCLICK_H

#include <QLabel>

class QLabelClick : public QLabel
{
    Q_OBJECT
public:
    explicit QLabelClick(QWidget* parent = 0, Qt::WindowFlags f = 0): QLabel(parent, f) {}
    explicit QLabelClick(const QString& text, QWidget* parent = 0, Qt::WindowFlags f = 0): QLabel(text, parent, f) {}

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event)override
    {
        QLabel::mousePressEvent(event);
        emit clicked();
    }
};

#endif // QLABELCLICK_H
