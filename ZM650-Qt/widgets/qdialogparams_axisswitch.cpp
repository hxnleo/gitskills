/*
 * qdialogparams_axisswitch.cpp
 *
 * 参数界面的轴开关弹出对话框
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qdialogparams_axisswitch.h"
#include "data/globaldata.h"
#include "data/utils.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>


QDialogParams_AxisSwitch::QDialogParams_AxisSwitch(Adapters::DataAdapter_ParamTable* adapter, int offset,
        QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f),
    adapter(adapter),
    offset(offset)
{
    setWindowIcon(QIcon(":/images/images/NULL.ico"));
    data = static_cast<unsigned short>(adapter->getData(offset).toUInt());

    auto layout_v = new QVBoxLayout(this);
    auto layout_g = new QGridLayout();
    int i;
    for(i = 0; i < GlobalData::maxAxisNum; i++)
    {
        checkBox[i] = new QCheckBox(GlobalData::axisName.at(i), this);
        layout_g->addWidget(checkBox[i],i/2,i%2);

        if(data & (1 << i))
            checkBox[i]->setChecked(true);
    }
    auto buttonBox = Utils::createOkCancelButtons(this);
    layout_v->addLayout(layout_g);
    layout_v->addWidget(buttonBox);
}

QString QDialogParams_AxisSwitch::getStringFromData(const QVariant& val)
{
    auto u16 = static_cast<unsigned short>(val.toUInt());
    QString returnStr = QString();

    for(auto i = 0; i < GlobalData::maxAxisNum; ++i)
    {
        if(u16 & (1 << i))
            returnStr += GlobalData::axisName.at(i);
    }

    if(returnStr.length() <= 0)
        return tr("未配置");

    return returnStr;
}

void QDialogParams_AxisSwitch::accept()
{
    for(auto i = 0; i < GlobalData::maxAxisNum; ++i)
    {
        if(checkBox[i]->isChecked())
            data |= (1 << i);
        else
            data &= (~(1 << i));
    }

    adapter->setData(offset, QVariant(data));
    QDialog::accept();
}
