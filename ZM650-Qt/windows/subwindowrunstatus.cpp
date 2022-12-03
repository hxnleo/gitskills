/*
 * subwindowrunstatus.cpp
 *
 * 运行状态界面
 *
 * Created on: 2019年 01月 21日 星期一 15:00:16 CST
 * Author: lixingcong
 */
#include "subwindowrunstatus.h"
#include "mainwindow.h"
#include "ui_subwindowrunstatus.h"
#include "data/globaldata.h"
#include "widgets/qpushbuttonspage.h"
#include "widgets/qdialogclampout.h"
#include "modbus/modbusaddress.h"
#include "data/adapters.h"
#include "data/utils.h"
#include "data/utilsforcontroller.h"
#include "data/uistyles.h"
#include <QProgressDialog>
#include "widgets/qdialogdataform.h"
#include <QMessageBox>
#include "widgets/qdialogfindzero.h"
#include "widgets/qwidgetaxisposition.h"
#include "widgets/qwidgetdrawtrail.h"
#include "data/workfile.h"
#include "widgets/qdialogloadingworkfile.h"
#include <QListView>
#include <QLineEdit>
#include <QSharedMemory>
#ifdef ARM
SubWindowRunStatus::SubWindowRunStatus(QWidget* parent) :
    QWidget(parent),
    modbusPendingCount(0),
    lastWorkStatus(0xff),
    lastPointNumber(0),
    timer(new QTimer(this)),
    ui(new Ui::SubWindowRunStatus)
{
    ui->setupUi(this);
    //在UI界面设置了widget_status 的拉伸因子 图形界面和按钮界面比例4:3
    if(GlobalData::screensize == TEN)
    {
        ui->widget_laber->setFixedHeight(100);
    }
    // position
    switchCentralWidget();//设置主界面图片布局框


    // bottom btns
    QList<QPushButtonsPage::item_t> listButtonItems =
    {//每一个界面对应一个下栏按钮
     {tr("补孔"), Definitions::BTN_SYS_BUKONG},
     {tr("定位"), Definitions::BTN_SYS_LOCATE},
     {tr("上孔"), Definitions::BTN_SYS_PREVHOLE},
     {tr("下孔"), Definitions::BTN_SYS_NEXTHOLE},
     {tr("轨迹"), Definitions::BTN_SYS_TRACEPATH}
    };

    GlobalData::pushButtonPage->init(listButtonItems);
    connect(GlobalData::pushButtonPage, SIGNAL(buttonClicked(int)),
            this, SLOT(_onClickedBottomButton(int)));

    connect(ui->pushButton_Start, SIGNAL(clicked()),
            this, SLOT(onButtonClickedStart()));

    // modbus callback
    connect(this, SIGNAL(_signal_modbusQueryCallback(bool, void*)),
            this, SLOT(onModbusQueryCallbackEnd(bool, void*)), Qt::QueuedConnection);
    connect(this, SIGNAL(sendModbusQuery(Definitions::ModbusQueryItem*)),
            this, SLOT(onSendModbusQuery(Definitions::ModbusQueryItem*)));

    // button clicked
    connect(ui->pushButton_FindZero, SIGNAL(clicked()),
            this, SLOT(onFindZero_Machine()));
    connect(ui->pushButton_MainZero, SIGNAL(clicked()),
            this, SLOT(onFindZero_MainAxis()));
#ifdef    ZUANZHI
    connect(ui->pushButton_BackZero, SIGNAL(clicked()),
            this, SLOT(onButtonClicked_FindZero()));
#else
    connect(ui->pushButton_BackZero, SIGNAL(clicked()),
            this, SLOT(onButtonClicked_GoToStart()));
#endif
    connect(ui->pushButton_Stop, SIGNAL(clicked()),
            this, SLOT(onButtonClicked_Stop()));
    connect(ui->comboBox_MotionMode, SIGNAL(activated(int)),
            this, SLOT(onComboIndexActivated(int)));
    connect(ui->label_ProducedValue, SIGNAL(clicked()),
            this, SLOT(onButtonClicked_Produced()));
    connect(ui->pushButton_speed1, SIGNAL(clicked()),
            this, SLOT(onButtonClicked_Speed1()));
    connect(ui->pushButton_speed2, SIGNAL(clicked()),
            this, SLOT(onButtonClicked_Speed2()));
    connect(ui->pushButton_table1, SIGNAL(clicked()),
            this, SLOT(onButtonClicked_Table1()));
    connect(ui->pushButton_table2, SIGNAL(clicked()),
            this, SLOT(onButtonClicked_Table2()));
    connect(ui->pushButton_drill, SIGNAL(clicked()),
            this, SLOT(onButtonClicked_Drill()));
    connect(ui->pushButton_ready, SIGNAL(clicked()),
            this, SLOT(onButtonClicked_Drill2()));
    connect(ui->pushButton_Hair, SIGNAL(clicked()),
            this, SLOT(onButtonClicked_Hair()));
    connect(ui->pushButton_mainAxis, SIGNAL(clicked()),
            this, SLOT(onButtonClicked_MainAxisBrake()));
    connect(ui->pushButton_cylinders, SIGNAL(clicked()),
            this, SLOT(onButtonClicked_ClampOut()));
    connect(ui->label_InverterSpeedValue, SIGNAL(clicked()),
            this, SLOT(onButtonClicked_SpeedValue()));
    connect(ui->pushButton_cylindersSJX, SIGNAL(clicked()),
            this, SLOT(onButtonClicked_Cylinder()));
    connect(ui->label_drillHoleValue, SIGNAL(clicked()),
            this, SLOT(onButtonClicked_DrillHole())); //当前钻头加工孔位数  add 2022.11.30
    // global signals
    connect(GlobalData::getInstance(), SIGNAL(updateRealtimeInfo()),
            this, SLOT(onUpdateRealtimeInfo()));

    //close axis
//    if(!GlobalData::closebtn)
//       ui->pushButton_mainAxis->closeaxis = false;
//    else
//       ui->pushButton_mainAxis->closeaxis = true;
    ui->pushButton_mainAxis->closeaxis = false;

    auto setBtnIcon = [](QPushButton * btn, QIcon icon)
    {
        btn->setIcon(icon);
        if(GlobalData::screensize == TEN)
        btn->setIconSize(QSize(32,32));
        else if(GlobalData::screensize == SEVEN)
        btn->setIconSize(QSize(24,24));

    };

    // icon
    setBtnIcon(ui->pushButton_Start, QApplication::style()->standardIcon(QStyle::SP_MediaPlay));
    setBtnIcon(ui->pushButton_Stop, QApplication::style()->standardIcon(QStyle::SP_MediaStop));
    setBtnIcon(ui->pushButton_FindZero, QIcon(":/images/mainIco/guiling.ico"));
    setBtnIcon(ui->pushButton_MainZero, QIcon(":/images/mainIco/mainzero.ico"));
    setBtnIcon(ui->pushButton_BackZero, QIcon(":/images/mainIco/zero.ico"));

    //add by yzg 190530 设置字体
    //QFont font ( "Microsoft YaHei", 10, 75);
    QFont font;
    font.setPointSize(12);
    ui->label_MotionMode->setFont(font);
    ui->label_InverterSpeedValue->setFont(font);
    ui->label_tablepos->setFont(font);
    ui->comboBox_MotionMode->setFont(font);
    ui->comboBox_MotionMode->setView(new QListView());
    ui->comboBox_MotionMode->setStyleSheet(UIStyles::css_maincolorconf_u1);
    ui->label_MotionMode->setStyleSheet(UIStyles::css_maincolorconf_u9);
    ui->label_InverterSpeedValue->setStyleSheet(UIStyles::css_maincolorconf_u3);
    ui->label_tablepos->setStyleSheet(UIStyles::css_maincolorconf_u3);
    ui->pushButton_table1->setStyleSheet(UIStyles::css_maincolorconf_u10);
    ui->pushButton_table2->setStyleSheet(UIStyles::css_maincolorconf_u10);
    ui->pushButton_speed1->setStyleSheet(UIStyles::css_maincolorconf_u10);
    ui->pushButton_speed2->setStyleSheet(UIStyles::css_maincolorconf_u10);
    ui->pushButton_Start->setStyleSheet(UIStyles::css_maincolorconf_u2);
    ui->pushButton_Stop->setStyleSheet(UIStyles::css_maincolorconf_u2);
    ui->pushButton_FindZero->setStyleSheet(UIStyles::css_maincolorconf_u2);
    ui->pushButton_MainZero->setStyleSheet(UIStyles::css_maincolorconf_u2);
    ui->pushButton_BackZero->setStyleSheet(UIStyles::css_maincolorconf_u2);
    ui->label_time->setStyleSheet(UIStyles::css_maincolorconf_u14);
    ui->pushButton_drill->init(":/images/press.png",":/images/images/reless.ico",QString(tr("钻头1")),0);
    ui->pushButton_mainAxis->init(":/images/press.png",":/images/images/reless.ico",QString(tr("植刹")),0);
    ui->pushButton_cylinders->init(":/images/press.png",":/images/images/reless.ico",QString(tr("夹具")),0);
    ui->pushButton_ready->init(":/images/press.png",":/images/images/reless.ico",QString(tr("钻刹")),0);
    ui->pushButton_Hair->init(":/images/press.png",":/images/images/reless.ico",QString(tr("挡毛")),0);
    ui->pushButton_cylindersSJX->init(":/images/press.png",":/images/images/reless.ico",QString(tr("顶缸")),0);
    ui->label_CurrentHoleValue->setText(Utils::numberToString(GlobalData::realTimeInfo.iCNum));
    timer->setSingleShot(true);
    timer->setInterval(300); // 双击间隔 未做超时处理
    QTimer *sysytime = new QTimer(this);
    connect(sysytime,SIGNAL(timeout()),this,SLOT(onTimerUpdate()));
    sysytime->start(1000);
    onUpdateRealtimeInfo(); // refresh now!
    onTimerUpdate();
}

SubWindowRunStatus::~SubWindowRunStatus()
{
    Utils::waitForPendingModbus(&modbusPendingCount);
    delete ui;
}

void SubWindowRunStatus::onTimerUpdate()
{
    QLocale locale = QLocale::English;//指定英文显示
    QDateTime time;
    ui->label_time->setText(/*locale.toString(QDateTime::currentDateTime(),QString("ddd"))+" "+*/
                            locale.toString(QDateTime::currentDateTime(),QString("yyyy-MM-dd"))+" "+
                            Utils::dataToString(time.currentDateTime().toString("h").toInt(),2)+":"+
                            Utils::dataToString(time.currentDateTime().toString("m").toInt(),2)+":"+
                            Utils::dataToString(time.currentDateTime().toString("s").toInt(),2));//累计时间
}

void SubWindowRunStatus::onUpdateRealtimeInfo()
{
    GlobalData::backZeroIcon = true;
    // 坐标
    emit updatePostion();
    //显示总孔
    int temp;
    if(GlobalData::realTimeInfo.iMNum>0)
        temp=GlobalData::realTimeInfo.iMNum-1;
    else
        temp=GlobalData::realTimeInfo.iMNum;

    //文件路径
    if(GlobalData::workfileName.isEmpty())
        ui->label_WorkFileNameValue->setText("/");
    else
        ui->label_WorkFileNameValue->setText(GlobalData::workfileName);

    ui->comboBox_MotionMode->setCurrentIndex(static_cast<int>(GlobalData::realTimeInfo.iWorkMode));

    // 当前孔
    if(lastPointNumber != GlobalData::realTimeInfo.iCNum){
        ui->label_CurrentHoleValue->setText(Utils::numberToString(GlobalData::realTimeInfo.iCNum));
        lastPointNumber=GlobalData::realTimeInfo.iCNum;

        // 轨迹预览
        auto i=static_cast<int>(GlobalData::realTimeInfo.iCNum)-1;
        emit setTrailCurrentPoint(i);
    }

    ui->label_InverterSpeedValue->setText(Utils::numberToString(GlobalData::realTimeInfo.iCvertPerct) + QString("%"));
    ui->label_MasterAxisSpeedValue->setText(Utils::numberToString(GlobalData::realTimeInfo.iSpd));
    ui->label_ProducedValue->setText(Utils::numberToString(GlobalData::realTimeInfo.iPdct));//产量
    ui->label_drillHoleValue->setText(Utils::numberToString(GlobalData::realTimeInfo.idrillHoleNum));// 当前钻头加工孔位数add 2022.11.30

    ui->label_TotalHolesValue->setText(Utils::numberToString(temp));

    ui->label_WorkStatusValue->setText(Adapters::getWorkstatusText(static_cast<int>(GlobalData::realTimeInfo.iWSta)));

    ui->label_tablepos->setText(Utils::numberToString(GlobalData::realTimeInfo.iMovesta+1));
    // 钻头状态
    //ui->pushButton_drill->setStyleSheet(GlobalData::realTimeInfo.iDrill ? UIStyles::CSS_IO_HIGHLIGHT : QString());

    auto setWidgetstatus=[&](int status,IcoButton* w){ // 设置某个widget为启用状态（当运行状态不是加工）
        if(status)
            w->setActive(true);
        else
            w->setActive(false);
        w->update();
    };
    setWidgetstatus(GlobalData::realTimeInfo.iDrill,ui->pushButton_drill);//钻孔
    setWidgetstatus(GlobalData::realTimeInfo.iBrake,ui->pushButton_mainAxis);//刹车
    setWidgetstatus(GlobalData::realTimeInfo.iDrill2,ui->pushButton_ready);//刹车2
    setWidgetstatus(GlobalData::realTimeInfo.iHair,ui->pushButton_Hair);//挡毛
    setWidgetstatus(GlobalData::realTimeInfo.iCylinder,ui->pushButton_cylindersSJX);//顶缸

    // 主轴输出
    //ui->pushButton_mainAxis->setStyleSheet(GlobalData::realTimeInfo.iBrake ? UIStyles::CSS_IO_HIGHLIGHT : QString());

    if(lastWorkStatus != GlobalData::realTimeInfo.iWSta)
    {
        //qDebug()<<GlobalData::realTimeInfo.iWSta;
        if(Stop != lastWorkStatus && Stop == GlobalData::realTimeInfo.iWSta)
            emit workstausChangedToStop();

        auto setWidgetEnableWhenStopped=[&](QWidget* w){ // 设置某个widget为启用状态（当运行状态为停止）
            bool isStopped=Utils::checkWorkstatusIfStop();
            w->setDisabled(!isStopped);
        };

        auto setWidgetEnableWhenNotRunning=[&](QWidget* w){ // 设置某个widget为启用状态（当运行状态不是加工）
            bool isRunning=(Run==GlobalData::realTimeInfo.iWSta);
            w->setDisabled(isRunning);
        };

        //运行关闭
        setWidgetEnableWhenStopped(ui->comboBox_MotionMode);
        setWidgetEnableWhenNotRunning(ui->pushButton_FindZero);
        setWidgetEnableWhenNotRunning(ui->pushButton_BackZero);
        setWidgetEnableWhenNotRunning(ui->pushButton_table1);
        setWidgetEnableWhenNotRunning(ui->pushButton_table2);
        setWidgetEnableWhenNotRunning(ui->comboBox_MotionMode);
        ui->comboBox_MotionMode->update();//解决字体不在中间BUG modify by cj 2019/9/26

        lastWorkStatus = GlobalData::realTimeInfo.iWSta;
    }
}

void SubWindowRunStatus::onButtonClickedStart()
{
    GlobalData::backZeroFlag = false;
    sendWorkCommand(START_KEY);
}

void SubWindowRunStatus::onButtonClicked_FindZero()
{//选择主轴归零
    auto d=new QDialogFindZero(this,Qt::FramelessWindowHint);
    d->setAttribute(Qt::WA_DeleteOnClose);
    connect(d, SIGNAL(startTable()),
            this, SLOT(onButtonClicked_GoToStart()));
    connect(d, SIGNAL(startTable1()),
            this, SLOT(onButtonClicked_GoToStart1()));
    d->open();
}

void SubWindowRunStatus::onButtonClicked_GoToStart()
{
    TEACH_SEND* teachSend = new TEACH_SEND;
    memset(teachSend,0,sizeof(TEACH_SEND));
    teachSend->Type = START_TABLE;
    sendTEACHSEND(teachSend);
}

void SubWindowRunStatus::onButtonClicked_GoToStart1()
{
    TEACH_SEND* teachSend = new TEACH_SEND;
    memset(teachSend,0,sizeof(TEACH_SEND));
    teachSend->Type = START_TABLE2;
    sendTEACHSEND(teachSend);
}

void SubWindowRunStatus::onButtonClicked_Stop()
{
    sendWorkCommand(STOP_KEY);
}

//产量清零对话框
void SubWindowRunStatus::onButtonClicked_Produced()
{
    if(timer->isActive())//是否已经开始计时
    {//是则stop
        // 双击
        timer->stop();
        if(false == Utils::checkWorkstatusIfStop())
            return;

        //    if(QMessageBox::Yes != QMessageBox::question(this, "?", QString::fromUtf8("产量清零吗？"),
        //            QMessageBox::Yes | QMessageBox::No))
        if(QMessageBox::Yes != QMessageBox::question(this, "?", tr("产量清零吗？"),
                                                     QMessageBox::Yes | QMessageBox::No))
            return;

        auto queryItem = new Definitions::ModbusQueryItem;
        queryItem->mixData.u16 = 0;
        queryItem->queryAction = Action_ClearProduced;
        emit sendModbusQuery(queryItem);

    }else{//否start
        timer->start();
    }
}

//当前钻头加工孔位数清零  add 2022.11.30
void SubWindowRunStatus::onButtonClicked_DrillHole()
{
    if(timer->isActive())//是否已经开始计时
    {//是则stop
        // 双击

        timer->stop();
        if(false == Utils::checkWorkstatusIfStop())
            return;

        //    if(QMessageBox::Yes != QMessageBox::question(this, "?", QString::fromUtf8("产量清零吗？"),
        //            QMessageBox::Yes | QMessageBox::No))

//        static int temp;
//        qDebug()<<"GlobalData::realTimeInfo.idrillHoleNum="<<GlobalData::realTimeInfo.idrillHoleNum;
//        GlobalData::realTimeInfo.idrillHoleNum=temp;
//        temp++;
        qDebug()<<"GlobalData::realTimeInfo.idrillHoleNum1="<<GlobalData::realTimeInfo.idrillHoleNum;
        if(QMessageBox::Yes != QMessageBox::question(this, "?", tr("当前钻头加工孔位数清零吗？"),
                                                     QMessageBox::Yes | QMessageBox::No))
            return;



//        auto queryItem = new Definitions::ModbusQueryItem;
//        queryItem->mixData.u16 = 0;
//        queryItem->queryAction = Action_ClearProduced;
//        emit sendModbusQuery(queryItem);

    }else{//否start
        timer->start();
    }
}

void SubWindowRunStatus::onComboIndexActivated(int i)
{//需要加?运行状态 试机无效
    auto queryItem = new Definitions::ModbusQueryItem;
    queryItem->mixData.u16 = static_cast<unsigned short>(i);
    queryItem->queryAction = Action_WriteWorkMode;
    emit sendModbusQuery(queryItem);
}

void SubWindowRunStatus::_onClickedBottomButton(int btn_id)
{
    if(Run == GlobalData::realTimeInfo.iWSta)
    {
        return;
    }
    switch(btn_id)
    {
    case Definitions::BTN_SYS_BUKONG:
    {

        if((Run != GlobalData::realTimeInfo.iWSta))
        {
            auto teachSend = new TEACH_SEND;
            teachSend->Type = PATCH_HOLE;
            sendTEACHSEND(teachSend);
        }
        break;
    }

    case Definitions::BTN_SYS_LOCATE: // 定位
    {
        QProgressDialog* d = nullptr;

        if(true == Utils::checkWorkstatusIfStop(true, &d, this))
        {
            locatePointIndex = GlobalData::realTimeInfo.iCNum;
            QList<QDialogDataForm::item_t> items =
            {
                //{QString::fromUtf8("定位"), Adapters::TYPE_S32, &locatePointIndex, false, 0, GlobalData::realTimeInfo.iMNum - 1.0, nullptr},
                {tr("定位"), Adapters::TYPE_S32, &locatePointIndex, false, 0, GlobalData::realTimeInfo.iMNum - 1.0, nullptr},
            };

            auto w = new QDialogDataForm(this, Qt::FramelessWindowHint);
            connect(w, SIGNAL(accepted()), this, SLOT(onDialogAcceptedLocate()));
            //w->showDialog(QString::fromUtf8("定位到某一个孔"), items);
            w->showDialog(tr("定位到某一个孔"), items);
        }
        else
        {
            connect(this, SIGNAL(workstausChangedToStop()),
                    d, SLOT(accept()));
            d->open();
        }

        break;
    }

    case Definitions::BTN_SYS_PREVHOLE: // 上孔
    case Definitions::BTN_SYS_NEXTHOLE: // 下孔
    {
        QProgressDialog* d = nullptr;

        if(true == Utils::checkWorkstatusIfStop(true, &d, this))
        {
            auto teachSend = new TEACH_SEND;
            teachSend->Type = (Definitions::BTN_SYS_PREVHOLE == btn_id ? LAST_HOLE : NEXT_HOLE);
            sendTEACHSEND(teachSend);
        }
        else
        {
            connect(this, SIGNAL(workstausChangedToStop()),
                    d, SLOT(accept()));
            d->open();
        }

        break;
    }

    default:
        break;
    }
}

void SubWindowRunStatus::onDialogAcceptedLocate()
{//定位框退出
    auto teachSend = new TEACH_SEND;
    teachSend->Type = FIXED_HOLE;
    teachSend->Num = locatePointIndex;
    sendTEACHSEND(teachSend);//发送教导数据
}

void SubWindowRunStatus::onButtonClicked_Speed1()
{
    qint16 speed = GlobalData::realTimeInfo.iCvertPerct;
    if(speed <= 0)
    {
        sendVFD(0);
    }
    else
        sendVFD(speed - 1);
}

void SubWindowRunStatus::onButtonClicked_Speed2()
{
    qint16 speed = GlobalData::realTimeInfo.iCvertPerct;
    if(speed >= 200)
    {
        sendVFD(200);
    }
    else
        sendVFD(speed + 1);
}

void SubWindowRunStatus::onButtonClicked_SpeedValue()
{
    vfdSpeed=GlobalData::realTimeInfo.iCvertPerct;

    QList<QDialogDataForm::item_t> items =
    {
        //{QString::fromUtf8("速比"), Adapters::TYPE_U8, &vfdSpeed, false, 0, 100, nullptr},
        {tr("速比"), Adapters::TYPE_U8, &vfdSpeed, false, 0, 100, nullptr},
    };

    auto w = new QDialogDataForm(this, Qt::FramelessWindowHint);
    connect(w, SIGNAL(accepted()), this, SLOT(onDialogAcceptedInputVFD()));
    //w->showDialog(QString::fromUtf8("设置变频器速比"), items);
    w->showDialog(tr("设置变频器速比"), items);
}

void SubWindowRunStatus::onButtonClicked_Table1()
{
    if(false == Utils::checkWorkstatusIfStop())
        return;

    if(0==GlobalData::realTimeInfo.iMNum)
        return;

    auto teachSend = new TEACH_SEND;
    teachSend->Type = LAST_TABLE;
    sendTEACHSEND(teachSend);
}

void SubWindowRunStatus::onButtonClicked_Table2()
{
    if(false == Utils::checkWorkstatusIfStop())
        return;

    if(0==GlobalData::realTimeInfo.iMNum)
        return;

    auto teachSend = new TEACH_SEND;
    teachSend->Type = NEXT_TABLE;
    sendTEACHSEND(teachSend);
}

void SubWindowRunStatus::onButtonClicked_Drill()
{
    if(false == Utils::checkWorkstatusIfStop())
        return;

    sendButtonCommand(IO_DRILL);
}

void SubWindowRunStatus::onButtonClicked_Drill2()
{
    if(false == Utils::checkWorkstatusIfStop())
        return;

    sendButtonCommand(IO_DRILL2);
}

void SubWindowRunStatus::onButtonClicked_MainAxisBrake()
{
    if(false == Utils::checkWorkstatusIfStop())
        return;

    sendButtonCommand(IO_MAINBRAKE);
}

void SubWindowRunStatus::onButtonClicked_Hair()
{
    if(false == Utils::checkWorkstatusIfStop())
        return;

    sendButtonCommand(IO_Hair);
}

void SubWindowRunStatus::onButtonClicked_Cylinder()
{//SJX气缸特殊动作
    if(false == Utils::checkWorkstatusIfStop())
        return;

    sendButtonCommand(IO_Cylinder);
}

void SubWindowRunStatus::onButtonClicked_ClampOut()
{
#define CYLINDER_CLAMP_COUNT 6
    //auto d = new QDialogClampOut(QString::fromUtf8("夹具"), CYLINDER_CLAMP_COUNT, this, Qt::FramelessWindowHint);
    auto d = new QDialogClampOut(CYLINDER_CLAMP_COUNT, true,this,Qt::FramelessWindowHint);\
    d->setAttribute(Qt::WA_DeleteOnClose);
    dialogStart.setX(d->pos().x());
    dialogStart.setY(d->pos().y()-d->height());
    dialogEnd.setX(d->pos().x()+d->width());
    dialogEnd.setY(d->pos().y());
    connect(d, SIGNAL(writeCylinder(int, int)),
            this, SLOT(onWriteClamp(int, int)));
    connect(this, SIGNAL(closedialog()), d, SLOT(accept()));
    d->exec();
}


void SubWindowRunStatus::onWriteClamp(int cylinderIndex, int /*status*/)
{
    debug_printf("cylinder=%d",cylinderIndex);
    auto cmd=static_cast<IO_Op_Type>(IO_CLAMP1+cylinderIndex);
    sendButtonCommand(cmd);
}

void SubWindowRunStatus::onDialogAcceptedInputVFD()
{
    sendVFD(vfdSpeed);
}

void SubWindowRunStatus::onFindZero_Machine()
{
    GlobalData::backZeroFlag = true;
    TEACH_SEND* teachSend = new TEACH_SEND;
    memset(teachSend,0,sizeof(TEACH_SEND));
    teachSend->Type = ZERO_TABLE;
    sendTEACHSEND(teachSend);
}

void SubWindowRunStatus::onFindZero_MainAxis()
{
    GlobalData::backZeroFlag = true;
    TEACH_SEND* teachSend = new TEACH_SEND;
    memset(teachSend,0,sizeof(TEACH_SEND));
    teachSend->Type = ZERO_MAINAXIS;
    sendTEACHSEND(teachSend);
}

void SubWindowRunStatus::switchCentralWidget()
{//初始化和双击调用
    QWidget* newCentralWidget=nullptr;

    if(nullptr != ui->centralWidget){
        ui->centralWidget->hide();
        ui->verticalLayout->removeWidget(ui->centralWidget);
        ui->centralWidget->deleteLater();
    }

    switch (GlobalData::doubleClickNextWidget%2) {
    case 0:
    {
        GlobalData::backZeroIcon = true;
        auto w=new QWidgetAxisPosition(this,true);

        w->initScreen(GlobalData::totalAxisNum,GlobalData::realaxisName,Qt::AlignHCenter);
        w->setLabelCSS(UIStyles::CSS_RUNSTATUS_POS_LABEL);
        w->setStyleSheet(UIStyles::CSS_RUNSTATUS_BACKGROUND);
        connect(this, SIGNAL(updatePostion()),
                w, SLOT(updatePostion()));
        connect(w, SIGNAL(doubleClicked()),
                this, SLOT(onDoubleClickCentralWidget()));
        newCentralWidget=w;

        break;
    }
    case 1:
    {
        auto w=new QWidgetDrawTrail(this);
        w->setStyleSheet(UIStyles::CSS_RUNSTATUS_BACKGROUND);
        w->init(positions, pointColors,4.0);
        w->setCoordText(GlobalData::realaxisName.at(GlobalData::selectedAxisNum[0]), GlobalData::realaxisName.at(GlobalData::selectedAxisNum[1]));
        w->setCoordDirectoin(GlobalData::selectedAxisDirection[0] > 0, GlobalData::selectedAxisDirection[1] > 0);
        w->setTotalPoint(0);

        connect(w, SIGNAL(doubleClicked()),
                this, SLOT(onDoubleClickCentralWidget()));
        connect(this, SIGNAL(resetTrailToFit()),
                w, SLOT(resetToFit()));
        connect(this, SIGNAL(setTrailCurrentPoint(int)),
                w, SLOT(setHighlightIndex(int)));
        connect(this, SIGNAL(setTrailTotalPoint(int)),
                w, SLOT(setTotalPoint(int)));

        // load work file
        if(false == Workfile::getInstance()->isAlreadyReadAll())
        {
            auto d = new QDialogLoadingWorkfile(this);
            d->setAttribute(Qt::WA_DeleteOnClose);
            connect(d, SIGNAL(accepted()),
                    this, SLOT(pickUpPositionData()));
            d->readWrite(true, -1, 0); // read all
        }else{
            pickUpPositionData();
        }

        newCentralWidget=w;
        break;
    }
    default:
        break;
    }
    ui->centralWidget=newCentralWidget;
    if(ui->centralWidget){
        ui->verticalLayout->insertWidget(0,ui->centralWidget);
    }
}

void SubWindowRunStatus::onDoubleClickCentralWidget()
{
    ++GlobalData::doubleClickNextWidget;
    switchCentralWidget();
}

void SubWindowRunStatus::pickUpPositionData()
{
    auto teachData = Workfile::getInstance()->getTeachData() + 1; // 跳过第一个点（起点）

    for(quint32 i = 0; i < GlobalData::realTimeInfo.iMNum; ++i)
    {
        positions[i][0] = (teachData + i)->fMove_pos[GlobalData::selectedAxisNum[0]];
        positions[i][1] = (teachData + i)->fMove_pos[GlobalData::selectedAxisNum[1]];
        pointColors[i] = static_cast<int>((teachData + i)->icolor);
    }

    // 使用定时器 等待一定时候再获取QRect才能准确知道窗口大小 lixingcong@20190226
    QTimer::singleShot(20,this,SLOT(onTimeoutSetTrailToFit()));//给定时间间隔后调用一个槽
}

void SubWindowRunStatus::onTimeoutSetTrailToFit()
{
    auto i=static_cast<int>(GlobalData::realTimeInfo.iMNum - 1);
    emit setTrailTotalPoint(i);
    emit resetTrailToFit();
}

void SubWindowRunStatus::onSendModbusQuery(Definitions::ModbusQueryItem* queryItem)
{
    bool isQueryValid = true;

    auto element = new ModbusQueue::element_t;

    switch(queryItem->queryAction)
    {
    case Action_WriteWorkCommand:
        element->data = &queryItem->mixData.u16;
        element->len = sizeof(queryItem->mixData.u16);
        element->operation = ModbusQueue::OP_WRITE_DATA;
        element->startAddress = ModbusAddress::WorkCommand;
        break;

    case Action_WriteWorkMode://运动模式改变
        element->data = &queryItem->mixData.u16;
        element->len = sizeof(queryItem->mixData.u16);
        element->operation = ModbusQueue::OP_WRITE_DATA;
        element->startAddress = ModbusAddress::WorkMode;
        break;

    case Action_TeachSend:
        element->data = queryItem->mixData.otherData;
        element->len = sizeof(TEACH_SEND);
        element->operation = ModbusQueue::OP_WRITE_DATA;
        element->startAddress = ModbusAddress::TeachWorkfileHead;
        break;

    case Action_ClearProduced:
        element->data = &queryItem->mixData.u16;
        element->len = sizeof(queryItem->mixData.u16);
        element->operation = ModbusQueue::OP_WRITE_DATA;
        element->startAddress = ModbusAddress::ProducedCount;
        break;

    case Action_SetVFD:
        element->data = &queryItem->mixData.u16;
        element->len = sizeof(queryItem->mixData.u16);
        element->operation = ModbusQueue::OP_WRITE_DATA;
        element->startAddress = ModbusAddress::VFD;
        break;

    case Action_WriteButtonCommand:
        element->data = &queryItem->mixData.u16;
        element->len = sizeof(queryItem->mixData.u16);
        element->operation = ModbusQueue::OP_WRITE_DATA;
        element->startAddress = ModbusAddress::ButtonOperate;
        break;

    case Action_WriteClamp:
    {
        auto cylinderIndex = queryItem->mixData.u32 & 0xffff;
        auto realIO = Adapters::getRealIOFromFunctionCode(OUT0_CLAMP1 + cylinderIndex, false);

        queryItem->mixData.u16 = static_cast<unsigned short>(queryItem->mixData.u32 >> 16);

        element->data = &queryItem->mixData.u16;
        element->len = sizeof(queryItem->mixData.u16);
        element->operation = ModbusQueue::OP_WRITE_DATA;
        element->startAddress = ModbusAddress::IOStatus_Out + realIO;
        break;
    }

    default:
        isQueryValid = false;
        break;
    }
    if(true == isQueryValid)
    {
        element->pointerObject = queryItem;
        element->callback = std::tr1::bind(&SubWindowRunStatus::_modbusQueryCallback, this, std::tr1::placeholders::_1,
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

void SubWindowRunStatus::onModbusQueryCallbackEnd(bool isOk, void* flag)
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
            //连接中断的后续操作
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
            auto teachSend = reinterpret_cast<TEACH_SEND*>(queryItem->mixData.otherData);
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

void SubWindowRunStatus::sendWorkCommand(KEYVALUE keyValue)
{
    auto queryItem = new Definitions::ModbusQueryItem;
    queryItem->queryAction = Action_WriteWorkCommand;
    queryItem->mixData.u16 = static_cast<unsigned short>(keyValue);

    emit sendModbusQuery(queryItem);
}

void SubWindowRunStatus::sendTEACHSEND(TEACH_SEND* teachSend)
{

    auto queryItem = new Definitions::ModbusQueryItem;
    memset(queryItem,0,sizeof(Definitions::ModbusQueryItem));
    queryItem->queryAction = Action_TeachSend;
    queryItem->mixData.otherData = teachSend;
    emit sendModbusQuery(queryItem);
}

void SubWindowRunStatus::sendVFD(quint8 value)
{
    auto queryItem = new Definitions::ModbusQueryItem;
    queryItem->queryAction = Action_SetVFD;
    queryItem->mixData.u16 = value;
    emit sendModbusQuery(queryItem);
}

void SubWindowRunStatus::sendButtonCommand(IO_Op_Type type)
{
    auto queryItem = new Definitions::ModbusQueryItem;
    queryItem->queryAction = Action_WriteButtonCommand;
    queryItem->mixData.u16 = type;
    emit sendModbusQuery(queryItem);
}

void SubWindowRunStatus::mousePressEvent(QMouseEvent *event)
{
    //qDebug()<<event->pos();
    if(event->pos().x()<=dialogStart.x()||event->pos().x()>=dialogStart.x()||
            event->pos().y()<=dialogEnd.y()||event->pos().y()>=dialogEnd.y())
    {
        emit closedialog();
//        qDebug()<<"closedialog";
    }
    QWidget::mousePressEvent(event);
}
void SubWindowRunStatus::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
}
#endif
