/*
 * #include "icobutton.h"
 * 自定义任意添加图片并设置距离的按钮
 * Author caojing 2019年12月6日
 */
#ifndef ICOBUTTON_H
#define ICOBUTTON_H
#include <QWidget>
#include <QEvent>

class IcoButton : public QWidget
{
    Q_OBJECT
public:
    explicit IcoButton(QWidget *parent = nullptr);


    void init(QString pixmap1, QString pixmap2, QString str, int width = 0, int flags = Qt::AlignCenter);
    void setActive(bool Ok);//设置活跃状态
    void setText(QString str);//设置活跃状态
    void setClear(bool ok);//清除图标
    void setNoStatus();//清除图标

    bool m_isMousePressed;
    QString text;
    QString relesspath;
    QString presspath;
    int icowidth;
    int option;
    bool active;
    bool closeaxis;
    bool clear;
    bool status;
private:
    // 绘制按钮;
    void paintEvent(QPaintEvent *);
    // 鼠标事件;
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
protected:

signals:
    void clicked();
public slots:
};

#endif // ICOBUTTON_H

