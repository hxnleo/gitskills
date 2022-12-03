/*
 * #include "textbutton.cpp"
 * 自定义任意添加图片并设置距离的按钮
 * Author caojing 2019年12月6日
 */
#include "textbutton.h"
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include "data/globaldata.h"
textButton::textButton(QWidget *parent): QWidget(parent),m_isMousePressed(false),active(false)
{

}
void textButton::setActive(bool ok)
{
    active = ok;
}
void textButton::setText(QString str)
{
    text = str;
    this->update();
}

void textButton::init(const QColor &color1, const QColor &color2, QString str1, QString str2, int width, int flags)
{
    this->color1 = color1;
    this->color2 = color2;
    text1 = str1;//释放按钮显示图片
    text2 = str2 ;//按下按钮显示图片
    icowidth = width;//设置图片距离
    option = flags;//设置文本对齐
}

void textButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    // 反走样
    painter.setRenderHint(QPainter::Antialiasing, true);
    // 绘制图标

    painter.setPen(QPen(QColor(168,168,168)));//轮廓
    //图形主体 x右边增大，y右边增大
    QPolygon elepolygon0;//保存点的个数
    elepolygon0<<QPoint(rect().left()+2,rect().top()+2);
    elepolygon0<<QPoint(rect().left()+2,rect().bottom());
    elepolygon0<<rect().bottomRight();
    elepolygon0<<QPoint(rect().right(),rect().top()+2);

    //阴影绘制
    if(!m_isMousePressed)
    {
        painter.setBrush(QColor(176,180,248));//设置画刷填充
        painter.drawPolygon(elepolygon0);//设置范围点
        painter.setPen(QPen(QColor(168,168,168),1));//轮廓
        painter.drawLine(rect().left(),rect().top(),rect().right(), rect().top());
        painter.drawLine(rect().left(),rect().top(),rect().left(), rect().bottom());
        painter.setPen(QPen(QColor(248,252,248),1));//轮廓
        painter.drawLine(rect().left()+1,rect().top()+1,rect().right()-1, rect().top()+1);
        painter.drawLine(rect().left()+1,rect().top()+1,rect().left()+1, rect().bottom()-1);

        painter.setPen(QPen(QColor(96,96,96),1));//轮廓
        painter.drawLine(rect().left()+2,rect().bottom()-1,rect().right()-1, rect().bottom()-1);
        painter.drawLine(rect().right()-1,rect().top()+2,rect().right()-1, rect().bottom()-1);
        painter.setPen(QPen(QColor(0,0,0),1));//轮廓
        painter.drawLine(rect().left()+3,rect().bottom(),rect().right(), rect().bottom());
        painter.drawLine(rect().right(),rect().top()+3,rect().right(), rect().bottom());

        //        if(active)//活跃状态
        //            painter.drawPixmap(icowidth,height()/6,height()*2/7+icowidth,height()*2/3, QPixmap(presspath));//起始X Y 结束X Y
        //        else
        //            painter.drawPixmap(icowidth,height()/6,height()*2/7+icowidth,height()*2/3, QPixmap(relesspath));//起始X Y 结束X Y
    }
    else
    {
        painter.setBrush(QColor(85, 170, 255));//设置画刷填充
        painter.drawPolygon(elepolygon0);//设置范围点
        painter.setPen(QPen(QColor(0,0,0),2));//轮廓
        painter.drawLine(rect().left(),rect().top(),rect().right(), rect().top());
        painter.drawLine(rect().left(),rect().top(),rect().left(), rect().bottom());
        painter.setPen(QPen(QColor(96,96,96),2));//轮廓
        painter.drawLine(rect().left()+2,rect().top()+2,rect().right()-2, rect().top()+2);
        painter.drawLine(rect().left()+2,rect().top()+2,rect().left()+2, rect().bottom()-2);
        painter.setPen(QPen(QColor(248,252,248),1));//轮廓
        painter.drawLine(rect().left()+3,rect().bottom()-1,rect().right()-1, rect().bottom()-1);
        painter.drawLine(rect().right()-1,rect().top()+3,rect().right()-1, rect().bottom()-1);
        painter.setPen(QPen(QColor(168,168,168),1));//轮廓0,0,0
        painter.drawLine(rect().left()+3,rect().bottom(),rect().right(), rect().bottom());
        painter.drawLine(rect().right(),rect().top()+3,rect().right(), rect().bottom());

        //painter.drawPixmap(icowidth,height()/6,height()*2/7+icowidth,height()*2/3, QPixmap(presspath));//起始X Y 结束X Y
    }
    //qDebug()<<rect().topLeft()<<rect().bottomRight()<<rect().top()<<rect().bottom();

    //绘制文字
    painter.setPen(QPen(QColor(Qt::black),1));
    painter.drawText(rect(), option, QString("|"));

    if(!active)
    {
//        painter.setPen(QPen(QColor(color1),1));
//        painter.drawText(rect(), Qt::AlignVCenter|Qt::AlignLeft, QString(text1));
//        painter.setPen(QPen(QColor(color2),1));
//        painter.drawText(rect(), Qt::AlignVCenter|Qt::AlignRight, QString(text2));
        painter.setPen(QPen(QColor(color1),1));
        painter.drawText(rect().left()+4,rect().bottom()*3/4, QString(text1));
        painter.setPen(QPen(QColor(color2),1));
        painter.drawText(rect().right()*3/5,rect().bottom()*3/4, QString(text2));
    }
    else
    {
        painter.setPen(QPen(QColor(color2),1));
        painter.drawText(rect().left()+4,rect().bottom()*3/4, QString(text1));
        painter.setPen(QPen(QColor(color1),1));
        painter.drawText(rect().right()*3/5,rect().bottom()*3/4, QString(text2));
    }
}

// 鼠标事件;
void textButton::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    //qDebug()<<event->pos();
}
void textButton::mousePressEvent(QMouseEvent *event)
{
    QPoint mousePressPoint = event->pos();
    if(mousePressPoint.y()>=rect().top() && rect().right()>=mousePressPoint.x())
    {//按钮被按下
        m_isMousePressed = true;
        update();
        qDebug()<<m_isMousePressed;
    }
    QWidget::mousePressEvent(event);
}
void textButton::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (m_isMousePressed)
    {
        m_isMousePressed = false;
        update();
        emit clicked();
    }
    QWidget::mouseReleaseEvent(event);
}
