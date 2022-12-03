/*
 * qdialogparams_axisswitch.h
 *
 * 参数界面的轴开关弹出对话框
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QDIALOGPARAMS_AXISSWITCH_H
#define QDIALOGPARAMS_AXISSWITCH_H

#include <QDialog>
#include <QCheckBox>
#include "data/adapters.h"

class QDialogParams_AxisSwitch : public QDialog
{
    Q_OBJECT
public:
    explicit QDialogParams_AxisSwitch(Adapters::DataAdapter_ParamTable* adapter, int offset, QWidget* parent = nullptr,
                                      Qt::WindowFlags f = 0);

    static QString getStringFromData(const QVariant& val);

protected:
    QCheckBox* checkBox[16];
    Adapters::DataAdapter_ParamTable* adapter;
    int offset;
    unsigned short data;

public slots:
    void accept();
};

#endif // QDIALOGPARAMS_AXISSWITCH_H
