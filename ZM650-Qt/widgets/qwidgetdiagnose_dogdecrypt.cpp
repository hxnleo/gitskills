/*
 * qwidgetdiagnose_dogdecrypt.cpp
 *
 * 加密狗解密窗口
 *
 * Created on: 2019年 11月 4日 星期一 11:30:40 CST
 * Author: Caojing
 */
#include "qwidgetdiagnose_dogdecrypt.h"
#include "data/dog.h"
#include "data/globaldata.h"
#include "data/utils.h"
#include <QGridLayout>
#include <QLabel>
#include "data/uistyles.h"

QWidgetDiagnose_DogDecrypt::QWidgetDiagnose_DogDecrypt(QWidget* parent,Qt::WindowFlags f):
    QDialog(parent, f),workTread(new DogDecryptThread)
{
    from.setupUi(this);
    QFont font ;
    font.setPointSize(10);
    this->setFont(font);

    from.edit_RremaDay->setText(QString("%1").arg(QDog::getInstance()->getRemainDays()));
    setWindowIcon(QIcon(":/images/images/NULL.ico"));
    updateData();
    connect(from.Ok, SIGNAL(pressed()),
            this, SLOT(onOk()));

    connect(workTread, SIGNAL(sendOk()),
            this, SLOT(upDataTime()));

    from.Ok->setFocus();
    workTread->start();
}

void QWidgetDiagnose_DogDecrypt::updateData()
{//跟新显示数据
    from.edit_MFserial->setText(QString("%1").arg(QDog::getInstance()->m_dog.Factory_SerialNo));
    from.edit_Random->setText(QString("%1").arg(QDog::getInstance()->m_dog.RandomNum));
    from.edit_Term->setText(QString("%1").arg(QDog::getInstance()->getDogLevel()));
    QDog::getInstance()->m_dog.User_RemainDay = from.edit_RremaDay->text().toInt();//赋值加密狗
    if(QDog::getInstance()->isUnlocked())
    {
        from.edit_UnlockStatus->setText(tr("已解锁"));
    }
    else
    {
        from.edit_UnlockStatus->setText(tr("未解锁"));
    }
    upDataTime();
}
QWidgetDiagnose_DogDecrypt::~QWidgetDiagnose_DogDecrypt()
{
    workTread->isStopped = true;
    Utils::waitForThreadFinished(workTread);
}
void QWidgetDiagnose_DogDecrypt::upDataTime()
{
//    const QChar LEADING_ZERO = QChar('0');
//    from.edit_Time->setText(QString("%1-%2-%3 %4:%5:%6")
//                            .arg(QDog::getInstance()->dog_time.today.year)
//                            .arg(QDog::getInstance()->dog_time.today.month)
//                            .arg(QDog::getInstance()->dog_time.today.day)
//                            .arg(static_cast<quint16>(QDog::getInstance()->dog_time.now.hour), 2, 10, LEADING_ZERO)
//                            .arg(static_cast<quint16>(QDog::getInstance()->dog_time.now.minute), 2, 10, LEADING_ZERO)
//                            .arg(static_cast<quint16>(QDog::getInstance()->dog_time.now.second), 2, 10, LEADING_ZERO));
    from.edit_Time->setText(QString("%1-%2-%3")
                            .arg(2000+QDog::getInstance()->dog_time.today.year)
                            .arg(QDog::getInstance()->dog_time.today.month)
                            .arg(QDog::getInstance()->dog_time.today.day));
}
void QWidgetDiagnose_DogDecrypt::onOk()
{
    QString pwd;
    int serial;
    int pwd_num;
    if(false == QDog::getInstance()->isUnlocked())
    {//已经解锁直接走

        serial = QDog::getInstance()->m_dog.Machine_SerialNo;//得到机器序列号
        if(serial != from.edit_MCserial->text().toInt())
        {
            Utils::MyMessageBox(QString("Error"), tr("机器序列号错误,请重新输入!"),QObject::tr("确定"),this);
            from.edit_MCserial->clear();
            return;
        }
        pwd = from.edit_Password->text();
        pwd_num = pwd.toInt();
        if(1 == QDog::getInstance()->getDogType())
        {
            //qDebug()<<" dog1 ";
            QString str;
            unsigned short level;
            if(false == QDog::getInstance()->validUserPassword(pwd_num,&level))
            {//识别密码是否有效
                Utils::MyMessageBox(QString("Error"), tr("无效密码,请重新输入!"),QObject::tr("确定"),this);
                from.edit_Password->clear();
                return;
            }
            QDog::getInstance()->setUserLevel(level+1);

            if(QDog::getInstance()->writeDog())
            {
//                if(QDog::getInstance()->readDog())
//                {
                    if(QDog::getInstance()->isUnlocked())
                    {
                        QDog::getInstance()->writeDogProgress(0);
                        Utils::MyMessageBox(QString("OK"), tr("您已经解锁所有时长,感谢使用"),QObject::tr("确定"),this);
                        emit success();
                    }
                    else
                    {
                        QString str1 = tr("已解锁期数");
                        QString str2 = tr("可使用天数");
                        str = QString("%1: %2: %3: %4")
                                .arg(str1).arg(level).arg(str2).arg(QDog::getInstance()->getRemainDays());
                        Utils::MyMessageBox(QString("Warning"), str,QObject::tr("确定"),this);
                        //qDebug()<<"day33"<<QDog::getInstance()->getRemainDays();
                        emit success();
                    }
//                }
            }
            else
            {

                Utils::MyMessageBox(QString("Error"), str,QObject::tr("写入加密狗失败"),this);
                emit success();
            }

        }
        else
        {
            QDog::getInstance()->writeUserPassword(pwd_num);
            emit fail();
        }
    }
    else
    {
        emit success();
    }
    return;
}


DogDecryptThread::DogDecryptThread(QObject* parent): QThread(parent),
    isStopped(false),out(false),timer(new QTimer)
{
    auto timer=new QTimer(this);
    connect(timer, SIGNAL(timeout()),
            this, SLOT(onTimeout()));
    timer->setInterval(100); // 更新间隔
    timer->start();
}
void DogDecryptThread::onTimeout()
{
    out = true;
}
void DogDecryptThread::run()
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
