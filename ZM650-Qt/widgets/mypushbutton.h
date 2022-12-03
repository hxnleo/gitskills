/*
 * mypushbutton.h
 *
 * 重写按钮控件用于获取按钮是否按下
 *
 * Created on: 2020年 08月 12日 星期二 18:46:59 CST
 * Author: caojing
 */

#ifndef MYPUSHBUTTON_H
#define MYPUSHBUTTON_H

#include<QPushButton>
#include<QKeyEvent>

class MyPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit MyPushButton(QWidget *parent = nullptr);


signals:

public slots:


protected:
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;


};

#endif // MYPUSHBUTTON_H
