/*
 * qpushbuttonlongpress.h
 *
 * 带短按长按事件的QPushButton，用于教导界面单轴移动
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QPUSHBUTTONLONGPRESS_H
#define QPUSHBUTTONLONGPRESS_H

#include <QObject>
#include <QPushButton>
#include <QTimer>

class QPushButtonLongPress : public QPushButton
{
    Q_OBJECT
public:
    explicit QPushButtonLongPress(QWidget *parent=0,const QString& text = "");
    ~QPushButtonLongPress();

    void setKeyPressDelay(int delay);

    enum PressType
    {
        PressShort,
        PressLongBegin,
        PressLongEnd
    };

signals:
    void keyPress(int pressType);

private slots:
    void onTimeout();
    void onPressed();
    void onReleased();

private:
    bool isLongPress;
    QTimer t;
};

#endif // QPUSHBUTTONLONGPRESS_H
