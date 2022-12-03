/*
 * qdialogparams_password.cpp
 *
 * 参数界面的密码修改弹出对话框
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qdialogparams_password.h"
#include <QGridLayout>
#include <QLabel>
#include <QIcon>
#include <QDialogButtonBox>
#include "data/adapters.h"
#include "data/utils.h"
#include <QMessageBox>

const QStringList QDialogParams_Password::labelText =
{
//    QString::fromUtf8("旧密码"),
//    QString::fromUtf8("新密码"),
//    QString::fromUtf8("确认新密码")
    //mod by yzg
        QObject::trUtf8("旧密码"),
        QObject::trUtf8("新密码"),
        QObject::trUtf8("确认新密码")
};

QDialogParams_Password::QDialogParams_Password(Adapters::DataAdapter_ParamTable* adapter, int offset, QWidget* parent,
        Qt::WindowFlags f):
    QDialog(parent, f),
    adapter(adapter),
    offset(offset)
{
    setWindowIcon(QIcon(":/images/images/NULL.ico"));
    data = adapter->getData(offset).toUInt();
    auto gridLayout = new QGridLayout(this);

    for(auto i = 0; i < 3; ++i)
    {
        //auto label = new QLabel(labelText.at(i), this);
        auto label = new QLabel(QObject::tr(labelText.at(i).toUtf8()), this);
        gridLayout->addWidget(label, i, 0, 1, 1);

        lineEdit[i] = new QLineEditStandard(this);
//        lineEdit[i]->init(nullptr, Adapters::TYPE_U32, 0.0, 999999999.0);
        lineEdit[i]->setPlaceholderText("0~999999999");
        lineEdit[i]->setEchoMode(QLineEdit::Password);
        gridLayout->addWidget(lineEdit[i], i, 1, 1, 1);
    }

    auto buttonBox = Utils::createOkCancelButtons(this);
    buttonBox->setFocus();
    gridLayout->addWidget(buttonBox, 3, 0, 1, 2);
}

QString QDialogParams_Password::getStringFromData(const QVariant& /*val*/)
{
    return QString("******");
}

void QDialogParams_Password::accept()
{
    bool isOk = true;

    if(lineEdit[0]->text().toUInt() != data)
    {
        isOk = false;
        Utils::MyMessageBox(QString("Error"), tr("旧密码错误"),QObject::tr("确定"),this);
    }

    if(lineEdit[1]->text().toUInt() != lineEdit[2]->text().toUInt())
    {
        isOk = false;
        Utils::MyMessageBox(QString("Error"), tr("新密码不一致"),QObject::tr("确定"),this);
    }

    if(0 == lineEdit[1]->text().length())
    {
        isOk = false;
        Utils::MyMessageBox(QString("Error"), tr("新密码不能空"),QObject::tr("确定"),this);
    }

    if(isOk)
    {
        data = lineEdit[1]->text().toUInt();
        adapter->setData(offset, data);
        QDialog::accept();
    }
}
