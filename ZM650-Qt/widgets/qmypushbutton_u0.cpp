#include "qmypushbutton_u0.h"
#include<QPaintEvent>
#include<QDebug>
#include<QMouseEvent>
#include "data/globaldata.h"


QMyPushButton_U0::QMyPushButton_U0(QWidget *parent) : QPushButton(parent),
    PressOrUpStatus(false)
{

}


void QMyPushButton_U0::setPressOrUp(bool PressOrUp)
{
    PressOrUpStatus=PressOrUp;
}


void QMyPushButton_U0::setImage(QString imagepath_up,QString imagepath_press)
{
    image_path_up=imagepath_up;
    image_Path_press=imagepath_press;

}


void QMyPushButton_U0::mouseReleaseEvent(QMouseEvent *e)
{
    //qDebug()<<"QEvent::MouseButtonRelease";
    if(e->type()==QEvent::MouseButtonRelease)
    {
        qDebug()<<"QEvent::MouseButtonRelease";
        setPressOrUp(false);
        update();
    }
    QPushButton::mouseReleaseEvent(e);
}

void QMyPushButton_U0::mousePressEvent(QMouseEvent *e)
{
    //qDebug()<<"QEvent::MouseButtonPress";
    if(e->type()==QEvent::MouseButtonPress)
    {
        qDebug()<<"QEvent::MouseButtonPress";
        setPressOrUp(true);
        update();
    }
    QPushButton::mousePressEvent(e);
}

void QMyPushButton_U0::paintEvent(QPaintEvent *e)
{
    if(!image_path_up.isEmpty() && !PressOrUpStatus)
    {
        //创建Qpixmap对象
        QPixmap pixmap;

        //加载图片
        bool ret=pixmap.load(image_path_up);
        if(!ret)
        {
            qDebug()<<image_path_up<<"加载图片失败";
        }

        //清除按钮显示的文字，否则图片无法居中显示
        //this->setText("");

        //设置按钮的尺寸为图片的固定尺寸
        //this->setFixedSize(pixmap.width(),pixmap.height());

        //设置不规则样式
        //this->setStyleSheet("QPushButton{border:0px;}");

        //设置图片
        this->setIcon(pixmap);

        //设置图片的尺寸
        //this->setIconSize(QSize(pixmap.width(),pixmap.height()));
        if(GlobalData::screensize == TEN)
            this->setIconSize(QSize(32,32));
        else if(GlobalData::screensize == SEVEN)
            this->setIconSize(QSize(24,24));
    }

    if(!image_Path_press.isEmpty() && PressOrUpStatus)
    {
        QPixmap pixmap;

        bool ret=pixmap.load(image_Path_press);
        if(!ret)
        {
            qDebug()<<image_Path_press<<"加载图片失败";
        }
        //this->setText("");

        //this->setFixedSize(pixmap.width(),pixmap.height());

        //this->setStyleSheet("QPushButton{border:0px;}");

        this->setIcon(pixmap);

        //this->setIconSize(QSize(pixmap.width(),pixmap.height()));
        if(GlobalData::screensize == TEN)
            this->setIconSize(QSize(32,32));
        else if(GlobalData::screensize == SEVEN)
            this->setIconSize(QSize(24,24));

    }

    QPushButton::paintEvent(e);
}



void QMyPushButton_U0::setActive(bool status)
{
    if(!image_path_up.isEmpty() && status)
    {
        setPressOrUp(true);
        update();
    }
    else
        if(!image_Path_press.isEmpty())
        {
            setPressOrUp(false);
            update();
        }
}














