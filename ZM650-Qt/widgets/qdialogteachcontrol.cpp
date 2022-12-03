/*
 * qdialogteachcontrol.cpp
 *
 * 面板对话框
 *
 * Created on: 2020年 2月 18日 星期一 16:31:40 CST
 * Author: caojing
 */
#include "qdialogteachcontrol.h"
#include "qdialogdataform.h"
#include "qdialogparamoffest.h"
#include "data/globaldata.h"
#include "data/utils.h"
#include "data/utilsforcontroller.h"
#include "widgets/qdialogparamoffest.h"
#include "modbus/modbusaddress.h"
#include "modbus/modbusqueue.h"
#include "Teach.h"
#include<QDebug>
QDialogTeachControl::QDialogTeachControl(QWidget* parent,Qt::WindowFlags f):
    QDialog(parent, f)
{
    setWindowIcon(QIcon(":/images/images/NULL.ico"));
//    setWindowFlags(Qt::Dialog|Qt::CustomizeWindowHint);
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
    QPushButton *MainBackZero = new QPushButton(tr("主轴归零"));
    QPushButton *Drill = new QPushButton(tr("钻头开关"));
    QPushButton *WorkBackZero = new QPushButton(tr("工台归零"));
    QPushButton *MainBrake = new QPushButton(tr("刹车"));
    QPushButton *ParmOffset = new QPushButton(tr("参数偏移"));
    QPushButton *Fixture = new QPushButton(tr("夹具开合"));
    connect(MainBackZero, SIGNAL(clicked()), this, SLOT(OnMainBackZero()));
    connect(Drill, SIGNAL(clicked()), this, SLOT(OnDrill()));
    connect(WorkBackZero, SIGNAL(clicked()), this, SLOT(OnWorkBackZero()));
    connect(MainBrake, SIGNAL(clicked()), this, SLOT(OnMainBrake()));
    connect(ParmOffset, SIGNAL(clicked()), this, SLOT(OnParamOffset()));
    connect(Fixture, SIGNAL(clicked()), this, SLOT(OnFixture()));

    Layout->addWidget(MainBackZero, 0, 0, 1, 1);
    Layout->addWidget(Drill, 0, 1, 1, 1);
    Layout->addWidget(WorkBackZero, 0, 2, 1, 1);
    Layout->addWidget(MainBrake, 1, 0, 1, 1);
    Layout->addWidget(ParmOffset, 1, 1, 1, 1);
    Layout->addWidget(Fixture, 1, 2, 1, 1);
    Layout -> setColumnStretch(1,1);
}

void QDialogTeachControl::OnMainBackZero()
{//主轴归零
//    qDebug()<<"OnMainBackZero";
    emit mainbackzero();
    accept();
}

void QDialogTeachControl::OnDrill()
{//砖头开关
//    qDebug()<<"OnDrill";
    emit drill();
    accept();
}

void QDialogTeachControl::OnWorkBackZero()
{//工作台归零
//    qDebug()<<"OnWorkBackZero";
    emit workbackzero();
    accept();
}

void QDialogTeachControl::OnMainBrake()
{//主轴刹车
//    qDebug()<<"OnMainBrake";
    emit mainbrake();
    accept();
}

void QDialogTeachControl::OnParamOffset()
{//参数偏移
//    qDebug()<<"QDialogParamOffest";
    QDialogParamOffest *dialog = new QDialogParamOffest(this);
    connect(dialog, SIGNAL(rejected()), this, SLOT(reject()));
    connect(dialog, SIGNAL(accepted()), this, SLOT(accept()));
    dialog->show();

}

void QDialogTeachControl::OnFixture()
{//夹具合开
    emit fixture();
    accept();
}


void QDialogTeachControl::accept()
{
    QDialog::accept();
}

void QDialogTeachControl::reject()
{
    QDialog::reject();
}
