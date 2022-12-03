/*
 * #include "icobutton.cpp"
 * 自定义任意添加图片并设置距离的按钮
 * Author caojing 2019年12月6日
 */
#include "icobutton.h"
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include "data/globaldata.h"
IcoButton::IcoButton(QWidget *parent): QWidget(parent),m_isMousePressed(false),active(false),closeaxis(false)
  ,clear(false),status(false)
{

}

void IcoButton::setActive(bool Ok)
{
    active = Ok;
}


void IcoButton::setText(QString str)
{
    text = str;
    this->update();
}

void IcoButton::setClear(bool ok)
{
    clear = ok;
}

void IcoButton::setNoStatus()
{
    status = true;
}

void IcoButton::init(QString pixmap1,QString pixmap2,QString str,int width,int flags)
{
    relesspath = pixmap1;//释放按钮显示图片
    presspath = pixmap2 ;//按下按钮显示图片
    text = str;//文本
    icowidth = width;//设置图片距离
    option = flags;//设置文本对齐
}
void IcoButton::paintEvent(QPaintEvent *)
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
    if((!m_isMousePressed&&!active))
    {
        painter.setBrush(QColor(168,168,168));//设置画刷填充
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
        if(!clear)
        {
            if(active)//活跃状态
            {
                painter.setPen(QPen(QColor(Qt::black),2));//轮廓
                painter.setBrush(QColor(Qt::red));
                painter.drawEllipse(icowidth+height()*2.0/5/2,height()/4,height()/3+2,height()/3+3);
            }
            else
            {
                painter.setPen(QPen(QColor(Qt::black),2));//轮廓
                painter.drawEllipse(icowidth+height()*2.0/5/2,height()/4,height()/3+2,height()/3+3);
                painter.setPen(QPen(QColor(Qt::gray),3));//轮廓
                painter.drawEllipse(icowidth+height()*2.0/5/2,height()/4,height()/3-1,height()/3);
            }
        }
    }

    else
    {
        //        painter.setBrush(QColor(211,211,211));//设置画刷填充
        painter.setBrush(QColor(168,168,168));//设置画刷填充
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

        if(!clear)
        {
            painter.setPen(QPen(QColor(Qt::black),2));//轮廓
            painter.setBrush(QColor(Qt::red));
            painter.drawEllipse(icowidth+height()*2.0/5/2,height()/4,height()/3+3,height()/3+3);
        }

    }
    //qDebug()<<rect().topLeft()<<rect().bottomRight()<<rect().top()<<rect().bottom();

    //画图标


    painter.setPen(QPen(QColor(Qt::black),1));
    // 绘制区域为当前界面的整个区域（居中）
    painter.drawText(rect(), option, QString(text));
}



// 鼠标事件;
void IcoButton::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    //qDebug()<<event->pos();
}
void IcoButton::mousePressEvent(QMouseEvent *event)
{
    if(closeaxis)
        return;
    QPoint mousePressPoint = event->pos();
    if(mousePressPoint.y()>=rect().top() && rect().right()>=mousePressPoint.x())
    {//按钮被按下
        m_isMousePressed = true;
        update();
        //qDebug()<<"m_isMousePressed"<<m_isMousePressed;
    }
    QWidget::mousePressEvent(event);
}
void IcoButton::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (m_isMousePressed)
    {
        emit clicked();
        m_isMousePressed = false;
        update();
    }
    QWidget::mouseReleaseEvent(event);
}
