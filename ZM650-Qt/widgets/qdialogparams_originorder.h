/*
 * qdialogparams_originorder.h
 *
 * 参数界面的归零顺序配置弹出对话框
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QDIALOGPARAMS_ORIGINORDER_H
#define QDIALOGPARAMS_ORIGINORDER_H

#include <QDialog>
#include <QCheckBox>
#include <QGroupBox>
#include "data/adapters.h"

class QDialogParams_OriginOrder : public QDialog
{
    Q_OBJECT
public:
    explicit QDialogParams_OriginOrder(Adapters::DataAdapter_ParamTable* adapter, int offset, QWidget* parent = nullptr,
                                       Qt::WindowFlags f = 0);

    static QString getStringFromData(const QVariant& val);

protected:
    unsigned int data;
    QGroupBox* groupBox[4];
    QCheckBox* checkBox[4][16];

    Adapters::DataAdapter_ParamTable* adapter;
    static const QStringList axisName;
    int offset;

public slots:
    void accept();

protected slots:
    void onCheckBoxStateChanged(int);
};

#endif // QDIALOGPARAMS_ORIGINORDER_H
