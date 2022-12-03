/*
 * qdialogparams_ioconfig.cpp
 *
 * 参数界面的IO配置弹出对话框
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qdialogparams_ioconfig.h"
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include "data/utils.h"
#include "data/globaldata.h"

const QStringList QDialogParams_IOConfig::radioButtonText =
{
    //    QString::fromUtf8("已禁用"),
    //    QString::fromUtf8("高"),
    //    QString::fromUtf8("低")

    QObject::trUtf8("已禁用"),
    QObject::trUtf8("高"),
    QObject::trUtf8("低")
};


QDialogParams_IOConfig::QDialogParams_IOConfig(Adapters::DataAdapter_IOConfigText* adapter, int offset, QWidget* parent,
                                               Qt::WindowFlags f) : QDialog(parent, f),
    adapter(adapter),
    offset(offset)
{
    setWindowIcon(QIcon(":/images/images/NULL.ico"));
    ioConfig = adapter->getData(offset).value<IOConfig>();

    auto gridLayout = new QGridLayout(this);

    // group
    //auto groupBox1 = new QGroupBox("有效电平", this);
    auto groupBox1 = new QGroupBox(tr("有效电平"), this);
    groupBox1->setLayout(new QVBoxLayout);
    //auto groupBox2 = new QGroupBox("端口号", this);
    auto groupBox2 = new QGroupBox(tr("端口号"), this);
    groupBox2->setLayout(new QVBoxLayout);

    gridLayout->addWidget(groupBox1, 0, 0, 1, 1);
    gridLayout->addWidget(groupBox2, 0, 1, 1, 1);

    // raidoButton
    for(auto i = 0; i < 3; ++i)
    {
        //radioButton[i] = new QRadioButton(radioButtonText.at(i), groupBox1);
        radioButton[i] = new QRadioButton(QObject::tr(radioButtonText.at(i).toUtf8()), groupBox1);//mod by yzg
        //为轴原点且为输入IO
        if((this->offset<=C_HOME))
        {
            if(GlobalData::iostatus)
            {
#ifdef ARM
                radioButton[i]->setEnabled(true);//轴电平不在此处设置
#else
                radioButton[i]->setEnabled(false);//轴电平不在此处设置
#endif
            }
        }
        groupBox1->layout()->addWidget(radioButton[i]);

        connect(radioButton[i], SIGNAL(clicked()),
                this, SLOT(enableLineEdit()));
    }

    // lineEdit
    lineEdit = new QLineEditNumber(this);
    lineEdit->init(&ioConfig.IONum, Adapters::TYPE_S16, 0, 255);
    groupBox2->layout()->addWidget(lineEdit);

    auto buttonBox = Utils::createOkCancelButtons(this);
    gridLayout->addWidget(buttonBox, 1, 0, 1, 2);

    // UI
    if(255 == ioConfig.IONum)
    {
        radioButton[0]->setChecked(true);
        lineEdit->setDisabled(true);
    }
    else if(0 != ioConfig.IOEle)
    {
        radioButton[1]->setChecked(true);
    }
    else
    {
        radioButton[2]->setChecked(true);
    }
}

QString QDialogParams_IOConfig::getStringFromData(const QVariant& val)
{
    auto _ioConfig = val.value<IOConfig>();
    QString returnStr;

    // UI
    if(255 == _ioConfig.IONum)
    {
        //returnStr = "已禁用";
        returnStr = tr("已禁用");
    }
    else if(0 != _ioConfig.IOEle)
    {
        //returnStr = QString("端口%1, ").arg(_ioConfig.IONum) + radioButtonText.at(1); // 高
        returnStr = tr("端口%1").arg(_ioConfig.IONum) +"|"+ QObject::tr(radioButtonText.at(1).toUtf8()); // 高
    }
    else
    {
        //returnStr = QString("端口%1, ").arg(_ioConfig.IONum) + radioButtonText.at(2); // 低
        returnStr = tr("端口%1").arg(_ioConfig.IONum) +"|"+ QObject::tr(radioButtonText.at(2).toUtf8()); // 低
    }

    return returnStr;
}

void QDialogParams_IOConfig::enableLineEdit()
{
    auto obj = sender();

    if(obj == radioButton[0])
    {
        lineEdit->setDisabled(true);
        lineEdit->setNumber(QVariant(255));
    }
    else
    {
        lineEdit->setDisabled(false);
    }
}

void QDialogParams_IOConfig::accept()
{
    QVariant val;
    ioConfig.IOEle = radioButton[1]->isChecked() ? 1 : 0;
    val.setValue(ioConfig);
    adapter->setData(offset, val);
    QDialog::accept();
}
