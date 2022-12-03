/*
 * qdialogclampout.h
 * 
 * 控制夹具输出
 * 
 * Created on: 2019年 02月 26日 星期二 11:46:59 CST
 * Author: lixingcong
 */

#ifndef QDIALOGCLAMPOUT_H
#define QDIALOGCLAMPOUT_H

#include "qdialogteachcylinder.h"

class QBitArray;

class QDialogClampOut : public QDialogTeachCylinder
{
    Q_OBJECT
public:
    QDialogClampOut(int cylinderCount,bool type = 0, QWidget* parent = 0, Qt::WindowFlags f = 0);

protected slots:
    void onTimeout();

protected:
    QBitArray clampBitArray;
};

#endif // QDIALOGCLAMPOUT_H
