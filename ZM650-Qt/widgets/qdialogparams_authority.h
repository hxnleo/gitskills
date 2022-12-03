/*
 * qdialogparams_authority.h
 *
 * 获取参数修改权限对话框
 *
 * Created on: 2020年 07月 3日 星期一 14:57:40 CST
 * Author: caojing
 */
#ifndef QDIALOGPARAMS_AUTHORITY_H
#define QDIALOGPARAMS_AUTHORITY_H

#include <QDialog>
#include <QLabel>
#include <QKeyEvent>
#include <QRadioButton>
#include "qlineeditstandard.h"
#include "data/adapters.h"

class QDialogParams_Authority : public QDialog
{
    Q_OBJECT
public:
    explicit QDialogParams_Authority(Adapters::DataAdapter_ParamTable* adapter, int offset, QWidget* parent = nullptr,
                                    Qt::WindowFlags f = 0);

    static QString getStringFromData(const QVariant& val);

protected:
    unsigned int data;
    Adapters::DataAdapter_ParamTable* adapter;
    int offset;

    QRadioButton* radioButton[3];
    QLineEditStandard* passedit;
    static const QStringList labelText;

public slots:
    void accept();
    void onCheckbtn();

protected:
    void keyPressEvent(QKeyEvent* evt)
    {
        if(evt->key() == Qt::Key_Enter || evt->key() == Qt::Key_Return)
            return;

        QDialog::keyPressEvent(evt);
    }
};
#endif // QDIALOGPARAMS_AUTHORITY_H
