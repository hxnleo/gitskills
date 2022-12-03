/*
 * qwidgetaxisposition.cpp
 *
 * 实时坐标模块
 *
 * Created on: 2019年 02月 26日 星期二 11:46:59 CST
 * Author: lixingcong
 */

#include "qwidgetaxisposition.h"
#include "data/globaldata.h"
#include "data/utils.h"
#include <QStyleOption>
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <QMouseEvent>
#include <QFontDatabase>

QWidgetAxisPosition::QWidgetAxisPosition(QWidget *parent, bool screenflg, Qt::WindowFlags f):
    QWidget(parent,f),
    widgetLayout(new QVBoxLayout(this)),
    mainLayout(new QGridLayout()),
    timer(new QTimer(this))
{
    //mainLayout->setMargin(0);
    if(!screenflg)
        widgetLayout->addLayout(mainLayout);

    mainLayout->setSpacing(1);
    widgetLayout->setMargin(0);
    widgetLayout->setSpacing(0);
#ifdef _MCGS
    if(GlobalData::fontindex!=-1)
        fontList = QFontDatabase::applicationFontFamilies(GlobalData::fontindex);
#endif

    timer->setSingleShot(true);
    timer->setInterval(300); // 双击间隔
}


void QWidgetAxisPosition::init(int axisCount, const QStringList &axisName,Qt::Alignment align1, Qt::Alignment align2)
{//设置教导界面坐标显示
    QFont font;
    font.setPointSize(8);
    this->axisName=axisName;
    this->axisCount=axisCount;
    //mainLayout->setContentsMargins(20,0,20,0);//设置边距
    const QString suffixNumber=Utils::numberToString(0, 3, 8);
    for(auto i=0;i<axisCount;++i){
        auto l1=new QLabel(axisName.at(i),this);
        l1->setFont(font);
        mainLayout->addWidget(l1,i,0,align1);
        label1Created.append(l1);
        auto l2=new QLabel(suffixNumber,this);
        l2->setFont(font);
        //设置laber对应的边界
        l2->setContentsMargins(0, 0, 15, 0);
        mainLayout->addWidget(l2,i,1,align2);
        label2Created.append(l2);
    }
}

void QWidgetAxisPosition::initScreen(int axisCount, const QStringList &axisName, Qt::Alignment align)
{//设置运动界面主屏显示
    QFont font;
#ifdef _MCGS
    if(GlobalData::fontindex!=-1)
        font.setFamily(fontList.at(0));
#endif
    if(GlobalData::screensize == FIFTEEN)
    {
        if(axisCount==6)
        {
            font.setPointSize(17);
        }
        else if(axisCount==5)
        {
            font.setPointSize(18);
        }
        else if(axisCount>6)
        {
             font.setPointSize(15);
        }
        else
            font.setPointSize(21);

    }
    else if(GlobalData::screensize == TEN)
    {
        if(axisCount==6)
        {
            font.setPointSize(15);
        }
        else if(axisCount==5)
        {
            font.setPointSize(17);
        }
        else if(axisCount>6)
        {
             font.setPointSize(13);
        }
        else
            font.setPointSize(21);
    }
    else
    {
        if(axisCount==6)
        {
            font.setPointSize(13);
        }
        else if(axisCount==5)
        {
            font.setPointSize(15);
        }
        else if(axisCount>6)
        {
             font.setPointSize(11);
        }
        else
            font.setPointSize(17);
    }
    this->axisName=axisName;
    this->axisCount=axisCount;
    const QString suffixNumber=Utils::numberToString(0, 2, 7);
    widgetLayout->addStretch();//首尾加弹簧
    for(auto i=0;i<axisCount;++i){
        QHBoxLayout *layout = new QHBoxLayout();//中间使用水平布局实现
        //        auto l0=new QLabel(this);
        //        l0->setFont(font);
        //        layout->addWidget(l0,1,align);//设置居中
        //        label0Created.append(l0);
        auto l1=new QLabel(axisName.at(i),this);
        l1->setFont(font);
        layout->addWidget(l1,1,align);//布件显示位子
        label1Created.append(l1);
        auto l2=new QLabel(suffixNumber,this);
        l2->setFont(font);
        if(GlobalData::screensize == FIFTEEN&&axisCount<=4)
            l2->setContentsMargins(0, 0, 15, 0);
        else
            l2->setContentsMargins(0, 0, 30, 0);
        layout->addWidget(l2,2,align);
        label2Created.append(l2);
        widgetLayout->addLayout(layout);//添加到主窗口中
    }
    widgetLayout->addStretch();
}

void QWidgetAxisPosition::setLayoutSpacing(int spacingH, int spacingV)
{
    mainLayout->setHorizontalSpacing(spacingH);
    mainLayout->setVerticalSpacing(spacingV);
}

void QWidgetAxisPosition::setLabelCSS(const QString &css)
{
    for(auto i=0;i<axisCount;++i){
        //        label0Created.at(i)->setStyleSheet(css);//归零标志
        label1Created.at(i)->setStyleSheet(css);//轴显示
        label2Created.at(i)->setStyleSheet(css);//轴位置
    }
}

void QWidgetAxisPosition::updatePostion()
{
    qreal fpos;
    for(auto i=0;i<axisCount;++i){
        //qDebug()<<GlobalData::realTimeInfo.fPos[i];
        //        if(GlobalData::backZeroIcon&&(GlobalData::backZeroFlag == true))
        //        {//归零完成图片显示
        //            if(GlobalData::realTimeInfo.fPos[i]==0)
        //            {
        //                //qDebug()<<"设置图标";
        //                QString temppath=":/images/images/backzero.ico";
        //                QImage image,result;
        //                image.load(temppath);//temppath为图片的路径
        //                result = image.scaled(30, 30,Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        //                label0Created.at(i)->setPixmap(QPixmap::fromImage(result));//在Label控件上显示图片
        //            }
        //            else
        //            {
        //                label0Created.at(i)->clear();
        //            }
        //        }
        if(GlobalData::realTimeInfo.fPos[i]>=0)
        {
            label2Created.at(i)->setText("﹢ "+Utils::numberToString(GlobalData::realTimeInfo.fPos[GlobalData::axis[i]], 3, 8));
        }
        else
        {
            fpos=-GlobalData::realTimeInfo.fPos[GlobalData::axis[i]];
            label2Created.at(i)->setText("﹣ "+Utils::numberToString(fpos, 3, 8));
        }
    }
}

void QWidgetAxisPosition::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(event);
}

void QWidgetAxisPosition::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons()&Qt::LeftButton) //鼠标左键按下的同时移动鼠标
    {
        if(timer->isActive())
        {
            // 双击
            timer->stop();
            emit doubleClicked();
        }else
        {
            timer->start();
        }
    }

    QWidget::mousePressEvent(event);
}
void QWidgetAxisPosition::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
}
