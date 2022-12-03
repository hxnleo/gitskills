/*
 * qdialogoffset.h
 *
 * XY轴偏移对话框对话框
 *
 * Created on: 2019年 10月 22日 星期一 17:02:40 CST
 * Author: caojing
 */
#include "data/globaldata.h"
#include "data/uistyles.h"
#include "data/utils.h"
#include "qdialogoffset.h"
#include "qpushbuttonlongpress.h"
#include <QDebug>
#include<QGridLayout>

#define PROP_AXISNUM "axisNum"
#define PROP_DIRECTION "direction"
QDialogOffset::QDialogOffset(int first, int last, int type,  QWidget* parent, Qt::WindowFlags f):
    QDialog(parent, f),last(last),temp(0),type(type)
{
    setWindowIcon(QIcon(":/images/images/NULL.ico"));
    this->setWindowTitle(tr("偏移设置"));

    QVBoxLayout *VLayout = new QVBoxLayout(this);
    QGridLayout *GLayout = new QGridLayout();


    if(type==OType::One||type==OType::Three)
    {//区分 种类
        for(auto i = first-1; i < last; ++i)
        {//显示第几轴到
            auto *label = new QLabel(GlobalData::axisName.at(i)+tr("轴位置"));
            QLabel *labeltext  =  new QLabel(Utils::numberToString(GlobalData::realTimeInfo.fPos[temp], 2, 6));

            label0.append(labeltext);
            auto btn_plus  = new QPushButtonLongPress(this,GlobalData::axisName.at(i) + "﹢");
            auto btn_minus = new QPushButtonLongPress(this,GlobalData::axisName.at(i) + "﹣");
            btn_plus->setMinimumWidth(100);
            btn_minus->setMinimumWidth(100);
            //label1->setAlignment(Qt::AlignHCenter);


            GLayout->addWidget(label,temp,0,1,2);
            GLayout->addWidget(labeltext,temp,2,1,1);
            GLayout->addWidget(btn_plus,temp,3,1,1);
            GLayout->addWidget(btn_minus,temp,4,1,1);
            temp++;
            //stylesheet
            btn_plus->setStyleSheet(UIStyles::css_teachcolorconf_u0);
            btn_minus->setStyleSheet(UIStyles::css_teachcolorconf_u0);

            // extra info
            btn_plus->setProperty(PROP_AXISNUM, GlobalData::axis[i] + 1);
            btn_minus->setProperty(PROP_AXISNUM, GlobalData::axis[i] + 1);
            btn_plus->setProperty(PROP_DIRECTION, 0);
            btn_minus->setProperty(PROP_DIRECTION, 1);

            connect(btn_plus, SIGNAL(keyPress(int)), this, SLOT(_onMoveButtonPress(int)));
            connect(btn_minus, SIGNAL(keyPress(int)), this, SLOT(_onMoveButtonPress(int)));
        }
        if(type==OType::Three)
        {
            auto *label = new QLabel(GlobalData::axisName.at(GlobalData::axisName.size()-2)+tr("轴位置"));
            QLabel *labeltext  =  new QLabel(Utils::numberToString(GlobalData::realTimeInfo.fPos[GlobalData::axisName.size()-1], 2, 6));

            label0.append(labeltext);
            auto btn_plus  = new QPushButtonLongPress(this,GlobalData::axisName.at(GlobalData::axisName.size()-2) + "﹢");
            auto btn_minus = new QPushButtonLongPress(this,GlobalData::axisName.at(GlobalData::axisName.size()-2) + "﹣");
            btn_plus->setMinimumWidth(100);
            btn_minus->setMinimumWidth(100);
            //label1->setAlignment(Qt::AlignHCenter);


            GLayout->addWidget(label,temp,0,1,2);
            GLayout->addWidget(labeltext,temp,2,1,1);
            GLayout->addWidget(btn_plus,temp,3,1,1);
            GLayout->addWidget(btn_minus,temp,4,1,1);
            temp++;
            //stylesheet
            btn_plus->setStyleSheet(UIStyles::css_teachcolorconf_u0);
            btn_minus->setStyleSheet(UIStyles::css_teachcolorconf_u0);

            // extra info
            btn_plus->setProperty(PROP_AXISNUM, GlobalData::axis[GlobalData::axisName.size()-2] + 1);
            btn_minus->setProperty(PROP_AXISNUM, GlobalData::axis[GlobalData::axisName.size()-2] + 1);
            btn_plus->setProperty(PROP_DIRECTION, 0);
            btn_minus->setProperty(PROP_DIRECTION, 1);

            connect(btn_plus, SIGNAL(keyPress(int)), this, SLOT(_onMoveButtonPress(int)));
            connect(btn_minus, SIGNAL(keyPress(int)), this, SLOT(_onMoveButtonPress(int)));
        }
    }
    if(type==OType::Two)
    {//毛道定制  坑！
        for(auto i = first-1; i < last; ++i)
        {//显示第几轴到
            auto *label = new QLabel(tr("毛道")+QString("%1").arg(i+1));
            QLabel *labeltext  =  new QLabel(Utils::numberToString(0, 2, 6));

            label0.append(labeltext);
            //label1->setAlignment(Qt::AlignHCenter);
            GLayout->addWidget(label,temp,0,1,2);
            GLayout->addWidget(labeltext,temp,2,1,1);
            temp++;

        }
        auto btn_plus1  = new QPushButtonLongPress(this,QString("POS") + "﹢");
        auto btn_minus1 = new QPushButtonLongPress(this,QString("POS") + "﹣");
        auto btn_plus2  = new QPushButtonLongPress(this,QString("POS") + "﹢");
        auto btn_minus2 = new QPushButtonLongPress(this,QString("POS") + "﹣");
        auto btn_plus3  = new QPushButtonLongPress(this,QString("POS") + "﹢");
        auto btn_minus3 = new QPushButtonLongPress(this,QString("POS") + "﹣");

        //stylesheet
        btn_plus1->setStyleSheet(UIStyles::css_teachcolorconf_u0);
        btn_minus1->setStyleSheet(UIStyles::css_teachcolorconf_u0);
        btn_plus2->setStyleSheet(UIStyles::css_teachcolorconf_u0);
        btn_minus2->setStyleSheet(UIStyles::css_teachcolorconf_u0);
        btn_plus3->setStyleSheet(UIStyles::css_teachcolorconf_u0);
        btn_minus3->setStyleSheet(UIStyles::css_teachcolorconf_u0);

        GLayout->addWidget(btn_plus1,0,3,1,1);
        GLayout->addWidget(btn_minus1,0,4,1,1);
        GLayout->addWidget(btn_plus2,1,3,1,1);
        GLayout->addWidget(btn_minus2,1,4,1,1);
        GLayout->addWidget(btn_plus3,2,3,1,1);
        GLayout->addWidget(btn_minus3,2,4,1,1);
        // extra info
        btn_plus1->setProperty(PROP_AXISNUM, GlobalData::axis[2] + 1);
        btn_minus1->setProperty(PROP_AXISNUM, GlobalData::axis[2] + 1);
        btn_plus1->setProperty(PROP_DIRECTION, 0);
        btn_minus1->setProperty(PROP_DIRECTION, 1);
        btn_plus2->setProperty(PROP_AXISNUM, GlobalData::axis[2] + 1);
        btn_minus2->setProperty(PROP_AXISNUM, GlobalData::axis[2] + 1);
        btn_plus2->setProperty(PROP_DIRECTION, 0);
        btn_minus2->setProperty(PROP_DIRECTION, 1);
        btn_plus3->setProperty(PROP_AXISNUM, GlobalData::axis[2] + 1);
        btn_minus3->setProperty(PROP_AXISNUM, GlobalData::axis[2] + 1);
        btn_plus3->setProperty(PROP_DIRECTION, 0);
        btn_minus3->setProperty(PROP_DIRECTION, 1);
        connect(btn_plus1, SIGNAL(keyPress(int)), this, SLOT(_onMoveButtonPress(int)));
        connect(btn_minus1, SIGNAL(keyPress(int)), this, SLOT(_onMoveButtonPress(int)));
        connect(btn_plus2, SIGNAL(keyPress(int)), this, SLOT(_onMoveButtonPress(int)));
        connect(btn_minus2, SIGNAL(keyPress(int)), this, SLOT(_onMoveButtonPress(int)));
        connect(btn_plus3, SIGNAL(keyPress(int)), this, SLOT(_onMoveButtonPress(int)));
        connect(btn_minus3, SIGNAL(keyPress(int)), this, SLOT(_onMoveButtonPress(int)));

        connect(btn_plus1, SIGNAL(keyPress(int)), this, SLOT(_onMoveColor1(int)));
        connect(btn_minus1, SIGNAL(keyPress(int)), this, SLOT(_onMoveColor1(int)));
        connect(btn_plus2, SIGNAL(keyPress(int)), this, SLOT(_onMoveColor2(int)));
        connect(btn_minus2, SIGNAL(keyPress(int)), this, SLOT(_onMoveColor2(int)));
        connect(btn_plus3, SIGNAL(keyPress(int)), this, SLOT(_onMoveColor3(int)));
        connect(btn_minus3, SIGNAL(keyPress(int)), this, SLOT(_onMoveColor3(int)));
    }


    GLayout->setSpacing(10);
    QPushButton *close = new QPushButton(tr("否"));
    QPushButton *enter = new QPushButton(tr("是"));

    connect(close, SIGNAL(clicked()), this, SLOT(reject()));
    connect(enter, SIGNAL(clicked()), this, SLOT(accept()));

    QHBoxLayout *HLayout = new QHBoxLayout();
    HLayout->addWidget(enter);
    HLayout->addWidget(close);

    VLayout->addLayout(GLayout);
    VLayout->addLayout(HLayout);

    QTimer *timer = new QTimer(this);
    //关联定时器计满信号和相应的槽函数
    connect(timer,SIGNAL(timeout()),this,SLOT(timerUpDate()));
    //定时器开始计时，其中1000表示1000ms即1秒
    timer->start(50);
}

void QDialogOffset::_onMoveButtonPress(int pressType)
{
    auto axis = qvariant_cast<int>(sender()->property(PROP_AXISNUM));
    auto direction = qvariant_cast<int>(sender()->property(PROP_DIRECTION));


    //轴号 位置 按下种类 移动距离
    if(type==OType::One||type==OType::Three)
        emit moveAxis(axis, direction, pressType, GlobalData::TEACH_STEPS[GlobalData::stepIndex]);
    else
        emit moveAxis(3, direction, pressType, GlobalData::TEACH_STEPS[GlobalData::stepIndex]);
}

void QDialogOffset::_onMoveColor1(int pressType)
{
    label0.at(0)->setText(Utils::numberToString(GlobalData::realTimeInfo.fPos[2], 2, 6));
    GlobalData::Color1 = GlobalData::realTimeInfo.fPos[2];
    qDebug()<<GlobalData::Color1;
}
void QDialogOffset::_onMoveColor2(int pressType)
{
    label0.at(1)->setText(Utils::numberToString(GlobalData::realTimeInfo.fPos[2], 2, 6));
    GlobalData::Color2 = GlobalData::realTimeInfo.fPos[2];
    qDebug()<<GlobalData::Color2;
}
void QDialogOffset::_onMoveColor3(int pressType)
{
    label0.at(2)->setText(Utils::numberToString(GlobalData::realTimeInfo.fPos[2], 2, 6));
    GlobalData::Color3 = GlobalData::realTimeInfo.fPos[2];
    qDebug()<<GlobalData::Color3;
}


void QDialogOffset::timerUpDate()
{
    //qDebug()<<"size"<<label0.size()<<"tmep"<<temp;
    if(type==OType::One)
    {
        for(int i=0;i<temp;i++)
        {
            label0.at(i)->setText(Utils::numberToString(GlobalData::realTimeInfo.fPos[i], 2, 6));

        }
    }
    if(type==OType::Three)
    {

        label0.last()->setText(Utils::numberToString(GlobalData::realTimeInfo.fPos[GlobalData::axisName.size()-2], 2, 6));
    }
}

void QDialogOffset::accept()
{
    //qDebug()<<edit->text();
    //GlobalData::renewEdit = edit->text();
    QDialog::accept();
}

void QDialogOffset::reject()
{
    QDialog::reject();
}
