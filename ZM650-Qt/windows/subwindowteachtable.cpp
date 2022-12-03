/*
 * subwindowteachtable.cpp
 *
 * 教导界面
 *
 * Created on: 2019年 01月 21日 星期一 15:00:16 CST
 * Author: lixingcong
 */
#include "subwindowteachtable.h"
#include "widgets/qdialogparamoffest.h"
#include "ui_subwindowteachtable.h"
#include "data/globaldata.h"
#include "data/definitions.h"
#include "data/workfile.h"
#include "data/utils.h"
#include "data/utilsforcontroller.h"
#include "Teach.h"
#include "widgets/qpushbuttonlongpress.h"
#include "widgets/qpushbuttonspage.h"
#include "widgets/qitemmodelteach.h"
#include "widgets/qdialogdataform.h"
#include "modbus/modbusqueue.h"
#include "modbus/modbusaddress.h"
#include <Teach.h>
#include "widgets/qdialogloadingworkfile.h"
#include <TestIO.h>
#include <QMessageBox>
#include "data/uistyles.h"

#define PROP_AXISNUM "axisNum"
#define PROP_DIRECTION "direction"
#ifndef Arm_662
SubWindowTeachTable::SubWindowTeachTable(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SubWindowTeachTable),
    modbusPendingCount(0)
{

    ui->setupUi(this);
    ui->widgetStatus->init(GlobalData::realaxisName, GlobalData::totalAxisNum);

    connect(ui->widgetStatus, SIGNAL(updateTeachStep(unsigned int)),
            this, SLOT(_onsendTeachAction(unsigned int)));

    connect(this, SIGNAL(SetTeachStepText()),
            ui->widgetStatus, SLOT(onSetTeachStepText()));
    connect(this, SIGNAL(linkage()),
            ui->widgetStatus, SLOT(onUpdataLinkBtn()));
#ifdef Arm_Kaiteli
    connect(ui->widgetStatus, SIGNAL(updateQuickteach()),
            this, SLOT(_onsendQuickTeach()));
    connect(this, SIGNAL(quickteach(bool)),
            ui->widgetStatus, SLOT(_onUpatarQuick(bool)));
#endif

    auto _totalPoint = static_cast<int>(GlobalData::realTimeInfo.iMNum);
    auto _pointIndex = static_cast<int>(GlobalData::realTimeInfo.iCNum);

    if(GlobalData::teachdisplay<5)
        ui->tableViewTeach->init(5, _totalPoint, MAXTEACHNUM, GlobalData::teachdisplay, GlobalData::teachaxisName, 9);
    else
        ui->tableViewTeach->init(5, _totalPoint, MAXTEACHNUM, 5, GlobalData::teachaxisName, 9);
    ui->horizontalLayoutButtons->setSpacing(2);
//    GlobalData::teachdisplay=16;
    for(auto i = 0; i < GlobalData::teachdisplay; ++i)
    {
        //        qDebug()<<"teachaxisName"<<GlobalData::teachaxisName.at(i);
        auto btn_plus = new QPushButtonLongPress(this,GlobalData::teachaxisName.at(i) + "+");
        auto btn_minus = new QPushButtonLongPress(this,GlobalData::teachaxisName.at(i) + "-");


        btn_plus->setMinimumHeight(this->height()/10);
        btn_minus->setMinimumHeight(this->height()/10);
        qDebug()<<btn_plus->height()<<this->height();
        QVBoxLayout *VLayout = new QVBoxLayout();
        VLayout->addWidget(btn_plus);
        VLayout->addWidget(btn_minus);
        ui->horizontalLayoutButtons->addLayout(VLayout);
        //stylesheet
        btn_plus->setStyleSheet(UIStyles::css_maincolorconf_u2);
        btn_minus->setStyleSheet(UIStyles::css_maincolorconf_u2);

        // extra info
        btn_plus->setProperty(PROP_AXISNUM, GlobalData::teachaxis[i] + 1);
        btn_minus->setProperty(PROP_AXISNUM, GlobalData::teachaxis[i] + 1);
        //        qDebug()<<GlobalData::teachdisplay<<GlobalData::teachaxis[i];
        btn_plus->setProperty(PROP_DIRECTION, MovePlus);
        btn_minus->setProperty(PROP_DIRECTION, MoveMinus);

        connect(btn_plus, SIGNAL(keyPress(int)), this, SLOT(_onMoveButtonPress(int)));
        connect(btn_minus, SIGNAL(keyPress(int)), this, SLOT(_onMoveButtonPress(int)));
    }

    // bottom btns
    QList<QPushButtonsPage::item_t> listButtonItems =
    {
        {tr("添加"), Definitions::BTN_TEACH_ADD},
        {tr("修改"), Definitions::BTN_TEACH_MODIFY},
        {tr("删除"), Definitions::BTN_TEACH_DELETE},
        {tr("上孔"), Definitions::BTN_TEACH_PREVHOLE},
        {tr("下孔"), Definitions::BTN_TEACH_NEXTHOLE},
        {tr("定位"), Definitions::BTN_TEACH_LOCATE},
        {tr("插入"), Definitions::BTN_TEACH_INSERT},
        {tr("分孔"), Definitions::BTN_TEACH_DIVIDE},
        {tr("上页"), Definitions::BTN_TEACH_PAGEUP},
        {tr("下页"), Definitions::BTN_TEACH_PAGEDOWN},
        {tr("复制"),  Definitions::BTN_TEACH_COPY},
        {tr("量色"), Definitions::BTN_TEACH_COLOR_MULTIPLE},
        {tr("量删"), Definitions::BTN_TEACH_DELETE_MULTIPLE},
        {tr("量速"), Definitions::BTN_TEACH_SPINDLE_MULTIPLE},
        {tr("平移"), Definitions::BTN_TEACH_SHIFT},
    #ifdef Arm_Kaiteli
        {tr("斜角"), Definitions::BTN_TEACH_ANGLE},
    #endif
        {tr("花样"), Definitions::BTN_TEACH_PATTERN},
        {tr("矩删"), Definitions::BTN_TEACH_DELETERECT},
        {tr("偏移"), Definitions::BTN_TEACH_AXISOFFSET},

    };

    GlobalData::pushButtonPage->init(listButtonItems);
    connect(GlobalData::pushButtonPage, SIGNAL(buttonClicked(int)),
            this, SLOT(_onClickedBottomButton(int)));

    // table
    connect(this, SIGNAL(TableChangePageNum(int)),
            ui->tableViewTeach, SLOT(onPageChanged(int)));
    connect(ui->tableViewTeach, SIGNAL(pointDataChangedByUserInput(int)),
            this, SLOT(onPointDataChangedByUserInput(int)));
    connect(ui->tableViewTeach, SIGNAL(reloadPageTeachData()),
            this, SLOT(onReloadPageTeachData()));
    connect(ui->tableViewTeach, SIGNAL(notifyTowriteCylinder(int, int)),
            this, SLOT(_onNotifyTowriteCylinder(int, int)));
    connect(ui->tableViewTeach, SIGNAL(pmoveToPoint(int)),
            this, SLOT(_onPmoveToPoint(int)));

    // modbus callback
    connect(this, SIGNAL(_signal_modbusQueryCallback(bool, void*)),
            this, SLOT(onModbusQueryCallbackEnd(bool, void*)));
    connect(this, SIGNAL(sendModbusQuery(Definitions::ModbusQueryItem*)),
            this, SLOT(onSendModbusQuery(Definitions::ModbusQueryItem*)));

    // global signals
    connect(GlobalData::getInstance(), SIGNAL(updateRealtimeInfo()),
            this, SLOT(onUpdateRealtimeInfo()));
    //鼠标点击最后一孔后空白孔位，自动添加一个孔
    connect(ui->tableViewTeach, SIGNAL(addNewPiont()),this, SLOT(_onAddNewPoint()));

    onUpdateRealtimeInfo(); // refresh now!
    onTotalPointChanged(_totalPoint);
    onPointIndexChanged(_pointIndex);
    onReloadPageTeachData();//刷新当前坐标
}

SubWindowTeachTable::~SubWindowTeachTable()
{
    Utils::waitForPendingModbus(&modbusPendingCount);
    delete ui;
}

void SubWindowTeachTable::onUpdateRealtimeInfo()
{
    static quint32 lastTeachCounter = 0xffffffff;
    static quint8 lastWorkStatus = 0xff;
    static quint16 lastHandRange = 0xffff;

    ui->widgetStatus->onUpdateAxisPosition();
#ifdef Arm_Kaiteli
    emit quickteach(GlobalData::realTimeInfo.iFastTeach);
#elif ARM
    emit linkage();
#endif
    if(lastTeachCounter != GlobalData::realTimeInfo.iTeachAcc)
    {
        onReloadPageTeachData();
        lastTeachCounter = GlobalData::realTimeInfo.iTeachAcc;
    }

    if(lastWorkStatus != GlobalData::realTimeInfo.iWSta)
    {
        if(Stop != lastWorkStatus && Stop == GlobalData::realTimeInfo.iWSta)
            emit workstausChangedToStop();

        lastWorkStatus = GlobalData::realTimeInfo.iWSta;
    }

    //add by yzg 190526
    if(lastHandRange != GlobalData::realTimeInfo.iHandRange)
    {
        emit SetTeachStepText();
        lastHandRange=GlobalData::realTimeInfo.iHandRange;
    }
    //ui->pushButton_linkage->setStyleSheet(GlobalData::realTimeInfo.iLinkage ? UIStyles::css_maincolorconf_u13 : UIStyles::css_maincolorconf_u8);
}


void SubWindowTeachTable::onPointIndexChanged(int pointIndex)
{//窗口跟随
    ui->widgetStatus->onUpdateCurrentPointIndex(pointIndex);

    //    if(Run != GlobalData::realTimeInfo.iWSta)
    ui->tableViewTeach->onNewPointIndexChanged(pointIndex);
}

void SubWindowTeachTable::onTotalPointChanged(int totalPoint)
{
    ui->widgetStatus->onUpdateTotalPoint(totalPoint);

    ui->tableViewTeach->onTotalPointNumChanged(totalPoint);
}

void SubWindowTeachTable::onPointDataChangedByUserInput(int pointIndex)
{//不通过修改按键,手动修改
    TEACH_SEND* teachSend = new TEACH_SEND;
    memset(teachSend,0,sizeof(TEACH_SEND));
    void* p = &teachSend->SendData.Teach_Data;//数据传递
    memcpy(p, Workfile::getInstance()->getTeachData() + pointIndex, sizeof(TEACH_DATA)); // 复制一个点的数据
    teachSend->Num = pointIndex;
#if (defined Arm_460Y) || (defined Arm_Kaiteli)|| (defined ARM)
    if(pointIndex!=0)
        teachSend->Type = TEACH_MODIFY3;
    else
        teachSend->Type = TEACH_MODIFY3;
#else
    if(pointIndex!=0)
        teachSend->Type = TEACH_MODIFY;
#endif

    sendTEACHSEND(teachSend);
}

void SubWindowTeachTable::onDialogAcceptedSplit()
{
    TEACH_SEND* teachSend = new TEACH_SEND;
    memset(teachSend,0,sizeof(TEACH_SEND));
    teachSend->Type = TEACH_SPLIT;

    Utils::copyValues(GlobalData::realTimeInfo.iProg, teachSend->SendData.N_Apart.IoSta, 6);
    teachSend->SendData.N_Apart.iSplit_N = teachDialogVars.divides;
    teachSend->SendData.N_Apart.iColor = GlobalData::realTimeInfo.iColorSta;

    sendTEACHSEND(teachSend);
}

void SubWindowTeachTable::onDialogAcceptedCopy()
{
    TEACH_SEND* teachSend = new TEACH_SEND;
    memset(teachSend,0,sizeof(TEACH_SEND));
    teachSend->Type = TEACH_COPY;
    teachSend->SendData.N_COPY.iStart_N = teachDialogVars.startIndex;
    teachSend->SendData.N_COPY.iEnd_N = teachDialogVars.endIndex;
    for(int i=0;i<TEACH_AXIS;i++)
        teachSend->SendData.N_COPY.fAxisPos[i] = GlobalData::realTimeInfo.fPos[i];
    sendTEACHSEND(teachSend);
}

void SubWindowTeachTable::onDialogAcceptedLocate()
{
    TEACH_SEND* teachSend = new TEACH_SEND;
    memset(teachSend,0,sizeof(TEACH_SEND));
#ifdef Arm_460Y
    teachSend->Type = TOOTHFIXED_HOLE;
#else
    teachSend->Type = FIXED_HOLE;
#endif
    teachSend->Num = teachDialogVars.startIndex;
    sendTEACHSEND(teachSend);
}

void SubWindowTeachTable::onDialogAcceptedDeleteMultiple()
{
    TEACH_SEND* teachSend = new TEACH_SEND;
    memset(teachSend,0,sizeof(TEACH_SEND));
    teachSend->SendData.N_Color.iStart_N = teachDialogVars.startIndex;
    teachSend->SendData.N_Color.iEnd_N = teachDialogVars.endIndex;
    teachSend->Type = TEACH_MUL_DEL;
    teachSend->Num = teachDialogVars.startIndex;
    sendTEACHSEND(teachSend);
}

void SubWindowTeachTable::onDialogAcceptedSpindleMultiple()
{
    TEACH_SEND* teachSend = new TEACH_SEND;
    memset(teachSend,0,sizeof(TEACH_SEND));
    teachSend->SendData.N_SPDL.iStart_N = teachDialogVars.startIndex;
    teachSend->SendData.N_SPDL.iEnd_N = teachDialogVars.endIndex;
    teachSend->SendData.N_SPDL.iSpindle = teachDialogVars.spindle;
    teachSend->Type = TEACH_MUL_SPL;
    teachSend->Num = teachDialogVars.startIndex;
    sendTEACHSEND(teachSend);
}
void SubWindowTeachTable::onDialogAcceptedDeleteRcet()
{
    TEACH_SEND* teachSend = new TEACH_SEND;
    memset(teachSend,0,sizeof(TEACH_SEND));
    teachSend->Type = TEACH_DELETERECT;
    teachSend->SendData.N_DelRect.fDistanx = delteachSend.SendData.N_DelRect.fDistanx;
    teachSend->SendData.N_DelRect.fDistany = delteachSend.SendData.N_DelRect.fDistany;
    teachSend->SendData.N_DelRect.indix1  = delteachSend.SendData.N_DelRect.indix1;
    teachSend->SendData.N_DelRect.indix2  = delteachSend.SendData.N_DelRect.indix2;

    qDebug()<<teachSend->SendData.N_DelRect.fDistanx<<teachSend->SendData.N_DelRect.fDistany<<teachSend->SendData.N_DelRect.indix1<<teachSend->SendData.N_DelRect.indix2 ;
    sendTEACHSEND(teachSend);
}

void SubWindowTeachTable::onDialogAcceptedMultiColor()
{
    TEACH_SEND* teachSend = new TEACH_SEND;
    memset(teachSend,0,sizeof(TEACH_SEND));
    teachSend->Type = TEACH_MUL_COLOR;
    teachSend->SendData.N_Color.iStart_N = teachDialogVars.startIndex;
    teachSend->SendData.N_Color.iEnd_N = teachDialogVars.endIndex;
    teachSend->SendData.N_Color.iColor = teachDialogVars.color;
    sendTEACHSEND(teachSend);
}

void SubWindowTeachTable::onDialogAcceptedShift()
{
    auto teachSend = new TEACH_SEND;
    memset(teachSend, 0, sizeof(TEACH_SEND));

    teachSend->Type = TEACH_SHIFT;
    teachSend->SendData.N_Offset.iStart_N=teachDialogVars.startIndex;
    teachSend->SendData.N_Offset.iEnd_N=teachDialogVars.endIndex;
    for(int i=0;i<TEACH_AXIS;i++)
        teachSend->SendData.N_COPY.fAxisPos[i] = GlobalData::realTimeInfo.fPos[i];
    Utils::copyValues(GlobalData::realTimeInfo.fPos,teachSend->SendData.N_Offset.fAxisShift,6);
    teachSend->SendData.N_Offset.fCoord[1]=GlobalData::realTimeInfo.iMovesta * ATPARA.fLength_Two;

    sendTEACHSEND(teachSend);
}

void SubWindowTeachTable::_onPmoveToPoint(int pointIndex)
{
    TEACH_SEND* teachSend = new TEACH_SEND;
    memset(teachSend,0,sizeof(TEACH_SEND));
    teachSend->Type = FIXED_HOLE;
    teachSend->Num = pointIndex;
    sendTEACHSEND(teachSend);
}

void SubWindowTeachTable::_onMoveButtonPress(int pressType)
{
    auto axis = qvariant_cast<int>(sender()->property(PROP_AXISNUM));
    auto direction = qvariant_cast<int>(sender()->property(PROP_DIRECTION));
#ifdef Arm_460Y
    //Ａ B　轴不能同时有数据　会撞击
    //    qDebug()<<axis<<GlobalData::realTimeInfo.fPos[4]<<GlobalData::realTimeInfo.fPos[5];
    if((axis==4)&&(fabs(GlobalData::realTimeInfo.fPos[4])>=0.01))
    {
        //        qDebug()<<"axis==4";
        return;
    }
    if((axis==5)&&(fabs(GlobalData::realTimeInfo.fPos[3])>=0.01))
    {
        //         qDebug()<<"axis==5";
        return;
    }
#endif
    emit moveAxis(axis, direction, pressType, GlobalData::TEACH_STEPS[GlobalData::stepIndex]);
}

void SubWindowTeachTable::_onAddNewPoint()
{//最后孔添加孔位
    //qDebug()<<"comming";
    TEACH_SEND* teachSend = new TEACH_SEND;
    memset(teachSend,0,sizeof(TEACH_SEND));
    //    Utils::copyValues(GlobalData::realTimeInfo.iProg, teachSend->SendData.Teach_Data.iOut, 6);
    //    Utils::copyValues(GlobalData::realTimeInfo.fPos, teachSend->SendData.Teach_Data.fMove_pos, GlobalData::totalAxisNum);
    //    teachSend->SendData.Teach_Data.icolor = GlobalData::realTimeInfo.iColorSta;
    teachSend->Type = TEACH_ADD;
    teachSend->Num = ui->tableViewTeach->getTotalPointNum();
    sendTEACHSEND(teachSend);
}
void SubWindowTeachTable::_onClickedBottomButton(int btn_id)
{
    memset(&teachDialogVars, 0, sizeof(teachDialogVars));

    if(Run == GlobalData::realTimeInfo.iWSta)
    {
        if(!(btn_id==Definitions::BTN_TEACH_PAGEUP||btn_id==Definitions::BTN_TEACH_PAGEDOWN))
        {//允许上下翻页
            return;
        }
    }
    switch(btn_id)
    {
    case Definitions::BTN_TEACH_ADD: // 添加
    case Definitions::BTN_TEACH_INSERT: // 插入
    {
        TEACH_SEND* teachSend = new TEACH_SEND;
        memset(teachSend,0,sizeof(TEACH_SEND));

        Utils::copyValues(GlobalData::realTimeInfo.iProg, teachSend->SendData.Teach_Data.iOut, 6);
        Utils::copyValues(GlobalData::realTimeInfo.fPos, teachSend->SendData.Teach_Data.fMove_pos, GlobalData::totalAxisNum);

        teachSend->SendData.Teach_Data.icolor = GlobalData::realTimeInfo.iColorSta;

        if(btn_id == Definitions::BTN_TEACH_ADD)
        {
            teachSend->Type = TEACH_ADD;
            teachSend->Num = ui->tableViewTeach->getTotalPointNum();
        }
        else
        {
            teachSend->Type = TEACH_INSERT;
            teachSend->Num = ui->tableViewTeach->getPointIndex();
        }

        sendTEACHSEND(teachSend);
        break;
    }

    case Definitions::BTN_TEACH_MODIFY: // 修改
    {//此处的修改  没有添加颜色 一般用于手盒移动坐标输入
        if(QMessageBox::Retry != Utils::MyMessageBox(QString("Warning"), tr("修改教导点%1吗？").arg(GlobalData::realTimeInfo.iCNum),
                                                     tr("确定"),tr("取消"),this))
            break;

        TEACH_SEND* teachSend = new TEACH_SEND;
        memset(teachSend,0,sizeof(TEACH_SEND));
        void* p = &teachSend->SendData.Teach_Data;//数据传递
        memcpy(p, Workfile::getInstance()->getTeachData() + GlobalData::realTimeInfo.iCNum, sizeof(TEACH_DATA)); // 复制一个点的数据
        teachSend->Type = TEACH_MODIFY;
        teachSend->Num = GlobalData::realTimeInfo.iCNum; // 实际孔
        Utils::copyValues(GlobalData::realTimeInfo.iProg, teachSend->SendData.Teach_Data.iOut, 6);
        Utils::copyValues(GlobalData::realTimeInfo.fPos, teachSend->SendData.Teach_Data.fMove_pos, 6);
        sendTEACHSEND(teachSend);
        break;
    }

    case Definitions::BTN_TEACH_DELETE: // 删除
    {
        if(QMessageBox::Retry != Utils::MyMessageBox(QString("Warning"), tr("删除教导点%1吗？").arg(ui->tableViewTeach->getPointIndex()),
                                                     tr("确定"),tr("取消"),this))
            break;

        TEACH_SEND* teachSend = new TEACH_SEND;
        memset(teachSend,0,sizeof(TEACH_SEND));
        teachSend->Num = ui->tableViewTeach->getPointIndex(); // 选中那个列
        teachSend->Type = TEACH_DELETE;
        sendTEACHSEND(teachSend);
        break;
    }
    case Definitions::BTN_TEACH_DELETERECT: // 矩形删除
    {
        memset(&delteachSend,0,sizeof(TEACH_SEND));

        QList<QDialogDataForm::item_t> items =
        {
            {
                tr("范围1"), Adapters::TYPE_F32, &delteachSend.SendData.N_DelRect.fDistanx, false, 0, 100, nullptr
            },
            {
                tr("范围2"), Adapters::TYPE_F32, &delteachSend.SendData.N_DelRect.fDistany, false, 0, 100, nullptr
            },
            {
                tr("轴1"), Adapters::TYPE_StringList, &delteachSend.SendData.N_DelRect.indix1, false, 0, GlobalData::totalAxisNum - 1.0,
                const_cast<QStringList*>(&GlobalData::axisName)
            },
            {
                tr("轴2"), Adapters::TYPE_StringList, &delteachSend.SendData.N_DelRect.indix2, false, 0, GlobalData::totalAxisNum - 1.0,
                const_cast<QStringList*>(&GlobalData::axisName)
            },

        };

        auto w = new QDialogDataForm(this, Qt::FramelessWindowHint);
        connect(w, SIGNAL(accepted()), this, SLOT(onDialogAcceptedDeleteRcet()));
        //w->showDialog(QString::fromUtf8(tr("批量删除教导点")), items);
        w->showDialog(tr("矩形删除教导点"), items);


        break;
    }
    case Definitions::BTN_TEACH_DELETE_MULTIPLE: // 量删
    {
        if(ui->tableViewTeach->getPointIndex())
            teachDialogVars.startIndex = ui->tableViewTeach->getPointIndex();
        teachDialogVars.endIndex = ui->tableViewTeach->getTotalPointNum() - 1;

        QList<QDialogDataForm::item_t> items =
        {
            {tr("从这个点"), Adapters::TYPE_S32, &teachDialogVars.startIndex, false, 0, ui->tableViewTeach->getTotalPointNum() - 1.0, nullptr},
            {tr("到这个点"), Adapters::TYPE_S32, &teachDialogVars.endIndex, false, 0, ui->tableViewTeach->getTotalPointNum() - 1.0, nullptr}

        };

        auto w = new QDialogDataForm(this, Qt::FramelessWindowHint);
        connect(w, SIGNAL(accepted()), this, SLOT(onDialogAcceptedDeleteMultiple()));
        //w->showDialog(QString::fromUtf8(tr("批量删除教导点")), items);
        w->showDialog(tr("批量删除教导点"), items);

        break;
    }

    case Definitions::BTN_TEACH_SPINDLE_MULTIPLE:
    {
        qDebug()<<"量速度\r\n";
        if(ui->tableViewTeach->getPointIndex())
            teachDialogVars.startIndex = ui->tableViewTeach->getPointIndex();
        teachDialogVars.endIndex = ui->tableViewTeach->getTotalPointNum() - 1;

        QList<QDialogDataForm::item_t> items =
        {
            {tr("速度"), Adapters::TYPE_S32, &teachDialogVars.spindle, false, 0, 100, nullptr},
            {tr("开始孔"), Adapters::TYPE_S32, &teachDialogVars.startIndex, false, 1, ui->tableViewTeach->getTotalPointNum() - 1.0, nullptr},
            {tr("结束孔"), Adapters::TYPE_S32, &teachDialogVars.endIndex, false, 1, ui->tableViewTeach->getTotalPointNum() - 1.0, nullptr},
        };

        auto w = new QDialogDataForm(this, Qt::FramelessWindowHint);
        connect(w, SIGNAL(accepted()), this, SLOT(onDialogAcceptedSpindleMultiple()));
        //w->showDialog(QString::fromUtf8(tr("批量编辑速度")), items);
        w->showDialog(tr("批量编辑速度"), items);


    }
        break;

    case Definitions::BTN_TEACH_PREVHOLE: // 上孔
    case Definitions::BTN_TEACH_NEXTHOLE: // 下孔
    {

        TEACH_SEND* teachSend = new TEACH_SEND;
        memset(teachSend,0,sizeof(TEACH_SEND));
#ifdef Arm_460Y
        teachSend->Type = (Definitions::BTN_TEACH_PREVHOLE == btn_id ? TOOTHLAST_HOLE : TOOTHNEXT_HOLE);
#else
        teachSend->Type = (Definitions::BTN_TEACH_PREVHOLE == btn_id ? LAST_HOLE : NEXT_HOLE);
#endif
        sendTEACHSEND(teachSend);

        break;
    }

    case Definitions::BTN_TEACH_LOCATE: // 定位
    {

        teachDialogVars.startIndex = ui->tableViewTeach->getPointIndex();
        QList<QDialogDataForm::item_t> items =
        {
            //{QString::fromUtf8(tr("定位")), Adapters::TYPE_S32, &teachDialogVars.startIndex, false, 0, ui->tableViewTeach->getTotalPointNum() - 1.0, nullptr},
            {tr("定位"), Adapters::TYPE_S32, &teachDialogVars.startIndex, false, 0, ui->tableViewTeach->getTotalPointNum() - 1.0, nullptr},

        };

        auto w = new QDialogDataForm(this, Qt::FramelessWindowHint);
        connect(w, SIGNAL(accepted()), this, SLOT(onDialogAcceptedLocate()));
        //w->showDialog(QString::fromUtf8(tr("定位到某一个孔")), items);
        w->showDialog(tr("定位到某一个孔"), items);

        break;
    }

    case Definitions::BTN_TEACH_DIVIDE: // 分孔
    {
        teachDialogVars.startIndex = ui->tableViewTeach->getPointIndex();

        if(2 > teachDialogVars.startIndex) // 分孔是当前孔与上一个孔进行分孔，pointIndex小于2则不分孔
            return;

        teachDialogVars.divides = 1;

        QList<QDialogDataForm::item_t> items =
        {
            //{QString::fromUtf8(tr("分孔数")), Adapters::TYPE_S32, &teachDialogVars.divides, false, 2, MAXTEACHNUM - 1, nullptr},
            {tr("分孔数"), Adapters::TYPE_S32, &teachDialogVars.divides, false, 1, MAXTEACHNUM - 1, nullptr},

        };

        auto w = new QDialogDataForm(this, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        connect(w, SIGNAL(accepted()), this, SLOT(onDialogAcceptedSplit()));
        //w->showDialog(QString::fromUtf8(tr("分孔：当前孔与上一个孔之间，要分多少孔")), items, true);
        w->showDialog(tr("分孔：当前孔与上一个孔之间，要分多少孔"), items, true);

        break;
    }

    case Definitions::BTN_TEACH_COPY: // 复制
    {
        teachDialogVars.startIndex = ui->tableViewTeach->getPointIndex();

        if(0 >= teachDialogVars.startIndex)
            teachDialogVars.startIndex = 1;

        teachDialogVars.endIndex = ui->tableViewTeach->getTotalPointNum() - 1;

        QList<QDialogDataForm::item_t> items =
        {
            //{QString::fromUtf8(tr("开始孔")), Adapters::TYPE_S32, &teachDialogVars.startIndex, false, 1, ui->tableViewTeach->getTotalPointNum() - 1.0, nullptr},
            //{QString::fromUtf8(tr("结束孔")), Adapters::TYPE_S32, &teachDialogVars.endIndex, false, 1, ui->tableViewTeach->getTotalPointNum() - 1.0, nullptr},
            {tr("开始孔"), Adapters::TYPE_S32, &teachDialogVars.startIndex, false, 1, ui->tableViewTeach->getTotalPointNum() - 1.0, nullptr},
            {tr("结束孔"), Adapters::TYPE_S32, &teachDialogVars.endIndex, false, 1, ui->tableViewTeach->getTotalPointNum() - 1.0, nullptr},

        };

        auto w = new QDialogDataForm(this, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        connect(w, SIGNAL(accepted()), this, SLOT(onDialogAcceptedCopy()));
        //w->showDialog(QString::fromUtf8(tr("复制：从开始孔")), items, true);
        w->showDialog(tr("复制：从开始孔"), items, true);
        break;
    }

    case Definitions::BTN_TEACH_PAGEUP: // 上页
    {
        auto pageNum = ui->tableViewTeach->getCurrentPageNum();

        if(pageNum <= 0)
            return;

        emit TableChangePageNum(--pageNum);
        break;
    }

    case Definitions::BTN_TEACH_PAGEDOWN: // 下页
    {
        auto pageNum = ui->tableViewTeach->getCurrentPageNum();

        if(pageNum >= ui->tableViewTeach->getMaxPage())
            return;

        emit TableChangePageNum(++pageNum);
        break;
    }

    case Definitions::BTN_TEACH_COLOR_MULTIPLE: // 量色
    {
        teachDialogVars.startIndex = ui->tableViewTeach->getPointIndex();
        teachDialogVars.endIndex = ui->tableViewTeach->getTotalPointNum() - 1;
        teachDialogVars.color=0;
        auto colorList=QStringList();
#ifdef Arm_460Y
        auto colorList=QStringList({tr("不变"),tr("颜色1"), tr("颜色2")});
#elif Arm_Kaiteli
        auto colorList=QStringList({tr("关"), tr("开")});
#else
        if(SYS_ZM.bThreeColor)
             colorList=QStringList({tr("不变"),tr("毛1"), tr("毛2"), tr("皮")});
         else
             colorList=QStringList({tr("不变"),tr("毛"), tr("皮")});
#endif

       /* if(SYS_ZM.bThreeColor)
            colorList<<tr("颜色3");*/

        QList<QDialogDataForm::item_t> items =
        {
            //            {QString::fromUtf8(tr("颜色")), Adapters::TYPE_StringList, &teachDialogVars.color, false, 0, 2, &colorList},
            //            {QString::fromUtf8(tr("开始孔")), Adapters::TYPE_S32, &teachDialogVars.startIndex, false, 1, ui->tableViewTeach->getTotalPointNum() - 1.0, nullptr},
            //            {QString::fromUtf8(tr("结束孔")), Adapters::TYPE_S32, &teachDialogVars.endIndex, false, 1, ui->tableViewTeach->getTotalPointNum() - 1.0, nullptr},
            {tr("颜色"), Adapters::TYPE_StringList, &teachDialogVars.color, false, 0, 2, &colorList},
            {tr("开始孔"), Adapters::TYPE_S32, &teachDialogVars.startIndex, false, 1, ui->tableViewTeach->getTotalPointNum() - 1.0, nullptr},
            {tr("结束孔"), Adapters::TYPE_S32, &teachDialogVars.endIndex, false, 1, ui->tableViewTeach->getTotalPointNum() - 1.0, nullptr},

        };

        auto w = new QDialogDataForm(this, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        connect(w, SIGNAL(accepted()), this, SLOT(onDialogAcceptedMultiColor()));
        //w->showDialog(QString::fromUtf8(tr("批量修改颜色")), items, true);
        w->showDialog(tr("批量修改颜色"), items, true);

        break;
    }

#ifdef Arm_Kaiteli
    case  Definitions::BTN_TEACH_ANGLE:
    {
        TEACH_SEND* teachSend = new TEACH_SEND;
        memset(teachSend,0,sizeof(TEACH_SEND));
        teachSend->Type = TEACH_CAM_XIEJIAO;
        sendTEACHSEND(teachSend);
        break;
    }
#endif


    case Definitions::BTN_TEACH_SHIFT: // 平移
    {
        teachDialogVars.startIndex = GlobalData::realTimeInfo.iCNum;
        teachDialogVars.endIndex = ui->tableViewTeach->getTotalPointNum() - 1;


        QList<QDialogDataForm::item_t> items =
        {
            //            {QString::fromUtf8(tr("开始孔")), Adapters::TYPE_S32, &teachDialogVars.startIndex, true, 1, ui->tableViewTeach->getTotalPointNum() - 1.0, nullptr},
            //            {QString::fromUtf8(tr("结束孔")), Adapters::TYPE_S32, &teachDialogVars.endIndex, false, 1, ui->tableViewTeach->getTotalPointNum() - 1.0, nullptr},
            {tr("开始孔"), Adapters::TYPE_S32, &teachDialogVars.startIndex, false, 1, ui->tableViewTeach->getTotalPointNum() - 1.0, nullptr},
            {tr("结束孔"), Adapters::TYPE_S32, &teachDialogVars.endIndex, false, 1, ui->tableViewTeach->getTotalPointNum() - 1.0, nullptr},

        };

        auto w = new QDialogDataForm(this, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        connect(w, SIGNAL(accepted()), this, SLOT(onDialogAcceptedShift()));
        //w->showDialog(QString::fromUtf8(tr("批量偏移加工点")), items, true);
        w->showDialog(tr("批量偏移加工点"), items, true);

        break;
    }
    case Definitions::BTN_TEACH_AXISOFFSET: // 偏移
    {
        QDialogParamOffest *dialog = new QDialogParamOffest(this);
        dialog->show();
        break;
    }
    default:
        break;
    }
    if(btn_id!=Definitions::BTN_TEACH_PATTERN)
        onReloadPageTeachData();//刷新当前坐标
}

void SubWindowTeachTable::_onNotifyTowriteCylinder(int cylinderIndex, int status)
{
    auto queryItem = new Definitions::ModbusQueryItem;
    queryItem->queryAction = Action_WriteCylinder;
    queryItem->mixData.u32  = static_cast<unsigned short>(cylinderIndex);
    queryItem->mixData.u32 |= static_cast<unsigned short>(status) << 16;
    emit sendModbusQuery(queryItem);
}

void SubWindowTeachTable::sendTEACHSEND(TEACH_SEND* teachSend)
{
    auto queryItem = new Definitions::ModbusQueryItem;
    queryItem->queryAction = Action_TeachSend;
    queryItem->mixData.otherData = teachSend;
    emit sendModbusQuery(queryItem);
}

//add by yzg 190529
void SubWindowTeachTable::_onsendTeachAction(unsigned int data)
{
    auto queryItem = new Definitions::ModbusQueryItem;
    queryItem->queryAction = Action_TeachStep;
    queryItem->mixData.u16 = static_cast<unsigned short>(data);
    emit sendModbusQuery(queryItem);
}

void SubWindowTeachTable::_onsendQuickTeach()
{
#ifdef Arm_Kaiteli
    auto queryItem = new Definitions::ModbusQueryItem;
    queryItem->queryAction = Action_WriteButtonCommand;
    queryItem->mixData.u16 = SW_FASTTEACH;
    emit sendModbusQuery(queryItem);
#endif
}

void SubWindowTeachTable::onReloadPageTeachData()
{
    int pageNum = ui->tableViewTeach->getCurrentPageNum();
    int startIndex = 0;
    int pointCount = 0;

    ui->tableViewTeach->getStartIndexFromPage(pageNum, &startIndex, &pointCount);

    // debug_printf("onReloadPageTeachData: startIndex=%d, pointCount=%d",startIndex,pointCount);

    auto d = new QDialogLoadingWorkfile(this);
    d->setAttribute(Qt::WA_DeleteOnClose);
    connect(d, SIGNAL(accepted()),
            ui->tableViewTeach, SLOT(reloadItemModel())); // 自动刷新教导数据
    d->readWrite(true, startIndex, pointCount,true);//刷新当前页数
    //qDebug()<<"readworkfile";
}

void SubWindowTeachTable::onSendModbusQuery(Definitions::ModbusQueryItem* queryItem)
{
    bool isQueryValid = true;
    auto element = new ModbusQueue::element_t;

    switch(queryItem->queryAction)
    {
    case Action_TeachSend:
        element->data = queryItem->mixData.otherData;
        element->len = sizeof(TEACH_SEND);
        element->operation = ModbusQueue::OP_WRITE_DATA;
        element->startAddress = ModbusAddress::TeachWorkfileHead;
        break;

    case Action_WriteCylinder:
    {
        auto cylinderIndex = queryItem->mixData.u32 & 0xffff;
        auto realIO = Adapters::getRealIOFromFunctionCode(OUT_PROG1 + cylinderIndex, false);

        queryItem->mixData.u16 = static_cast<unsigned short>(queryItem->mixData.u32 >> 16);

        element->data = &queryItem->mixData.u16;
        element->len = sizeof(queryItem->mixData.u16);
        element->operation = ModbusQueue::OP_WRITE_DATA;
        element->startAddress = ModbusAddress::IOStatus_Out + realIO;
        break;
    }

        //教导步长 add  by yzg 190529
    case Action_TeachStep:
        element->data = &queryItem->mixData.u16;
        element->len = sizeof(queryItem->mixData.u16);
        element->operation = ModbusQueue::OP_WRITE_DATA;
        element->startAddress = ModbusAddress::g_WheelGear;
        break;
    case Action_WriteButtonCommand:
        element->data = &queryItem->mixData.u16;
        element->len = sizeof(queryItem->mixData.u16);
        element->operation = ModbusQueue::OP_WRITE_DATA;
        element->startAddress = ModbusAddress::ButtonOperate;
        break;

    default:
        isQueryValid = false;
        break;
    }

    if(true == isQueryValid)
    {
        element->pointerObject = queryItem;
        element->callback = std::tr1::bind(&SubWindowTeachTable::_modbusQueryCallback, this, std::tr1::placeholders::_1,
                                           std::tr1::placeholders::_2);
        GlobalData::modbusQueue->sendQuery(element); // send it!
        ++modbusPendingCount;
    }
    else
    {
        Utils::delete_pointer(element);
        Utils::delete_pointer(queryItem);
    }
}

void SubWindowTeachTable::onModbusQueryCallbackEnd(bool isOk, void* flag)
{
    bool isNeedDelete = true;

    auto element = reinterpret_cast<ModbusQueue::element_t*>(flag);
    auto queryItem = reinterpret_cast<Definitions::ModbusQueryItem*>(element->pointerObject);

    if(isOk)
    {
        retrytime=0;
        switch(queryItem->queryAction)
        {
        default:
            break;
        }
    }
    else
    {
        if(true == Utils::dialogIsModbusRetry(this, GlobalData::modbusQueue))
        {
            GlobalData::modbusQueue->sendQuery(element, true, false);
            GlobalData::modbusQueue->queryResume();
            isNeedDelete = false;
        }
        else
        {
            GlobalData::modbusQueue->queryStopThenOffline();
        }
    }

    if(true == isNeedDelete)
    {
        // free special memory
        switch(queryItem->queryAction)
        {
        case Action_TeachSend:
        {
            onUpdateRealtimeInfo();
            auto teachSend = reinterpret_cast<TEACH_SEND*>(queryItem->mixData.otherData);
            if(teachSend->Type == TEACH_DELETE)
            {//如果删除的是最后一空，红框跳到前面一空
                unsigned int current = ui->tableViewTeach->getPointIndex();
                if(current == GlobalData::realTimeInfo.iMNum-1)
                    onPointIndexChanged(GlobalData::realTimeInfo.iMNum-2);
            }
            Utils::delete_pointer(teachSend);
            break;
        }

        default:
            break;
        }

        // free general memory
        Utils::delete_pointer(queryItem);
        Utils::delete_pointer(element);

        --modbusPendingCount;
    }
}

void SubWindowTeachTable::mousePressEvent(QMouseEvent *event)
{//弹出框之外的点击关闭弹出框
    //qDebug()<<event->pos();
    if(event->pos().x()<=dialogStart.x()||event->pos().x()>=dialogStart.x()||
            event->pos().y()<=dialogEnd.y()||event->pos().y()>=dialogEnd.y())
    {
        emit closedialog();
        //        qDebug()<<"closedialog";
    }
    QWidget::mousePressEvent(event);
}
void SubWindowTeachTable::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
}
#endif
