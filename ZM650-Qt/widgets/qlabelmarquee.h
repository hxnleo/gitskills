/*
 * qlabelmarquee.h
 *
 * 文字滚动的QLabel
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef _QWIDGETMARQUEELABEL_H_
#define _QWIDGETMARQUEELABEL_H_

#include <QLabel>
#include <QTimer>

// WidgetMarqueeLabel来源：https://www.linux-apps.com/p/1132075
class QLabelMarquee : public QLabel
{
    Q_OBJECT

public: //Member Functions
    enum Direction {LeftToRight, RightToLeft};
    QLabelMarquee(QWidget* parent = 0);
    ~QLabelMarquee();
    void setAlignment(Qt::Alignment);
    int getSpeed();
    void setMarqueeEnable(bool enable);

public slots: //Public Member Slots
    void setSpeed(int s);
    void setDirection(int d);
    void setText(const QString&);

protected: //Member Functions
    void paintEvent(QPaintEvent* evt);
    void resizeEvent(QResizeEvent* evt);
    void updateCoordinates();
    void updateIsForceDisableMarquee();

private: //Data Members
    int px;
    int py;
    QTimer timer;
    Qt::Alignment m_align;
    int speed;
    int direction;
    int fontPointSize;
    int textLength;
    bool marqueeEnable;
    bool forceDisableMarquee;

private slots: //Private Member Slots
    void refreshLabel();
};
#endif /*_QWIDGETMARQUEELABEL_H_*/
