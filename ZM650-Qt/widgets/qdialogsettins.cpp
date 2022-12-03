/*
 * qdialogsettins.cpp
 *
 * 设置班次对话框
 *
 * Created on: 2020年 2月 18日 星期一 11:03:40 CST
 * Author: caojing
 */
#include "qdialogsettins.h"
#include "data/globaldata.h"
#include "data/uistyles.h"
#include "data/utils.h"
#include "data/adapters.h"
#include "modbus/modbusaddress.h"
#include "modbus/modbusqueue.h"
#include "qlineeditstandard.h"
#include <QDialogButtonBox>

#include<QDebug>
const QStringList QDialogSettins::NAMES =
{
    QObject::trUtf8("总量"),
    QObject::trUtf8("一班"),
    QObject::trUtf8("二班"),
    QObject::trUtf8("三班"),
    QObject::trUtf8("当班"),

};
QDialogSettins::QDialogSettins(QWidget* parent,Qt::WindowFlags f):
    QDialog(parent, f),value(new unsigned int[5])
{
    memset(value,0,sizeof(*value)*5);
    onReadData();
    setWindowIcon(QIcon(":/images/images/NULL.ico"));
    QVBoxLayout* layout_v=new QVBoxLayout(this);
    QGridLayout *Layout = new QGridLayout();
    for(int i = 0;i<5;i++)
    {
        QLabel *labe = new QLabel(NAMES.at(i));
        QLineEditStandard *line = new QLineEditStandard(QString("%1").arg(value[i]),this);
        line->setAlignment(Qt::AlignHCenter);
        line->setStyleSheet(UIStyles::css_maincolorconf_u12);
        line->setFixedWidth(parent->width()/8);
        labe_value.append(labe);
        line_value.append(line);
    }
    int temp = 0,r,l;
    for(r=0;r<3;r++)
    {
        for(l=0;l<2;l++)
        {
            Layout->addWidget(labe_value.at(temp), r, l*2, 1, 1);
            Layout->addWidget(line_value.at(temp), r, l*2+1, 1, 1);
            temp++;
            if(temp>=5)
            {
                break;
            }
        }
    }
    layout_v->addLayout(Layout);
    QDialogButtonBox* buttonBox=Utils::createOkCancelButtons(this);
    layout_v->addWidget(buttonBox);
    buttonBox->setFocus();
    this->setWindowTitle(tr("班数产量对话框"));
}

QDialogSettins::~QDialogSettins()
{
    Utils::delete_array(value);
    labe_value.clear();
    line_value.clear();
    qDebug()<<"~QDialogSettins";
}

void QDialogSettins::onReadData(bool ok)
{
    //获取三个地址
    sendOffest(&value[1],CLASSPRODUCTADDR,3*sizeof(value[1]),ok);//班产量
    sendOffest(&value[4],CURRPRODUCTADDR,sizeof(value[4]),ok);//当前班
    value[0]= value[1]+value[2]+value[3];
}

void QDialogSettins::sendOffest(void* data,int addr,int size,bool write)
{//数据读写
    ModbusQueue::element_t element;
    element.data=data;
    element.len=size;
    element.operation=write?ModbusQueue::OP_WRITE_DATA:ModbusQueue::OP_READ_DATA;
    element.startAddress=addr;
    GlobalData::modbusQueue->sendQuery(&element,false);
    //qDebug()<<size;
}

void QDialogSettins::accept()
{
    for(int i=0;i<5;i++)
    {
        value[i] = line_value.at(i)->text().toInt();
    }
    onReadData(true);
    QDialog::accept();
}

void QDialogSettins::reject()
{
    QDialog::reject();
}

