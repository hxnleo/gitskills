/*
 * qwidgetshowparams_daconfig.cpp
 *
 * DA测试界面
 *
 * Created on: 2019年 09月 2日 星期一 14:34:20 CST
 * Author: caojing
 */
#include "qwidgetshowparams_daconfig.h"
#include "widgets/qlineeditnumber.h"
#include "data/utils.h"
#include "data/globaldata.h"
#include "modbus/modbusaddress.h"
#include "data/definitions.h"
#include "Teach.h"

QWidgetShowParams_DAConfig::QWidgetShowParams_DAConfig(int funcNameCount,QWidget *parent) :
    QWidgetShowParamsBase(parent),
    funcNameCount(funcNameCount),
    daArray(new float[funcNameCount]), // crash in gcc 4.9.4(MingW64)
    daInit(new float[funcNameCount]) // crash in gc c 4.9.4(MingW64)
{
    DAInit();
    //  qDebug()<<DA_Fun_Name[10].fun<<DA_Fun_Name[10].name;

}
QWidgetShowParams_DAConfig::~QWidgetShowParams_DAConfig()
{
    Utils::delete_array(daArray);
    Utils::delete_array(daInit);
    qDebug()<<"~QWidgetShowParams_DAConfig";
}
void QWidgetShowParams_DAConfig::onChangData(float data)
{
    //qDebug()<<"得到数据"<<data;
    getData = data;
}
void QWidgetShowParams_DAConfig::daReadData()
{//读取DA数据 
    auto queryItem = new Definitions::ModbusQueryItem;
    queryItem->queryAction = Action_ReadIO;
    auto element = new ModbusQueue::element_t;
    element->data = daArray;
    element->len = sizeof(*daArray) * MAX_DA_NUM;
    element->operation = ModbusQueue::OP_READ_DATA;
    element->startAddress = ModbusAddress::IOStatus_DA;
    element->pointerObject = queryItem;
    sendModbusElement(element);
    Utils::sleep1(20);
}
void QWidgetShowParams_DAConfig::DAInit()
{
    float temp;
    for(int i=0;i<MAX_DA_NUM+2;i++)
    {
        *(daArray+i)=0;
        if(i<MAX_DA_NUM/2)
            temp=i*plus;
        else if(i<MAX_DA_NUM)
            temp=(i-MAX_DA_NUM/2)*plus;
        else
            temp= 0;
        //        *(daArray+i)=10;
        if(i<MAX_DA_NUM)
        {
            daName.append(QString::number(temp,'f',2));
            //qDebug()<<daName.at(i);
        }
        *(daInit+i) = temp;
    }
    daReadData();
}

QWidget* QWidgetShowParams_DAConfig::createFieldWidget(int index)
{
    QWidget* w = nullptr;
    auto lineEdit = new QLineEditNumber(this);
    //数据指针 类型 最小值 最大值
    //float flo=ArryData.at(index);
    void* addr = daArray+index;
    //qDebug()<<"daArray"<<index<<*(daArray+index);
    lineEdit->init(addr,0,15);
    lineEdit->setDaSingleData(true);
    connect(lineEdit, SIGNAL(wroteValueFromUI()),
            this, SLOT(onSendDAModbus()));//写入到
    connect(lineEdit, SIGNAL(sendSingleData()),
            this, SLOT(onSingleData()));//写入到
    connect(lineEdit, SIGNAL(changData(float)),
            this, SLOT(onChangData(float)));
    w = lineEdit;
    w->setMinimumWidth(GlobalData::widthRate/6-20);
    w->setMaximumWidth(GlobalData::widthRate/6-20);
    return w;
}

QString QWidgetShowParams_DAConfig::getParamName(int offset)const
{
    if(offset<(MAX_DA_NUM/2))// 数字 位数 进制 自动补全数
        return QString("(%1)").arg(offset,2,10,QLatin1Char('0'))+daName[offset]+"V";
    else
        return QString("(%1)").arg(offset-MAX_DA_NUM/2,2,10,QLatin1Char('0'))+daName[offset]+"V";
}

bool QWidgetShowParams_DAConfig::getDataAuthority(int offset) const
{
    return GlobalData::DataAuthority(true);
}

void QWidgetShowParams_DAConfig::onSingleData()
{
    auto index = sender()->property(PROP_PARAMTABLE_INDEX).toInt();//得到序列号 名 "offest" 号 value
    //设置以后打印出来      adapter->getData(offset).toString()设置的数字
    //qDebug()<<"onSingleData index"<<QString("%1").arg(index);
    auto queryItem = new Definitions::ModbusQueryItem;
    auto element = new ModbusQueue::element_t;
    if(index<MAX_DA_NUM)
    {//整个单个
        auto teachsend = new TEACH_SEND;//数据结构体
        memset(teachsend, 0, sizeof(TEACH_SEND));
        if(index<(MAX_DA_NUM/2))
            teachsend->Type = DA_OUT_VOLTAGE_0;//数据种类
        else
            teachsend->Type = DA_OUT_VOLTAGE_1;
        teachsend->SendData.DA_Test.type = DATypeData::One;//type种类
        teachsend->SendData.DA_Test.fVolt = daName[index].toFloat();//得到数据 fVolt 修改为下面的那个参数
        //qDebug()<<teachsend->Type<<getData;
        queryItem->queryAction = Action_TeachSend;

        element->data = teachsend;
        element->len = sizeof(TEACH_SEND);
        element->operation = ModbusQueue::OP_WRITE_DATA;
        element->startAddress = ModbusAddress::TeachWorkfileHead;
        element->pointerObject = queryItem;
    }
    else
        return;
    sendModbusElement(element);
}

void QWidgetShowParams_DAConfig::onSendDAModbus()
{
    auto offset = sender()->property(PROP_PARAMTABLE_INDEX).toInt();//得到序列号 名 "offest" 号 value
    //qDebug()<<"createModbusElement"<<QString("%1").arg(offset);
    auto queryItem = new Definitions::ModbusQueryItem;
    auto element = new ModbusQueue::element_t;

    if(offset<MAX_DA_NUM)
    {//整个DA发送下来
        //        /***********************按下DA按钮数据发送****************************/
        queryItem->queryAction = Action_WriteIO;
        element->data = daArray;
        element->len = sizeof(*daArray)*MAX_DA_NUM;
        element->startAddress = ModbusAddress::IOStatus_DA;
        element->operation = ModbusQueue::OP_WRITE_DATA;
        element->pointerObject = queryItem;
        sendModbusElement(element);
        Utils::sleep1(50);
    }
    else
    {//发送AD输出
        auto teachsend = new TEACH_SEND;//数据结构体
        memset(teachsend, 0, sizeof(TEACH_SEND));
        if(offset==MAX_DA_NUM)
            teachsend->Type = DA_OUT_VOLTAGE_0;//数据种类
        else
            teachsend->Type = DA_OUT_VOLTAGE_1;
        teachsend->SendData.DA_Test.type = DATypeData::Two;//type种类
        teachsend->SendData.DA_Test.fVolt = *(daArray + offset);//得到数据 fVolt 修改为下面的那个参数
        //qDebug()<<teachsend->Type<<getData;
        queryItem->queryAction = Action_TeachSend;//指定发送类型 防止程序异常bug

        element->data = teachsend;
        element->len = sizeof(TEACH_SEND);
        element->operation = ModbusQueue::OP_WRITE_DATA;
        element->startAddress = ModbusAddress::TeachWorkfileHead;
        element->pointerObject = queryItem;
        sendModbusElement(element);
        Utils::sleep1(50);
    }

}

ModbusQueue::element_t* QWidgetShowParams_DAConfig::createModbusElement(int offset) const
{
   //DA对延时有特殊需求
    Q_UNUSED(offset);
}
void QWidgetShowParams_DAConfig::onSendModbusOK(ModbusQueue::element_t* element)
{
    auto queryItem = reinterpret_cast<Definitions::ModbusQueryItem*>(element->pointerObject);

    switch(queryItem->queryAction)
    {
    case Action_ReadIO:
        //debug_printf("read io ok!!!!");
        break;
    case Action_WriteIO:
        //        debug_printf("send  ok!!!!");
        break;
    case Action_TeachSend:
        //debug_printf("send  ok!!!!");
        break;

    default:
        break;
    }

    Utils::delete_pointer(queryItem);
}

void QWidgetShowParams_DAConfig::onSendModbusFail(ModbusQueue::element_t* element)
{
    auto queryItem = reinterpret_cast<Definitions::ModbusQueryItem*>(element->pointerObject);
    Utils::delete_pointer(queryItem);
}




