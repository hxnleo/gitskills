/*
 * qdialogparams_limit.cpp
 *
 * 参数界面的限位配置弹出对话框
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qdialogparams_limit.h"
#include "data/adapters.h"
#include "data/utils.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>


//mod by yzg tr
const QStringList QDialogParams_Limit::groupBoxText =
{
//    QString::fromUtf8("负限位"),
//    QString::fromUtf8("正限位")

    QObject::trUtf8("负限位"),
    QObject::trUtf8("正限位")
};

const QStringList QDialogParams_Limit::radioText =
{
//    QString::fromUtf8("无效"),
//    QString::fromUtf8("高电平"),
//    QString::fromUtf8("低电平")

     QObject::trUtf8("无效"),
     QObject::trUtf8("高电平"),
     QObject::trUtf8("低电平")
};

QDialogParams_Limit::QDialogParams_Limit(Adapters::DataAdapter_ParamTable* adapter, int offset, QWidget* parent,
        Qt::WindowFlags f) : QDialog(parent, f),
    adapter(adapter),
    offset(offset)
{
    setWindowIcon(QIcon(":/images/images/NULL.ico"));
    data = static_cast<unsigned short>(adapter->getData(offset).toUInt());
    //setWindowTitle("限位设置");
    setWindowTitle(tr("限位设置"));

    auto gridLayout = new QGridLayout(this);

    for(auto i = 0; i < 2; ++i)
    {
        groupBox[i] = new QGroupBox(this);
        //groupBox[i]->setTitle(groupBoxText.at(i));
        groupBox[i]->setTitle(QObject::tr(groupBoxText.at(i).toUtf8()));
        gridLayout->addWidget(groupBox[i], 0, i, 1, 1);

        groupBox[i]->setLayout(new QVBoxLayout(groupBox[i]));

        for(auto j = 0; j < 3; ++j)
        {
            //radioButton[i][j] = new QRadioButton(radioText.at(j), groupBox[i]);
            radioButton[i][j] = new QRadioButton(QObject::tr(radioText.at(j).toUtf8()), groupBox[i]);
            groupBox[i]->layout()->addWidget(radioButton[i][j]);
        }
    }

    auto buttonBox = Utils::createOkCancelButtons(this);
    gridLayout->addWidget(buttonBox, 1, 0, 1, 2);

    // update UI checked/unchecked
    for(auto i = 0; i < 2; ++i)
    {
        auto valueMasked = data >> (2 * i);

        if(valueMasked & 0x02)
        {
            if(valueMasked & 0x01)
                radioButton[i][1]->setChecked(true); // 高电平
            else
                radioButton[i][2]->setChecked(true); // 低电平
        }
        else
        {
            radioButton[i][0]->setChecked(true); // 无效
        }
    }
}

QString QDialogParams_Limit::getStringFromData(const QVariant& val)
{
    auto value = static_cast<unsigned short>(val.toUInt());

    QString resultString = "";

    for(auto i = 0; i < 2; ++i)
    {
        auto valueMasked = value >> (2 * i);

        if(valueMasked & 0x02)
        {
            if(valueMasked & 0x01)
                //resultString += radioText.at(1); // 高电平
                resultString += QObject::tr(radioText.at(1).toUtf8()); // 高电平
            else
                //resultString += radioText.at(2); // 低电平
                resultString += QObject::tr(radioText.at(2).toUtf8()); // 低电平
        }
        else
        {
            //resultString += radioText.at(0); // 无效
            resultString += QObject::tr(radioText.at(0).toUtf8()); // 无效
        }

        if(i < 1)
            resultString += " / ";
    }

    return resultString;
}

void QDialogParams_Limit::accept()
{
    for(auto i = 0; i < 2; ++i)
    {
        auto radioButtonIndex = 0;
        unsigned short value = 0;

        for(; radioButtonIndex < 3; ++radioButtonIndex)
        {
            if(radioButton[i][radioButtonIndex]->isChecked())
                break;
        }

        switch(radioButtonIndex)
        {
        case 1:
            value |= 0x03;
            break;

        case 2:
            value |= 0x02;
            break;

        case 0:
        default:
            break;
        }

        // clear the existed bits
        unsigned short mask = ~(0x03 << (2 * i));
        data &= mask;

        // set the new bits
        data |= (value << (2 * i));
    }

    adapter->setData(offset, QVariant(data));

    QDialog::accept();
}

