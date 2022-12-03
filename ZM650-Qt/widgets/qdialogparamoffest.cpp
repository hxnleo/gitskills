/*
 * qdialogparamoffest.h
 *
 * 设置班次对话框
 *
 * Created on: 2020年 2月 18日 星期一 11:03:40 CST
 * Author: caojing
 */
#include "qdialogparamoffest.h"
#include "data/globaldata.h"
#include "data/uistyles.h"
#include "data/utils.h"
#include "data/workfile.h"
#include "data/globaldata.h"
#include "modbus/modbusaddress.h"
#include "modbus/modbusqueue.h"
#include <QDialogButtonBox>
#include<QDebug>
//const QStringList QDialogParamOffest::NAMES =
//{
//    QObject::trUtf8("X偏移"),
//    QObject::trUtf8("Y偏移"),
//    QObject::trUtf8("Z偏移"),
//    QObject::trUtf8("A偏移"),
//    QObject::trUtf8("B偏移"),
//    QObject::trUtf8("C偏移"),
//    QObject::trUtf8("颜色"),
//    QObject::trUtf8("拍毛"),
//    QObject::trUtf8("起始"),
//    QObject::trUtf8("结束"),
//};
QDialogParamOffest::QDialogParamOffest(QWidget* parent,Qt::WindowFlags f):
                       QDialog(parent, f),value(new int[10]),teachRead(new TEACH_SEND)
{
    NAMES=
    {
        tr("X偏移"),
        tr("Y偏移"),
        tr("Z偏移"),
        tr("A偏移"),
        tr("B偏移"),
        tr("C偏移"),
        tr("颜色"),
        tr("拍毛"),
        tr("起始"),
        tr("结束"),
    };
    setWindowIcon(QIcon(":/images/images/NULL.ico"));
    QVBoxLayout* layout_v=new QVBoxLayout(this);
    QGridLayout* layoutf_v=new QGridLayout();
    QHBoxLayout* layout_h=new QHBoxLayout();
    QGridLayout *Layout = new QGridLayout();
    layout_h->addLayout(Layout);
    layout_h->addLayout(layoutf_v);
    layout_v->addLayout(layout_h);
    auto buttonBox=Utils::createOkCancelButtons(this);
    layout_v->addWidget(buttonBox);

    auto teachData = Workfile::getInstance()->getTeachData()+1; // 跳过第一个点（起点）
    int positions = 0;
    for(int r=0;r<TEACH_AXIS;r++)
    {

        positions = (teachData + GlobalData::realTimeInfo.iCNum)->fMove_pos[r];
        QLabel *labe0 = new QLabel(NAMES.at(r));
        QLabel *labe1 = new QLabel(Utils::numberToString(GlobalData::realTimeInfo.fPos[GlobalData::axis[r]]-positions,3,8));
        layoutf_v->addWidget(labe0, r, 0, 1, 1);
        layoutf_v->addWidget(labe1, r, 1, 1, 1);

    }
    for(int i = 0;i<NAMES.size();i++)
    {
        //value[i]=i;
        QLabel *labe = new QLabel(NAMES.at(i));
//        QLineEdit *line = new QLineEdit(QString("%1").arg(value[i]));
        if("颜色" == NAMES.at(i))
        {
            auto colorList=QStringList({tr("禁用"),tr("不变"),tr("毛"), tr("皮")});
            auto line = new QComboBox(parent);
            line->setView(new QListView(line));//弹出的时候使用设定的item
            line->setStyleSheet(UIStyles::CSS_TEACH_COMBOBOX);
            line->addItems(colorList);

            line_value.append(line);

        }
        else if("拍毛" == NAMES.at(i))
        {
            auto colorList=QStringList({tr("禁用"),tr("关闭"), tr("开启")});
            auto line = new QComboBox(parent);
            line->setView(new QListView(line));//弹出的时候使用设定的item
            line->setStyleSheet(UIStyles::CSS_TEACH_COMBOBOX);
            line->addItems(colorList);
            line_value.append(line);
        }
        else
        {
            QLineEditStandard *line = new QLineEditStandard("");
            line->setAlignment(Qt::AlignHCenter);
            line->setStyleSheet(UIStyles::css_maincolorconf_u11);
            line_value.append(line);
            if(i==NAMES.size()-2)
            {
                line->setText(Utils::numberToString(GlobalData::realTimeInfo.iCNum));
            }
        }
        labe_value.append(labe);
    }
    int temp = 0,r,l;
    for(r=0;r<NAMES.size()/2;r++)
    {
        for(l=0;l<2;l++)
        {
            Layout->addWidget(labe_value.at(temp), r, l*2, 1, 1);
            Layout->addWidget(line_value.at(temp), r, l*2+1, 1, 1);
            temp++;
            if(temp>=NAMES.size())
            {
                break;
            }
        }
    }
    buttonBox->setFocus();
    this->setWindowTitle(tr("参数偏移对话框"));
}

QDialogParamOffest::~QDialogParamOffest()
{
    Utils::delete_array(value);
    labe_value.clear();
    line_value.clear();
    qDebug()<<"~QDialogParamOffest";
}

void QDialogParamOffest::ReadData()
{
    ModbusQueue::element_t* element = new ModbusQueue::element_t;
    memset(teachRead,0,sizeof(TEACH_SEND));
    element->data = teachRead;
    element->len = sizeof(TEACH_SEND);
    element->operation = ModbusQueue::OP_READ_DATA;
    element->startAddress = ModbusAddress::TeachWorkfileHead;
    GlobalData::modbusQueue->sendQuery(element, false);
}

void QDialogParamOffest::accept()
{
    int i=0;
//    TEACH_DATAOFFSET
    ModbusQueue::element_t* element = new ModbusQueue::element_t;
    TEACH_SEND* teachSend = new TEACH_SEND;
    memset(teachSend,0,sizeof(TEACH_SEND));
//    void* p = &teachSend->SendData.N_Offset;//数据传递
    for(i=0; i<TEACH_AXIS ; i++)
    {
        teachSend->SendData.N_Offset.fAxisShift[i] = qobject_cast<QLineEdit*>(line_value.at(i))->text().toDouble();
        qDebug()<<"++"<<qobject_cast<QLineEdit*>(line_value.at(i))->text().toDouble();
    }
    teachSend->SendData.N_Offset.iColorShift = qobject_cast<QComboBox*>(line_value.at(i++))->currentIndex();
    teachSend->SendData.N_Offset.bPatShift = qobject_cast<QComboBox*>(line_value.at(i++))->currentIndex();
    teachSend->SendData.N_Offset.iStart_N = qobject_cast<QLineEdit*>(line_value.at(i++))->text().toInt();
    teachSend->SendData.N_Offset.iEnd_N = qobject_cast<QLineEdit*>(line_value.at(i))->text().toInt();
    teachSend->Type = TEACH_DATAOFFSET;

    qDebug()<<"++"<<qobject_cast<QComboBox*>(line_value.at(6))->currentIndex();
    qDebug()<<"++"<<qobject_cast<QComboBox*>(line_value.at(7))->currentIndex();
    qDebug()<<"++"<<qobject_cast<QLineEdit*>(line_value.at(8))->text().toDouble();
    qDebug()<<"++"<<qobject_cast<QLineEdit*>(line_value.at(9))->text().toDouble();
    if(teachSend->SendData.N_Offset.iStart_N==0)
    {
        Utils::MyMessageBox(QString("Err"), tr("起始位置不能进行偏移操作!"),tr("确定"),this);
        return;
    }

    element->data = teachSend;
    element->len = sizeof(TEACH_SEND);
    element->operation = ModbusQueue::OP_WRITE_DATA;
    element->startAddress = ModbusAddress::TeachWorkfileHead;
    GlobalData::modbusQueue->sendQuery(element, false);
    QDialog::accept();
}

void QDialogParamOffest::reject()
{
    QDialog::reject();
}

