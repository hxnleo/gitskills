/*
 * qdialogclampout.cpp
 * 
 * 控制夹具输出
 * 
 * Created on: 2019年 02月 26日 星期二 11:46:59 CST
 * Author: lixingcong
 */

#include "qdialogclampout.h"
#include <QTimer>
#include <QIcon>
#include "data/globaldata.h"

QDialogClampOut::QDialogClampOut(int cylinderCount, bool type, QWidget *parent, Qt::WindowFlags f):
    QDialogTeachCylinder (cylinderCount,parent,f,type)
{
    clampBitArray=QBitArray(cylinderCount);

    setWindowIcon(QIcon(":/images/images/NULL.ico"));
    auto timer=new QTimer(this);
    connect(timer, SIGNAL(timeout()),
            this, SLOT(onTimeout()));
    timer->setInterval(100); // 更新间隔
    timer->start();
}

void QDialogClampOut::onTimeout()
{
    for(auto i = 0; i < cylinderCount; ++i)
        clampBitArray.setBit(i, (GlobalData::realTimeInfo.iClamp[i] > 0));

    setBitArray(clampBitArray);//设置开关状态(包括了颜色设置)
}
