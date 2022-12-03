/*
 * qdialogcontrol.cpp
 *
 * 控制对话框
 *
 * Created on: 2020年 2月 17日 星期一 17:21:40 CST
 * Author: caojing
 */
#include "qdialogcontrol.h"
#include "qdialogdataform.h"
#include "data/globaldata.h"
#include "data/utils.h"
#include "data/utilsforcontroller.h"
#include "modbus/modbusaddress.h"
#include "modbus/modbusqueue.h"
#include<QDebug>
QDialogControl::QDialogControl(QWidget* parent,Qt::WindowFlags f):
                       QDialog(parent, f)
{
    setWindowIcon(QIcon(":/images/images/NULL.ico"));
    //setWindowFlags(Qt::Dialog|Qt::CustomizeWindowHint);//获取点不在窗口范围closs

    QGridLayout *Layout = new QGridLayout(this);
    if(GlobalData::screensize == FIFTEEN)
    {
        setContentsMargins(20, 10, 20, 10);
    }
    else
    {
        setContentsMargins(10, 5, 10, 5);
    }
    Layout->setSpacing(10);
    QPushButton *Workbench = new QPushButton(tr("工作台移动"));
    QPushButton *backZero = new QPushButton(tr("工作台归零"));
    QPushButton *fristHole = new QPushButton(tr("到第一个孔"));
    QPushButton *Position = new QPushButton(tr("到指定孔"));
    connect(Workbench, SIGNAL(clicked()), this, SLOT(OnWorkBench()));
    connect(backZero, SIGNAL(clicked()), this, SLOT(OnbackZero()));
    connect(fristHole, SIGNAL(clicked()), this, SLOT(OnfristHole()));
    connect(Position, SIGNAL(clicked()), this, SLOT(OnPosition()));  
    Layout->addWidget(Workbench, 0, 0, 1, 1);
    Layout->addWidget(backZero, 0, 1, 1, 1);
    Layout->addWidget(fristHole, 1, 0, 1, 1);
    Layout->addWidget(Position, 1, 1, 1, 1);
}

void QDialogControl::OnWorkBench()
{//工作台移动
    qDebug()<<"OnWorkBench";
    emit onWorkBench();
    accept();
}

void QDialogControl::OnbackZero()
{//归零
    qDebug()<<"OnbackZero";
    emit onbackZero();
    accept();
}

void QDialogControl::OnfristHole()
{//回到第一孔
    qDebug()<<"OnfristHole";
    emit onfristHole();
    accept();
}

void QDialogControl::OnPosition()
{//定位到孔
    qDebug()<<"OnPosition";
    QProgressDialog* d = nullptr;
    if(true == Utils::checkWorkstatusIfStop(true, &d, this))
    {
        locatePointIndex = GlobalData::realTimeInfo.iCNum;
        QList<QDialogDataForm::item_t> items =
        {
            //{QString::fromUtf8("定位"), Adapters::TYPE_S32, &locatePointIndex, false, 0, GlobalData::realTimeInfo.iMNum - 1.0, nullptr},
            {tr("定位"), Adapters::TYPE_S32, &locatePointIndex, false, 0, GlobalData::realTimeInfo.iMNum - 1.0, nullptr},
        };

        auto w = new QDialogDataForm(this, Qt::FramelessWindowHint);
        connect(w, SIGNAL(accepted()), this, SLOT(OnSendPostion()));
        connect(w, SIGNAL(rejected()), this, SLOT(reject()));
        //w->showDialog(QString::fromUtf8("定位到某一个孔"), items);
        w->showDialog(tr("定位到某一个孔"), items);
    }

}

void QDialogControl::OnSendPostion()
{
    emit onPosition(locatePointIndex);
    accept();
}

void QDialogControl::accept()
{
    QDialog::accept();
}

void QDialogControl::reject()
{
    QDialog::reject();
}
