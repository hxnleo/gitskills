/*
 * startupdialog.cpp
 *
 * 启动加载参数
 *
 * Created on: 2019年 01月 21日 星期一 15:00:16 CST
 * Author: lixingcong
 */
#include "startupdialog.h"
#include "ui_startupdialog.h"
#include "data/utils.h"
#include "modbus/modbusqueue.h"
#include "modbus/modbusaddress.h"
#include "data/adapters.h"
#include "data/globaldata.h"
#include <public.h>
#include <SysText.h>
#include <Parameter.h>
#include <QElapsedTimer>
#include "data/dog.h"
#include "widgets/qdialogmature.h"
#include "widgets/qdialogfailconnect.h"

#define WORKER_SLEEP_INTERVAL 50
#define FIRST_CONNECT_TIMEOUT 30000 // 30秒，首次启动超时

enum WorkStep
{
    WS_UNDEFINED,
    WS_INIT_PRAMTABLE_INDEX,//初始化参数索引
    WS_TEST_CONNECTION,//测试连接
    WS_TEST_GOG,//测试连接
    WS_INIT_ETHERCAT1,//总线初始化
    WS_READ_PARATABLE,//读取参数
    WS_READ_IOCONFIG,//读取IO
    WS_DONE
};

StartUpDialog::StartUpDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::StartUpDialog)
{
    ui->setupUi(this);
    Utils::setFullScreen(this);
    setWindowTitle(tr("正在启动"));
    //setFixedSize(600, 400);

    /*
     * TODO
     * read local software config
     * .....
     */

    workerThread = new QThreadStartupWindow(GlobalData::modbusQueue);

    connect(workerThread, SIGNAL(workStepChanged(int)),
            this, SLOT(onWorkStepChanged(int)), Qt::QueuedConnection);

    connect(workerThread, SIGNAL(failed()),
            this, SLOT(onFailed()), Qt::QueuedConnection);

    connect(workerThread, SIGNAL(noTime()),
            this, SLOT(onNoTime()), Qt::QueuedConnection);

    connect(workerThread, SIGNAL(creatDecrypt()),
            this, SLOT(onCreatDecrypt()), Qt::QueuedConnection);


    connect(ui->btn_continue, SIGNAL(clicked()),
            this, SLOT(onButtonClickedContinue()));

    connect(ui->btn_ignore, SIGNAL(clicked()),
            this, SLOT(onButtonClickedIgnore()));

    ui->btn_continue->setDisabled(true);

    workerThread->setWorkStep(WS_INIT_PRAMTABLE_INDEX);
    workerThread->start();

}

StartUpDialog::~StartUpDialog()
{
    forceStopThread();
    delete workerThread;
    delete ui;
}


void StartUpDialog::onNoTime()
{
    if(QMessageBox::Retry==Utils::MyMessageBox(QString("Info"), tr("使用天数只剩")+QString("%1")
                                            .arg(QDog::getInstance()->getRemainDays())
                                            +tr("天,请尽快联系商家解密!"),tr("确定"),this))
    {
        workerThread->timeok = true;
    }

}

void StartUpDialog::onCreatDecrypt()
{//解密框处理 是否发送成功
    QDialogMature* d = new QDialogMature(this);
    d->setAttribute(Qt::WA_DeleteOnClose);
    d->open();
    connect(d, SIGNAL(success()),
            this, SLOT(decryptOk()));
    connect(d, SIGNAL(fail()),
            this, SLOT(decryptFail()));
    connect(this, SIGNAL(continu()),
            d, SLOT(close()));

}

void StartUpDialog::decryptOk()
{//继续运行
    workerThread->dogflag  = true;
    emit continu();
}

void StartUpDialog::decryptFail()
{//失败提示 如果未到期直接走,如果到期循环提示输入密码
    //    qDebug()<<"dog fail";
    if(GlobalData::dogflag)
    {
        decryptOk();

    }
    else
    {
        QDog::getInstance()->readDog();//读取加密狗
        if(GlobalData::online == false)
        {
            Utils::MyMessageBox(QString("Info"), tr("通信中断中,请先连接控制器!"),tr("确定"),this);

        }
        if(2 == QDog::getInstance()->getDogProcessCode())
        {
            if(QMessageBox::Retry==Utils::MyMessageBox(QString("Info"), tr("解密失败,还可以使用")+QString("%1")
                                                    .arg(QDog::getInstance()->getRemainDays())+tr("天,请尽快联系商家解密!"),tr("确定"),this))
            {
                workerThread->dogflag  = true;
                emit continu();

            }
        }
        else if(0 == QDog::getInstance()->getDogProcessCode())
        {
            Utils::MyMessageBox(QString("Info"), tr("解密成功"),tr("确定"),this);
            decryptOk();
        }
        else
        {
            qDebug()<<"解密失败";
            Utils::MyMessageBox(QString("Info"), tr("解密失败,请重新输入"),tr("确定"),this);
        }
    }
}

void StartUpDialog::onButtonClickedContinue()
{
    if(false == workerThread->isRunning())
    {
        workerThread->start();
        ui->btn_continue->setDisabled(true);
        ui->btn_ignore->setDisabled(true);
        ui->progressBar->setMaximum(0);
    }
}

void StartUpDialog::onButtonClickedIgnore()
{
#ifdef _MCGS
    QDialogFailConnect *dialog = new QDialogFailConnect(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog, SIGNAL(rejected()),this, SLOT(onQuitFialConnect()));
    //居中显示
    dialog->move(QApplication::desktop()->screen()->rect().center() - dialog->rect().center());
    dialog->exec();
#else
    onQuitFialConnect();
#endif
    Adapters::initParamTable();
    Adapters::initIOName();
    Adapters::initAlarmMessage();
}

void StartUpDialog::onQuitFialConnect()
{//modbusQueue 通信终止
    GlobalData::modbusQueue->queryStopThenOffline();
    accept();
}

void StartUpDialog::onWorkStepChanged(int workstep)
{
    switch(workstep)
    {
    case WS_INIT_PRAMTABLE_INDEX:
        ui->label->setText(tr("初始化参数"));
        break;

    case WS_TEST_CONNECTION:
        ui->label->setText(tr("通讯连接中"));
        break;

    case WS_TEST_GOG:
        ui->label->setText(tr("加密狗检测"));
        break;
    case WS_INIT_ETHERCAT1:
        ui->label->setText(tr("总线初始化"));
        break;
    case WS_READ_PARATABLE:
        ui->btn_ignore->setDisabled(true);
        ui->label->setText(tr("加载系统参数"));
        break;

    case WS_READ_IOCONFIG:
        ui->label->setText(tr("加载IO参数"));
        break;

    case WS_DONE:
        accept();
        break;

    default:
        //qDebug("StartUpDialog::onWorkStepChanged = %d",workstep);
        break;
    }
}

void StartUpDialog::reject()
{
    forceStopThread();
    QDialog::reject();
}

void StartUpDialog::onFailed()
{
    // qDebug("StartUpDialog::onFailed()");
    ui->label->setText(tr("通讯失败，请点击重试"));
    ui->btn_continue->setDisabled(false);
    ui->btn_ignore->setDisabled(false);
    ui->progressBar->setMaximum(100);
}

void StartUpDialog::forceStopThread()
{
    workerThread->setQuitFlag(true);

    while(workerThread->isRunning())
        Utils::sleep(WORKER_SLEEP_INTERVAL);
}

QThreadStartupWindow::QThreadStartupWindow(ModbusQueue* mbq, QObject* parent):
    QThread(parent), timeok(false),dogflag(false),quitFlag(false), workstep(WS_UNDEFINED), mbq(mbq)
{

}

QThreadStartupWindow::~QThreadStartupWindow()
{
    //qDebug("~QThreadStartupWindow()");
}

void QThreadStartupWindow::setWorkStep(int workstep)
{
    this->workstep = workstep;
}

void QThreadStartupWindow::run()
{
    int lastWorkstep = WS_UNDEFINED;
    signed short intmotion = -1;
    bool isOk = false;
    QElapsedTimer elapsedTimer;
    elapsedTimer.start();

    //emit creatDecrypt();//测试解密软件
    while(false == quitFlag)
    {
        if(lastWorkstep != workstep)
        {
            emit workStepChanged(workstep);
            lastWorkstep = workstep;
        }

        switch(workstep)
        {
        case WS_INIT_PRAMTABLE_INDEX:

            workstep = WS_INIT_ETHERCAT1;

            isOk = true;
            break;

        case WS_INIT_ETHERCAT1:

            Utils::sendOffest(&intmotion,ModbusAddress::Motioninit,sizeof(intmotion));

            if(intmotion!=-1)
            {
                if(intmotion==0)
                    qDebug()<<"运动裤初始化成功";
                else
                    qDebug()<<"运动裤初始化失败";

                Adapters::initParamTable();
                Adapters::initIOName();
                Adapters::initAlarmMessage();
#ifndef DEBUG_OFFLINE
                workstep = WS_TEST_CONNECTION;
#else
                workstep = WS_TEST_GOG;
#endif
                break;
            }
            else
            {
                qDebug()<<"运动裤初始化中 intmotion"<<intmotion;
                Utils::sleep(100);
            }
            break;

        case WS_TEST_CONNECTION:
            if(true == testConnection())
            {
                qDebug()<<"ok";
                workstep=WS_TEST_GOG; // next step
                isOk=true;
            }
            else
            {
                qDebug()<<"fail";
                if(elapsedTimer.elapsed() > FIRST_CONNECT_TIMEOUT)
                {
                    workstep=WS_TEST_GOG;
                    isOk=false;
                }
                else
                {
                    isOk=true; // repeatly read....
                }
            }

            break;

        case WS_TEST_GOG:
            //#ifdef _MCGS 加密狗检车
            isOk = QDog::getInstance()->readDog();
            if(isOk)
            {
                if(1 == QDog::getInstance()->getDogType())
                {//多期狗
                    if(true == QDog::getInstance()->isUnlocked())
                    {
                        workstep=WS_READ_PARATABLE;
                    }
                    else if(1 == QDog::getInstance()->getDogProcessCode())
                    {//过期了
                        emit creatDecrypt();
                        while (1)
                        {
                            if(dogflag == true)
                                break;
                            Utils::sleep(100);
                        }
                        workstep=WS_READ_PARATABLE;
                        break;
                    }
                    else if( QDog::getInstance()->getRemainDays()<=3)
                    {//只剩三天
                        emit noTime();
                        while(1)
                        {
                            //qDebug()<<timeok;
                            if(timeok==true)
                                break;
                            Utils::sleep(100);
                        };
                        workstep=WS_READ_PARATABLE;
                    }
                    else
                    {
                        workstep=WS_READ_PARATABLE;
                    }
                }
                else
                {
                    switch (QDog::getInstance()->getDogProcessCode())
                    {
                    case 0://无限制
                        workstep=WS_READ_PARATABLE;
                        break;
                    case 2://快到期了
                        qDebug()<<"快到期了";
                        GlobalData::dogflag = true;
                        emit creatDecrypt();
                        while (1)
                        {
                            if(dogflag == true)
                                break;
                            GlobalData::dogflag = false;
                            Utils::sleep(100);
                        }
                        workstep=WS_READ_PARATABLE;
                        break;
                    case 1://已经过期
                    default:
                        qDebug()<<"过期了";
                        emit creatDecrypt();
                        while (1)
                        {
                            if(dogflag == true)
                                break;
                            Utils::sleep(100);
                        }
                        workstep=WS_READ_PARATABLE;
                        break;
                    }
                }

            }
            break;
        case WS_READ_PARATABLE:
            isOk = Adapters::readParamTable(mbq, false);

            if(isOk)
                workstep = WS_READ_IOCONFIG;

            break;

        case WS_READ_IOCONFIG:
            isOk = Adapters::readIOConfig(mbq);

            if(isOk)
                workstep = WS_DONE;

            break;

        default:
            break;
        }

        // notify failures
        if(false == isOk)
        {
            emit failed();
            break;
        }

        // Done
        if(WS_DONE == workstep)
        {
            Adapters::initGlobalData(); // 所有参数读完，初始化全局数据

            emit workStepChanged(workstep);
            break; // while loop
        }

        Utils::sleep(WORKER_SLEEP_INTERVAL);
    }

    //qDebug("exit run()");
}

void QThreadStartupWindow::setQuitFlag(bool quitFlag)
{
    this->quitFlag = quitFlag;
}

bool QThreadStartupWindow::testConnection()
{
    quint32 u32;
    ModbusQueue::element_t element;
    element.data=&u32;
    element.len=sizeof(u32);
    element.operation=ModbusQueue::OP_READ_DATA;
    element.startAddress=ModbusAddress::HeartBeat;

    return GlobalData::modbusQueue->sendQuery(&element,false);
}
