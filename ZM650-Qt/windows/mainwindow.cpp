/*
 * mainwindow.cpp
 *
 * 主界面
 *
 * Created on: 2019年 01月 21日 星期一 15:00:15 CST
 * Author: lixingcong
 */
#include "mainwindow.h"
#include "data/utils.h"
#include "widgets/qdialogmanage.h"
#include "platform/common.h"
#include "data/globaldata.h"
#include "data/adapters.h"
#include "ui_mainwindow.h"
#include "windows/subwindowparams.h"
#include "windows/subwindowdiagnose.h"
#include "windows/subwindowtrail.h"
#include "windows/subwindowpatterns.h"
#include "windows/subwindowfilemanage.h"
#include "modbus/modbusaddress.h"
#include <tr1/functional>
#include <ManualMotion.h>
#include "widgets/qpushbuttonlongpress.h"
#include "widgets/qdialogloadingworkfile.h"
#include "data/uistyles.h"
#include <windows/subwindowlocalconfig.h>
#include <widgets/tabbutton.h>

#ifdef Arm_460Y
#include "widgets/qdialogmanage.h"
#include "windows/subwindowrunstatus_460y.h"
#include "windows/subwindowteachtable.h"
#endif

#ifdef Arm_Kaiteli
#include "windows/subwindowrunstatus_kaiteli.h"
#include "windows/subwindowteachtable.h"
#endif
#ifdef Arm_662
#include "widgets/qdialogmanage.h"
#include "windows/subwindowrunstatus_zm662.h"
#include "windows/subwindowteachtable_zm662.h"
#endif
#ifdef  ARM
#include "windows/subwindowrunstatus.h"
#include "windows/subwindowteachtable.h"
#endif

Q_DECLARE_METATYPE(MainWindow::ModbusQueryItemType)


MainWindow::MainWindow(QWidget* parent) : QWidget(parent),inparam(false),
    ui(new Ui::MainWindow),
    screenTimer(new QTimer(this)),
    labelCurrentTabHint(new QLabel(this)),
    lastSubWindow(nullptr),
    workerThread(new QThreadMainWindow(this)),
    heartBeatCounter(0),
    modbusPendingCount(0)
{

    TAB_BUTTON_NAMES =
    {
        tr("系统"),
        tr("教导"),
        tr("参数"),
        tr("文件"),
        tr("诊断"),
        //   QObject::trUtf8("急救"),//测试程序
    };
    ui->setupUi(this);
    Utils::setFullScreen(this);//设置屏幕大小
    //设置界面显示
    GlobalData::pushButtonPage = ui->buttons;
    GlobalData::labelTopTab = ui->tabText;
    if(GlobalData::screensize == SEVEN)
    {
        ui->widget->setFixedHeight(MENUWIDTHSEV);//优化界面栏显示大小
        ui->tabText->setFixedHeight(MENUWIDTHSEV);
        ui->buttons->setFixedHeight(MENUWIDTHSEV);
        ui->tabText->setFixedWidth(GlobalData::widthRate*1/3+30);
    }
    else
    {
        ui->widget->setFixedHeight(MENUWIDTHTEN);//优化界面栏显示大小
        ui->tabText->setFixedHeight(MENUWIDTHTEN);
        ui->tabText->setFixedWidth(GlobalData::widthRate*1/3+30*TENTOSEV);
        ui->buttons->setFixedHeight(MENUWIDTHTEN+5);
    }
    //防止栏目因为布局变动
    ui->buttons->setFixedHeight(ui->buttons->height());
    ui->alarmMessage->setFixedHeight(ui->alarmMessage->height());
    //    ui->tabText->setFixedWidth(GlobalData::widthRate/4+90);
    ui->tabText->setStyleSheet(UIStyles::css_maincolorconf_u7);

    // Create the tabs
    ui->tabs->init(true);
    connect(ui->tabs, SIGNAL(indexchange(int)),
            this, SLOT(onTabIndexChanged(int)));
    ui->tabText->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->buttons->initLayout(5);

    Workfile::getInstance()->readAll();
    //显示系统时间
    //新建定时器
    QTimer *timer = new QTimer(this);
    //关联定时器计满信号和相应的槽函数
    connect(timer,SIGNAL(timeout()),this,SLOT(timerUpDate()));
    connect(screenTimer,SIGNAL(timeout()),this,SLOT(timerScreen()));
    //定时器开始计时，其中1000表示1000ms即1秒
    timer->start(1000);


    // load MainFrame
    //    auto subWindow=new SubWindowRunStatus(this);
    //    ui->layout_SubWindow->addWidget(subWindow);

    // systemAlarm
    QList<QLabelSystemAlarm::BlinkStyleType> alarmMsgStyle =
    {// 设置 普通 和闪烁颜色
     {false, UIStyles::CSS_ALARMMSG_LEVEL_3, UIStyles::CSS_ALARMMSG_LEVEL_3},//正常
     {true,  UIStyles::CSS_ALARMMSG_LEVEL_2_1, UIStyles::CSS_ALARMMSG_LEVEL_2_2},//黄色
     {true,  UIStyles::CSS_ALARMMSG_LEVEL_3_1, UIStyles::CSS_ALARMMSG_LEVEL_3_2},//红色
    };
    ui->alarmMessage->init(alarmMsgStyle);
    ui->alarmMessage->setContentsMargins(10,0,0,0);
    connect(ui->alarmMessage, SIGNAL(clicked()),
            this, SLOT(onButtonClicked_ResetAlarm()));

    // modbus callback
    connect(this, SIGNAL(_signal_modbusQueryCallback(bool, void*)),
            this, SLOT(onModbusQueryCallbackEnd(bool, void*)), Qt::QueuedConnection);
    connect(this, SIGNAL(sendModbusQuery(MainWindow::ModbusQueryItemType*, int)),
            this, SLOT(onSendModbusQuery(MainWindow::ModbusQueryItemType*, int)));

    // worker thread
    connect(workerThread, SIGNAL(sendModbusQuery(MainWindow::ModbusQueryItemType*, int)),
            this, SLOT(onSendModbusQuery(MainWindow::ModbusQueryItemType*, int)), Qt::QueuedConnection);
    connect(this, SIGNAL(_signal_notifyWorkerQueryDone(int)),
            workerThread, SLOT(queryDone(int)), Qt::QueuedConnection);

    // update real time
    connect(this, SIGNAL(_updateRealtimeInfo()),
            this, SLOT(onUpdateRealtimeInfo()));

    // bottom buttons
    connect(GlobalData::pushButtonPage, SIGNAL(buttonClicked(int)),
            this, SLOT(_onClickedBottomButton(int)));

    // global signals
    connect(this, SIGNAL(_updateRealtimeInfo()),
            GlobalData::getInstance(), SLOT(_updateRealtimeInfo()));
    InitPattern();
    //获取操作员权限
    //Utils::sendOffest(&GlobalData::paramauthority,ModbusAddress::Authority,sizeof(GlobalData::paramauthority));
}

void MainWindow::timerUpDate()
{

#ifdef _MCGS
    QDateTime time = platform_sys_info_c::get_rtc_time();
#else
    QDateTime time = QDateTime::currentDateTime();
#endif
    //获取系统现在的时间
    //QString str = time.toString("yyyy-MM-dd hh:mm:ss dddd");
    //QString str = time.toString("yyyy-MM-dd hh:mm:ss");
    QString str = time.toString("hh:mm:ss");
    //设置系统时间显示格式
    //ui->label_datetime_main->setText(str);
    //在标签上显示时间
}

void MainWindow::timerScreen()
{//截屏溢出时间
    screenTimer->stop();
    if(GlobalData::ScreenCapture)
    {
        if(Utils::slotGrabFullScreen())
        {
            qDebug()<<"screen success";
#ifdef _MCGS
            int nRet;
            for(int i=0;i<3;i++)
            {
                nRet = hal_set_bee_time(30);
                if(0 != nRet)
                {
                    qDebug( "beep failed: %d!", nRet);
                }
                Utils::sleep1(100);
            }
#else
            Utils::MyMessageBox(QString("Info"), tr("截屏成功"),tr("确定"),this);
#endif
        }
        else
        {
#ifdef _MCGS
            hal_set_bee_time(30);
#endif
        }
    }
}

MainWindow::~MainWindow()
{
    workerThread->setQuitFlag(true);
    Utils::waitForThreadFinished(workerThread);
    Utils::waitForPendingModbus(&modbusPendingCount);

    auto workfile = Workfile::getInstance();
    Utils::delete_pointer(workfile);

    delete ui;
}

void MainWindow::InitPattern()
{
    //初始化花样数组
    for(int i=0;i<MAX_PATTERN_ROW;i++)
    {
        for(int j=0;j<MAX_PATTERN_COL;j++)
        {
            GlobalData::patternarry[i][j] = QString("0");
        }
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{

    if(event->type()==QEvent::MouseButtonPress)
    {
        return GlobalData::DISABLETOUCH;
        qDebug("MainWindow::eventFilter mousePressEvent");
        if(ATPARA.TouchBeep)
        {
#ifdef _MCGS
            int nRet = hal_set_bee_time(30);
            if(0 != nRet)
            {
                qDebug( "beep failed: %d!", nRet);
            }
#endif
        }
    }
    if(event->type()==QEvent::MouseButtonRelease)
    {
        return GlobalData::DISABLETOUCH;
    }
    if(event->type()==QEvent::MouseButtonDblClick)
    {
        return GlobalData::DISABLETOUCH;
    }
    return QWidget::eventFilter(obj, event);
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    //qDebug()<<"press";
    screenTimer->start(1000);
    if(e->type()==QEvent::MouseButtonPress)
    {
        //qDebug()<<"MainWindow QEvent::MouseButtonPress";
    }
    if(GlobalData::realTimeInfo.iAlarmSta!=NORMAL)
    {
        onButtonClicked_ResetAlarm();
    }

    QWidget::mousePressEvent(e);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    //qDebug()<<"reless";
    screenTimer->stop();
    QWidget::mouseReleaseEvent(e);
}

void MainWindow::onTabIndexChanged(int tabIndex)
{
    QWidget* subWindow = nullptr;
    GlobalData::infile=false;
    if(tabIndex!=2)
    {
        inparam = false;
    }

    switch(tabIndex)
    {
    case 0:
    {
        auto window = new SubWindowRunStatus(this);

        subWindow = window;
#ifdef Arm_460Y
        connect(window, SIGNAL(rmoveAxis(int, int, int, double)),
                this, SLOT(onMoveAxis(int, int, int, double)));

        connect(window, SIGNAL(openOffset(int)),
                this, SLOT(writeInterface(int)));
#endif

#ifdef _MCGS
        QString temppath=PICTUR_PATH;//PICTUR_PATH      linux  ":/images/images/logo.bmp"
        //QString temppath=":/images/images/logo.bmp";
        QImage image,result;
        image.load(temppath);//temppath为图片的路径
        image.rgbSwapped();
        result = image.scaled(ui->tabText->width()-2, ui->tabText->height()-2,Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//放缩图片，以固定大小显示
        ui->tabText->setPixmap(QPixmap::fromImage(result));//在Label控件上显示图片
#else
        ui->tabText->setText(QObject::trUtf8("系统界面"));
#endif
        //告诉下位机当前界面
        writeInterface(SHOW_SYSTEM);

        break;
    }

    case 1:
    {
        auto window = new SubWindowTeachTable(this);
        connect(window, SIGNAL(moveAxis(int, int, int, double)),
                this, SLOT(onMoveAxis(int, int, int, double)));
        connect(this, SIGNAL(pointIndexChanged(int)),
                window, SLOT(onPointIndexChanged(int)));
        connect(this, SIGNAL(totalPointChanged(int)),
                window, SLOT(onTotalPointChanged(int)));
        subWindow = window;
        //ui->tabText->setText(TAB_BUTTON_NAMES.at(tabIndex));
        ui->tabText->setText(QObject::tr(TAB_BUTTON_NAMES.at(tabIndex).toUtf8()));
        writeInterface(SHOW_TEACH);
        break;
    }

    case 2:
    {
#if (defined Arm_460Y) || (defined Arm_662)||(defined Arm_lansi)
        //在MCGS上设置参数管理密码
#ifdef _MCGS
        if(!inparam)
        {//切换界面进来时候需要输入密码
            QDialogManage* offDlg = new QDialogManage(this);
            connect(offDlg, SIGNAL(decrypt()), this, SLOT(_onOpenParnm()));
            offDlg->setAttribute(Qt::WA_DeleteOnClose);
            offDlg->open();
        }
        else
        {
            auto window = new SubWindowParams(this);
            subWindow = window;
            connect(window, SIGNAL(openOffset(int)),
                    this, SLOT(writeInterface(int)));
            connect(window, SIGNAL(moveAxis(int, int, int, double)),
                    this, SLOT(onMoveAxis(int, int, int, double)));
            //ui->tabText->setText(TAB_BUTTON_NAMES.at(tabIndex));
            ui->tabText->setText(QObject::tr(TAB_BUTTON_NAMES.at(tabIndex).toUtf8()));
            writeInterface(SHOW_PARA);
        }

#else
        auto window = new SubWindowParams(this);
        subWindow = window;
        connect(window, SIGNAL(moveAxis(int, int, int, double)),
                this, SLOT(onMoveAxis(int, int, int, double)));
        connect(window, SIGNAL(openOffset(int)),
                this, SLOT(writeInterface(int)));
        //ui->tabText->setText(TAB_BUTTON_NAMES.at(tabIndex));
        ui->tabText->setText(QObject::tr(TAB_BUTTON_NAMES.at(tabIndex).toUtf8()));
        writeInterface(SHOW_PARA);
#endif
#else
        auto window = new SubWindowParams(this);
        subWindow = window;
        //ui->tabText->setText(TAB_BUTTON_NAMES.at(tabIndex));
        ui->tabText->setText(QObject::tr(TAB_BUTTON_NAMES.at(tabIndex).toUtf8()));
        writeInterface(SHOW_PARA);
#endif
        break;
    }

    case 3:
    {
        GlobalData::infile=true;
        auto window = new SubWindowFileManage(this);
        subWindow = window;
        //ui->tabText->setText(TAB_BUTTON_NAMES.at(tabIndex));
        ui->tabText->setText(QObject::tr(TAB_BUTTON_NAMES.at(tabIndex).toUtf8()));
        writeInterface(SHOW_FILE);
        break;
    }

    case 4:
    {
        auto window = new SubWindowDiagnose(this);
        subWindow = window;
        //ui->tabText->setText(TAB_BUTTON_NAMES.at(tabIndex));
        ui->tabText->setText(QObject::tr(TAB_BUTTON_NAMES.at(tabIndex).toUtf8()));
        writeInterface(SHOW_DIAG);
        break;
    }

    default:
        break;
    }

    if(nullptr != subWindow)
        setSubWindow(subWindow);
}

void MainWindow::onMoveAxis(int axisNum, int direction, int pressType, double distance)
{
    debug_printf("moveAxis, axis=%d, dir=%d, type=%d, distance=%f",axisNum,direction,pressType,distance);
    auto manualMove = new MANUALCOMM;
    manualMove->type = 0;
    manualMove->type |= axisNum << 8;
    unsigned short moveType = 0;
    if(QPushButtonLongPress::PressShort == pressType)  // 短按
    {
        moveType = (SubWindowTeachTable::MovePlus == direction ? Manual_P_Step : Manual_M_Step);
    }
    else if(QPushButtonLongPress::PressLongBegin == pressType)   // 长按开始
    {
        moveType = (SubWindowTeachTable::MovePlus == direction ? Manual_P_Continue : Manual_M_Continue);
    }
    else   // 长按结束
    {
        moveType = Manual_Stop;
    }

    manualMove->type |= moveType;
    manualMove->pos = static_cast<float>(distance);

    auto queryItem = new ModbusQueryItemType;
    queryItem->queryAction = Action_WriteManualMove;
    queryItem->mixData.otherData = manualMove;
    emit sendModbusQuery(queryItem);
}

void MainWindow::onNotifyOffline()
{
    ui->alarmMessage->setBlinkStyle(2);
    ui->alarmMessage->setText(tr("通信中断，请重启软件"));
}

void MainWindow::setSubWindow(QWidget* subWindow)
{
    if(nullptr != lastSubWindow)
    {
        ui->layout_SubWindow->removeWidget(lastSubWindow);
        lastSubWindow->close();
        lastSubWindow->deleteLater();
    }

    ui->layout_SubWindow->addWidget(subWindow);
    lastSubWindow = subWindow;
}

void MainWindow::writeInterface(int i)
{//发送当前界面
    auto queryItem = new ModbusQueryItemType;
    queryItem->queryAction = Action_WriteInterface;
    queryItem->mixData.u16 = static_cast<quint16>(i);
    emit sendModbusQuery(queryItem);
}

void MainWindow::readWorkfileName()
{
    auto queryItem = new ModbusQueryItemType;
    queryItem->queryAction = Action_ReadWorkfileName;
    queryItem->mixData.otherData = new char[24];
    emit sendModbusQuery(queryItem);
}

void MainWindow::onUpdateRealtimeInfo()
{
    static quint32 lastPointIndex = 0xffff;
    static quint32 lastTotalPoint = 0xffff;
#ifndef Debug
    if(GlobalData::realTimeInfo.iAlarmSta!=NORMAL)
    {//系统故障
        //qDebug()<<GlobalData::realTimeInfo.iAlarmSta;
        if(GlobalData::lastAlarmNumber != GlobalData::realTimeInfo.iAlarmSta)
        {
            //qDebug("alarm=%d",GlobalData::realTimeInfo.iAlarmSta);
            auto i = static_cast<int>(GlobalData::realTimeInfo.iAlarmSta);
            //qDebug()<<Adapters::getAlarmMessage(i);
            ui->alarmMessage->setText(Adapters::getAlarmMessage(i));
            ui->alarmMessage->setBlinkStyle(Adapters::getAlarmLevel(i));

            GlobalData::lastAlarmNumber = GlobalData::realTimeInfo.iAlarmSta;
        }
    }
    else
    {
        ui->alarmMessage->setText(tr("系统正常"));
        ui->alarmMessage->setBlinkStyle(Adapters::getAlarmLevel(NORMAL));
        GlobalData::lastAlarmNumber=0;
    }
#else
    ui->alarmMessage->setText(GlobalData::Debugtext);
#endif

    // 当前孔数
    if(lastPointIndex != GlobalData::realTimeInfo.iCNum)
    {
        auto i = static_cast<int>(GlobalData::realTimeInfo.iCNum);

        emit pointIndexChanged(i);
        lastPointIndex = GlobalData::realTimeInfo.iCNum;
    }

    // 总孔数
    if(lastTotalPoint != GlobalData::realTimeInfo.iMNum)
    {
        auto i = static_cast<int>(GlobalData::realTimeInfo.iMNum);

        emit totalPointChanged(i);
        lastTotalPoint = GlobalData::realTimeInfo.iMNum;
    }
}

void MainWindow::onButtonClicked_ResetAlarm()
{
    auto queryItem = new ModbusQueryItemType;
    queryItem->queryAction = Action_WriteResetAlarm;
    queryItem->mixData.u16 = ALMCLEAR_KEY;
    GlobalData::lastAlarmNumber=0;
    emit sendModbusQuery(queryItem);
}

void MainWindow::_onOpenParnm()
{
    inparam= true;
    auto window = new SubWindowParams(this);
    connect(window, SIGNAL(moveAxis(int, int, int, double)),
            this, SLOT(onMoveAxis(int, int, int, double)));
    connect(window, SIGNAL(openOffset(int)),
            this, SLOT(writeInterface(int)));
    //ui->tabText->setText(TAB_BUTTON_NAMES.at(tabIndex));
    ui->tabText->setText(QObject::tr(TAB_BUTTON_NAMES.at(2).toUtf8()));
    writeInterface(SHOW_PARA);
    setSubWindow(window);
}

void MainWindow::_onClickedBottomButton(int btn_id)
{
    QWidget* subWindow = nullptr;

    switch(btn_id)
    {
    case Definitions::BTN_SYS_TRACEPATH://轨迹
    {
        auto window = new SubWindowTrail(this);
        subWindow = window;
        break;
    }
    case Definitions::BTN_TEACH_PATTERN://花样
    {
        auto window = new SubWindowPatterns(this);
        subWindow = window;
        break;
    }
    default:
        break;
    }
    if(nullptr != subWindow)
        setSubWindow(subWindow);
}

void MainWindow::show()
{
    onTabIndexChanged(0); // page one
    onUpdateRealtimeInfo(); // refresh now!
    // load workfile
    //    if(Workfile::getInstance()->isAlreadyReadAll() == false){
    //        auto workfileDialog=new QDialogLoadingWorkfile(this);
    //        workfileDialog->setAttribute(Qt::WA_DeleteOnClose);
    //        workfileDialog->readWrite(true,-1,0);
    //    }
    // 这个connect不放在构造函数内，因为有可能onNotifyOffline()比show()更早执行，导致Segmentation fault
    connect(GlobalData::modbusQueue, SIGNAL(notifyOffline()),
            this, SLOT(onNotifyOffline()));
    readWorkfileName();
    workerThread->start();
    QWidget::show();
}

//数据发送队列
void MainWindow::onSendModbusQuery(MainWindow::ModbusQueryItemType* queryItem, int workerQueryItemIndex)
{
    bool isQueryValid = true;

    auto element = new ModbusQueue::element_t;
    queryItem->workerQueryItemIndex = workerQueryItemIndex;

    switch(queryItem->queryAction)
    {
    case Action_ReadStatus:
        element->data = &GlobalData::realTimeInfo;//数据传输
        element->len = sizeof(GlobalData::realTimeInfo);
        element->operation = ModbusQueue::OP_READ_DATA;
        element->startAddress = ModbusAddress::ControllerRealTimeInfo;
        break;

    case Action_WriteManualMove:
        element->data = queryItem->mixData.otherData;
        element->len = sizeof(MANUALCOMM);
        element->operation = ModbusQueue::OP_WRITE_DATA;
        element->startAddress = ModbusAddress::ManulMove;
        break;

    case Action_WriteResetAlarm:
        element->data = &queryItem->mixData.u16;
        element->len = sizeof(unsigned short);
        element->operation = ModbusQueue::OP_WRITE_DATA;
        element->startAddress = ModbusAddress::WorkCommand;
        break;

    case Action_WriteInterface:
        element->data = &queryItem->mixData.u16;
        element->len = sizeof(unsigned short);
        element->operation = ModbusQueue::OP_WRITE_DATA;
        element->startAddress = ModbusAddress::Interface;
        break;

    case Action_ReadWorkfileName:
        element->data = queryItem->mixData.otherData;
        element->len = sizeof(char) * 24;
        element->operation = ModbusQueue::OP_READ_DATA;
        element->startAddress = ModbusAddress::CurrentWorkFilename;
        break;

    case Action_WriteHeartBeat:
        element->data = &heartBeatCounter;
        element->len = sizeof(heartBeatCounter);
        element->operation = ModbusQueue::OP_WRITE_DATA;
        element->startAddress = ModbusAddress::HeartBeat;
        break;

    default:
        isQueryValid = false;
        break;
    }

    if(true == isQueryValid)
    {
        element->pointerObject = queryItem;
        element->callback = std::tr1::bind(&MainWindow::_modbusQueryCallback, this, std::tr1::placeholders::_1,
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

void MainWindow::onModbusQueryCallbackEnd(bool isOk, void* flag)
{
    bool isNeedDelete = true;

    auto element = reinterpret_cast<ModbusQueue::element_t*>(flag);
    auto queryItem = reinterpret_cast<ModbusQueryItemType*>(element->pointerObject);

    if(isOk)
    {
        retrytime=0;
        switch(queryItem->queryAction)
        {
        case Action_ReadStatus:
            emit _signal_notifyWorkerQueryDone(queryItem->workerQueryItemIndex);
            emit _updateRealtimeInfo();
            break;

        case Action_ReadWorkfileName:
            GlobalData::workfileName = Utils::gbkCharToQString(reinterpret_cast<char*>(queryItem->mixData.otherData));
            break;

        case Action_WriteHeartBeat:
            ++heartBeatCounter;
            emit _signal_notifyWorkerQueryDone(queryItem->workerQueryItemIndex);
            break;

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
        case Action_WriteManualMove:
        {
            auto manulMove = reinterpret_cast<MANUALCOMM*>(element->data);
            Utils::delete_pointer(manulMove);
            break;
        }

        case Action_ReadWorkfileName:
        {
            auto filenameBuffer = reinterpret_cast<char*>(element->data);
            Utils::delete_array(filenameBuffer);
            break;
        }

        default:
            break;
        }

        // free normal memory
        Utils::delete_pointer(queryItem);
        Utils::delete_pointer(element);

        --modbusPendingCount;
    }
}

QThreadMainWindow::QThreadMainWindow(QObject* parent): QThread(parent),
    quitFlag(false)
{
    ConstantlyQueryItemType item;

    item.queryType = MainWindow::Action_ReadStatus;
    item.elapsedTimer.start();
    item.interval = 50;
    item.pendingMutex = new QMutex;
    allQueries.append(item);

    item.queryType = MainWindow::Action_WriteHeartBeat;
    item.elapsedTimer.start();
    item.interval = 1000;
    item.pendingMutex = new QMutex;
    allQueries.append(item);
}

QThreadMainWindow::~QThreadMainWindow()
{
    //debug_printf("~QThreadMainWindow();");

    foreach(const ConstantlyQueryItemType& item, allQueries)
    {
        delete item.pendingMutex;
    }
}

void QThreadMainWindow::run()
{
    while(false == quitFlag)
    {
        for(auto i = 0; i < allQueries.size(); ++i)
        {
            ConstantlyQueryItemType& item = allQueries[i];

            if(item.elapsedTimer.elapsed() >= item.interval && item.pendingMutex->tryLock())
            {
                auto queryItem = new MainWindow::ModbusQueryItemType;
                queryItem->queryAction = item.queryType;
                emit sendModbusQuery(queryItem, i);
                item.elapsedTimer.start();
            }
            Utils::sleep(10);
        }
    }
    //debug_printf("QThreadMainWindow::run() END");
}

void QThreadMainWindow::setQuitFlag(bool quitFlag)
{
    this->quitFlag = quitFlag;
}

void QThreadMainWindow::queryDone(int workerQueryItemIndex)
{
    if(workerQueryItemIndex < 0 || workerQueryItemIndex >= allQueries.size())
        return;

    //debug_printf("unlocking...");
    allQueries.at(workerQueryItemIndex).pendingMutex->unlock();
}
