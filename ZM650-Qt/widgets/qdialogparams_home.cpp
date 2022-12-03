/*
 * qdialogparams_home.cpp
 *
 * 参数界面的原点配置弹出对话框
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qdialogparams_home.h"
#include "data/adapters.h"
#include "data/utils.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QDebug>

//mod by yzg
const QStringList QDialogParams_Home::groupBoxText =
{
    //    QString::fromUtf8("外部零点"),
    //    QString::fromUtf8("伺服零点")
    QObject::trUtf8("外部零点"),
    QObject::trUtf8("伺服零点")
};

const QStringList QDialogParams_Home::radioText =
{
    //    QString::fromUtf8("无效"),
    //    QString::fromUtf8("高电平"),
    //    QString::fromUtf8("低电平")

    QObject::trUtf8("无效"),
    QObject::trUtf8("高电平"),
    QObject::trUtf8("低电平")
    //    QObject::trUtf8("无效"),
    //    QObject::trUtf8("有效"),
};

const QStringList QDialogParams_Home::comboBoxText =
{
    //    QString::fromUtf8("正向"),
    //    QString::fromUtf8("负向")

    QObject::trUtf8("正向"),
    QObject::trUtf8("负向")

};

const QStringList QDialogParams_Home::Text =
{
    //    QString::fromUtf8("正向"),
    //    QString::fromUtf8("负向")

    QObject::trUtf8("外部"),
    QObject::trUtf8("伺服")

};

QDialogParams_Home::QDialogParams_Home(Adapters::DataAdapter_ParamTable* adapter, int offset, QWidget* parent,
                                       Qt::WindowFlags f) : QDialog(parent, f),
    adapter(adapter),
    offset(offset)
{
    setWindowIcon(QIcon(":/images/images/NULL.ico"));
    data = static_cast<unsigned short>(adapter->getData(offset).toUInt());
    //setWindowTitle("原点配置");
    setWindowTitle(tr("原点配置"));

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
#ifdef ARM
            if(i==1)
                radioButton[i][j]->setEnabled(false);//轴电平不在此处设置
            radioButton[i][j]->setEnabled(true);//轴电平不在此处设置
#else
            radioButton[i][j]->setEnabled(true);//轴电平不在此处设置
#endif

            groupBox[i]->layout()->addWidget(radioButton[i][j]);
        }

        //auto label = new QLabel("归零方向", this);
        auto label = new QLabel(tr("归零方向"), this);
        groupBox[i]->layout()->addWidget(label);

        comboBox[i] = new QComboBox(this);
        //comboBox[i]->addItems(comboBoxText);
        for(auto k = 0; k < 2; k++)//mod by yzg
        {
            comboBox[i]->addItem(QObject::tr(comboBoxText.at(k).toUtf8()));
        }
        groupBox[i]->layout()->addWidget(comboBox[i]);
    }

    auto buttonBox = Utils::createOkCancelButtons(this);
    gridLayout->addWidget(buttonBox, 1, 0, 1, 2);

    // update UI checked/unchecked
    for(auto i = 0; i < 2; ++i)
    {
        auto valueMasked = data >> (4 * i);

        if(valueMasked & 0x01)
        {
            if(valueMasked & 0x02)
                radioButton[i][1]->setChecked(true); // 高电平
            else
                radioButton[i][2]->setChecked(true); // 低电平
            //            radioButton[i][1]->setChecked(true); // 有效
        }
        else
        {
            radioButton[i][0]->setChecked(true); // 无效
        }

        if(valueMasked & 0x04)
            comboBox[i]->setCurrentIndex(1);
        else
            comboBox[i]->setCurrentIndex(0);
    }
}

QString QDialogParams_Home::getStringFromData(const QVariant& val)
{
    auto u16 = static_cast<unsigned short>(val.toUInt());
    QString resultString = "";

    //判断两次 外部 和伺服原点
    for(auto i = 0; i < 2; ++i)
    {
        auto valueMasked = u16 >> (4 * i);
        //显示过长 去掉
        //        if(valueMasked & 0x01)
        //        {
        //            if(valueMasked & 0x02)
        //                //resultString += radioText.at(1); // 高电平
        //                resultString += QObject::tr(radioText.at(1).toUtf8()); // 高电平
        //            else
        //                //resultString += radioText.at(2); // 低电平
        //                resultString += QObject::tr(radioText.at(2).toUtf8()); // 低电平
        //            resultString += QObject::tr(radioText.at(1).toUtf8()); // 有效
        //        }
        //        else
        //        {
        //            //resultString += radioText.at(0); // 无效
        //            resultString += QObject::tr(radioText.at(0).toUtf8()); // 无效
        //        }

        if(valueMasked & 0x01)
        {

            if(i == 1 && (u16 & 0x01))
            {
                resultString += "/";
            }
            resultString += QObject::tr(Text.at(i).toUtf8());

        }
    }
    if(resultString.isEmpty())
        return resultString +="无" ;
    return resultString;
}

void QDialogParams_Home::accept()
{
    for(auto i = 0; i < 2; ++i)
    {
        auto radioButtonIndex = 0;//有效 无效
        unsigned short value = 0;

        for(; radioButtonIndex < 3; ++radioButtonIndex)//修改成有效 无效 变成2个
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
            value |= 0x01;
            break;

        case 0:
        default:
            break;
        }

        switch(comboBox[i]->currentIndex())
        {
        case 1:
            value |= 0x04;
            break;

        case 0:
        default:
            break;
        }

        // clear the existed bits
        unsigned short mask = ~(0x07 << (4 * i));
        data &= mask;

        // set the new bits
        data |= (value << (4 * i));
    }

    adapter->setData(offset, QVariant(data));

    QDialog::accept();
}

