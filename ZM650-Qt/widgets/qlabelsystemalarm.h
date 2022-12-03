/*
 * qlabelsystemalarm.h
 *
 * 支持多种css样式的QLabel，用于系统报警栏
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QLABELSYSTEMALARM_H
#define QLABELSYSTEMALARM_H

#include <QLabel>
#include <QTimer>

class QLabelSystemAlarm : public QLabel
{
    Q_OBJECT
public:
    struct BlinkStyleType
    {
        bool isBlink;
        QString cssStyle1;
        QString cssStyle2;
    };
public:
    QLabelSystemAlarm(QWidget* parent = nullptr);

    void init(const QList<BlinkStyleType>& blinkStyles);
    void setBlinkFrequency(int changeTime);
    void setBlinkStyle(int styleIndex);

signals:
    void clicked();
private slots:
#if 0
    void onClicked();
#endif
    void onTimeOut();

protected:
    QList<BlinkStyleType> blinkStyles;
    QTimer timer;
    int blinkStyleIndex;
    bool isStyle1;

protected:
    void mousePressEvent(QMouseEvent* event);
    void changeStyle();
};

#endif // QLABELSYSTEMALARM_H
