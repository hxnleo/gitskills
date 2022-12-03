/*
 * qdialogpassword.cpp
 * 登录加密狗界面
 *
 * Created on: 2019年 10月 22日 星期一 17:02:40 CST
 * Author: caojing
 */
#include "qdialogpassword.h"
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include "data/dog.h"
#include "data/utils.h"
#include "qlineeditstandard.h"
#include <QMessageBox>

QDialogPassword::QDialogPassword(QWidget* parent,Qt::WindowFlags f,int minVal
                                 ,int maxVal):
    QDialog(parent, f),password(new QLineEditStandard(this)),pwd(0),minVal(minVal),maxVal(maxVal)
{
    this->setWindowTitle(tr("登入加密狗"));
    setWindowIcon(QIcon(":/images/images/NULL.ico"));
    QGridLayout* layout = new QGridLayout(this);
    QLabel* line0 = new QLabel(tr("厂商序列号:"));
    QLabel* line1 = new QLabel(QString());
    line1->setAlignment(Qt::AlignHCenter);

    line1->setText(QString("%1").arg(QDog::getInstance()->m_dog.Factory_SerialNo));
    QLabel* line2 = new QLabel(tr("管理密码:"));
    QPushButton *close = new QPushButton(tr("取消"));
    QPushButton *enter = new QPushButton(tr("确定"));

    connect(close, SIGNAL(pressed()), this, SLOT(reject()));
    connect(enter, SIGNAL(pressed()), this, SLOT(accept()));

    password->setFixedWidth(180);
    close->setFixedWidth(100);
    enter->setFixedWidth(100);


    password->setPlaceholderText(tr("输入密码"));
    password->setEchoMode(QLineEdit::Password);
    layout->addWidget(line0,0,0,1,1);
    layout->addWidget(line1,0,1,1,1);
    layout->addWidget(line2,1,0,1,1,Qt::AlignCenter);
    layout->addWidget(password,1,1,1,1);
    layout->addWidget(enter,2,0,Qt::AlignLeft);
    layout->addWidget(close,2,1,Qt::AlignRight);
    layout->setSpacing(20);
    enter->setFocus();//焦点设置在回车建上
}

void QDialogPassword::accept()
{
    //qDebug()<<"jiemi ";
    pwd = password->text().toInt();
    if(0>=password->text().length()||(pwd>maxVal||pwd<minVal))
    {
        Utils::MyMessageBox(QString("Error"), tr("输入范围不符合!"),QObject::tr("确定"),this);
        return;
    }


    if(pwd != SuperDogPassW&&pwd!=QDog::getInstance()->m_dog.Machine_Password)
    {
        Utils::MyMessageBox(QString("Error"), tr("密码错误!"),QObject::tr("确定"),this);
        return;
    }

    QDialog::accept();
}

void QDialogPassword::reject()
{
    QDialog::reject();
}
