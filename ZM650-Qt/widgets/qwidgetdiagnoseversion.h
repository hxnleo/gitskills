/*
 * qwidgetdiagnoseversion.h
 *
 * 诊断界面的版本信息
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QWIDGETDIAGNOSEVERSION_H
#define QWIDGETDIAGNOSEVERSION_H

#include <QWidget>
#include "data/adt_datatype_qt.h"

class QLabel;

typedef struct _PRODUCT_INFO{
 INT8S pcinfo[18];   //计算机名（前8个字符）
 char name[20];   //产品名
 INT32U id;     //产品ID
 INT8S SN[20];    //产品序列号
}PRODUCT_INFO;

class QWidgetDiagnoseVersion : public QWidget
{
    Q_OBJECT
    /*+++++++++++ 注册数据  +++++++++++*/


public:
    explicit QWidgetDiagnoseVersion(QWidget* parent = nullptr);

protected:
    void _modbusQueryCallback(bool isOk, void* flag)
    {
        emit _signal_modbusQueryCallback(isOk, flag);
    }
protected slots:
    void onModbusQueryCallbackEnd(bool isOk, void* flag);
signals:
    void _signal_modbusQueryCallback(bool isOk, void* flag);

protected: // members
    QLabel* label_ProductID;
    QLabel* label_LibVersion;
    QLabel* label_MotionVersion;
    QLabel* label_FpgaVersion1;
    QLabel* label_FpgaVersion2;
    QLabel* label_registered;
    QLabel* label_BuildDate[2]; // 上下位机
    QLabel* label_SoftwareVersion[2]; // 上下位机
    PRODUCT_INFO registered_array;
    int registered;
};

#endif // QWIDGETDIAGNOSEVERSION_H
