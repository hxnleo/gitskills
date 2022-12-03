/*
 * qwidgetdiagnose_dogencrypt.cpp
 *
 * 加密狗加密窗口
 *
 * Created on: 2019年 11月 4日 星期一 11:30:40 CST
 * Author: Caojing
 */
#include "qwidgetdiagnose_dogencrypt.h"
#include "modbus/modbusaddress.h"
#include "modbus/modbusqueue.h"
#include "data/globaldata.h"
#include "data/utils.h"
#include <QGridLayout>
#include <QMessageBox>
#include <QLabel>
#include <QDebug>
#include "data/uistyles.h"
#include "qtablewidgetdog.h"
#include "platform/common.h"
#define FONTSIZE 8
//初始密码 1234
QWidgetDiagnose_DogEncrypt::QWidgetDiagnose_DogEncrypt(QWidget* parent,Qt::WindowFlags f):
    QDialog(parent, f),workTread(new DogEncryptThread),writeOk(false)
{
    from.setupUi(this);
    QDog::getInstance()->copyToDog(&dog);
    setWindowIcon(QIcon(":/images/images/NULL.ico"));
    //初始化链表
    from.tableViewdog->init(&dog,FONTSIZE);

    display();
    refreshData(true);
    connect(from.comboBox_DogStatus, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onCombox(int)));

    connect(from.check_Time, SIGNAL(stateChanged(int)),
            this, SLOT(onSateChanged(int)));

    connect(from.btn_Init, SIGNAL(clicked(bool)),
            this, SLOT(onInit(bool)));

    connect(from.Appliy, SIGNAL(clicked()),
            this, SLOT(onAppliy()));

    connect(from.Ok, SIGNAL(clicked(bool)),
            this, SLOT(onOk()));

    connect(from.pre, SIGNAL(pressed()),
            this, SLOT(onPre()));

    connect(from.next, SIGNAL(pressed()),
            this, SLOT(onNext()));

    connect(workTread, SIGNAL(sendOk()),
            this, SLOT(upDataTime()));

    auto timer=new QTimer(this);
    connect(timer, SIGNAL(timeout()),
            this, SLOT(onTimeout()));
    timer->setInterval(500); // 更新间隔
    timer->start();
    from.Ok->setFocus();
    workTread->start();

}
QWidgetDiagnose_DogEncrypt::~QWidgetDiagnose_DogEncrypt()
{
    workTread->isStopped = true;
    Utils::waitForThreadFinished(workTread);
}

void QWidgetDiagnose_DogEncrypt::display()
{
    from.edit_MCserial->setText(QString("%1").arg(dog.Machine_SerialNo));//机器序列
    from.edit_MFserial->setText(QString("%1").arg(dog.Factory_SerialNo));//厂商序列
    from.edit_RremaDay->setText(QString("%1").arg(dog.User_RemainDay));//剩余时间
    from.edit_Password->setText(QString("%1").arg(dog.Machine_Password));//管理密码
    from.edit_Random->setText(QString("%1").arg(dog.RandomNum));//随机索引
}

void QWidgetDiagnose_DogEncrypt::updateData()
{//更新编辑框

    QDog::getInstance()->m_dog.Machine_SerialNo = from.edit_MCserial->text().toInt();
    QDog::getInstance()->m_dog.Factory_SerialNo = from.edit_MFserial->text().toInt();
    QDog::getInstance()->m_dog.Machine_Password = from.edit_Password->text().toInt();
    QDog::getInstance()->m_dog.User_RemainDay   = from.edit_RremaDay->text().toInt();

    upDataTime();
    onTimeout();
    onSateChanged(0);
}

void QWidgetDiagnose_DogEncrypt::refreshData(bool isNeedRefresh)
{//刷新整个界面
    int selected;
    if(isNeedRefresh)
    {
        if(QDog::getInstance()->isUnlocked())
            from.comboBox_DogStatus->setCurrentIndex(0);
        else
        {
            selected = QDog::getInstance()->getDogType();
            switch (selected) {
            case 1:
            case 2:
                from.comboBox_DogStatus->setCurrentIndex(selected);
                break;
            default:
                from.comboBox_DogStatus->setCurrentIndex(0);
                break;
            }
        }
    }

    selected=from.comboBox_DogStatus->currentIndex();
    QString str;
    //设置加密狗的期数和密码
    switch(selected){
    case 0: // 关闭加密狗
        from.tableViewdog->showPassword(0);
        //from.tableViewdog->selectionModel()->currentIndex();
        from.edit_RremaDay->setEnabled(false);
        from.edit_Term->setText(QString());
        break;
    case 1: // 开狗1
        from.tableViewdog->showPassword(12);
        from.edit_RremaDay->setEnabled(false);
        str=QString("%1").arg(dog.User_Level);
        from.edit_Term->setText(str);
        break;
    case 2: // 开狗2
        from.tableViewdog->showPassword(0);
        from.edit_RremaDay->setEnabled(true);
        from.edit_Term->setText(QString());
        break;
    default:
        break;
    }
    display();
    updateData();
}



void QWidgetDiagnose_DogEncrypt::onTimeout()
{//当前标准时间显示
#ifdef _MCGS
    QDateTime currTime = platform_sys_info_c::get_rtc_time();
    if(currTime.isValid())
    {
        from.edit_ModifyDate->setDateTime(currTime);
        from.edit_ModifyTime->setDateTime(currTime);
    }
    else
    {
        qDebug("get rtc time invalid");
    }
#else

    from.edit_ModifyDate->setDisplayFormat("yyyy/MM/dd");
    from.edit_ModifyTime->setDisplayFormat("HH:mm:ss");

    // 设置日期时间
    from.edit_ModifyDate->setDateTime(QDateTime::currentDateTime());
    from.edit_ModifyTime->setDateTime(QDateTime::currentDateTime());
#endif

}

void QWidgetDiagnose_DogEncrypt::onPre()
{//翻页 多期狗才起作用
    if(from.comboBox_DogStatus->currentIndex()==1)
        from.tableViewdog->showPassword(12,0);
}

void QWidgetDiagnose_DogEncrypt::onNext()
{//翻页 多期狗才起作用
    if(from.comboBox_DogStatus->currentIndex()==1)
        from.tableViewdog->showPassword(12,1);
}


void QWidgetDiagnose_DogEncrypt::onCombox(int index)
{
    //qDebug()<<index;
    dog.User_RemainDay = 0;
    refreshData(false);
}
void QWidgetDiagnose_DogEncrypt::onSateChanged(int index)
{//是否修改时间
    //qDebug()<<from.check_Time->checkState();
    if(index==0)
    {
        from.edit_ModifyDate->setEnabled(false);
        from.edit_ModifyTime->setEnabled(false);
    }
    else
    {
        from.edit_ModifyDate->setEnabled(true);
        from.edit_ModifyTime->setEnabled(true);
    }
}

void QWidgetDiagnose_DogEncrypt::onInit(bool)
{//重置加密狗
    if(QMessageBox::Cancel == Utils::MyMessageBox(QString("Warning"), tr("确定重置加密狗？"),tr("确定"),tr("取消"),this))
    {
        return;
    }
    from.btn_Init->setDisabled(true);
    if(QDog::getInstance()->init())
    {
        QDog::getInstance()->copyToDog(&dog);
        refreshData(true);
    }
    else
    {
        Utils::MyMessageBox(QString("Info"), tr("初始化失败"),tr("确定"),this);
    }
    from.btn_Init->setDisabled(false);
}

void QWidgetDiagnose_DogEncrypt::onAppliy()
{//应用设置
    updateData();
    from.btn_Init->setDisabled(true);
    int selected = from.comboBox_DogStatus->currentIndex();
    bool isUnlock = true;
    switch (selected) {
    case 0://禁用加密狗
        isUnlock = true;
        QDog::getInstance()->setDogType(2);//默认为单期狗
        break;
    case 1:
    case 2:
        isUnlock = false;
        QDog::getInstance()->setDogType(selected);
        break;
    default:
        return;
        break;
    }
    QDog::getInstance()->setUnlock(isUnlock);//设置是否解锁
    if(true == QDog::getInstance()->writeDog())
    {
        if(false == isUnlock)
            QDog::getInstance()->writeDogProgress(1);
        Utils::MyMessageBox(QString("Info"), tr("操作成功"),tr("确定"),this);
    }
    else
        Utils::MyMessageBox(QString("Info"), tr("写入加密狗失败"),tr("确定"),this);

    SYSTIME new_time;
    if(from.check_Time->checkState())
    {
        new_time.today.year = from.edit_ModifyDate->date().year();
        new_time.today.month = from.edit_ModifyDate->date().month();
        new_time.today.day = from.edit_ModifyDate->date().day();

        new_time.now.hour = from.edit_ModifyTime->time().hour();
        new_time.now.minute = from.edit_ModifyTime->time().minute();
        new_time.now.second = from.edit_ModifyTime->time().second();
        QDog::getInstance()->readWriteModbus(&new_time,sizeof(new_time),ModbusAddress::Dog_SysTime,false);
    }
    from.btn_Init->setDisabled(false);

}

void QWidgetDiagnose_DogEncrypt::onOk()
{//退出
    //    onAppliy();
    workTread->isStopped = true;
    this->close();
}


void QWidgetDiagnose_DogEncrypt::upDataTime()
{//跟新控制器时间
    const QChar LEADING_ZERO = QChar('0');
    //    from.edit_Time->setText(QString("%1-%2-%3 %4:%5:%6")
    //                            .arg(QDog::getInstance()->dog_time.today.year)
    //                            .arg(QDog::getInstance()->dog_time.today.month)
    //                            .arg(QDog::getInstance()->dog_time.today.day)
    //                            .arg(static_cast<quint16>(QDog::getInstance()->dog_time.now.hour), 2, 10, LEADING_ZERO)
    //                            .arg(static_cast<quint16>(QDog::getInstance()->dog_time.now.minute), 2, 10, LEADING_ZERO)
    //                            .arg(static_cast<quint16>(QDog::getInstance()->dog_time.now.second), 2, 10, LEADING_ZERO));
    from.edit_Time->setText(QString("%1:%2:%3")
                            .arg(static_cast<quint16>(QDog::getInstance()->dog_time.now.hour), 2, 10, LEADING_ZERO)
                            .arg(static_cast<quint16>(QDog::getInstance()->dog_time.now.minute), 2, 10, LEADING_ZERO)
                            .arg(static_cast<quint16>(QDog::getInstance()->dog_time.now.second), 2, 10, LEADING_ZERO));
}

//加密狗获取时间线程
DogEncryptThread::DogEncryptThread(QObject* parent): QThread(parent),
    isStopped(false),out(false)
{
    auto timer=new QTimer(this);
    connect(timer, SIGNAL(timeout()),
            this, SLOT(onTimeout()));
    timer->setInterval(500); // 更新间隔
    timer->start();
}
void DogEncryptThread::onTimeout()
{
    out = true;
}
void DogEncryptThread::run()
{
    ModbusQueue::element_t element;
    while(false == isStopped)
    {
        if(out)
        {
            out = false;
            element.data = &QDog::getInstance()->dog_time;
            element.len = sizeof(QDog::getInstance()->dog_time);
            element.operation = ModbusQueue::OP_READ_DATA;
            element.startAddress = ModbusAddress::Dog_SysTime;
            if(GlobalData::modbusQueue->sendQuery(&element, false))
            {
                emit sendOk();
            }
        }
        else
            Utils::sleep(100);
    }
}
