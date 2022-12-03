/*
 * qdialogparams_ioconfig.h
 *
 * 参数界面的IO配置弹出对话框
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QDIALOGPARAMS_IOCONFIG_H
#define QDIALOGPARAMS_IOCONFIG_H

#include <QDialog>
#include "data/adapters.h"
#include <Parameter.h>
#include <QRadioButton>
#include <QKeyEvent>
#include "widgets/qlineeditnumber.h"

class QDialogParams_IOConfig : public QDialog
{
    Q_OBJECT
public:
    explicit QDialogParams_IOConfig(Adapters::DataAdapter_IOConfigText* adapter, int offset, QWidget* parent = nullptr,
                                    Qt::WindowFlags f = 0);

    static QString getStringFromData(const QVariant& val);

protected:
    IOConfig ioConfig;
    Adapters::DataAdapter_IOConfigText* adapter;
    int offset;
    QRadioButton* radioButton[3];
    QLineEditNumber* lineEdit;

    static const QStringList radioButtonText;

protected slots:
    void enableLineEdit();

public slots:
    void accept();
protected:
    void keyPressEvent(QKeyEvent* evt)
    {
        if(evt->key() == Qt::Key_Enter || evt->key() == Qt::Key_Return)
            return;

        QDialog::keyPressEvent(evt);
    }
};

#endif // QDIALOGPARAMS_IOCONFIG_H
