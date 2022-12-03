/*
 * qdialogparams_password.h
 *
 * 参数界面的密码修改弹出对话框
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QDIALOGPARAMS_PASSWORD_H
#define QDIALOGPARAMS_PASSWORD_H

#include <QDialog>
#include <QKeyEvent>
#include "qlineeditstandard.h"
#include "data/adapters.h"

class QDialogParams_Password : public QDialog
{
    Q_OBJECT
public:
    explicit QDialogParams_Password(Adapters::DataAdapter_ParamTable* adapter, int offset, QWidget* parent = nullptr,
                                    Qt::WindowFlags f = 0);

    static QString getStringFromData(const QVariant& val);

protected:
    unsigned int data;
    Adapters::DataAdapter_ParamTable* adapter;
    int offset;

    QLineEditStandard* lineEdit[3];

    static const QStringList labelText;

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

#endif // QDIALOGPARAMS_PASSWORD_H
