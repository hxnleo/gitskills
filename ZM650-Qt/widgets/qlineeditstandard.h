/*
 * qLineeditstandard.h
 *
 * 封装标准的 QLineEdit 类(实现单击触发按键框)
 *
 * Created on: 2020年 06月 26日 星期一 14:57:40 CST
 * Author: caojing
 */
#ifndef QLINEEDITSTANDARD_H
#define QLINEEDITSTANDARD_H
#include <QLineEdit>

class QLineEditStandard : public QLineEdit
{
public:
    explicit QLineEditStandard(QWidget* parent = nullptr);
    explicit QLineEditStandard(const QString& str, QWidget* parent = nullptr)
        : QLineEdit(str, parent){}

protected:
    void focusInEvent(QFocusEvent* e);
    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent *e);


};

#endif // QLINEEDITSTANDARD_H
