/*
 * #include "icobutton.h"
 * 自定义任意添加图片并设置距离的按钮
 * Author caojing 2019年12月6日
 */
#ifndef TEXTBUTTON_H
#define TEXTBUTTON_H
#include <QWidget>
#include <QEvent>

class textButton : public QWidget
{
    Q_OBJECT
public:
    explicit textButton(QWidget *parent = nullptr);


    void init(const QColor &color1, const QColor &color2, QString str1, QString str2,
              int width = 0, int flags = Qt::AlignCenter);
    void setText(QString str);//设置文本
    void setActive(bool ok);

    bool m_isMousePressed;
    QString text;
    QString text1;
    QString text2;
    int icowidth;
    int option;
    bool active;
    QColor color1;
    QColor color2;
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

#endif // TEXTBUTTON_H

