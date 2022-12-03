/*
 * qwidgetshowparams_paramtable.cpp
 *
 * 参数界面的的SysText.c显示项目
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qwidgetshowparams_paramtable.h"
#include <QLabel>
#include "data/utils.h"
#include "data/uistyles.h"
#include "data/globaldata.h"
#include "widgets/qcomboboxforparams.h"
#include "widgets/qpushbuttonparams.h"
#include "widgets/qdialogparams_limit.h"
#include "widgets/qdialogparams_home.h"
#include "widgets/qdialogparams_password.h"
#include "widgets/qdialogparams_originorder.h"
#include "widgets/qdialogparams_writeanything.h"
#include "widgets/qdialogparams_axisswitch.h"
#include "widgets/qlineeditnumber.h"
#ifdef Arm_460Y
#include "qdialogmanage.h"
#include "qdialogoffset.h"
#include "Teach.h"
#include "data/workfile.h"
#include "data/uistyles.h"
#endif

QWidgetShowParams_ParamTable::QWidgetShowParams_ParamTable(TParamTable* paramTable,
                                                           QWidget* parent): QWidgetShowParamsBase(parent),
    type(0),adapter(new Adapters::DataAdapter_ParamTable(paramTable))
{

}

QWidgetShowParams_ParamTable::QWidgetShowParams_ParamTable(TParamTable* paramTable, int startnameindex,
                                                           QWidget* parent): QWidgetShowParamsBase(parent),
    type(0),adapter(new Adapters::DataAdapter_ParamTable(paramTable,startnameindex))
{
#ifdef Arm_460Y
    GlobalData::Color1 = ATPARA.fColorPos1;
    GlobalData::Color2 = ATPARA.fColorPos2;
    GlobalData::Color3 = ATPARA.fColorPos3;
#endif
}

QWidgetShowParams_ParamTable::~QWidgetShowParams_ParamTable()
{
    Utils::waitForPendingModbus(&pendingQueryCount);
    Utils::delete_pointer(adapter);
}

QWidget* QWidgetShowParams_ParamTable::createFieldWidget(int index)
{
    auto p = adapter->getBaseAddr() + index;

    QWidget* w = nullptr;
    auto dataType = static_cast<Adapters::DataType>(adapter->getDataType(index));

    switch(adapter->getParamWidgetType(index))
    {
    case Adapters::WidgetType_NUMBER:
    {
        auto lineEdit = new QLineEditNumber(this);
        lineEdit->setStyleSheet(UIStyles::css_maincolorconf_u11);
        lineEdit->init(p->pData, dataType, p->fLmtVal[0], p->fLmtVal[1]);
        connect(lineEdit, SIGNAL(wroteValueFromUI()),
                this, SLOT(onWroteGoodValue()));
        w = lineEdit;
        break;
    }

    case Adapters::WidgetType_SELECT:
    {
        auto comboBox = new QComboBoxForParams(this);
        comboBox->init(p->pData, dataType, Adapters::getParamTableDropdownList(p->pData), p->fLmtVal[0], p->fLmtVal[1]);
        //        if(&ATPARA.bMetalCheck==p->pData)
        //        {
        //            qDebug()<<"测试"<<Adapters::getParamTableDropdownList(p->pData);
        //            comboBox->init(p->pData, dataType, Adapters::getParamTableDropdownList(p->pData), p->fLmtVal[0], p->fLmtVal[1]);
        //            qDebug()<<"种类"<<dataType;
        //        }
        connect(comboBox, SIGNAL(wroteValueFromUI()),
                this, SLOT(onWroteGoodValue()));
        if(p->pData==&ATPARA.bTeach)
        {
            connect(comboBox, SIGNAL(wroteValueFromUI()),
                    this, SLOT(onReadIO()));
        }
        w = comboBox;
        break;
    }

    case Adapters::WidgetType_LIMIT_CONFIG:
    {
        auto pushButton = new QPushButtonParams<QDialogParams_Limit, Adapters::DataAdapter_ParamTable>(adapter, index, this);
        pushButton->setStyleSheet(UIStyles::css_paracolorconf_u4);
        connect(pushButton, SIGNAL(wroteValueFromUI()),
                this, SLOT(onWroteGoodValue()));
        w = pushButton;
        break;
    }

    case Adapters::WidgetType_HOME_CONFIG:
    {
        auto pushButton = new QPushButtonParams<QDialogParams_Home, Adapters::DataAdapter_ParamTable>(adapter, index, this);
        pushButton->setStyleSheet(UIStyles::css_paracolorconf_u4);
        connect(pushButton, SIGNAL(wroteValueFromUI()),
                this, SLOT(onWroteGoodValue()));
        w = pushButton;
        break;
    }

    case Adapters::WidgetType_Authority:
    {
        auto pushButton = new QPushButtonParams<QDialogParams_Authority, Adapters::DataAdapter_ParamTable>(adapter, index, this);
        pushButton->setText(tr("执行"));
        pushButton->setStyleSheet(UIStyles::css_paracolorconf_u4);
        connect(pushButton, SIGNAL(wroteValueFromUI()),
                this, SLOT(onWroteGoodValue()));
        w = pushButton;
        break;
    }

    case Adapters::WidgetType_PASSWORD:
    {
        auto pushButton = new QPushButtonParams<QDialogParams_Password, Adapters::DataAdapter_ParamTable>(adapter, index, this);
        pushButton->setStyleSheet(UIStyles::css_paracolorconf_u4);
        connect(pushButton, SIGNAL(wroteValueFromUI()),
                this, SLOT(onWroteGoodValue()));
        w = pushButton;
        break;
    }

    case Adapters::WidgetType_ORIGIN_ORDER:
    {
        auto pushButton = new QPushButtonParams<QDialogParams_OriginOrder, Adapters::DataAdapter_ParamTable>(adapter, index,
                                                                                                             this);
        pushButton->setStyleSheet(UIStyles::css_paracolorconf_u4);
        connect(pushButton, SIGNAL(wroteValueFromUI()),
                this, SLOT(onWroteGoodValue()));
        w = pushButton;
        break;
    }

    case Adapters::WidgetType_WIRTE_ANYTHING:
    {
        auto pushButton = new QPushButtonParams<QDialogParams_WriteAnyThing, Adapters::DataAdapter_ParamTable>(adapter, index,
                                                                                                               this);
        pushButton->setText(tr("执行"));
        pushButton->setStyleSheet(UIStyles::css_paracolorconf_u4);
#ifdef Arm_460Y
        if(p->pData==&AxParam.ZPOS_OFFSET)
        {//机械原点设置
            connect(pushButton, SIGNAL(wroteValueFromUI()),
                    this, SLOT(onOpenDialog()));
            w = pushButton;
            break;
        }
        if(p->pData==&SYS_AXIS[Ay].ZPOS_OFFSET||p->pData==&AzParam.ZPOS_OFFSET)
        {
            connect(pushButton, SIGNAL(wroteValueFromUI()),
                    this, SLOT(onOpenDialog()));
            w = pushButton;
            break;
        }
#endif
        connect(pushButton, SIGNAL(wroteValueFromUI()),
                this, SLOT(onWroteGoodValue()));
        w = pushButton;
        break;
    }

    case Adapters::WidgetType_AXIS_SWITCH:
    {
        auto pushButton = new QPushButtonParams<QDialogParams_AxisSwitch, Adapters::DataAdapter_ParamTable>(adapter, index,
                                                                                                            this);
        pushButton->setStyleSheet(UIStyles::css_paracolorconf_u4);
        connect(pushButton, SIGNAL(wroteValueFromUI()),
                this, SLOT(onWroteGoodValue()));
        if(p->pData==&g_Sysparam.AXISTeachMap)
        {
            connect(pushButton, SIGNAL(wroteValueFromUI()),
                    this, SLOT(onUpdataTeachAxis()));
        }
        if(p->pData==&g_Sysparam.AXISSHOWSMAP)
        {
            connect(pushButton, SIGNAL(wroteValueFromUI()),
                    this, SLOT(onUpdataSysAxis()));
        }
        w = pushButton;
        break;
    }
    case Adapters::WidgetType_QLaber:
    {
        auto laber = new QLabel(this);
        QVariant val = Adapters::DataToQVariant(p->pData, dataType);
        laber->setText(val.toString());
        laber->setStyleSheet(UIStyles::css_maincolorconf_u15);
        w = laber;
        break;
    }

    default:
        w = new QLabel("EMPTY", this);
        break;
    }
    w->setMinimumWidth(GlobalData::widthRate/4-60);
    w->setMaximumWidth(GlobalData::widthRate/4-60);
    w->setMinimumHeight(GlobalData::highRate/9-10);
    w->setMaximumHeight(GlobalData::highRate/9-10);
    //无焦点 则输入框会直接弹出来 不需要点两下  但是会照成找不到焦点
    //#ifdef _MCGS
    //    w->setFocusPolicy(Qt::NoFocus);
    //#endif
    return w;
}

QString QWidgetShowParams_ParamTable::getParamName(int offset) const
{
    return adapter->getParamName(offset);
}
bool QWidgetShowParams_ParamTable::getDataAuthority(int offset) const
{
    bool authorit;
    if(adapter->getParamName(offset).contains("选择管理模式",Qt::CaseSensitive))
        authorit = true;
    else
        authorit = GlobalData::DataAuthority(adapter->getDataAuthority(offset));
    return authorit;
}
ModbusQueue::element_t* QWidgetShowParams_ParamTable::createModbusElement(int offset) const
{
    Utils::printQString(tr("发送信息:")+adapter->getParamName(offset) + adapter->getData(offset).toString());
    auto element = new ModbusQueue::element_t;
    element->data = adapter->getDataPointer(offset);
    element->len = static_cast<unsigned short>(adapter->getDataTypeSize(offset));
    element->startAddress = adapter->getDataModbusAddress(offset);
    element->operation = ModbusQueue::OP_WRITE_DATA;
    return element;
}



void QWidgetShowParams_ParamTable::onUpdataTeachAxis()
{//更新教导界面坐标显示
    GlobalData::teachdisplay = 0;
    GlobalData::teachaxisName.clear();

    for(auto axisCount = 0; axisCount < TotalAxis; ++axisCount)
    {
        if(g_Sysparam.AXISTeachMap & (0x01 << axisCount))
        {
            GlobalData::teachaxisName.insert(GlobalData::teachdisplay,GlobalData::axisName.at(axisCount));//得到实际轴号
            GlobalData::teachaxis[GlobalData::teachdisplay]=axisCount;
            GlobalData::teachdisplay++;
            //qDebug()<<k-1<<GlobalData::teachaxisName[k-1];
        }
    }
}

void QWidgetShowParams_ParamTable::onUpdataSysAxis()
{//更新系统坐标
    GlobalData::totalAxisNum = 0;
    GlobalData::realaxisName.clear();
    for(auto axisCount = 0; axisCount < TotalAxis; ++axisCount)
    {
        //AXISTeachMap  // AXISSHOWSMAP
        GlobalData::axis[GlobalData::totalAxisNum]=axisCount;//初始化
        if(g_Sysparam.AXISSHOWSMAP & (0x01 << axisCount))//1左移axisCount位
        {
            GlobalData::realaxisName.insert(GlobalData::totalAxisNum,GlobalData::axisName.at(axisCount));//得到实际轴号
            GlobalData::axis[GlobalData::totalAxisNum]=axisCount;
            GlobalData::totalAxisNum++; // 实际有效的轴数
        }
    }
}

#ifdef Arm_460Y
void QWidgetShowParams_ParamTable::onSetOffset()
{//设置偏移
    switch (type)
    {
    case OType::One:
    {
        //偏移 = 原本偏移 + 实际坐标
        TEACH_SEND* teachSend = new TEACH_SEND();//发送第一孔时  记录第一个点数据
        memset(teachSend,0,sizeof(TEACH_SEND));
        void* p = &teachSend->SendData.Teach_Data;//数据传递
        p = &teachSend->SendData.Teach_Data;//数据传递
        memcpy(p, Workfile::getInstance()->getTeachData()+1, sizeof(TEACH_DATA)); // 复制第一个孔的数据


        for(int i=0;i<=5;i++)
            qDebug()<<"轴"<<i<<AxParam.fAxisOffset<<GlobalData::realTimeInfo.fPos[i]<<teachSend->SendData.Teach_Data.fMove_pos[i];

        //偏移点 只有XY有效  其实设置完后需要读取下位机数据
        //但是　因为全局变量　　可以达到这个效果不需要重复读取
        AxParam.fAxisOffset = AxParam.fAxisOffset+GlobalData::realTimeInfo.fPos[0]-teachSend->SendData.Teach_Data.fMove_pos[0];
        AyParam.fAxisOffset = AyParam.fAxisOffset+GlobalData::realTimeInfo.fPos[1]-teachSend->SendData.Teach_Data.fMove_pos[1];

        AaParam.fAxisOffset = AaParam.fAxisOffset+GlobalData::realTimeInfo.fPos[3]-teachSend->SendData.Teach_Data.fMove_pos[3];
        AbParam.fAxisOffset = AbParam.fAxisOffset+GlobalData::realTimeInfo.fPos[4]-teachSend->SendData.Teach_Data.fMove_pos[4];
        AcParam.fAxisOffset = AcParam.fAxisOffset+GlobalData::realTimeInfo.fPos[5]-teachSend->SendData.Teach_Data.fMove_pos[5];
        float temp  = 0;
        if(teachSend->SendData.Teach_Data.icolor ==1||teachSend->SendData.Teach_Data.icolor ==0)
            temp = ATPARA.fColorPos1;
        else if(teachSend->SendData.Teach_Data.icolor ==2)
            temp = ATPARA.fColorPos2;
        else if(teachSend->SendData.Teach_Data.icolor ==2)
            temp = ATPARA.fColorPos3;

        AzParam.fAxisOffset = AzParam.fAxisOffset+GlobalData::realTimeInfo.fPos[2]-temp;

        qDebug()<<AxParam.fAxisOffset<<AyParam.fAxisOffset<<AzParam.fAxisOffset<<AaParam.fAxisOffset<<AbParam.fAxisOffset;

        sendOffest(&AxParam.fAxisOffset,AxParam.fAxisOffset);
        sendOffest(&AyParam.fAxisOffset,AyParam.fAxisOffset);
        sendOffest(&AzParam.fAxisOffset,AzParam.fAxisOffset);
        sendOffest(&AaParam.fAxisOffset,AaParam.fAxisOffset);
        sendOffest(&AbParam.fAxisOffset,AbParam.fAxisOffset);
        sendOffest(&AcParam.fAxisOffset,AcParam.fAxisOffset);


        //用于发送机械偏移数据　设置逻辑坐标
        TEACH_SEND* teachSend1 = new TEACH_SEND();//发送第一孔时  记录第一个点数据
        memset(teachSend1,0,sizeof(TEACH_SEND));
        teachSend1->SendData.Teach_Data.fMove_pos[0]=AxParam.fAxisOffset;
        teachSend1->SendData.Teach_Data.fMove_pos[1]=AyParam.fAxisOffset;
        teachSend1->SendData.Teach_Data.fMove_pos[2]=AzParam.fAxisOffset;
        teachSend1->SendData.Teach_Data.fMove_pos[3]=AaParam.fAxisOffset;
        teachSend1->SendData.Teach_Data.fMove_pos[4]=AbParam.fAxisOffset;
        teachSend1->SendData.Teach_Data.fMove_pos[5]=AcParam.fAxisOffset;

        ModbusQueue::element_t element;
        teachSend1->Type = COMMAND_SETOFFSETMACHINE;
        element.data = teachSend1;
        element.len = sizeof(TEACH_SEND);
        element.operation = ModbusQueue::OP_WRITE_DATA;
        element.startAddress = ModbusAddress::TeachWorkfileHead;
        GlobalData::modbusQueue->sendQuery(&element,false);
        break;
    }
    case OType::Two:
    {
        //旋转零点 = 实际坐标
        ATPARA.fRotatePos1 = GlobalData::realTimeInfo.fPos[0];
        ATPARA.fRotatePos2 = GlobalData::realTimeInfo.fPos[1];

        sendOffest(&ATPARA.fRotatePos1,ATPARA.fRotatePos1);
        sendOffest(&ATPARA.fRotatePos2,ATPARA.fRotatePos2);

        break;
    }
    case OType::Three:
    {
        ATPARA.fColorPos1 = GlobalData::Color1;
        ATPARA.fColorPos2 = GlobalData::Color2;
        ATPARA.fColorPos3 = GlobalData::Color3;

        sendOffest(&ATPARA.fColorPos1,ATPARA.fColorPos1);
        sendOffest(&ATPARA.fColorPos2,ATPARA.fColorPos2);
        sendOffest(&ATPARA.fColorPos3,ATPARA.fColorPos3);
        break;
    }
    default:
        break;
    }
    refreshPage(getCurrentPageNum());//发送完成刷新
}

void QWidgetShowParams_ParamTable::onOpenDialog()
{
    auto index = sender()->property(PROP_PARAMTABLE_INDEX).toInt();//得到序列号 名 "offest" 号 value
    auto data = adapter->getDataPointer(index);
    //此处不能加 this作为parent  因为只有widget才能生成widget child
    //QDialogOffset* offDlg = new QDialogOffset(1,6,this);
    if(GlobalData::realTimeInfo.iCNum!=1)
    {
        QMessageBox::information(this, tr("提示"), tr("当前孔不是第一孔，请先回起"));
        return;
    }
    QDialogOffset* offDlg;
    if(data == &AxParam.ZPOS_OFFSET)
    {//机械原点
        qDebug()<<"SYS_AXIS[Ay].ZPOS_OFFSET";
        offDlg = new QDialogOffset(1,6);//显示X,Y轴  1-2
        type = OType::One;
    }
    else if(data == &SYS_AXIS[Ay].ZPOS_OFFSET)
    {//旋转原点
        //qDebug()<<"AxParam.ZPOS_OFFSET";
        offDlg = new QDialogOffset(1,2,2);//显示X,Y轴  1-2
        type = OType::Two;
    }
    else if(data == &AzParam.ZPOS_OFFSET)
    {//毛道调节
        //qDebug()<<"AxParam.ZPOS_OFFSET";
        offDlg = new QDialogOffset(1,3,1);//显示X,Y轴  1-2
        type = OType::Three;
    }


    emit openOffset(MODIFY_OFFSET);
    connect(offDlg, SIGNAL(moveAxis(int,int,int, double)), this, SLOT(onMoveAxis(int,int,int, double)));
    connect(offDlg, SIGNAL(accepted()), this, SLOT(onSetOffset()));

    connect(offDlg, SIGNAL(accepted()), this, SLOT(onrestore()));
    connect(offDlg, SIGNAL(rejected()), this, SLOT(onrestore()));

    offDlg->setAttribute(Qt::WA_DeleteOnClose);
    offDlg->open();
}

void QWidgetShowParams_ParamTable::onrestore()
{
    //界面恢复
    emit openOffset(SHOW_SYSTEM);
}


void QWidgetShowParams_ParamTable::onMoveAxis(int axisNum, int direction, int pressType, double distance)
{
    emit moveAxis(axisNum, direction,pressType,distance);
}

void QWidgetShowParams_ParamTable::sendOffest(void* data,float size,bool write)
{
    ModbusQueue::element_t element;
    element.data=data;
    element.len=sizeof(size);
    element.operation=write?ModbusQueue::OP_WRITE_DATA:ModbusQueue::OP_READ_DATA;
    element.startAddress=Utils::getParaAdress(data);//旋转零点位置X
    //qDebug()<<Utils::getParaAdress(data);
    GlobalData::modbusQueue->sendQuery(&element,false);
}
#endif
