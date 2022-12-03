/*
 * qdialogmanage.cpp
 * 修改参数密码
 *
 * Created on: 2019年 10月 22日 星期一 17:02:40 CST
 * Author: caojing
 */
#include "qdialogmanage.h"
#include "data/utils.h"
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
#include "qlineeditstandard.h"

QDialogManage::QDialogManage(QWidget* parent,Qt::WindowFlags f):
    QDialog(parent, f),password(new QLineEditStandard)
{
    setWindowIcon(QIcon(":/images/images/NULL.ico"));
    QGridLayout *layout = new QGridLayout(this);
    QLabel* line0 = new QLabel(tr("操作密码:"));

    QPushButton *close = new QPushButton(tr("取消"));
    QPushButton *enter = new QPushButton(tr("确定"));

    connect(close, SIGNAL(pressed()), this, SLOT(reject()));
    connect(enter, SIGNAL(pressed()), this, SLOT(accept()));

    //password->setFixedWidth(180);
    close->setFixedWidth(100);
    enter->setFixedWidth(100);

    password->setPlaceholderText(tr("输入密码"));
    password->setEchoMode(QLineEdit::Password);

    password->setAlignment(Qt::AlignHCenter);
    layout->addWidget(line0,0,0,1,1);
    layout->addWidget(password,0,1,1,2);
    layout->addWidget(enter,2,1,Qt::AlignLeft);
    layout->addWidget(close,2,2,Qt::AlignRight);
    layout->setSpacing(20);
    enter->setFocus();
    this->setWindowTitle(tr("管理密码"));

}

void QDialogManage::accept()
{

    QString yearstr = Utils::dataToString(QDateTime::currentDateTime().toString("yyyy").toInt(),4);
    QString monustr = Utils::dataToString(QDateTime::currentDateTime().toString("MM").toInt(),2);
    QString datastr = Utils::dataToString(QDateTime::currentDateTime().toString("dd").toInt(),2);

    //日期 + 年份×日期 + 月份   比如  今天  30+60600+11
    QString str = datastr + QString("%1").arg(yearstr.toInt()*datastr.toInt())+monustr;

    qDebug()<<yearstr<<monustr<<datastr<<str.toInt();

    if(password->text().toInt()!=str.toInt())
    {
        QMessageBox::warning(this, "!",tr("密码错误"),QMessageBox::Ok);
    }
    else
    {
        emit decrypt();
    }
    QDialog::accept();
    return;

}

void QDialogManage::reject()
{
    QDialog::reject();
}
