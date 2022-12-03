/*
 * qdialogparams_limit.h
 *
 * 参数界面的限位配置弹出对话框
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QDIALOGPARAMS_LIMIT_H
#define QDIALOGPARAMS_LIMIT_H

#include <QDialog>
#include <QRadioButton>
#include <QGroupBox>
#include "data/adapters.h"

class QDialogParams_Limit : public QDialog
{
    Q_OBJECT
public:
    explicit QDialogParams_Limit(Adapters::DataAdapter_ParamTable* adapter, int offset, QWidget* parent = nullptr,
                                 Qt::WindowFlags f = 0);

    static QString getStringFromData(const QVariant& val);

protected:
    unsigned short data;
    Adapters::DataAdapter_ParamTable* adapter;
    int offset;

    QGroupBox* groupBox[2];
    static const QStringList groupBoxText;

    QRadioButton* radioButton[2][3];
    static const QStringList radioText;


public slots:
    void accept();
};

#endif // QDIALOGPARAMS_LIMIT_H
