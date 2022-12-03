/*
 * qdialogparams_authority.cpp
 *
 * 获取参数修改权限对话框
 *
 * Created on: 2020年 07月 3日 星期一 14:57:40 CST
 * Author: caojing
 */
#include "qdialogparams_authority.h"
#include <QGridLayout>
#include <QLabel>
#include <QIcon>
#include <QDialogButtonBox>
#include "data/adapters.h"
#include "data/utils.h"
#include "data/uistyles.h"
#include <QMessageBox>
#include <QGroupBox>
#include <QApplication>
#include <QDesktopWidget>
#include "data/globaldata.h"
#include "Teach.h"

const QStringList QDialogParams_Authority::labelText =
{
        QObject::trUtf8("操作员"),
        QObject::trUtf8("调试员"),
        QObject::trUtf8("超用户")
};

QDialogParams_Authority::QDialogParams_Authority(Adapters::DataAdapter_ParamTable* adapter, int offset, QWidget* parent,
        Qt::WindowFlags f):
    QDialog(parent, f),
    adapter(adapter),
    offset(offset)
{
    setWindowIcon(QIcon(":/images/images/NULL.ico"));
    auto layout_v=new QVBoxLayout(this);
    auto layout_h=new QHBoxLayout();

    auto groupBox=new QGroupBox();
    groupBox->setLayout(new QVBoxLayout);
    groupBox->layout()->setSpacing(1);
    for(auto i=0;i<3;++i)
    {
        radioButton[i]=new QRadioButton(labelText[i],groupBox);
        radioButton[i]->setStyleSheet(UIStyles::css_QRadioButton);
        connect(radioButton[i], SIGNAL(pressed()),this, SLOT(onCheckbtn()));
        groupBox->layout()->addWidget(radioButton[i]);
        groupBox->layout()->setAlignment(Qt::AlignHCenter);
    }
    QLabel* laber = new QLabel(tr("请输入密码:"));
    passedit = new QLineEditStandard("0");
    passedit->setAlignment(Qt::AlignCenter);
    layout_h->addWidget(groupBox);
    layout_v->addLayout(layout_h);
    layout_v->addWidget(laber);
    layout_v->addWidget(passedit);
    auto buttonBox=Utils::createOkCancelButtons(this);
    layout_v->addWidget(buttonBox);
    layout_v->setSpacing(10);
    radioButton[0]->setChecked(true);
    passedit->setDisabled(true);
}

void QDialogParams_Authority::onCheckbtn()
{
   QRadioButton* w = qobject_cast<QRadioButton*>(sender());
   if(w->text()==labelText[0])
   {
       passedit->setDisabled(true);
   }
   else
   {
       passedit->setDisabled(false);
   }
}

QString QDialogParams_Authority::getStringFromData(const QVariant& /*val*/)
{
    return QString("执行");
}

void QDialogParams_Authority::accept()
{
    if(radioButton[0]->isChecked())
    {
            data = GlobalData::USER_LEVEL_L;
            PASSWFLAG = data;
            Utils::MyMessageBox(QString("Info"), tr("设置成功"),tr("确定"),this);
    }
    else if(radioButton[1]->isChecked())
    {
        if(passedit->text().toUInt() == g_Sysparam.ProgrammerPassw)
        {
            data = GlobalData::USER_LEVEL_M;
            PASSWFLAG = data;
            Utils::MyMessageBox(QString("Info"), tr("设置成功"),tr("确定"),this);
        }
        else
        {
            Utils::MyMessageBox(QString("Info"), tr("密码错误"),tr("确定"),this);
        }
    }
    else
    {
        if(passedit->text().toUInt() == g_Sysparam.SuperPassw)
        {
            data = GlobalData::USER_LEVEL_H;
            PASSWFLAG = data;
            Utils::MyMessageBox(QString("Info"), tr("设置成功"),tr("确定"),this);
        }
        else
        {
            Utils::MyMessageBox(QString("Info"), tr("密码错误"),tr("确定"),this);
        }
    }
    adapter->setData(offset, PASSWFLAG);
    GlobalData::paramauthority = PASSWFLAG;
    QDialog::accept();
}


