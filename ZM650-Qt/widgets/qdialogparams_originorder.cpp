/*
 * qdialogparams_originorder.cpp
 *
 * 参数界面的归零顺序配置弹出对话框
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qdialogparams_originorder.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QVariant>
#include "data/globaldata.h"
#include "data/utils.h"

#define PROP_INDEX_GROUP "GROUP_INDEX"
#define PROP_INDEX_AXIS "AXIS_INDEX"
//const QStringList QDialogParams_OriginOrder::axisName =
//{("X"), ("Y"), ("Z"), ("A"), ("B"), ("C"),("A1"), ("B1"), ("C1")};
QDialogParams_OriginOrder::QDialogParams_OriginOrder(Adapters::DataAdapter_ParamTable* adapter, int offset,
        QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f),
    adapter(adapter),
    offset(offset)
{
    setWindowIcon(QIcon(":/images/images/NULL.ico"));
    data = adapter->getData(offset).toUInt();

    auto gridLayout = new QGridLayout(this);

    for(auto i = 0; i < 2; ++i)
    {
        QGridLayout* gridLayout_x = new QGridLayout();
        groupBox[i] = new QGroupBox(this);
        //groupBox[i]->setTitle(QString("第%1组").arg(i + 1));
        groupBox[i]->setTitle(tr("第%1组").arg(i + 1));//mod by yzg
        gridLayout->addWidget(groupBox[i], 0, i);

//        groupBox[i]->setLayout(new QVBoxLayout(groupBox[i]));
        groupBox[i]->setLayout(gridLayout_x);
        unsigned int u32Masked = data >> (i * 16);

        for(auto j = 0; j < GlobalData::axisName.size(); ++j)
        {
            checkBox[i][j] = new QCheckBox(GlobalData::axisName.at(j));
//            groupBox[i]->layout()->addWidget(checkBox[i][j]);
            gridLayout_x->addWidget(checkBox[i][j],j/2,j%2);

            // axis index property
            checkBox[i][j]->setProperty(PROP_INDEX_GROUP, QVariant(i));
            checkBox[i][j]->setProperty(PROP_INDEX_AXIS, QVariant(j));

            if(0x01 & u32Masked)
                checkBox[i][j]->setChecked(true);

            // shift bit
            u32Masked >>= 1;

            // on Clicked!
            connect(checkBox[i][j], SIGNAL(stateChanged(int)),
                    this, SLOT(onCheckBoxStateChanged(int)));
        }
    }

    auto buttonBox = Utils::createOkCancelButtons(this);
    gridLayout->addWidget(buttonBox, 1, 0, 1, 4);
}

QString QDialogParams_OriginOrder::getStringFromData(const QVariant& val)
{
    QString returnStr = "";
    auto u32 = val.toUInt();
    bool isEmpty = true;

    for(auto i = 0; i < 2; ++i)
    {
        unsigned int u32Masked = u32 >> (16 * i);
        auto validAxisCount = 0;

        for(auto j = 0; j < GlobalData::axisName.size(); ++j)
        {
            //qDebug()<<"j+u32Masked"<<j<<u32Masked;
            if(0x01 & u32Masked)
            {
                returnStr += GlobalData::axisName.at(j);
                ++validAxisCount;
                //qDebug()<<"returnStr"<<returnStr;
            }

            u32Masked >>= 1;
        }

        if(validAxisCount > 0)
        {
            isEmpty = false;
            returnStr += "-";
        }
    }

    if(true == isEmpty)
        //return QString("未配置");
        return tr("未配置");

    return returnStr.mid(0, returnStr.length() - 1);
}

void QDialogParams_OriginOrder::accept()
{
    unsigned int u32 = 0;

    for(auto i = 0; i < 2; ++i)
    {
        for(auto j = 0; j < GlobalData::axisName.size(); ++j)
        {
            if(checkBox[i][j]->isChecked())
                u32 |= (1 << (j + i * 16));

            //qDebug()<<"j+u32"<<j<<u32<<checkBox[i][j]->isChecked();
        }
    }

    data = u32;
    adapter->setData(offset, QVariant(data));

    QDialog::accept();
}

void QDialogParams_OriginOrder::onCheckBoxStateChanged(int newState)
{
    auto cb = qobject_cast<QCheckBox*>(sender());

    if(Qt::Checked == newState)
    {
        int groupIndex = cb->property(PROP_INDEX_GROUP).toInt();
        int axisIndex = cb->property(PROP_INDEX_AXIS).toInt();
        for(auto i = 0; i < 2; ++i)
        {
            if(i != groupIndex)
                checkBox[i][axisIndex]->setChecked(false);
        }
    }
}
