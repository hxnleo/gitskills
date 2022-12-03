/*
 * qwidgetfieldlayout.h
 *
 * 捆绑两个widget为一个widget
 *
 * Created on: 2019年 01月 26日 星期六 17:05:50 CST
 * Author: lixingcong
 */

#ifndef QWIDGETFIELDLAYOUT_H
#define QWIDGETFIELDLAYOUT_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>

class QWidgetFieldLayout: public QWidget
{
public:
    QWidgetFieldLayout(bool isHorizontal, QWidget* w1, QWidget* w2, QWidget* parent): QWidget(parent)
    {
        QLayout* myLayout = nullptr;

        if(isHorizontal)
            myLayout = new QHBoxLayout(this);
        else
            myLayout = new QVBoxLayout(this);

        myLayout->setMargin(0);
        myLayout->setSpacing(1);

        myLayout->addWidget(w1);
        myLayout->addWidget(w2);
    }

};

#endif // QWIDGETFIELDLAYOUT_H
