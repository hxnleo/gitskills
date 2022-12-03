/*
 * qdialogfindzero.cpp
 * 
 * 归零对话框
 * 
 * Created on: 2019年 02月 26日 星期二 11:46:59 CST
 * Author: lixingcong
 */

#include "qdialogfindzero.h"
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QDesktopWidget>
#include <QApplication>
#include "data/utils.h"
#include "data/uistyles.h"

const QString QDialogFindZero::radioButtonText[2]=
{
    //    QString::fromUtf8("机械归零"),
    //    QString::fromUtf8("主轴归零")
    QString::fromUtf8("回起一工位"),
    QString::fromUtf8("回起二工位")
};

QDialogFindZero::QDialogFindZero(QWidget *parent, Qt::WindowFlags f):QDialog (parent,f)
{
    setWindowIcon(QIcon(":/images/images/NULL.ico"));
#ifdef _MCGS
    QRect rectScreen = QApplication::desktop()->availableGeometry();
    setFixedSize(rectScreen.width()*1/3,rectScreen.height()*2/5);
#endif
    auto layout_v=new QVBoxLayout(this);

    auto groupBox=new QGroupBox(this);
    groupBox->setLayout(new QVBoxLayout);
    groupBox->layout()->setSpacing(20);
    QFont font;
    for(auto i=0;i<2;++i)
    {
        font.setPointSize(16);
        radioButton[i]=new QRadioButton(radioButtonText[i],groupBox);
        radioButton[i]->setFont(font);
        radioButton[i]->setStyleSheet(UIStyles::css_QRadioButton);
        groupBox->layout()->addWidget(radioButton[i]);
        groupBox->layout()->setAlignment(Qt::AlignHCenter);
    }

    radioButton[0]->setChecked(true);

    layout_v->addWidget(groupBox);

    auto buttonBox=Utils::createOkCancelButtons(this);
    layout_v->addWidget(buttonBox);
}

void QDialogFindZero::accept()
{
    if(radioButton[0]->isChecked())
        emit startTable();
    else if(radioButton[1]->isChecked())
        emit startTable1();

    QDialog::accept();
}
