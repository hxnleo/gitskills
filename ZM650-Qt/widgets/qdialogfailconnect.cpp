#include "qdialogfailconnect.h"
#include "data/globaldata.h"
#include "data/uistyles.h"
#include "data/utils.h"
#include<QPainter>
#include<QDebug>
#include <unistd.h>
/*
 * qdialogfailconnect.cpp
 *
 * 通信失败界面框，用于更新系统程序
 *
 * Created on: 2020年 3月 17日 星期一 17:02:40 CST
 * Author: caojing
 */
QDialogFailConnect::QDialogFailConnect(QWidget* parent,Qt::WindowFlags f):
    QDialog(parent, f),localDir(new QDir()),workThread(new ToolThread(this))
{
    this->setWindowFlags(Qt::CustomizeWindowHint);
    Utils::setFullScreen(this);
    connect(workThread, SIGNAL(operationDone(bool)),
            this, SLOT(onWorkerThreadOperationDone(bool)));

    QPushButton *btn_updatadown = new QPushButton(tr("更新控制器"));
    QPushButton *btn_updataup = new QPushButton(tr("更新显示器"));
    QPushButton *btn_reboot = new QPushButton(tr("重启控制器"));
    QPushButton *btn_loadfont = new QPushButton(tr("更新字库"));
    QPushButton *btn_closedialog = new QPushButton(tr("退出系统"));
    QPushButton *btn_sysclose = new QPushButton(tr("进入主界面"));
    btn_updatadown->setFixedWidth(width()/3);
    btn_updataup->setFixedWidth(width()/3);
    btn_reboot->setFixedWidth(width()/3);
    btn_closedialog->setFixedWidth(width()/3);
    btn_sysclose->setFixedWidth(width()/3);
    btn_loadfont->setFixedWidth(width()/3);

    btn_updatadown->setStyleSheet(UIStyles::css_maincolorconf_u8);
    btn_updataup->setStyleSheet(UIStyles::css_maincolorconf_u8);
    btn_reboot->setStyleSheet(UIStyles::css_maincolorconf_u8);
    btn_closedialog->setStyleSheet(UIStyles::css_maincolorconf_u8);
    btn_sysclose->setStyleSheet(UIStyles::css_maincolorconf_u8);
    btn_loadfont->setStyleSheet(UIStyles::css_maincolorconf_u8);

    connect(btn_updatadown, SIGNAL(clicked()), this, SLOT(onUpdataDown()));
    connect(btn_updataup, SIGNAL(clicked()), this, SLOT(onUpdataUp()));
    connect(btn_reboot, SIGNAL(clicked()), this, SLOT(onReboot()));
    connect(btn_loadfont, SIGNAL(clicked()), this, SLOT(onLoad_Font()));
    connect(btn_closedialog, SIGNAL(clicked()), this, SLOT(close()));
    connect(btn_sysclose, SIGNAL(clicked()), this, SLOT(reject()));

    QHBoxLayout *HLayout = new QHBoxLayout(this);
    QVBoxLayout *VLayout = new QVBoxLayout();
    VLayout->addStretch();
    VLayout->addWidget(btn_updatadown);
    VLayout->addWidget(btn_updataup);
    VLayout->addWidget(btn_reboot);
    VLayout->addWidget(btn_loadfont);
    VLayout->addWidget(btn_closedialog);
    VLayout->addWidget(btn_sysclose);
    VLayout->addStretch();
    HLayout->addStretch();
    HLayout->addLayout(VLayout);
    HLayout->addStretch();

}

void QDialogFailConnect::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter p(this);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(176,196,222));
    p.drawRect(rect());
}

bool QDialogFailConnect::Lode_function(QString font)
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

void QDialogFailConnect::onLoad_Font()
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

void QDialogFailConnect::onUpdataDown()
{//在通讯失败的情况下进行数据传递更新下位机
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
            //if(false == GlobalData::modbusQueue->isOffline())
            //{
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
            d->move((QApplication::desktop()->width() - d->width())/2,(QApplication::desktop()->height() - d->height())/2-10);
            d->open();

            workThread->setFile(&filenameLocal,ToolThread::WS_UPGRADE_COPY_TO_REMOTE);
            workThread->start();
            //}

            break;
        }
    }
    if(i==localFileList.size())
    {
        Utils::MyMessageBox(QString("Error Range"), tr("没有找到控制器程序."),QObject::tr("确定"),this);
    }
}

void QDialogFailConnect::onUpdataUp()
{//更新上位机
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
            //qDebug()<< GlobalData::filenameProgramQt.toLower()<<QString("%1").arg(i)<<fileInfo.filePath();
            const QString scriptFile = "/tmp/upgrade.sh";
            //auto fullpath = QApplication::applicationFilePath();//返回可执行文件路径
            auto fullpath = "/root/app/adtrom";//返回文件存放路径
            QFile f(scriptFile);

            if(!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
                return;

            QTextStream stream(&f);
            /*杀死进程 复制文件 赋予权限 重启*/
            stream << "killall " << QFileInfo(fullpath).fileName() << endl;
            stream << "cp " << fileInfo.filePath() << " " << fullpath << endl;
            stream << "chmod a+x " << fullpath << endl;
            stream << "reboot" << endl;

            stream.flush();

            f.close();

            QProcess::startDetached("/bin/sh", QStringList() << scriptFile);

            ModbusQueue::element_t* element = new ModbusQueue::element_t;
            auto mixData = new Definitions::MixDataType;
            mixData->u16 = 1;//重启设置系统
            element->data = &mixData->u16;//取地址
            element->len = sizeof(mixData->u16);
            element->startAddress = ModbusAddress::Reboot;
            element->operation = ModbusQueue::OP_WRITE_DATA;
            GlobalData::modbusQueue->sendQuery(element,false);
#else
            Utils::MyMessageBox(QString("Error Range"), tr("仅支持Linux操作系统升级"),QObject::tr("确定"),this);
#endif
            return;
        }
        //qDebug()<< QString("%1").arg(i)<<fileInfo.fileName();
    }
    if(i==localFileList.size())
    {
        Utils::MyMessageBox(QString("Error Range"), tr("没有找到触摸屏更新程序\n请确认是否将其放入U盘."),QObject::tr("确定"),this);
    }
}

void QDialogFailConnect::onReboot()
{//发送重启命令
    ModbusQueue::element_t* element = new ModbusQueue::element_t;
    auto mixData = new Definitions::MixDataType;
    mixData->u16 = 1;//重启设置1
    element->data = &mixData->u16;//取地址
    element->len = sizeof(mixData->u16);
    element->startAddress = ModbusAddress::Reboot;
    element->operation = ModbusQueue::OP_WRITE_DATA;
    GlobalData::modbusQueue->sendQuery(element,false);

    // 重启上位机
#ifdef __arm__
    if(ModbusAddress::Reboot == element->startAddress){
        QProcess::startDetached("reboot");
    }
#endif

}

void QDialogFailConnect::onWorkerThreadOperationDone(bool isOk)
{//回发消息关闭进步条

    if(isOk)
    {
        Utils::MyMessageBox(QString("OK"), tr("升级控制器成功\n请重启控制器"),QObject::tr("确定"));
    }
    else
    {
        //QMessageBox::warning(this, "Error", QString::fromUtf8("升级控制器失败"));
        Utils::MyMessageBox(QString("Error"), tr("升级控制器失败"),QObject::tr("确定"));
    }
    emit dismissProgressDialog();
}

void QDialogFailConnect::close()
{
    QApplication* app;
    app->quit();
}

void QDialogFailConnect::accept()
{
    QDialog::accept();
}

void QDialogFailConnect::reject()
{
    QDialog::reject();
}

