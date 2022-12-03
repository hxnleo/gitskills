/*
 * qwidgetshowparams_diagnoseutils.cpp
 *
 * 诊断界面-辅助工具
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include <QScreen>
#include <QApplication>
#include "qwidgetshowparams_diagnoseutils.h"
#include "widgets/qpushbuttonparams.h"
#include "widgets/qdialogparams_writeanything.h"
#include "data/definitions.h"
#include "data/utils.h"
#include "data/globaldata.h"
#include "modbus/modbusaddress.h"
#include <QProcess>
#include <QMessageBox>
#include <QDesktopWidget>
#include "qdialogpassword.h"
#include <unistd.h>
#include <widgets/qwidgetdiagnose_dogdecrypt.h>
#include <widgets/qwidgetdiagnose_dogencrypt.h>


QWidgetShowParams_DiagnoseUtils::QWidgetShowParams_DiagnoseUtils(QWidget* parent):
    QWidgetShowParamsBase(parent),
    localDir(new QDir),
    workThread(new ToolThread(this)),
    adapter(new DataAdapter_DiagnoseUtils)
{
    connect(workThread, SIGNAL(operationDone(bool)),
            this, SLOT(onWorkerThreadOperationDone(bool)));
}

QWidgetShowParams_DiagnoseUtils::~QWidgetShowParams_DiagnoseUtils()
{
    // 关闭FTP
    if(false == GlobalData::modbusQueue->isOffline())
    {
        auto element = new ModbusQueue::element_t;
        auto mixData = new Definitions::MixDataType;
        mixData->u16 = 0;
        element->data = &mixData->u16;
        element->len = sizeof(mixData->u16);
        element->startAddress = ModbusAddress::FtpConnect;
        element->operation = ModbusQueue::OP_WRITE_DATA;
        element->pointerObject = mixData;
        sendModbusElement(element);
        //qDebug("close FTP");
    }

    // 等待modbus通信完成
    Utils::waitForPendingModbus(&pendingQueryCount);
    workThread->isStopped = true;
    Utils::waitForThreadFinished(workThread);
    delete adapter;
    delete localDir;
}
void QWidgetShowParams_DiagnoseUtils::onWorkerThreadOperationDone(bool isOk)
{
    if(isOk)
    {
        Utils::MyMessageBox(QString("OK"), tr("升级成功,请重启控制器"),tr("确定"),this);
    }
    else
    {
        Utils::MyMessageBox(QString("Error"), tr("升级失败"),tr("确定"),this);
    }
    emit dismissProgressDialog();
}

void QWidgetShowParams_DiagnoseUtils::onIoDisplay()
{
    if(false == GlobalData::iodisplay)
    {
        if(QMessageBox::Retry == Utils::MyMessageBox(QString("Info"), tr("是否打开IO显示功能？"),tr("确定"),tr("取消"),this))
        {
            GlobalData::iodisplay = true;
        }
    }
    else
    {
        if(QMessageBox::Retry == Utils::MyMessageBox(QString("Info"), tr("是否关闭IO显示功能？"),tr("确定"),tr("取消"),this))
        {
            GlobalData::iodisplay = false;
        }
    }
}

void QWidgetShowParams_DiagnoseUtils::onPatter1()
{//蓝思 花样参数
    Utils::sendOffest(&GlobalData::parambite,ModbusAddress::ParamUse,sizeof(GlobalData::parambite));
    int status= READ_BIT(GlobalData::parambite,0);//去读对应的位数
    //    qDebug()<<"status"<<status<<GlobalData::parambite;
    if(!status)
    {
        if(QMessageBox::Retry == Utils::MyMessageBox(QString("Info"), tr("是否打开样式1"),tr("确定"),tr("取消"),this))
        {
            if(READ_BIT(GlobalData::parambite,0)&&READ_BIT(GlobalData::parambite,1))
            {
                Utils::MyMessageBox(QString("Error"), tr("请关闭花样2"),tr("确定"),this);
                return;
            }
            GlobalData::parambite = SET_BIT(GlobalData::parambite,0);
            Utils::sendOffest(&GlobalData::parambite,ModbusAddress::ParamUse,sizeof(GlobalData::parambite),true);
        }
    }
    else
    {
        if(QMessageBox::Retry == Utils::MyMessageBox(QString("Info"), tr("是否关闭样式1"),tr("确定"),tr("取消"),this))
        {
            GlobalData::parambite = CLR_BIT(GlobalData::parambite,0);
            Utils::sendOffest(&GlobalData::parambite,ModbusAddress::ParamUse,sizeof(GlobalData::parambite),true);
        }
    }
}

void QWidgetShowParams_DiagnoseUtils::onPatter2()
{//红叶 花样参数
    Utils::sendOffest(&GlobalData::parambite,ModbusAddress::ParamUse,sizeof(GlobalData::parambite));
    int status= READ_BIT(GlobalData::parambite,1);//去读对应的位数
    //    qDebug()<<"status"<<status<<GlobalData::parambite;
    if(!status)
    {
        if(QMessageBox::Retry == Utils::MyMessageBox(QString("Info"), tr("是否打开样式2"),tr("确定"),tr("取消"),this))
        {
            if(READ_BIT(GlobalData::parambite,0)&&READ_BIT(GlobalData::parambite,1))
            {
                Utils::MyMessageBox(QString("Error"), tr("请关闭花样1"),tr("确定"),this);
                return;
            }
            GlobalData::parambite = SET_BIT(GlobalData::parambite,1);
            Utils::sendOffest(&GlobalData::parambite,ModbusAddress::ParamUse,sizeof(GlobalData::parambite),true);
        }
    }
    else
    {
        if(QMessageBox::Retry == Utils::MyMessageBox(QString("Info"), tr("是否关闭样式2"),tr("确定"),tr("取消"),this))
        {
            GlobalData::parambite = CLR_BIT(GlobalData::parambite,1);
            Utils::sendOffest(&GlobalData::parambite,ModbusAddress::ParamUse,sizeof(GlobalData::parambite),true);
        }
    }
}

void QWidgetShowParams_DiagnoseUtils::onReadwrite()
{
    QString pFilePath="";
    QDir* localDir1 = new QDir();
    //    localDir1->cd(QDir::rootPath());
    localDir1->cd("/home/mcgs/work");
    localFileList = localDir1->entryInfoList();
    for(int i = 0;i < localFileList.size(); ++i)
    {
        auto fileInfo = localFileList.at(i);

        qDebug()<<"localFileList"<<QString("%1").arg(i)<<fileInfo.filePath();
    }

    //    localDir->cd("media/usb0");//设置U盘挂载路径
    pFilePath = "text";
    //    QFile  pFile("/home/mcgs/work/filename");
    QFile  pFile("/root/app/filename");
    QString g_iLanguage("123");

    pFile.open(QIODevice::ReadWrite | QIODevice::Text);
    //写入内容,这里需要转码，否则报错。
    QByteArray str = g_iLanguage.toUtf8();
    //写入QByteArray格式字符串
    pFile.write(str);
    pFile.close();
}


void QWidgetShowParams_DiagnoseUtils::onPassWord()
{
    Utils::sendOffest(&GlobalData::parambite,ModbusAddress::ParamUse,sizeof(GlobalData::parambite));
    int status= READ_BIT(GlobalData::parambite,2);//去读对应的位数
    //    qDebug()<<"status"<<status<<GlobalData::parambite;
    if(!status)
    {
        if(QMessageBox::Retry == Utils::MyMessageBox(QString("Info"), tr("参数密码打开"),tr("确定"),tr("取消"),this))
        {
            GlobalData::parambite = SET_BIT(GlobalData::parambite,2);
            Utils::sendOffest(&GlobalData::parambite,ModbusAddress::ParamUse,sizeof(GlobalData::parambite),true);
            GlobalData::parampassword = true;
        }
    }
    else
    {
        if(QMessageBox::Retry == Utils::MyMessageBox(QString("Info"), tr("参数密码关闭"),tr("确定"),tr("取消"),this))
        {
            GlobalData::parambite = CLR_BIT(GlobalData::parambite,2);
            Utils::sendOffest(&GlobalData::parambite,ModbusAddress::ParamUse,sizeof(GlobalData::parambite),true);
            GlobalData::parampassword = false;
        }
    }
}

void QWidgetShowParams_DiagnoseUtils::onOpenCut()
{
    if(false == GlobalData::ScreenCapture)
    {
        if(QMessageBox::Retry == Utils::MyMessageBox(QString("Info"), tr("是否打开截屏工具？"),tr("确定"),tr("取消"),this))
        {
            GlobalData::ScreenCapture = true;
        }
    }
    else
    {
        if(QMessageBox::Retry == Utils::MyMessageBox(QString("Info"), tr("是否关闭截屏工具？"),tr("确定"),tr("取消"),this))
        {
            GlobalData::ScreenCapture = false;
        }
    }
}

void QWidgetShowParams_DiagnoseUtils::onLoad()
{//图片文件拷贝  因为一般用单的图片有限此处 简单编写
    if(QMessageBox::Retry == Utils::MyMessageBox(QString("Info"), tr("是否加载U盘图片？"),tr("确定"),tr("取消"),this))
    {
        //加载图片
        if(QFile::exists("/media/usb0/ADT/logo.bmp"))
        {
            if(QFile::exists(GlobalData::PicturePath)) QFile::remove(GlobalData::PicturePath);
            if(!QFile::copy("/media/usb0/ADT/logo.bmp", GlobalData::PicturePath))
            {
                //                qDebug()<<"copy fail";
                Utils::MyMessageBox(QString("Erro"), tr("图片加载失败"),tr("确定"),this);
            }
            else
            {
                Utils::MyMessageBox(QString("Erro"), tr("图片加载成功"),tr("确定"),this);
#ifdef  Q_OS_LINUX
                sync();
#endif
            }

        }
        else
        {
            Utils::MyMessageBox(QString("Erro"), tr("没有找到图片"),tr("确定"),this);
        }
    }

}

void QWidgetShowParams_DiagnoseUtils::onLoad_Font()
{//字库文件拷贝
    if(QMessageBox::Retry == Utils::MyMessageBox(QString("Info"), tr("是否加载字库？"),tr("确定"),tr("取消"),this))
    {
        if(Lode_function("x_simsun.ttc")&&Lode_function("msyh.ttf"))
        {
            Utils::MyMessageBox(QString("Info"), tr("通用字库加载成功"),tr("确定"),this);
        }
        else
        {
            Utils::MyMessageBox(QString("Info"), tr("通用字库加载失败"),tr("确定"),this);
        }
        if(Lode_function("ADT_Font.ttf"))
        {
            Utils::MyMessageBox(QString("Info"), tr("加载字库 ADT_Font.ttf成功"),tr("确定"),this);
        }
        else
        {
            Utils::MyMessageBox(QString("Info"), tr("加载字库 ADT_Font.ttf失败"),tr("确定"),this);
        }
    }
}

bool QWidgetShowParams_DiagnoseUtils::Lode_function(QString font)
{
    if(QFile::exists("/media/usb0/ADT/"+font))
    {
        //加载前检查是否已有图片
        if(QFile::exists(GlobalData::FontPath+font))
        {
            QFile fileTemp(GlobalData::FontPath+font);
            fileTemp.remove();
        }

        if(QFile::copy("/media/usb0/ADT/"+font, GlobalData::FontPath+font))
        {

            const QString scriptFile = GlobalData::FontPath+"/font.sh";//sh脚本
            auto fullpath = GlobalData::FontPath+font;//返回文件存放路径
            QFile f(scriptFile);
            if(!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
                return false;
            QTextStream stream(&f);
            stream << "chmod a+x " << fullpath << endl;
            stream.flush();

            f.close();
            QProcess::startDetached("/bin/sh", QStringList() << scriptFile);//执行脚本命令
#ifdef  Q_OS_LINUX
            sync();
#endif
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

void QWidgetShowParams_DiagnoseUtils::onClosebtn()
{//图片文件拷贝  因为一般用单的图片有限此处 简单编写
    
    if(false == GlobalData::closebtn)
    {
        if(QMessageBox::Retry == Utils::MyMessageBox(QString("Info"), tr("是否关闭刹车按钮？"),tr("确定"),tr("取消"),this))
        {
#ifdef ARM
            GlobalData::parambite = SET_BIT(GlobalData::parambite,3);
            Utils::sendOffest(&GlobalData::parambite,ModbusAddress::ParamUse,sizeof(GlobalData::parambite),true);
            GlobalData::closebtn = true;
#endif
        }
    }
    else
    {
        if(QMessageBox::Retry == Utils::MyMessageBox(QString("Info"), tr("是否打开刹车按钮？"),tr("确定"),tr("取消"),this))
        {
#ifdef ARM
            GlobalData::parambite = CLR_BIT(GlobalData::parambite,3);
            Utils::sendOffest(&GlobalData::parambite,ModbusAddress::ParamUse,sizeof(GlobalData::parambite),true);
            GlobalData::closebtn = false;
#endif
        }
    }

}

void QWidgetShowParams_DiagnoseUtils::onClickUp()
{//更新上位机
    if(QMessageBox::Retry != Utils::MyMessageBox(QString("Info"),
                                                 tr("是否继续更新上位机程序？"),tr("是"),tr("否"),this))
    {
        return;
    }
#ifdef Q_OS_LINUX
    localDir->cd(QDir::rootPath());
    // 仅在发行release模式下打开"/mnt"
#ifndef QT_DEBUG
    localDir->cd("media/usb0");//设置U盘挂载路径
#endif
    localFileList = localDir->entryInfoList();
#else
    localFileList = QDir::drives();
#endif
    int i;
    for(i = 0; i < localFileList.size(); ++i)
    {
        auto fileInfo = localFileList.at(i);
        if(GlobalData::filenameProgramQt.toLower()==fileInfo.fileName().toLower())
        {
#ifdef Q_OS_LINUX

            auto fullpath = "/root/app/adtrom";//返回文件存放路径
            qDebug()<< GlobalData::filenameProgramQt.toLower()<<fileInfo.filePath();

            //加载前检查是否已有图片
            if(QFile::exists(fullpath))
            {
                QFile fileTemp(fullpath);
                fileTemp.remove();
            }
            /*********如果拷贝前已经有cpoy则不会重复拷贝**********/
            if(QFile::copy(fileInfo.filePath(),fullpath))
            {
                const QString scriptFile = "/tmp/upgrade.sh";
                QFile f(scriptFile);
                if(!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
                    return;
                QTextStream stream(&f);
                /*杀死进程 复制文件 赋予权限 重启*/
                //stream << "killall " << QFileInfo(fullpath).fileName() << endl;
                //stream << "cp " << fileInfo.filePath() << " " << fullpath << endl;
                stream << "chmod a+x " << fullpath << endl;
                stream << "reboot" << endl;
                stream.flush();

                f.close();
                auto element = new ModbusQueue::element_t;
                auto mixData = new Definitions::MixDataType;
                mixData->u16 = 1;//重启设置1
                element->data = &mixData->u16;//取地址
                element->len = sizeof(mixData->u16);
                element->startAddress = ModbusAddress::Reboot;
                element->operation = ModbusQueue::OP_WRITE_DATA;
                element->pointerObject = mixData;
                GlobalData::modbusQueue->sendQuery(element, false);

                QProcess::startDetached("/bin/sh", QStringList() << scriptFile);
    #ifdef  Q_OS_LINUX
                sync();
    #endif
            }
            else
            {
                Utils::MyMessageBox(QString("Error"), tr("升级触摸屏失败"),tr("确定"),tr("取消"),this);
            }

#else
            Utils::MyMessageBox(QString("Error"), tr("仅支持Linux操作系统升级"),tr("确定"),tr("取消"),this);
#endif
            return;
        }
        //qDebug()<< QString("%1").arg(i)<<fileInfo.fileName();
    }
    if(i==localFileList.size())
    {
        Utils::MyMessageBox(QString("Error"), tr("没有找到触摸屏更新程序请确认\n是否将其放入U盘."),tr("确定"),this);
    }

}
/*系统更新后可能参数不匹配造成通讯不上,
 * 如果先更新上位,则上下通讯问题导致无
 * 法更新下位机,只能通过FTP跟新,对客户
 * 来说,是完全操作不过来的!!!!*/
void QWidgetShowParams_DiagnoseUtils::onClickLower()
{
    if(true == GlobalData::modbusQueue->isOffline())
    {//通选失败只跟新上位机
        onClickUp();
        return;
    }
#ifdef Q_OS_LINUX
    localDir->cd(QDir::rootPath());
    // 仅在发行release模式下打开"/mnt"
#ifndef QT_DEBUG
    localDir->cd("media/usb0");//设置U盘挂载路径
#endif
    localFileList = localDir->entryInfoList();
#else
    localFileList = QDir::drives();
#endif
    int i = 0;
    for(i = 0;i < localFileList.size(); ++i)
    {
        auto fileInfo = localFileList.at(i);
        if(GlobalData::filenameProgramA9.toLower()==fileInfo.fileName().toLower())
        {
            //qDebug()<< GlobalData::filenameProgramA9.toLower()<<QString("%1").arg(i)<<fileInfo.filePath();
            filenameLocal= fileInfo.filePath();//本地
            strcpy(workThread->filenameRemote, "0:/ADT/A9Rom.bin");//远端
            QProgressDialog* d=new QProgressDialog(this, Qt::FramelessWindowHint);//进度条
            if(false == GlobalData::modbusQueue->isOffline())
            {
                d->setAttribute(Qt::WA_DeleteOnClose);
                d->setLabelText(tr("操作进行中...请稍侯"));
                d->setCancelButton(nullptr);
                d->setRange(0, 100);
                connect(this, SIGNAL(dismissProgressDialog()),
                        d, SLOT(accept()));
                //等待下位机完成更新
                connect(d, SIGNAL(accepted()),
                        this, SLOT(onClickUp()));
                if(!d)
                    return;
                connect(workThread, SIGNAL(updateFileProgess(int)),
                        d, SLOT(setValue(int)));
                d->setRange(0, 101); // 比100多一点...
                d->open();

                d->move ((QApplication::desktop()->width() - d->width())/2,(QApplication::desktop()->height() - d->height())/2-10);
                workThread->setFile(&filenameLocal,ToolThread::WS_UPGRADE_COPY_TO_REMOTE);
                workThread->start();
            }

            break;
        }

    }
    if(i==localFileList.size())
    {
        Utils::MyMessageBox(QString("Error"), tr("没有找到控制器更新程序\n开始更新触摸屏程序."),tr("确定"),this);
        //没找到下位机直接升级上位机
        onClickUp();
    }
}

void QWidgetShowParams_DiagnoseUtils::onUpMotion()
{
    if(true == GlobalData::modbusQueue->isOffline())
    {
        return;
    }
#ifdef Q_OS_LINUX
    localDir->cd(QDir::rootPath());
    // 仅在发行release模式下打开"/mnt"
#ifndef QT_DEBUG
    localDir->cd("media/usb0");//设置U盘挂载路径
#endif
    localFileList = localDir->entryInfoList();
#else
    localFileList = QDir::drives();
#endif
    int i = 0;
    for(i = 0;i < localFileList.size(); ++i)
    {
        auto fileInfo = localFileList.at(i);
        if(GlobalData::filenameProgramMotion.toLower()==fileInfo.fileName().toLower())
        {
            //qDebug()<< GlobalData::filenameProgramA9.toLower()<<QString("%1").arg(i)<<fileInfo.filePath();
            filenameLocal= fileInfo.filePath();//本地
            strcpy(workThread->filenameRemote, "0:/ADT/Motion.bin");//远端
            QProgressDialog* d=new QProgressDialog(this, Qt::FramelessWindowHint);//进度条
            if(false == GlobalData::modbusQueue->isOffline())
            {
                d->setAttribute(Qt::WA_DeleteOnClose);
                d->setLabelText(tr("操作进行中...请稍侯"));
                d->setCancelButton(nullptr);
                d->setRange(0, 100);
                connect(this, SIGNAL(dismissProgressDialog()),
                        d, SLOT(accept()));
                if(!d)
                    return;
                connect(workThread, SIGNAL(updateFileProgess(int)),
                        d, SLOT(setValue(int)));
                d->setRange(0, 101); // 比100多一点...
                d->open();

                d->move ((QApplication::desktop()->width() - d->width())/2,(QApplication::desktop()->height() - d->height())/2-10);
                workThread->setFile(&filenameLocal,ToolThread::WS_UPGRADE_COPY_TO_REMOTE_MOTIONLIB);
                workThread->start();
            }

            break;
        }
    }
    if(i==localFileList.size())
    {
        Utils::MyMessageBox(QString("Error"), tr("没有找到运动库，请检查U盘"),tr("确定"),this);
    }
}

void QWidgetShowParams_DiagnoseUtils::onReHost()
{//重启上位机
#ifdef __arm__
    QProcess::startDetached("reboot");
#endif
}

void QWidgetShowParams_DiagnoseUtils::onEncrypt()
{//加密狗管理密码框
    //qDebug()<<"软件加密";
    QDog::getInstance()->readDog();
#ifdef _MCGS

    if(GlobalData::online == false)
    {
        Utils::MyMessageBox(QString("Error"), tr("通信中断中,请先连接控制器!"),tr("确定"),this);
        return;
    }
#endif
    QDialogPassword *inputdlg = new QDialogPassword(this);
    connect(inputdlg, SIGNAL(accepted()), this, SLOT(onOpenEncrypt()));
    inputdlg->setAttribute(Qt::WA_DeleteOnClose);
    inputdlg->open();

}
void QWidgetShowParams_DiagnoseUtils::onOpenEncrypt()
{//加密狗加密框
    QWidgetDiagnose_DogEncrypt* d = new QWidgetDiagnose_DogEncrypt(this);
    d->setWindowTitle(tr("加密狗管理"));
    d->setAttribute(Qt::WA_DeleteOnClose);
    d->open();
}

void QWidgetShowParams_DiagnoseUtils::onDecrypt()
{//加密狗解密框
    //qDebug()<<"软件解密";
    QDog::getInstance()->readDog();
#ifdef _MCGS

    if(GlobalData::online == false)
    {
        Utils::MyMessageBox(QString("Error"), tr("通信中断中,请先连接控制器!"),tr("确定"),this);
        return;
    }
#endif
    QWidgetDiagnose_DogDecrypt* d = new QWidgetDiagnose_DogDecrypt(this);
    d->setWindowTitle(tr("加密狗管理"));
    d->setAttribute(Qt::WA_DeleteOnClose);
    d->open();
    connect(d, SIGNAL(success()),
            this, SLOT(decryptOk()));
    connect(d, SIGNAL(fail()),
            this, SLOT(decryptFail()));
    connect(this, SIGNAL(continu()),
            d, SLOT(accept()));
}

void QWidgetShowParams_DiagnoseUtils::decryptOk()
{
    //    qDebug()<<"dog ok";
    emit continu();
}

void QWidgetShowParams_DiagnoseUtils::decryptFail()
{

    QDog::getInstance()->readDog();
    if(0 == QDog::getInstance()->getDogProcessCode())
    {
        emit continu();
    }
    else
    {
        Utils::MyMessageBox(QString("Error"), tr("解密失败,请重新输入"),tr("确定"),this);
    }

}

QWidget* QWidgetShowParams_DiagnoseUtils::createFieldWidget(int index)
{
    QWidget* w = nullptr;

    switch(index)
    {
    case Item_Reboot:
    {
        auto pushButton = new QPushButtonParams<QDialogParams_WriteAnyThing, DataAdapter_DiagnoseUtils>(adapter, index, this);
        connect(pushButton, SIGNAL(wroteValueFromUI()),
                this, SLOT(onWroteGoodValue()));
        pushButton->setText(tr("执行"));
        w = pushButton;
        break;
    }
    case Item_Host:
    {
        auto pushButton = new QPushButtonParams<QDialogParams_WriteAnyThing, DataAdapter_DiagnoseUtils>(adapter, index, this);
        pushButton->setText(tr("执行"));
        connect(pushButton, SIGNAL(wroteValueFromUI()),
                this, SLOT(onReHost()));
        w = pushButton;
        break;
    }

    case Item_Ftp:
    {
        auto pushButton = new QPushButtonParams<QDialogParams_WriteAnyThing, DataAdapter_DiagnoseUtils>(adapter, index, this);
        connect(pushButton, SIGNAL(wroteValueFromUI()),
                this, SLOT(onWroteGoodValue()));
        pushButton->setText(tr("执行"));
        w = pushButton;
        break;
    }
        //    case Item_UpHost:
        //    {
        //        auto pushButton = new QPushButtonParams<QDialogParams_WriteAnyThing, DataAdapter_DiagnoseUtils>(adapter, index, this);
        //        connect(pushButton, SIGNAL(wroteValueFromUI()),
        //                this, SLOT(onClickUp()));
        //        w = pushButton;
        //        break;
        //    }
    case Item_UpLower:
    {
        auto pushButton = new QPushButtonParams<QDialogParams_WriteAnyThing, DataAdapter_DiagnoseUtils>(adapter, index, this);
        connect(pushButton, SIGNAL(wroteValueFromUI()),
                this, SLOT(onClickLower()));
        pushButton->setText(tr("执行"));
        w = pushButton;
        break;
    }
    case Item_Motion:
    {
        auto pushButton = new QPushButton(tr("执行"),this);
        connect(pushButton, SIGNAL(clicked()),
                this, SLOT(onUpMotion()));
        w = pushButton;
        break;
    }
    case Item_EncryptDog:
    {
        auto pushButton = new QPushButton(tr("执行"),this);
        connect(pushButton, SIGNAL(clicked()),
                this, SLOT(onEncrypt()));
        w = pushButton;
        break;
    }

    case Item_DecryptDog:
    {
        auto pushButton = new QPushButton(tr("执行"),this);
        connect(pushButton, SIGNAL(clicked()),
                this, SLOT(onDecrypt()));
        w = pushButton;
        break;
    }

    case Item_Screen:
    {
        auto pushButton = new QPushButton(tr("执行"),this);
        connect(pushButton, SIGNAL(clicked()),
                this, SLOT(onOpenCut()));
        w = pushButton;
        break;
    }

    case Item_Picture:
    {
        auto pushButton = new QPushButton(tr("执行"),this);
        connect(pushButton, SIGNAL(clicked()),
                this, SLOT(onLoad()));
        w = pushButton;
        break;
    }
    case Item_Font:
    {
        auto pushButton = new QPushButton(tr("执行"),this);
        connect(pushButton, SIGNAL(clicked()),
                this, SLOT(onLoad_Font()));
        w = pushButton;
        break;
    }

    case Item_Closebtn:
    {
        auto pushButton = new QPushButton(tr("执行"),this);
        connect(pushButton, SIGNAL(clicked()),
                this, SLOT(onClosebtn()));
        w = pushButton;
        break;
    }

    case Item_IoDisplay:
    {
        auto pushButton = new QPushButton(tr("执行"),this);
        connect(pushButton, SIGNAL(clicked()),
                this, SLOT(onIoDisplay()));
        w = pushButton;
        break;
    }

        //    case Item_PATTER1:
        //    {
        //        auto pushButton = new QPushButton(tr("执行"),this);
        //        connect(pushButton, SIGNAL(clicked()),
        //                this, SLOT(onPatter1()));
        //        w = pushButton;
        //        break;
        //    }
    case Item_PATTER2:
    {
        auto pushButton = new QPushButton(tr("执行"),this);
        connect(pushButton, SIGNAL(clicked()),
                this, SLOT(onPatter2()));
        w = pushButton;
        break;
    }
    case Item_PASSWOD:
    {
        auto pushButton = new QPushButton(tr("执行"),this);
        connect(pushButton, SIGNAL(clicked()),
                this, SLOT(onPassWord()));
        w = pushButton;
        break;
    }
    default:
        break;
    }
    w->setMinimumWidth(GlobalData::widthRate/4-60);
    w->setMaximumWidth(GlobalData::widthRate/4-60);
    w->setMinimumHeight(GlobalData::highRate/8-10);
    w->setMaximumHeight(GlobalData::highRate/8-10);
    return w;
}

bool QWidgetShowParams_DiagnoseUtils::getDataAuthority(int offset) const
{
    //    return GlobalData::DataAuthority(true);
    return true;
}


QString QWidgetShowParams_DiagnoseUtils::getParamName(int offset) const
{
    QString returnStr;

    switch(offset)
    {
    case Item_Reboot:

#ifdef __arm__
        returnStr = tr("重启控系统");//relese
#else
        //returnStr = "重启控制器";
        returnStr = tr("重启控制器");
#endif
        break;

    case Item_Host:
        returnStr = tr("重启触摸屏");//relese
        break;

    case Item_Ftp:
        //returnStr = "开启FTP连接";
        returnStr = tr("开启FTP连接");
        break;
        //    case Item_UpHost:
        ////        returnStr = tr("升级触摸屏");
        //        break;
    case Item_UpLower:
        returnStr = tr("系统升级");
        //        returnStr = tr("升级控制器");
        break;
    case Item_Motion:
        returnStr = tr("升级运动库");
        break;
    case Item_EncryptDog:
        returnStr = tr("软件加密");
        break;
    case Item_DecryptDog:
        returnStr = tr("软件解密");
        break;
    case Item_Screen:
        returnStr = tr("截屏工具");
        break;
    case Item_Picture:
        returnStr = tr("图片加载");
        break;
    case Item_Font:
        returnStr = tr("字库加载");
        break;

    case Item_Closebtn:
        returnStr = tr("刹车功能");
        break;
    case Item_IoDisplay:
        returnStr = tr("IO显示功能");
        break;

        //    case Item_PATTER1:
        //        returnStr = tr("显示样式1");
        //        break;
    case Item_PATTER2:
        returnStr = tr("显示样式");
        break;
    case Item_PASSWOD:
        returnStr = tr("参数密码");
        break;
    default:
        break;
    }

    return returnStr;
}

ModbusQueue::element_t* QWidgetShowParams_DiagnoseUtils::createModbusElement(int offset) const
{
    ModbusQueue::element_t* element = nullptr;

    switch(offset)
    {
    case Item_Reboot:
    {
        element = new ModbusQueue::element_t;
        auto mixData = new Definitions::MixDataType;
        mixData->u16 = 1;//重启设置1
        element->data = &mixData->u16;//取地址
        element->len = sizeof(mixData->u16);
        element->startAddress = ModbusAddress::Reboot;
        element->operation = ModbusQueue::OP_WRITE_DATA;
        element->pointerObject = mixData;
        break;
    }

    case Item_Ftp:
    {
        element = new ModbusQueue::element_t;
        auto mixData = new Definitions::MixDataType;
        mixData->u16 = 1;
        element->data = &mixData->u16;
        element->len = sizeof(mixData->u16);
        element->startAddress = ModbusAddress::FtpConnect;
        element->operation = ModbusQueue::OP_WRITE_DATA;
        element->pointerObject = mixData;
        break;
    }

    default:
        break;
    }

    return element;
}

void QWidgetShowParams_DiagnoseUtils::onSendModbusOK(ModbusQueue::element_t* element)
{
    auto mixData = reinterpret_cast<Definitions::MixDataType*>(element->pointerObject);
    delete mixData;

    // 重启上位机
#ifdef __arm__
    if(ModbusAddress::Reboot == element->startAddress){
        QProcess::startDetached("reboot");
    }
#endif


}

void QWidgetShowParams_DiagnoseUtils::onSendModbusFail(ModbusQueue::element_t* element)
{
    auto mixData = reinterpret_cast<Definitions::MixDataType*>(element->pointerObject);
    delete mixData;

    // 重启上位机
#ifdef __arm__
    if(ModbusAddress::Reboot == element->startAddress){
        QProcess::startDetached("reboot");
    }
#endif
    //QMessageBox::information(this, "OK", tr("重启控制器失败"));
}


ToolThread::ToolThread(QObject* parent): QThread(parent),
    workstep(WS_DONE),
    isStopped(false)

{

}
void ToolThread::setFile(QString* local,int type)
{
    this->filenameLocal = local;
    this->workstep = type;

}
void ToolThread::_onNotifyFileProgress(int progress)
{
    emit updateFileProgess(progress);
}
void ToolThread::run()
{//升级下位机
    bool isOk = false;
    ModbusQueue::element_t element;
    //    qDebug() << "workstep"<<workstep;

    while(workstep != WS_DONE && false == isStopped)
    {

        switch(workstep)
        {
        case WS_UPGRADE_COPY_TO_REMOTE:
        case WS_UPGRADE_COPY_TO_REMOTE_MOTIONLIB:
        {//发送更新程序
            element.operation =  ModbusQueue::OP_WRITE_FILE;
            element.mixParam.file.local = filenameLocal;
            element.mixParam.file.remote = filenameRemote;
            connect(GlobalData::modbusQueue, SIGNAL(notifyFileProgress(int)),
                    this, SLOT(_onNotifyFileProgress(int)));

            isOk = GlobalData::modbusQueue->sendQuery(&element, false);
            //isOk = true;
            disconnect(GlobalData::modbusQueue, SIGNAL(notifyFileProgress(int)),
                       this, SLOT(_onNotifyFileProgress(int)));
            if(true == isOk)
            {
                if(workstep == WS_UPGRADE_COPY_TO_REMOTE)
                    workstep = WS_UPGRADE_START;
                else if(workstep == WS_UPGRADE_COPY_TO_REMOTE_MOTIONLIB)
                    workstep = WS_UPGRADE_MOTIONLIB_START;
                else
                    workstep = WS_DONE;
            }
            break;
        }
        case WS_UPGRADE_START:
        {//开始更新
            quint16 data = 1;
            element.data = &data;
            element.len = sizeof(data);
            element.operation = ModbusQueue::OP_WRITE_DATA;
            element.startAddress = ModbusAddress::Upgrade;
            isOk = GlobalData::modbusQueue->sendQuery(&element, false);

            if(true == isOk)
            {
                workstep = WS_UPGRADE_CHECK_IF_FINISH;
            }

            break;
        }
        case WS_UPGRADE_CHECK_IF_FINISH:
        {//检查更新结果
            qint16 data = -100;
            element.data = &data;
            element.len = sizeof(data);
            element.operation = ModbusQueue::OP_READ_DATA;
            element.startAddress = ModbusAddress::Upgrade;
            isOk = GlobalData::modbusQueue->sendQuery(&element, false);

            if(true == isOk && 0 == data)
            {
                workstep = WS_DONE;
            }

            break;
        }
        case WS_UPGRADE_MOTIONLIB_START:
        {//更新MOTION
            quint16 data = 1;
            element.data = &data;
            element.len = sizeof(data);
            element.operation = ModbusQueue::OP_WRITE_DATA;
            element.startAddress = ModbusAddress::Motion;
            isOk = GlobalData::modbusQueue->sendQuery(&element, false);

            if(true == isOk)
            {
                workstep = WS_DONE;
            }

            break;
        }
        default:
            break;
        }

        if(false == isOk)
            break; // while loop

        Utils::sleep(10);
    }
    emit operationDone(isOk);

}
