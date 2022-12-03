/*
 * subwindowfilemanage.cpp
 *
 * 文件管理界面
 *
 * Created on: 2019年 02月 13日 星期三 18:26:02 CST
 * Author: lixingcong
 */
#include "subwindowfilemanage.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "widgets/qlistwidgetdragscroll.h"
#include <QListWidgetItem>
#include <QApplication>
#include "widgets/qpushbuttonspage.h"
#include "widgets/qlabelmarquee.h"
#include "widgets/qdialogrenew.h"
#include "data/globaldata.h"
#include "data/adapters.h"
#include <QListWidgetItem>
#include "data/utils.h"
#include "data/uistyles.h"
#include "modbus/modbusaddress.h"
#include "modbus/modbusqueue.h"
#include <QMessageBox>
#include "widgets/qdialogdataform.h"
#include <QDir>
#include <QFile>
#include <QDialogButtonBox>
#include <QDateTime>
#include <QProgressDialog>
#include "data/workfile.h"
#include <QProcess>
#include <QTimer>
#include <unistd.h>
#include "public.h"

#define ITEM_DATAROLE (Qt::UserRole+1)
#define NEWFILE_MAXLEN 16
#define FONTSIZE 10 //字体大小

//const QStringList SubWindowFileManage::rootPathString =
//QStringList SubWindowFileManage::rootPathString =
//{
//    //QString::fromUtf8("系统盘"),
//    //QString::fromUtf8("用户盘"),
//    //QString::fromUtf8("触摸屏U盘")

//    QObject::trUtf8("系统盘"),
//    QObject::trUtf8("用户盘"),
//    QObject::trUtf8("触摸屏U盘")

//};


SubWindowFileManage::SubWindowFileManage(QWidget* parent) : QWidget(parent),
    layout_v(new QVBoxLayout(this)),
    labelCurrentDir(new QLabel(this)),
    labelFileInfo(new QLabel(this)),
    labelFilename(new QLabelMarquee(this)),
    listWidget(new QListWidgetDragScroll(this)),
    workerThread(new QThreadFileManage(&listFileInfo, this)),
    currentAction(Action_None),
    currentRootPath(Root_None),
    timerSingleClick(new QTimer(this)),
    isSingleClick(false),
    pathDepth(0),
    localDir(new QDir),
    strlast(QString("")),
    in_paste(false)
{
    this->setContentsMargins(1,1,1,1);//设置边距
    SubWindowFileManage::rootPathString =
    {
        //    QString::fromUtf8("系统盘"),
        //    QString::fromUtf8("用户盘"),
        //    QString::fromUtf8("触摸屏U盘")

        QObject::tr("系统盘"),
        QObject::tr("用户盘"),
        QObject::tr("触摸屏U盘")

    };
    // QDir
    localDir->setFilter(QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::AllEntries);

    // Qtimer
    timerSingleClick->setSingleShot(true);
    timerSingleClick->setInterval(300); // 双击间隔
    connect(timerSingleClick, SIGNAL(timeout()),
            this, SLOT(onTimeoutSingleClicked()));

    // 顶栏
    labelCurrentDir->setText(tr("dirname"));
    auto buttonRefresh = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_BrowserReload), tr("刷新"), this);
    buttonRefresh->setFixedSize(QSize(100, 30));

    auto buttonHome = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_DesktopIcon), tr("主页"), this);
    buttonHome->setFixedSize(QSize(100, 30));

    auto buttonLevelUp = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_FileDialogToParent), tr("向上"),
                                         this);
    buttonLevelUp->setFixedSize(QSize(100, 30));

    auto layoutTop = new QHBoxLayout;
    layoutTop->addWidget(labelCurrentDir);
    layoutTop->addWidget(buttonLevelUp);
    layoutTop->addWidget(buttonHome);
    layoutTop->addWidget(buttonRefresh);
    layout_v->addItem(layoutTop);

    connect(buttonRefresh, SIGNAL(clicked()),
            this, SLOT(onClickedRefreshDir()));
    connect(buttonLevelUp, SIGNAL(clicked()),
            this, SLOT(levelUp()));
    connect(buttonHome, SIGNAL(clicked()),
            this, SLOT(showRootPath()));

    // 文件列表
    listWidget->setViewMode(QListView::IconMode);
    listWidget->setIconSize(QSize(64, 64));
    listWidget->setGridSize(QSize(150, 100));//设置布局大小
    listWidget->setMovement(QListView::Static);
    listWidget->setFlow(QListView::LeftToRight);
    listWidget->setResizeMode(QListView::Adjust);
    listWidget->setTextElideMode(Qt::ElideMiddle);

    layout_v->addWidget(listWidget);

    // 默认导航
    //showRootPath();
    startFileIntoUser();//直接进入用程序户盘

    //单击进入双击
    connect(listWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(onItemClicked(QListWidgetItem*)));
    // 由于双击效果在触摸屏上，并不理想，改用QTimer定时器 lixingcong@20190215
#if 0
    connect(listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(onItemDoubleClicked(QListWidgetItem*)));
#endif

    // 底部信息栏
    labelFileInfo->setText(tr("文件信息:"));
    labelFilename->setText(tr("文件名:"));
    labelFilename->setFixedWidth(250);

    auto layoutBottom = new QHBoxLayout;
    UIStyles::setMarginAndSpacing(layoutBottom);
    layoutBottom->addWidget(labelFilename);
    layoutBottom->addWidget(labelFileInfo);
    layout_v->addItem(layoutBottom);

    // bottom btns
    QList<QPushButtonsPage::item_t> listButtonItems =
    {
        {tr("打开"), Definitions::BTN_FILE_OPEN},
        {tr("新建"), Definitions::BTN_FILE_NEW},
        {tr("复制"), Definitions::BTN_FILE_COPY},
        {tr("粘贴"), Definitions::BTN_FILE_PASTE},
        {tr("删除"), Definitions::BTN_FILE_DELETE},
        {tr("生成CSV"), Definitions::BTN_FLIE_GenerateCSV},
        {tr("解析CSV"), Definitions::BTN_FLIE_AnalyzeCSV},
    #ifdef Arm_lansi
        {tr("解析"), Definitions::BTN_FLIE_CHANG},
        {tr("生成"), Definitions::BTN_FLIE_GENERATE},
    #endif
        {tr("拷入Ｕ盘"), Definitions::BTN_FILE_COPYALL},
        {tr("Ｕ盘拷入"), Definitions::BTN_UDisk_COPYALL},
        {tr("新文件夹"), Definitions::BTN_FILE_NEWFOLDER}
    };

    GlobalData::pushButtonPage->init(listButtonItems);
    connect(GlobalData::pushButtonPage, SIGNAL(buttonClicked(int)),
            this, SLOT(_onClickedBottomButton(int)));

    // workerthread
    connect(workerThread, SIGNAL(operationDone(bool)),
            this, SLOT(onWorkerThreadOperationDone(bool)));

    copySource.isValid = false;

    // scroll
    connect(this, SIGNAL(resetScrollPostion()),
            listWidget, SLOT(onResetScrollPostion()));

}

SubWindowFileManage::~SubWindowFileManage()
{
    workerThread->setStopFlag();
    Utils::waitForThreadFinished(workerThread);

    delete localDir;
    debug_printf("~SubWindowFileManage()");
}

//解析CSV到教导数据
void SubWindowFileManage::AnalyzeCSV()
{
    int sec=0;
    qDebug()<<"int AnalyzeCSV";
    disconnect(this, SIGNAL(dismissProgressDialog()),
               this, SLOT(AnalyzeCSV()));

    QFile  file(filenameLocal);
    while((file.size()!=bytes_temp))
    {
        localFileList = localDir->entryInfoList();
        sec+=1;
        if(!Utils::isDirExist(QDir::rootPath()+"media/usb0"))
        {
            Utils::MyMessageBox(QString("ERR"), tr("没有检测到Ｕ盘"),tr("确定"),this);
            return;
        }
        if(sec>1000||localFileList.size()==0)
        {
            Utils::MyMessageBox(QString("ERR"), tr("数据解析错误"),tr("确定"),this);
            return;
        }
        Utils::sleep(10);
        qDebug()<<file.size()<<bytes_temp<<"sec="<<(float)sec/10<<"s";
    }

#ifdef Q_OS_LINUX
    sync();
#endif
    //开始数据反解析
    if(WriteCSVFile(filenameLocal,orignfile))
    {
        qDebug()<<"Del Flie"<< QFile::remove(filenameLocal);
#ifdef Q_OS_LINUX
        sync();
#endif
        labelFilename->setText(orignfile);
        labelCurrentDir->setText(QCoreApplication::applicationDirPath());
        onClickedRefreshDir();
    }
    else
    {
        Utils::MyMessageBox(QString("ERR"), tr("数据解析错误，请重新生成"),tr("确定"),this);
    }
}

void SubWindowFileManage::onOK()
{
    char*  ch;
    QByteArray ba = (GlobalData::renewEdit+strlast).toLatin1();
    ch=ba.data();
    strcpy(filenamePaste, ch);//需要粘贴的文件名
    if(currentAction != Action_PasteLocalToLocal)
    {
        workerThread->setPasteRemoteToRemote(filenamePaste);
        workerThread->start();
        in_paste=false;//复制粘贴标志位
    }
    else
    {
        filenameLocal = localDir->path() + QDir::separator() + filenamePaste;
        workerThread->setPasteLocalToLocal(&copySource.fullpath, &filenameLocal);
        workerThread->start();
        in_paste=false;
    }
    //qDebug()<<"filenamePaste"<<"完成"<<filenamePaste;
}
void SubWindowFileManage::onNot()
{
    qDebug()<<"noNot";
    in_paste=false;//复制粘贴标志位
}
void SubWindowFileManage::onWaitFileFinish()
{
    while (1)
    {//等待进入文件夹文件

        if(workerThread->sendok)
        {
            break;
        }
        Utils::sleep1(1);
    }
    workerThread->sendok = false;
}

void SubWindowFileManage::_onClickedBottomButton(int btn_id)
{//按下下面的按钮
    if(in_paste==true)
    {//处于复制粘贴过程
        if(btn_id!=Definitions::BTN_FILE_PASTE&&btn_id!=Definitions::BTN_FILE_COPY&&btn_id!=Definitions::BTN_FILE_OPEN)
        {
            if(QMessageBox::Retry == Utils::MyMessageBox(QString("Info"), tr("是否取消复制操作？"),tr("确定"),this))
            {
                in_paste = false;
            }
            else
                return;
        }
    }
    switch(btn_id)
    {
    case Definitions::BTN_FILE_NEW:
    {
        if(currentRootPath != Root_UserProg)
        {
            //QMessageBox::information(this, QString::fromUtf8("新建"), QString("仅允许在用户盘新建文件"));
            Utils::MyMessageBox(QString("Erro"), tr("仅允许在用户盘新建文件"),tr("确定"),this);
            return;
        }

        filenameTemp = QString();
        QList<QDialogDataForm::item_t> items =
        {
            //{QString::fromUtf8("新文件名"), Adapters::TYPE_String, &filenameTemp, false, 0, 0, nullptr},
            {tr("新文件名"), Adapters::TYPE_String, &filenameTemp, false, 0, 0, nullptr},

        };

        auto w = new QDialogDataForm(this, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        connect(w, SIGNAL(accepted()), this, SLOT(onDialogAcceptedNewFile()));
        //        w->showDialog(QString::fromUtf8("输入新文件名字，最大长度%1字符，无需输入后缀名").arg(
        //                          NEWFILE_MAXLEN), items, true);
        w->showDialog(tr("输入新文件名字，最大长度%1字符，无需输入后缀名").arg(
                          NEWFILE_MAXLEN), items, true);
        break;
    }

    case Definitions::BTN_FILE_DELETE:
    {
        auto itemSelected = listWidget->currentItem();

        if(nullptr == itemSelected)
            return;

        auto itemData = itemSelected->data(ITEM_DATAROLE).toInt();

        if(Root_System == currentRootPath || Root_UserProg == currentRootPath)
        {
            auto dirInfo = listFileInfo.at(itemData);
            auto fileNameToBeDeleted = Utils::gbkCharToQString(dirInfo.file_name);

            if(isARemoteFile(dirInfo))
            {
                //                if(Utils::gbkCharToQString(dirname) == "0:/PROG")
                {//只有 PROG目录才可以删除
                    if(fileNameToBeDeleted == GlobalData::workfileName)
                    {
                        {
                            Utils::MyMessageBox(QString("Erro"), tr("不能删除当前加工文件") + fileNameToBeDeleted,tr("确定"),this);
                            return;
                        }
                    }
                    if(QMessageBox::Retry == Utils::MyMessageBox(QString("Info"), tr("确定删除%1吗？")
                                                                 .arg(fileNameToBeDeleted),tr("确定"),tr("取消"),this))
                    {
                        Utils::qStringToGbkChar(fileNameToBeDeleted, filenameNew, sizeof(filenameNew));
                        currentAction = Action_Delete;
                        workerThread->setDelete(filenameNew);
                        workerThread->start();
                    }
                }
                //                else
                //                {
                //                    Utils::MyMessageBox(QString("Erro"), tr("不能删除加工文件外的文件") + fileNameToBeDeleted,tr("确定"),this);
                //                    return;
                //                }
            }
        }
        else if(Root_HMI_UDisk == currentRootPath)
        {
            auto fileInfo = localFileList.at(itemData);

            if(fileInfo.isFile())
            {
                if(QMessageBox::Retry == Utils::MyMessageBox(QString("Info"), tr("确定删除%1吗？")
                                                             .arg(fileInfo.fileName()),tr("确定"),tr("取消"),this))
                {
                    QFile file(fileInfo.filePath());
                    file.remove();
#ifdef  Q_OS_LINUX
                    sync();
#endif
                    localDir->refresh();
                    localFileList = localDir->entryInfoList();
                    refreshLocalDir();
                }
            }
        }
        break;
    }

    case Definitions::BTN_FLIE_GenerateCSV:
    {//教导数据生成CSV数据
        if(0 == pathDepth)
            return;

        qDebug()<<"成CSV数据"<<pathDepth;
        if(!Utils::isDirExist(QDir::rootPath()+"media/usb0"))
        {
            Utils::MyMessageBox(QString("Error"), tr("未检测到Ｕ盘"),tr("确定"),this);
            break;
        }
        //qDebug()<<"成CSV数据1";

        auto itemSelected = listWidget->currentItem();
       // qDebug()<<itemSelected;
        if(nullptr == itemSelected)
        {
            copySource.isValid = false;
            return;
        }
        copySource.isValid = false;
        copySource.root = currentRootPath;

        auto itemData = itemSelected->data(ITEM_DATAROLE).toInt();

        //qDebug()<<currentRootPath;
        if(Root_UserProg == currentRootPath)
        {
            auto dirInfo = listFileInfo.at(itemData);

            if(false == isARemoteFile(dirInfo))
                return;
            strcpy(filenamePaste, dirInfo.file_name);//需要粘贴的文件名
            currentAction = Action_Copy;
            currentRootPath = Root_HMI_UDisk;
            workerThread->setCopy(filenamePaste);
            workerThread->start();

            bytes_temp = dirInfo.file_type_size & (0x7fffffff);

            copySource.filename = itemSelected->text();
            copySource.fullpath = Utils::gbkCharToQString(dirname) + QString("/") + itemSelected->text();
            while(workerThread->workstep != QThreadFileManage::WS_DONE) Utils::sleep(1);
            if(copySource.filename.endsWith(".PHM"))
            {
                /*********检查csv文件是否存在*********/
                localDir->cd(QDir::currentPath());
                // 仅在发行release模式下打开"/mnt"
#ifndef QT_DEBUG
                localDir->cd(QDir::rootPath());
                localDir->cd("media/usb0");//设置U盘挂载路径
#endif
                orignfile = localDir->path() + QDir::separator()+copySource.filename.split(".").first() +".csv";
                //覆盖文本
                if((QFile::exists(orignfile)))
                {
                    qDebug()<<"remove exit Flie"<< QFile::remove(orignfile);
#ifdef Q_OS_LINUX
                    sync();
#endif
                }
                /************************************/

                /************把文件复制到U盘***********/

                Utils::qStringToGbkChar(copySource.fullpath, filenameRemote, sizeof(filenameRemote));
                filenameLocal = localDir->path() + QDir::separator() +"9999.PHM";
                qDebug()<<"filenameLocal"<<filenameLocal;
                auto d = createLoadingDialog(tr("数据拷贝"));
                if(!d)
                    return;
                connect(workerThread, SIGNAL(updateFileProgess(int)),
                        d, SLOT(setValue(int)));
                connect(this, SIGNAL(dismissProgressDialog()),
                        this, SLOT(AnalyzeCSV()));
                d->open();

                currentRootPath = Root_HMI_UDisk;
                currentAction = Action_PasteRemoteToLocal;
                workerThread->setPasteRemoteToLocal(filenameRemote, &filenameLocal);
                workerThread->start();
                /*************等待文件复制完成************/
                //                while(workerThread->workstep != QThreadFileManage::WS_DONE) Utils::sleep(1);

            }
            else
                Utils::MyMessageBox(QString("Info"), tr("数据格式错误"),tr("确定"),this);
        }
        else
            Utils::MyMessageBox(QString("Info"), tr("只能数据区操作"),tr("确定"),this);

        break;
    }
    case Definitions::BTN_FLIE_AnalyzeCSV:
    {//CSV解析成教导数据
        auto itemSelected = listWidget->currentItem();
        if(nullptr == itemSelected)
        {
            copySource.isValid = false;
            return;
        }
        copySource.isValid = false;
        copySource.root = currentRootPath;

        auto itemData = itemSelected->data(ITEM_DATAROLE).toInt();

        if(Root_HMI_UDisk == currentRootPath)
        {
//            QCoreApplication::applicationDirPath();
            auto fileInfo = localFileList.at(itemData);
            if(false == fileInfo.isFile())
            {
                qDebug()<<"fileInfo ERR";
                return;
            }

            copySource.filename = itemSelected->text();
            copySource.isValid = true;
            if(QMessageBox::Cancel == Utils::MyMessageBox(QString("Warning"), tr("是否解析文件数据")+itemSelected->text(),tr("确定"),tr("取消"),this))
            {
                return;
            }

            if(fileInfo.filePath().endsWith(".csv"))
            {
                if(ReadCSVFile(fileInfo.filePath()))
                {
                    Utils::MyMessageBox(QString("Info"), tr("文件解析成功,开始拷贝数据"),tr("确定"),this);
                    copySource.fullpath = fileInfo.filePath().split(".").first()+".PHM";
                    auto remoteStr = "0:/PROG/"+copySource.filename.split(".").first()+".PHM";

                    Utils::qStringToGbkChar(remoteStr, filenameRemote, sizeof(filenameRemote));
                    auto d = createLoadingDialog();
                    if(!d)
                        return;
                    connect(workerThread, SIGNAL(updateFileProgess(int)),
                            d, SLOT(setValue(int)));
                    connect(this, SIGNAL(dismissProgressDialog()),
                            this, SLOT(DelLanSiFile()));
                    d->open();
                    currentRootPath = Root_UserProg;
                    currentAction = Action_PasteLocalToRemote;
                    workerThread->setPasteLocalToRemote(filenameRemote, &copySource.fullpath);
                    workerThread->start();
                }
                else
                    Utils::MyMessageBox(QString("Info"), tr("数据解析错误"),tr("确定"),this);
            }
            else
            {
                Utils::MyMessageBox(QString("Info"), tr("文件格式错误"),tr("确定"),this);
            }

        }
        else
        {
            Utils::MyMessageBox(QString("Info"), tr("需要在U盘下面进行"),tr("确定"),this);
        }
        break;
    }

        //蓝思数数据解析
    case Definitions::BTN_FLIE_CHANG:
    {
        if(0 == pathDepth)
            return;
        auto itemSelected = listWidget->currentItem();
        if(nullptr == itemSelected)
        {
            copySource.isValid = false;
            return;
        }
        copySource.isValid = false;
        copySource.root = currentRootPath;

        auto itemData = itemSelected->data(ITEM_DATAROLE).toInt();

        if(Root_HMI_UDisk == currentRootPath)
        {
            auto fileInfo = localFileList.at(itemData);

            if(false == fileInfo.isFile())
                return;

            copySource.filename = itemSelected->text();
            copySource.isValid = true;
            if(QMessageBox::Cancel == Utils::MyMessageBox(QString("Warning"), tr("是否解析文件数据")+itemSelected->text(),tr("确定"),tr("取消"),this))
            {
                return;
            }
            if(fileInfo.filePath().endsWith(".BIN"))
            {
                if(ReadLanSiFile(fileInfo.filePath()))
                {
                    Utils::MyMessageBox(QString("Info"), tr("文件解析成功,开始拷贝数据"),tr("确定"),this);
                    copySource.fullpath = fileInfo.filePath().split(".").first()+".PHM";
                    auto remoteStr = "0:/PROG/"+copySource.filename.split(".").first()+".PHM";

                    Utils::qStringToGbkChar(remoteStr, filenameRemote, sizeof(filenameRemote));
                    auto d = createLoadingDialog();
                    if(!d)
                        return;
                    connect(workerThread, SIGNAL(updateFileProgess(int)),
                            d, SLOT(setValue(int)));
                    connect(this, SIGNAL(dismissProgressDialog()),
                            this, SLOT(DelLanSiFile()));
                    d->open();
                    currentRootPath = Root_UserProg;
                    currentAction = Action_PasteLocalToRemote;
                    workerThread->setPasteLocalToRemote(filenameRemote, &copySource.fullpath);
                    workerThread->start();
                }
                else
                    Utils::MyMessageBox(QString("Info"), tr("数据解析错误"),tr("确定"),this);
            }
            else
            {
                Utils::MyMessageBox(QString("Info"), tr("文件格式错误"),tr("确定"),this);
            }

        }
        break;
    }

        //生成蓝思数据
    case Definitions::BTN_FLIE_GENERATE:
    {
        if(0 == pathDepth)
            return;
        auto itemSelected = listWidget->currentItem();
        if(nullptr == itemSelected)
        {
            copySource.isValid = false;
            return;
        }
        copySource.isValid = false;
        copySource.root = currentRootPath;

        auto itemData = itemSelected->data(ITEM_DATAROLE).toInt();

        if(Root_UserProg == currentRootPath)
        {
            auto dirInfo = listFileInfo.at(itemData);

            if(false == isARemoteFile(dirInfo))
                return;

            strcpy(filenamePaste, dirInfo.file_name);//需要粘贴的文件名
            currentAction = Action_Copy;
            workerThread->setCopy(filenamePaste);
            workerThread->start();

            copySource.filename = itemSelected->text();
            copySource.fullpath = Utils::gbkCharToQString(dirname) + QString("/") + itemSelected->text();
            if(copySource.filename.endsWith(".PHM"))
            {
                /*********检查.BIN文件是否存在*********/
                QString orignfile;//U盘原始BIN文件路径
                orignfile = localDir->path() + QDir::separator()+copySource.filename.split(".").first() +".BIN";
                if(!(QFile::exists(orignfile)))
                {
                    Utils::MyMessageBox(QString("Err"), tr("请检查U盘原BIN文件是否存在"),tr("确定"),this);
                    return;
                }

                /************************************/


                /************把文件复制到U盘***********/
                Utils::qStringToGbkChar(copySource.fullpath, filenameRemote, sizeof(filenameRemote));
                filenameLocal = localDir->path() + QDir::separator() +"9999.PHM";
                qDebug()<<"filenameLocal"<<filenameLocal;
                auto d = createLoadingDialog(tr("数据拷贝"));
                if(!d)
                    return;
                connect(workerThread, SIGNAL(updateFileProgess(int)),
                        d, SLOT(setValue(int)));
                d->open();
                currentAction = Action_PasteRemoteToLocal;
                workerThread->setPasteRemoteToLocal(filenameRemote, &filenameLocal);
                workerThread->start();
                /*************文件复制完成************/

                //开始数据反解析
                if(WriteLanSiFile(filenameLocal,orignfile))
                {
                    QFile filetemp(filenameLocal);
                    filetemp.remove();
#ifdef  Q_OS_LINUX
                    sync();
#endif

                }
            }
            else
                Utils::MyMessageBox(QString("Info"), tr("数据格式错误"),tr("确定"),this);
        }
        else
            Utils::MyMessageBox(QString("Info"), tr("只能数据区操作"),tr("确定"),this);

        break;
    }
    case Definitions::BTN_FILE_COPY:
    {
        if(0 == pathDepth)
            return;

        auto itemSelected = listWidget->currentItem();
        if(nullptr == itemSelected)
        {
            copySource.isValid = false;
            return;
        }

        copySource.isValid = false;
        copySource.root = currentRootPath;

        auto itemData = itemSelected->data(ITEM_DATAROLE).toInt();

        if(Root_System == currentRootPath || Root_UserProg == currentRootPath)
        {
            auto dirInfo = listFileInfo.at(itemData);

            if(false == isARemoteFile(dirInfo))
                return;

            strcpy(filenamePaste, dirInfo.file_name);//需要粘贴的文件名
            currentAction = Action_Copy;
            workerThread->setCopy(filenamePaste);
            workerThread->start();

            copySource.filename = itemSelected->text();
            copySource.fullpath = Utils::gbkCharToQString(dirname) + QString("/") + itemSelected->text();
        }
        else if(Root_HMI_UDisk == currentRootPath)
        {
            auto fileInfo = localFileList.at(itemData);

            if(false == fileInfo.isFile())
                return;
            copySource.filename = itemSelected->text();
            copySource.fullpath = fileInfo.filePath();
            copySource.isValid = true;
        }
        in_paste=true;//处于过程中
        //Utils::printQString(copySource.fullpath);
        break;
    }
    case Definitions::BTN_FILE_PASTE:
    {
        if(0 == pathDepth)
        {
            Utils::MyMessageBox(QString("Info"), tr("操作无效"),tr("确定"),this);
            return;
        }

        if(false == copySource.isValid)
        {
            //QMessageBox::information(this, "Info", QString::fromUtf8("粘贴无效，请先复制一个文件"), QMessageBox::Ok);
            Utils::MyMessageBox(QString("Info"), tr("粘贴无效，请先复制一个文件"),tr("确定"),this);
            return;
        }

        if(Root_System == currentRootPath || Root_UserProg == currentRootPath)
        {
            //qDebug()<<"Root_System=Root_System";
            if(Root_System == copySource.root || Root_UserProg == copySource.root)  // 远端文件内部复制
            {
                int i;
                currentAction = Action_PasteRemoteToRemote;
                for( i= 0; i < listFileInfo.size(); ++i)
                {
                    //qDebug()<<listFileInfo.at(i).file_name;
                    if(copySource.filename==listFileInfo.at(i).file_name)
                    {
                        QStringList list = copySource.filename.split(".");
                        strlast = "."+list.last();
                        QDialogReNew* newDlg = new QDialogReNew(this);
                        connect(newDlg, SIGNAL(accepted()), this, SLOT(onOK()));
                        connect(newDlg, SIGNAL(rejected()), this, SLOT(onNot()));
                        newDlg->setAttribute(Qt::WA_DeleteOnClose);
                        newDlg->open();
                        break;
                    }
                }
                if(i==listFileInfo.size())
                {//不在文件复制这层路径
                    workerThread->setPasteRemoteToRemote(filenamePaste);
                    workerThread->start();
                    in_paste=false;//复制粘贴标志位
                }
            }
            else if(Root_HMI_UDisk == copySource.root)   // 本机复制到远端
            {
                //qDebug()<<"Root_System=Root_HMI_UDisk";
                auto remoteStr = Utils::gbkCharToQString(dirname) + QString("/") + copySource.filename;
                Utils::qStringToGbkChar(remoteStr, filenameRemote, sizeof(filenameRemote));
                // TODO 询问是否覆盖已存在文件
                auto d = createLoadingDialog();
                if(!d)
                    return;
                connect(workerThread, SIGNAL(updateFileProgess(int)),
                        d, SLOT(setValue(int)));
                d->open();
                currentAction = Action_PasteLocalToRemote;
                workerThread->setPasteLocalToRemote(filenameRemote, &copySource.fullpath);
                workerThread->start();
                in_paste=false;
            }
        }
        else if(Root_HMI_UDisk == currentRootPath)
        {
            if(Root_System == copySource.root || Root_UserProg == copySource.root)  // 远端复制到本机
            {
                //qDebug()<<"Root_HMI_UDisk=Root_System";
                Utils::qStringToGbkChar(copySource.fullpath, filenameRemote, sizeof(filenameRemote));
                filenameLocal = localDir->path() + QDir::separator() + copySource.filename;
                qDebug()<<"filenameLocal"<<filenameLocal;
                // TODO 询问是否覆盖已存在文件
                auto d = createLoadingDialog();
                if(!d)
                    return;
                connect(workerThread, SIGNAL(updateFileProgess(int)),
                        d, SLOT(setValue(int)));
                d->open();
                currentAction = Action_PasteRemoteToLocal;
                workerThread->setPasteRemoteToLocal(filenameRemote, &filenameLocal);
                workerThread->start();
                in_paste=false;
            }
            else if(Root_HMI_UDisk == copySource.root)   // 本机内部复制
            {//测试本地文件复制
                //qDebug()<<"Root_HMI_UDisk=Root_HMI_UDisk";
                int i;
                currentAction = Action_PasteLocalToLocal;
                //qDebug()<<localFileList.at(0).fileName();
                for( i= 0; i < localFileList.size(); ++i)
                {
                    qDebug()<<localFileList.at(i).fileName();
                    if(copySource.filename==localFileList.at(i).fileName())
                    {
                        QDialogReNew* newDlg = new QDialogReNew(this);
                        connect(newDlg, SIGNAL(accepted()), this, SLOT(onOK()));
                        connect(newDlg, SIGNAL(rejected()), this, SLOT(onNot()));
                        newDlg->setAttribute(Qt::WA_DeleteOnClose);
                        newDlg->open();
                        break;
                    }
                }
                if(i==localFileList.size())
                {//不在文件复制这层路径
                    filenameLocal = localDir->path() + QDir::separator() + copySource.filename;
                    workerThread->setPasteLocalToLocal(&copySource.fullpath, &filenameLocal);
                    workerThread->start();
                    in_paste=false;
                }
            }
        }
        break;
    }


    case Definitions::BTN_FILE_OPEN:
    {
        auto itemSelected = listWidget->currentItem();

        if(nullptr == itemSelected)
        {
            return;
        }

        onItemDoubleClicked(itemSelected);
        break;
    }

    case Definitions::BTN_FILE_UPGRADE:
    {
        QString helpInfo =
                //QString::fromUtf8("升级方法：\n将下位机程序重命名为A9Rom.bin\n将上位机程序重命名为QtRom.bin\n放置在触摸屏U盘内双击打开即可升级");
                tr("升级方法：\n将下位机程序重命名为A9Rom.bin\n将上位机程序重命名为QtRom.bin\n放置在触摸屏U盘内双击打开即可升级");
        Utils::MyMessageBox(QString("Info"), helpInfo,tr("确定"),this);
        break;
    }
    case  Definitions::BTN_FILE_COPYALL:
    {//一键拷贝系统加工文件到Ｕ盘
        int replace = 0;//同名文件处理
        QString localstr;
        char progname[FM_MAX_CHAR_DIRNAME]; // 打开新的目录
        QFileInfoList rootList;
        QString newPath = Utils::gbkCharToQString("0:") + QString("/") + Utils::gbkCharToQString("PROG");
        Utils::qStringToGbkChar(newPath, progname, sizeof(progname));
        //qDebug()<<progname<<dirnameNew;
        if(strcmp(progname, dirnameNew)!=0)
        {
            Utils::MyMessageBox(QString("Info"), tr("请在加工目录进行一键拷贝"),tr("确定"),this);
            break;
        }

        //进入Ｕ盘
#ifdef Q_OS_LINUX
        localDir->cd(QDir::rootPath()+"/home/mcgs/app");
        localstr = "/home/mcgs/app";
        // 仅在发行release模式下打开"/mnt"
#ifndef QT_DEBUG
        localDir->cd("media/usb0");//设置U盘挂载路径
        localstr = "media/usb0";
#endif
        rootList = localDir->entryInfoList();
#else
        rootList = QDir::drives();
#endif
        for(int i = 0;i < rootList.size(); ++i)
        {//查看是否有PROG目录如果没有则新建
            auto fileInfo = rootList.at(i);
            //qDebug()<<rootList.at(i).fileName();
            if("PROG"==fileInfo.fileName().toLower())
            {
                break;
            }
            if(i == rootList.size())
                localDir->mkdir("PROG");
        }
        //进入PROG目录
        localDir->cd("PROG");
        rootList = localDir->entryInfoList();

        //锁定目录

        currentRootPath = Root_HMI_UDisk;
        pathDepth = 2;

        //复制文件
        for(int i= 0; i < listFileInfo.size(); ++i)
        {
            //qDebug()<<"PROG"<<listFileInfo.at(i).file_name<<rootList.at(i).fileName();
            //检查重名文件
            if(replace != Replace::YES_ALL)
            {
                replace = checkFile(listFileInfo.at(i).file_name,rootList);
                if(replace ==Replace::NOT)
                    continue;
            }
            copySource.filename=Utils::gbkCharToQString(dirname) + QString("/")+listFileInfo.at(i).file_name;
            Utils::qStringToGbkChar(copySource.filename, filenameRemote, sizeof(filenameRemote));

            filenameLocal = localstr+QString("/PROG/")+ Utils::gbkCharToQString(listFileInfo.at(i).file_name);

            //qDebug()<<"本地"<<filenameLocal<<"远端"<<filenameRemote;
            currentAction = Action_PasteRemoteToLocal;
            workerThread->sendok = false;
            workerThread->setPasteRemoteToLocal(filenameRemote, &filenameLocal);//
            workerThread->start();
            onWaitFileFinish();
        }
        break;
    }
    case Definitions::BTN_UDisk_COPYALL:
    {//一键拷贝Ｕ盘的加工文件到系统加工文件
        int replace = 0;//同名文件处理
        QString localstr;//保存设置的路径
        QFileInfoList rootList;

#ifdef Q_OS_LINUX
        localDir->cd(QDir::rootPath()+"/home/mcgs/app");
        localstr = "/home/mcgs/app";
        // 仅在发行release模式下打开"/mnt"
#ifndef QT_DEBUG
        localDir->cd("media/usb0");//设置U盘挂载路径
        localstr = "media/usb0";
#endif
        rootList = localDir->entryInfoList();
#else
        rootList = QDir::drives();
#endif
        for(int i = 0;i < rootList.size(); ++i)
        {//查看是否有PROG目录如果没有则新建
            auto fileInfo = rootList.at(i);
            //qDebug()<<rootList.at(i).fileName();
            if("PROG"==fileInfo.fileName().toLower())
            {
                break;
            }
            if(i == rootList.size())
                Utils::MyMessageBox(QString("Info"), tr("U盘PROG路径错误，请检查U盘"),tr("确定"),this);
        }
        //进入Ｕ盘PROG目录
        localDir->cd("PROG");
        rootList = localDir->entryInfoList();

        //进入PROG系统界面
        startFileIntoUser();
        pathDepth = 2;
        //复制文件

        for(int i= 0; i < rootList.size(); ++i)
        {
            //检查重名文件
            if(replace != Replace::YES_ALL)
            {
                replace = checkFile(rootList.at(i).fileName(),listFileInfo);
                if(replace ==Replace::NOT)
                    continue;
            }
            filenameLocal=localstr + QString("/PROG/")+rootList.at(i).fileName();

            auto remoteStr = Utils::gbkCharToQString(dirname) + QString("/") + rootList.at(i).fileName();
            Utils::qStringToGbkChar(remoteStr, filenameRemote, sizeof(filenameRemote));

            qDebug()<<rootList.size()<<listFileInfo.size()<<"localstr"<<filenameLocal<<"remoteStr"<<remoteStr;
            currentAction = Action_PasteLocalToRemote;
            workerThread->sendok = false;
            workerThread->setPasteLocalToRemote(filenameRemote, &filenameLocal);
            workerThread->start();
            onWaitFileFinish();
        }
        onClickedRefreshDir();//文件刷新

        break;
    }


    case Definitions::BTN_FILE_NEWFOLDER:
    {
        if(0 == pathDepth)
            return;

        if(Root_HMI_UDisk != currentRootPath)
        {
            //QMessageBox::information(this, "?", QString::fromUtf8("仅支持U盘内新建文件夹"));
            Utils::MyMessageBox(QString("Info"), tr("仅支持U盘内新建文件夹"),tr("确定"),this);
            break;
        }

        filenameTemp = QString();
        QList<QDialogDataForm::item_t> items =
        {
            //{QString::fromUtf8("新文件夹名"), Adapters::TYPE_String, &filenameTemp, false, 0, 0, nullptr},
            {tr("新文件夹名"), Adapters::TYPE_String, &filenameTemp, false, 0, 0, nullptr},

        };

        auto w = new QDialogDataForm(this, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        connect(w, SIGNAL(accepted()), this, SLOT(onDialogAcceptedNewFolder()));
        //w->showDialog(QString::fromUtf8("输入新文件名字"), items, true);
        w->showDialog(tr("输入新文件名字"), items, true);

        break;
    }

    default:
        break;
    }
}

int SubWindowFileManage::checkFile(QString remot,QFileInfoList lock)
{
    for(int j=0;j<lock.size();j++)
    {
        if(remot ==lock.at(j).fileName())
        {
            QMessageBox:: StandardButton result= QMessageBox::information(this, "Title", tr("是否替换原有文件")+remot,
                                                                          QMessageBox::Yes|QMessageBox::No|QMessageBox::YesToAll);
            switch (result)
            {
            case QMessageBox::Yes:
                return Replace::YSE;
                break;
            case QMessageBox::No:
                return Replace::NOT;
                break;
            case QMessageBox::YesToAll:
                return Replace::YES_ALL;
                break;
            default:
                break;
            }
        }
    }
    return 0;
}
int SubWindowFileManage::checkFile(QString remot,QList<DIR_FILE_INFO> lock)
{
    for(int j=0;j<lock.size();j++)
    {
        if(remot ==lock.at(j).file_name)
        {
            QMessageBox:: StandardButton result= QMessageBox::information(this, "Title", tr("是否替换原有文件")+remot,
                                                                          QMessageBox::Yes|QMessageBox::No|QMessageBox::YesToAll);
            switch (result)
            {
            case QMessageBox::Yes:
                return Replace::YSE;
                break;
            case QMessageBox::No:
                return Replace::NOT;
                break;
            case QMessageBox::YesToAll:
                return Replace::YES_ALL;
                break;
            default:
                break;
            }
        }
    }
    return 0;
}

void SubWindowFileManage::onItemClicked(QListWidgetItem* item)
{
    if(timerSingleClick->isActive())
    {
        // 双击
        timerSingleClick->stop();
        isSingleClick = false;
        onItemDoubleClicked(item);
        return;
    }
    else if(false == isSingleClick)
    {
        // 开启定时器 等待单击结束
        timerSingleClick->start();
        return;
    }
    else
    {
        // 重置该标志
        isSingleClick = false;
    }

    if(0 == pathDepth)
        return;

    QString fileSizeAndDate;
    auto itemData = item->data(ITEM_DATAROLE).toInt();

    if(Root_System == currentRootPath || Root_UserProg == currentRootPath)
    {
        if(itemData < 0 || itemData >= listFileInfo.size())
            return;

        auto dirInfo = listFileInfo.at(itemData);

        auto year = ((dirInfo.file_date >> 9) & 0x7f) + 1980;
        auto month = (dirInfo.file_date >> 5) & 0xf;
        auto day = dirInfo.file_date & 0x1f;

        auto hour = (dirInfo.file_time >> 11) & 0x1f;
        auto minute = (dirInfo.file_time >> 5) & 0x3f;
        auto second = dirInfo.file_time & 0x1f;

        const QChar ZERO = QChar('0');

        //        fileSizeAndDate = QString::fromUtf8("修改日期：%1-%2-%3 %4:%5:%6").arg(year).arg(month).arg(day)
        //                          .arg(hour, 2, 10, ZERO).arg(minute, 2, 10, ZERO).arg(second, 2, 10, ZERO);
        fileSizeAndDate = tr("修改日期：%1-%2-%3 %4:%5:%6").arg(year).arg(month).arg(day)
                .arg(hour, 2, 10, ZERO).arg(minute, 2, 10, ZERO).arg(second, 2, 10, ZERO);

        if(isARemoteFile(dirInfo))
        {
            int bytes = dirInfo.file_type_size & (0x7fffffff);
            //fileSizeAndDate += QString::fromUtf8("  大小：") + Utils::getHumanReadableSize(bytes);
            fileSizeAndDate += tr("  大小：") + Utils::getHumanReadableSize(bytes);
        }

    }
    else if(Root_HMI_UDisk == currentRootPath)
    {
        if(itemData < 0 || itemData >= localFileList.size())
            return;

        auto fileInfo = localFileList.at(itemData);

        fileSizeAndDate = fileInfo.lastModified().toString("yyyy-M-d HH:mm:ss");
        //fileSizeAndDate = QString::fromUtf8("修改日期：") + fileSizeAndDate;
        fileSizeAndDate = tr("修改日期：") + fileSizeAndDate;

        if(fileInfo.isFile())
            //fileSizeAndDate += QString::fromUtf8("  大小：") + Utils::getHumanReadableSize(fileInfo.size());
            fileSizeAndDate += tr("  大小：") + Utils::getHumanReadableSize(fileInfo.size());

    }

    labelFileInfo->setText(fileSizeAndDate);
    labelFilename->setText(item->text());
}

void SubWindowFileManage::onItemDoubleClicked(QListWidgetItem* item)
{
    auto itemData = item->data(ITEM_DATAROLE).toInt();//得到角色号
    emit resetScrollPostion();

    if(0 == pathDepth)
    {
        switch(itemData)
        {
        case Root_System:
            if(GlobalData::modbusQueue->isOffline())
                break;

            currentAction = Action_ListDirFiles;
            strcpy(dirnameNew, "1:");
            workerThread->setDir(dirnameNew);
            workerThread->start();
            ++pathDepth;
            currentRootPath = Root_System;
            break;

        case Root_UserProg:
            if(GlobalData::modbusQueue->isOffline())
                break;

            currentAction = Action_ListDirFiles;
            strcpy(dirnameNew, "0:");
            workerThread->setDir(dirnameNew);
            workerThread->start();
            ++pathDepth;
            currentRootPath = Root_UserProg;
            break;

        case Root_HMI_UDisk:
        {//u盘设置

            if(!Utils::isDirExist(QDir::rootPath()+"media/usb0"))
            {
                Utils::MyMessageBox(QString("Error"), tr("未检测到Ｕ盘"),tr("确定"),this);
                break;
            }
#ifdef Q_OS_LINUX
            localDir->cd(QDir::currentPath());

            // 仅在发行release模式下打开"/mnt"
#ifndef QT_DEBUG

            localDir->cd(QDir::rootPath());
            localDir->cd("media/usb0");//设置U盘挂载路径
#endif
            localFileList = localDir->entryInfoList();
            refreshLocalDir();
#else
            localDir->cd(QDir::currentPath());
            //            localFileList = QDir::drives();
            localFileList = localDir->entryInfoList();
            refreshLocalDir(true);
#endif
            ++pathDepth;
            currentRootPath = Root_HMI_UDisk;

            break;
        }

        default:
            break;
        }
    }
    else if(Root_UserProg == currentRootPath || Root_System == currentRootPath)
    {
        //qDebug()<<currentRootPath;
        //itemData文件个数
        auto dirInfo = listFileInfo.at(itemData);//返回选择的文件信息（每个对应四个信息）
        //qDebug()<<itemData<<dirInfo.file_name;
        if(isARemoteFile(dirInfo))
        {
            if(in_paste==true)
                return;
            if(Root_UserProg != currentRootPath)
            {
                //QMessageBox::warning(this, "Error", QString::fromUtf8("无法打开，仅支持用户盘打开加工文件"));
                Utils::MyMessageBox(QString("Error"), tr("无法打开，仅支持用户盘打开加工文件"),tr("确定"),this);
                return;
            }

            // file
            auto filename = Utils::gbkCharToQString(dirInfo.file_name);

            if(false == filename.endsWith(".PHM", Qt::CaseInsensitive))
            {
                Utils::MyMessageBox(QString("Error"), tr("文件格式不是.PHM"),tr("确定"),this);
                return;
            }

            if(filename == GlobalData::workfileName)
            {
                //QMessageBox::information(this, "?", filename + QString::fromUtf8("已经是当前加工文件"));
                Utils::MyMessageBox(QString("Error"), filename + tr("已经是当前加工文件"),tr("确定"),this);
                return;
            }

            //            if(QMessageBox::Ok == QMessageBox::question(this, "?",
            //                    QString::fromUtf8("确定打开加工文件%1吗？").arg(filename),
            //                    QMessageBox::Ok | QMessageBox::Cancel))
            if(QMessageBox::Retry == Utils::MyMessageBox(QString("Error"),
                                                         tr("确定打开加工文件%1吗？").arg(filename),tr("确定"),tr("取消"),this))
            {
                Utils::qStringToGbkChar(filename, filenameNew, sizeof(filenameNew));

                currentAction = Action_OpenWorkfile;
                workerThread->setWorkfile(filenameNew);
                workerThread->start();
            }
        }
        else
        {
            // dir
            QString newPath = Utils::gbkCharToQString(dirname) + QString("/") + Utils::gbkCharToQString(dirInfo.file_name);
            Utils::qStringToGbkChar(newPath, dirnameNew, sizeof(dirnameNew));

            currentAction = Action_ListDirFiles;
            workerThread->setDir(dirnameNew);
            workerThread->start();
            ++pathDepth;
        }

    }
    else if(Root_HMI_UDisk == currentRootPath)
    {
        auto fileInfo = localFileList.at(itemData);

        if(fileInfo.isFile())
        {//是否为文件
            // file
            if(fileInfo.fileName().toLower() == GlobalData::filenameProgramQt.toLower())
            {//判断是否为跟新文件
                //                if(QMessageBox::Ok == QMessageBox::question(this, "?",
                //                        QString("确实要升级触摸屏上位机吗？\n升级过程中会自动重启"),
                //                        QMessageBox::Ok | QMessageBox::Cancel))
                if(QMessageBox::Retry == Utils::MyMessageBox(QString("Error"),
                                                             tr("确实要升级触摸屏上位机吗？\n升级过程中会自动重启")
                                                             ,tr("确定"),tr("取消"),this))
                {
                    upgradeQtApp(fileInfo.filePath());
                }
            }
            else if(fileInfo.fileName().toLower() == GlobalData::filenameProgramA9.toLower())
            {
                //                if(QMessageBox::Ok == QMessageBox::question(this, "?",
                //                        QString("确实要升级控制器下位机吗？\n升级后，请手动重启"),
                //                        QMessageBox::Ok | QMessageBox::Cancel))
                if(QMessageBox::Retry == Utils::MyMessageBox(QString("Error"),
                                                             tr("确实要升级控制器下位机吗？\n升级后，请手动重启")
                                                             ,tr("确定"),tr("取消"),this))
                {
                    auto d = createLoadingDialog();

                    if(!d)
                        return;

                    connect(workerThread, SIGNAL(updateFileProgess(int)),
                            d, SLOT(setValue(int)));
                    d->setRange(0, 101); // 比100多一点...
                    d->open();

                    strcpy(filenameRemote, "0:/ADT/A9Rom.bin");
                    filenameLocal = fileInfo.filePath();

                    currentAction = Action_UpgradeA9Rom;
                    workerThread->setUpgrade(filenameRemote, &filenameLocal);
                    workerThread->start();
                }
            }
        }
        else
        {
            // dir
            localDir->cd(item->text());
            localFileList = localDir->entryInfoList();
            refreshLocalDir();
            ++pathDepth;
        }
    }
}

void SubWindowFileManage::onWorkerThreadOperationDone(bool isOk)
{
    debug_printf("onWorkerThreadOperationDone=%d action=%d", isOk ? 1 : 0, currentAction);
    if(Action_PasteLocalToRemote == currentAction || Action_PasteRemoteToLocal == currentAction)
    {
        qDebug()<<"workerThread->sendok";
        workerThread->sendok = true;
    }
    // dismiss
    if(Action_PasteLocalToRemote == currentAction || Action_PasteRemoteToLocal == currentAction
            || Action_PasteLocalToLocal == currentAction)
        emit dismissProgressDialog();

    if(true == isOk)
    {
        // 成功
        switch(currentAction)
        {
        case Action_ListDirFiles://把文件和文件名显示在界面
            memcpy(dirname, dirnameNew, sizeof(dirname));//那个"1:"路径

            listWidget->clear();

            for(auto i = 0; i < listFileInfo.size(); ++i)
            {
                auto icon = QApplication::style()->standardIcon(isARemoteFile(listFileInfo.at(i)) ?
                                                                    QStyle::SP_FileIcon : QStyle::SP_DirIcon);//文件还是路径
                auto item = new QListWidgetItem(icon, Utils::gbkCharToQString(listFileInfo.at(i).file_name));
                QFont font;
                font.setPointSize(FONTSIZE);
                item->setData(ITEM_DATAROLE, QVariant(i));//设置角色 1、2....
                item->setFont(font);
                listWidget->addItem(item);
            }

            listWidget->sortItems();//排序

            labelCurrentDir->setText(rootPathString.at(currentRootPath) + QString(", ") + Utils::gbkCharToQString(dirname));
            //            qDebug()<<"gbkCharToQString"<<Utils::gbkCharToQString(dirname);
            break;

        case Action_NewFile:
        case Action_Delete:
        case Action_PasteRemoteToRemote:
        case Action_PasteLocalToRemote:
            currentAction = Action_ListDirFiles;
            workerThread->setDir(dirname);
            workerThread->start();
            return;
            break;

        case Action_Copy:
            copySource.isValid = true;
            break;

        case Action_RefreshLocalDir:
        case Action_PasteRemoteToLocal:
        case Action_PasteLocalToLocal:
            localDir->refresh();
            localFileList = localDir->entryInfoList();
            refreshLocalDir();
            break;

        case Action_OpenWorkfile:
            //QMessageBox::information(this, "OK", QString::fromUtf8("加载文件成功") + Utils::gbkCharToQString(filenameNew));
            Utils::MyMessageBox(QString("Error"),tr("加载文件成功")+
                                Utils::gbkCharToQString(filenameNew)
                                ,tr("确定"),this);
            break;

        case Action_UpgradeA9Rom:
            emit dismissProgressDialog();
            //QMessageBox::information(this, "OK", QString::fromUtf8("升级控制器成功"));
            Utils::MyMessageBox(QString("OK"), tr("升级控制器成功"),tr("确定"),this);
            break;

        default:
            break;
        }
    }
    else
    {
        // 失败
        switch(currentAction)
        {
        case Action_UpgradeA9Rom:
            emit dismissProgressDialog();
            //QMessageBox::warning(this, "Error", QString::fromUtf8("升级控制器失败"));
            Utils::MyMessageBox(QString("Error"), tr("升级控制器失败"),tr("确定"),this);
            break;

        default:
            break;
        }
    }

    currentAction = Action_None;
}

void SubWindowFileManage::showRootPath()
{
    pathDepth = 0;
    currentRootPath = Root_None;

    labelCurrentDir->setText(tr("Home"));

    emit resetScrollPostion();

    auto createAnItem = [&](enum QStyle::StandardPixmap icon, int index, const QString & text)
    {
        auto stdIcon = QApplication::style()->standardIcon(icon);
        auto item = new QListWidgetItem(stdIcon, text);
        item->setData(ITEM_DATAROLE, QVariant(index));
        listWidget->addItem(item);
    };

    listWidget->clear();

    createAnItem(QStyle::SP_DirIcon, Root_System, rootPathString.at(0));
    createAnItem(QStyle::SP_DirIcon, Root_UserProg, rootPathString.at(1));
    createAnItem(QStyle::SP_DirIcon, Root_HMI_UDisk, rootPathString.at(2));
    //createAnItem(QStyle::SP_DirIcon, Root_UserProg, QObject::tr(rootPathString.at(1).toUtf8()));
    //createAnItem(QStyle::SP_DirIcon, Root_HMI_UDisk, QObject::tr(rootPathString.at(2).toUtf8()));

}

void SubWindowFileManage::startFileIntoUser()
{
    if(GlobalData::modbusQueue->isOffline())
    {
        showRootPath();
        return;
    }
    emit resetScrollPostion();
    currentRootPath = Root_UserProg;
    //固定路径为PROG目录
    QString newPath = Utils::gbkCharToQString("0:") + QString("/") + Utils::gbkCharToQString("PROG");
    Utils::qStringToGbkChar(newPath, dirnameNew, sizeof(dirnameNew));

    currentAction = Action_ListDirFiles;
    workerThread->setDir(dirnameNew);//读取文件　添加到localFileList
    workerThread->start();
    pathDepth+=2;

}

void SubWindowFileManage::onClickedRefreshDir()
{
    qDebug()<<"onClickedRefreshDir"<<pathDepth;
    if(0 == pathDepth)
        return;

    switch(currentRootPath)
    {
    case Root_System:
    case Root_UserProg:
        currentAction = Action_ListDirFiles;
        workerThread->setDir(dirnameNew);
        workerThread->start();
        break;

    case Root_HMI_UDisk:
        currentAction = Action_RefreshLocalDir;
        onWorkerThreadOperationDone(true);
        break;

    default:
        break;
    }
}

void SubWindowFileManage::levelUp()
{
    if(0 == pathDepth)
        return;

    if(1 == pathDepth)
    {
        showRootPath();
        return;
    }

    emit resetScrollPostion();

    if(Root_System == currentRootPath || Root_UserProg == currentRootPath)
    {

        // 删掉'/'字符后面的文字
        // https://stackoverflow.com/questions/47201292/qstring-remove-last-characters
        auto qStringPath = Utils::gbkCharToQString(dirname);
        auto pos = qStringPath.lastIndexOf(QChar('/'));

        if(-1 < pos)
        {
            Utils::qStringToGbkChar(qStringPath.left(pos), dirnameNew, sizeof(dirnameNew));

            currentAction = Action_ListDirFiles;
            workerThread->setDir(dirnameNew);
            workerThread->start();
            --pathDepth;
        }
        //qDebug()<<"qStringPath"<<qStringPath<<"pos"<<pos<<"dirnameNew"<<dirnameNew;
    }
    else if(Root_HMI_UDisk == currentRootPath)
    {
        localDir->cdUp();
        localFileList = localDir->entryInfoList();
        refreshLocalDir();
        --pathDepth;
    }
}

void SubWindowFileManage::onDialogAcceptedNewFile()
{//新建文件
    if(filenameTemp.length() > NEWFILE_MAXLEN)
    {
        //QMessageBox::critical(this, "Error", QString::fromUtf8("文件名太长"));
        Utils::MyMessageBox(QString("Error"), tr("文件名太长"),tr("确定"),this);
        return;
    }
    if(filenameTemp.length() == 0)
    {
        Utils::MyMessageBox(QString("Error"), tr("文件名不能为空"),tr("确定"),this);
        return;
    }

    filenameTemp += QString(".PHM");

    for(auto i = 0; i < listFileInfo.size(); ++i)
    {
        auto dirInfo = listFileInfo.at(i);
        auto filenameExisted = Utils::gbkCharToQString(dirInfo.file_name);

        if(filenameExisted == filenameTemp)
        {
            //QMessageBox::critical(this, "Error", QString::fromUtf8("已有相同文件，无法新建") + filenameTemp);
            Utils::MyMessageBox(QString("Error"), tr("已有相同文件，无法新建") + filenameTemp,tr("确定"),this);
            return;
        }
    }

    Utils::qStringToGbkChar(filenameTemp, filenameNew, sizeof(filenameNew));
    currentAction = Action_NewFile;
    workerThread->setNewFile(filenameNew);
    workerThread->start();
}

void SubWindowFileManage::onDialogAcceptedNewFolder()
{
    localDir->mkdir(filenameTemp);
    localDir->refresh();
    localFileList = localDir->entryInfoList();
    refreshLocalDir();
}

void SubWindowFileManage::onTimeoutSingleClicked()
{
    auto itemSelected = listWidget->currentItem();

    if(nullptr != itemSelected)
    {
        isSingleClick = true;
        onItemClicked(itemSelected);
    }
}

bool SubWindowFileManage::isARemoteFile(const SubWindowFileManage::DIR_FILE_INFO& dirInfo)
{//检查大小
    return dirInfo.file_type_size & (0x1 << 31);
}

void SubWindowFileManage::refreshLocalDir(bool isDrive)
{
    listWidget->clear();

    for(auto i = 0; i < localFileList.size(); ++i)
    {
        auto fileInfo = localFileList.at(i);
        auto icon = QApplication::style()->standardIcon(fileInfo.isFile() ? QStyle::SP_FileIcon : QStyle::SP_DirIcon);

        QListWidgetItem* item = nullptr;

        if(false == isDrive)
            item = new QListWidgetItem(icon, fileInfo.fileName());
        else
            item = new QListWidgetItem(icon, fileInfo.absolutePath());

        QFont font;
        font.setPointSize(FONTSIZE);
        item->setFont(font);
        item->setData(ITEM_DATAROLE, QVariant(i));
        listWidget->addItem(item);
    }

    listWidget->sortItems();
    //labelCurrentDir->setText(QObject::tr(rootPathString.at(2).toUtf8()) + QString(", ") + localDir->path());
    labelCurrentDir->setText(rootPathString.at(2) + QString(", ") + localDir->path());

}

QProgressDialog* SubWindowFileManage::createLoadingDialog(QString str)
{
    QProgressDialog* d = nullptr;

    if(false == GlobalData::modbusQueue->isOffline())
    {
        d = new QProgressDialog(this, Qt::FramelessWindowHint);
        d->setAttribute(Qt::WA_DeleteOnClose);
        //d->setLabelText("操作进行中...请稍侯");
        if(str=="")
            d->setLabelText(tr("操作进行中...请稍侯"));
        else
            d->setLabelText(str);
        d->setCancelButton(nullptr);
        d->setRange(0, 100);
        connect(this, SIGNAL(dismissProgressDialog()),
                d, SLOT(accept()));
    }

    return d;
}

void SubWindowFileManage::upgradeQtApp(const QString& filenameNewProgram)
{
#ifdef Q_OS_LINUX
    const QString scriptFile = "/tmp/upgrade.sh";
    //    auto fullpath = QApplication::applicationFilePath()+"/app";//返回文件存放路径
    auto fullpath = "/root/app/adtrom";//返回文件存放路径
    QFile f(scriptFile);

    if(!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        return;

    QTextStream stream(&f);
    stream << "killall " << QFileInfo(fullpath).fileName() << endl;
    stream << "cp " << filenameNewProgram << " " << fullpath<< endl;
    stream << "chmod a+x " << fullpath << endl;
    stream << "reboot" << endl;

    stream.flush();

    f.close();

    QProcess::startDetached("/bin/sh", QStringList() << scriptFile);
#else
    Utils::MyMessageBox(QString("Error"), tr("仅支持Linux操作系统升级"),tr("确定"),this);
    //    QMessageBox::critical(this, "Error", QString("仅支持Linux操作系统升级"));
#endif
}

// 参数:拷贝的文件  写入的文件
INT16U SubWindowFileManage::WriteCSVFile(QString copyfile, QString orinefile, int addr)
{//CSV数据解析 函数
    QFile       file(copyfile),trf(orinefile);
    INT32U		Filelen,i,n;
    INT8U		*pbuf;
    //    tLANSI_BIN	Lanse;
    TEACH_DATA  Lanse;
    TEACH_DATA  tempdata;
    bool         isok;


    qDebug()<<"head size"<<sizeof(TEACH_HEAD)<<"data size"<<sizeof(TEACH_DATA);
    isok =  file.open(QIODevice::ReadOnly);
    qDebug()<<"isok"<<isok;
    if(isok)
    {
        qDebug()<<"filename"<<copyfile<<orinefile;
        pbuf = (INT8U*)malloc(file.size());
        Filelen = file.size()-sizeof(TEACH_HEAD);
        qDebug()<<"Filelen"<<Filelen<<"tLANSI_BIN_size"<<sizeof(TEACH_DATA)<<"size_head"<<sizeof(TEACH_HEAD);
        if(Filelen%sizeof(TEACH_DATA) != 0)
        {
            file.close();
            qDebug()<<"Filelen err 格式不对"<<Filelen/sizeof(TEACH_DATA);
            return 0;
        }
        else
        {
            file.seek(sizeof(TEACH_HEAD));
            n = Filelen/sizeof(TEACH_DATA);
            qDebug()<<"数据点数 Num"<<n;
            isok = trf.open(QIODevice::WriteOnly | QIODevice::Truncate);
            if(addr!=0)
                trf.seek(addr);
            qDebug()<<"open flie isok"<<isok;

            auto d = createLoadingDialog(tr("数据生成"));
            if(!d)
                return 0;
            connect(this, SIGNAL(upRate(int)),
                    d, SLOT(setValue(int)));
            d->open();
            QTextStream textStream;
            textStream.setDevice(&trf);
            textStream.setCodec("GB2312");
            QString HardStr;
            {
                HardStr =QString("孔位");
                for(int t=0;t<6;t++)
                {
                    HardStr +=","+GlobalData::axisName.at(t);
                }
                HardStr +=","+QString("颜色");
                HardStr +=","+QString("速度");
                for(int t=0;t<6;t++)
                {
                    HardStr +=","+QString("输出%1").arg(t);
                }

                HardStr +=","+QString("停高");
                textStream << HardStr+"\n";
            }

            QString dataStr;
            for(i=1;i<=n;i++)
            {
                //qDebug()<<"index"<<i<<n;
                dataStr = QString("%1").arg(i-1);
                isok = file.read(reinterpret_cast<char*>(&tempdata), sizeof(tempdata));
                memset(&Lanse,0,sizeof(Lanse));
                for(int k=0;k<6;k++)
                {
                    Lanse.fMove_pos[k] = tempdata.fMove_pos[k];
                    dataStr += ","+Utils::numberToString(Lanse.fMove_pos[k], 2, 6);
                }
                //颜色
                Lanse.icolor =tempdata.icolor;
                dataStr += ","+QString("%1").arg(Lanse.icolor);
                //速度输出
                Lanse.iMainSpeed =tempdata.iMainSpeed;
                dataStr += ","+QString("%1").arg(Lanse.iMainSpeed);
                //输出

                for(auto bit = 0; bit < 6; ++bit)
                {
                    dataStr += ","+QString("%1").arg(tempdata.iOut[bit]);
                }
                //高位刹车
                Lanse.iSkid =tempdata.iSkid;
                dataStr += ","+QString("%1").arg(Lanse.iSkid);

                textStream << dataStr +"\n";
                dataStr.clear();

                emit upRate((((float)i/n)*100));
                //qDebug()<<"write lansi file"<<(((float)i/n)*100)<<"%"<<Lanse.icolor<<tempdata.iMainSpeed<<tempdata.iSkid;
                Utils::sleep1(1);
            }
            emit dismissProgressDialog();
#ifdef  Q_OS_LINUX
            trf.flush();
            fsync(trf.handle());
#endif
            trf.close();
        }
    }
    else
    {
        return 0;
    }

    file.close();
    free(pbuf);

    return 1;
}

INT16U SubWindowFileManage::ReadCSVFile(QString filename, int StarAddr)
{//CSV数据解析 函数
    QFile       file(filename);
    INT32U		Filelen,i,n;
    INT8U		*pbuf;
    TEACH_HEAD	temphead;
    INT32U		ret = 0;
    TEACH_DATA  tempdata;
    bool         isok;

    qDebug()<<"head size"<<sizeof(TEACH_HEAD)<<"data size"<<sizeof(TEACH_DATA);
    //	ps_debugout("open%s\r\n",filename);

    isok = file.open(QIODevice::ReadOnly);
    qDebug()<<"filename"<<filename<<isok;
    if(isok)
    {
        QTextStream * out = new QTextStream(&file);//文本流
        QStringList tempOption = out->readAll().split("\n");//每行以\n区分
        QStringList singledate;
        //    qDebug()<<"filename"<<filename;
        pbuf = (INT8U*)malloc(file.size());
        Filelen = file.size()-StarAddr;
        file.seek(StarAddr);
        n = tempOption.count()-2;//减去第一个头  另外一个是计算方式的差异
        qDebug()<<"文件总孔数"<<n<<"trf"<<filename.split(".").first()+".PHM";
        QFile trf(filename.split(".").first()+".PHM");
        isok = trf.open(QIODevice::WriteOnly | QIODevice::Truncate);
        qDebug()<<"open flie isok"<<isok;
        {
            memset(&temphead,0,sizeof(temphead));
            //获取当前日期
            QDateTime time;
            QString str;
            str=time.currentDateTime().toString("h")+":"+
                    time.currentDateTime().toString("m")+":"+time.currentDateTime().toString("s");
            QByteArray ba = str.toLatin1();
            for(int i=0;i<ba.size();i++)
            {
                if(i>15)
                    break;
                temphead.sModifyTime[i] = ba.at(i);//时间头
            }
            temphead.iTotalNum = n;//产品数头
            temphead.sWorkPice[0] = 0;//工作台
            temphead.sWorkPice[1]=0;

            //write 会返回实际写入的字节数
            ret = trf.write(reinterpret_cast<char *>(&temphead), sizeof(temphead));
            qDebug()<<"write head biyt"<<ret;
        }
        //        qDebug()<<"write start"<<isok;
        auto d = createLoadingDialog(tr("数据解析"));
        if(!d)
            return 0;
        connect(this, SIGNAL(upRate(int)),
                d, SLOT(setValue(int)));
        d->open();

        for(int i = 1 ; i < tempOption.count()-1 ; i++)
        {
            QStringList tempbar = tempOption.at(i).split(",");//一行中的单元格以，区分
            //具体对数据的操作
            for(int j=0;j<tempbar.count();j++)
            {//取出每一个数字
                if(j!=0||tempbar.at(j)!=","||tempbar.at(j)!="\n")
                    singledate.append(tempbar.at(j));
            }
            //qDebug()<<singledate;
            //结构体赋值
            memset(&tempdata,0,sizeof(tempdata));
            tempdata.fMove_pos[0] = singledate.at(1).toFloat();
            tempdata.fMove_pos[1] = singledate.at(2).toFloat();
            tempdata.fMove_pos[2] = singledate.at(3).toFloat();
            tempdata.fMove_pos[3] = singledate.at(4).toFloat();
            tempdata.fMove_pos[4] = singledate.at(5).toFloat();
            tempdata.fMove_pos[4] = singledate.at(6).toFloat();
            tempdata.icolor = singledate.at(7).toInt();
            tempdata.iMainSpeed = singledate.at(8).toInt();
            for(int bit=0;bit<6;bit++)
            {

                //qDebug()<<"bit"<<bit;
                if(singledate.at(9+bit)=="1")
                    tempdata.iOut[bit]=1;
                else
                    tempdata.iOut[bit]=0;
            }
            tempdata.iSkid = singledate.at(9+6).toInt();
            singledate.clear();
            isok = trf.write(reinterpret_cast<char*>(&tempdata),sizeof(tempdata));
            emit upRate((((float)i/n)*100));
            //            qDebug()<<"write CSV file"<<(((float)i/n)*100)<<"%"<<tempdata.fMove_pos[0]<<tempdata.fMove_pos[1]<<tempdata.fMove_pos[2]
            //                   <<tempdata.fMove_pos[3]<<tempdata.fMove_pos[4]<<tempdata.fMove_pos[5]<<tempdata.fMove_pos[6]<<tempdata.fMove_pos[7]
            //                  <<tempdata.fMove_pos[8]<<tempdata.fMove_pos[9];
            if(!Utils::isDirExist(QDir::rootPath()+"media/usb0"))
            {
                Utils::MyMessageBox(QString("ERR"), tr("没有检测到Ｕ盘"),tr("确定"),this);
                return 0;
            }
//            Utils::sleep1(1);

        }
        emit dismissProgressDialog();
#ifdef  Q_OS_LINUX
        trf.flush();
        fsync(trf.handle());
#endif
        trf.close();
    }
    else
    {
        file.close();
        return 0;	//格式不对
    }

    file.close();
    free(pbuf);

    return 1;
}
INT16U SubWindowFileManage::ReadLanSiFile(QString filename)
{//蓝思数据解析 函数
    QFile       file(filename);
    INT32U		Filelen,i,n;
    INT8U		*pbuf;
    tLANSI_BIN	Lanse;
    TEACH_HEAD	temphead;
    INT32U		ret = 0;
    TEACH_DATA  tempdata;
    bool         isok;


    qDebug()<<"head size"<<sizeof(TEACH_HEAD)<<"data size"<<sizeof(TEACH_DATA);
    //	ps_debugout("open%s\r\n",filename);
    isok =  file.open(QIODevice::ReadOnly | QIODevice::Truncate);
    //    qDebug()<<"filename"<<filename;
    pbuf = (INT8U*)malloc(file.size());
    Filelen = file.size()-404;
    qDebug()<<"Filelen"<<Filelen<<"tLANSI_BIN_size"<<sizeof(tLANSI_BIN);
    if(Filelen%sizeof(tLANSI_BIN) != 0)
    {
        file.close();
        return 0;	//格式不对
    }
    else
    {
        file.seek(404);
        n = Filelen/sizeof(tLANSI_BIN);

        qDebug()<<"文件总孔数"<<n<<"trf"<<filename.split(".").first()+".PHM";
        QFile trf(filename.split(".").first()+".PHM");
        isok = trf.open(QIODevice::WriteOnly | QIODevice::Truncate);
        qDebug()<<"open flie isok"<<isok;
        memset(&temphead,0,sizeof(temphead));
        //获取当前日期
        QDateTime time;
        QString str;
        str=time.currentDateTime().toString("h")+":"+
                time.currentDateTime().toString("m")+":"+time.currentDateTime().toString("s");
        QByteArray ba = str.toLatin1();
        for(int i=0;i<ba.size();i++)
        {
            if(i>15)
                break;
            temphead.sModifyTime[i] = ba.at(i);
        }

        temphead.iTotalNum = n;
        temphead.sWorkPice[0] = 0;
        temphead.sWorkPice[1]=0;

        //write 会返回实际写入的字节数
        ret = trf.write(reinterpret_cast<char *>(&temphead), sizeof(temphead));
        qDebug()<<"write head biyt"<<ret;

        //起点
        memset(&tempdata,0,sizeof(tempdata));
        isok = trf.write(reinterpret_cast<char *>(&tempdata), sizeof(tempdata));
        //        qDebug()<<"write start"<<isok;
        auto d = createLoadingDialog(tr("数据解析"));
        if(!d)
            return 0;
        connect(this, SIGNAL(upRate(int)),
                d, SLOT(setValue(int)));
        d->open();
        for(i=1;i<=n;i++)
        {
            isok = file.read(reinterpret_cast<char*>(&Lanse), sizeof(Lanse));

            memset(&tempdata,0,sizeof(tempdata));
            tempdata.fMove_pos[0] = Lanse.x;
            tempdata.fMove_pos[1] = Lanse.z;
            tempdata.fMove_pos[2] = Lanse.y;
            tempdata.fMove_pos[3] = Lanse.a;
            tempdata.fMove_pos[4] = Lanse.b;
            if(fabs(Lanse.cr - 1.0) < 0.001)
            {
                tempdata.icolor = 2;
            }
            else
            {
                tempdata.icolor = 1;
            }
            isok = trf.write(reinterpret_cast<char*>(&tempdata),sizeof(tempdata));
            emit upRate((((float)i/n)*100));
            //qDebug()<<"write lansi file"<<(((float)i/n)*100)<<"%"<<tempdata.fMove_pos[0]<<tempdata.fMove_pos[1]<<Lanse.cr;
            Utils::sleep1(1);
        }
        emit dismissProgressDialog();
#ifdef  Q_OS_LINUX
        trf.flush();
        fsync(trf.handle());
#endif
        trf.close();
    }

    file.close();
    free(pbuf);

    return 1;
}
// 参数:拷贝的文件  写入的文件
INT16U SubWindowFileManage::WriteLanSiFile(QString copyfile,QString orinefile)
{//蓝思数据解析 函数
    QFile       file(copyfile),trf(orinefile);
    INT32U		Filelen,i,n;
    INT8U		*pbuf;
    tLANSI_BIN	Lanse;
    TEACH_DATA  tempdata;
    bool         isok;


    qDebug()<<"head size"<<sizeof(TEACH_HEAD)<<"data size"<<sizeof(TEACH_DATA);
    isok =  file.open(QIODevice::ReadOnly | QIODevice::Truncate);
    qDebug()<<"filename"<<copyfile<<orinefile;
    pbuf = (INT8U*)malloc(file.size());
    Filelen = file.size()-120;
    qDebug()<<"Filelen"<<Filelen<<"tLANSI_BIN_size"<<sizeof(TEACH_DATA);
    if(Filelen%sizeof(TEACH_DATA) != 0)
    {
        file.close();
        return 0;	//格式不对
    }
    else
    {
        file.seek(120);
        n = Filelen/sizeof(TEACH_DATA);
        isok = trf.open(QIODevice::WriteOnly | QIODevice::Truncate);
        trf.seek(404);
        qDebug()<<"open flie isok"<<isok;

        auto d = createLoadingDialog(tr("数据生成"));
        if(!d)
            return 0;
        connect(this, SIGNAL(upRate(int)),
                d, SLOT(setValue(int)));
        d->open();
        for(i=1;i<=n;i++)
        {
            isok = file.read(reinterpret_cast<char*>(&tempdata), sizeof(tempdata));

            memset(&Lanse,0,sizeof(Lanse));
            Lanse.x = tempdata.fMove_pos[0];
            Lanse.z = tempdata.fMove_pos[1];
            Lanse.y = tempdata.fMove_pos[2];
            Lanse.a = 80;
            Lanse.b = tempdata.fMove_pos[4];
            Lanse.cr =(float)(tempdata.icolor-1);

            isok = trf.write(reinterpret_cast<char*>(&Lanse),sizeof(Lanse));
            emit upRate((((float)i/n)*100));
            //qDebug()<<"write lansi file"<<(((float)i/n)*100)<<"%"<<tempdata.fMove_pos[0]<<tempdata.fMove_pos[1]<<tempdata.fMove_pos[2];
            Utils::sleep1(1);
        }
        emit dismissProgressDialog();
#ifdef  Q_OS_LINUX
        trf.flush();
        fsync(trf.handle());
#endif
        trf.close();
    }

    file.close();
    free(pbuf);

    return 1;
}

void SubWindowFileManage::DelLanSiFile()
{
    QFile file(copySource.fullpath);
    file.remove();
#ifdef  Q_OS_LINUX
    sync();
#endif
    onClickedRefreshDir();
}


QThreadFileManage::QThreadFileManage(QList<SubWindowFileManage::DIR_FILE_INFO>* returnInfo,
                                     QObject* parent): QThread(parent),sendok(false),
    workstep(WS_DONE),
    filenameOrDir(nullptr),
    dirFileInfo(returnInfo),
    isStopped(false)
{

}

void QThreadFileManage::run()
{
    bool isOk = false;
    ModbusQueue::element_t element;
    quint16 fileCount = 0;//当前路径文件数
    qDebug()<<"++workstep++"<<workstep;
    while(workstep != WS_DONE && false == isStopped)
    {
        switch(workstep)
        {
        case WS_WRITE_DIR:
        {
            element.data = filenameOrDir;//路径
            element.len = strlen(filenameOrDir);

            if(element.len > 256)
            {
                isOk = false;
                break; // error len
            }

            element.operation = ModbusQueue::OP_WRITE_DATA;
            element.startAddress = ModbusAddress::FileManage_SetDir;
            isOk = GlobalData::modbusQueue->sendQuery(&element, false);

            if(true == isOk)
                workstep = WS_READ_FILE_COUNT;
            break;
        }

        case WS_READ_FILE_COUNT:
        {
            element.data = &fileCount;
            element.len = sizeof(fileCount);
            element.operation = ModbusQueue::OP_READ_DATA;
            element.startAddress = ModbusAddress::FileManage_GetFileCount;
            isOk = GlobalData::modbusQueue->sendQuery(&element, false);

            if(true == isOk)
            {
                workstep = WS_READ_FILE_CONTENT;
            }
            break;
        }

        case WS_READ_FILE_CONTENT:
        {
            //qDebug()<<"fileCount"<<fileCount;
            if(fileCount > 255)
            {
                isOk = false;
                break;
            }

            auto data = new SubWindowFileManage::DIR_FILE_INFO[fileCount];
            //得到每一个文件的信息 文件名 大小 修改时间
            if(fileCount > 0)
            {
                element.data = data;
                element.len = sizeof(SubWindowFileManage::DIR_FILE_INFO) * fileCount;
                element.operation = ModbusQueue::OP_READ_DATA;
                element.startAddress = ModbusAddress::FileManage_GetInfo;
                isOk = GlobalData::modbusQueue->sendQuery(&element, false);
            }

            if(true == isOk)
            {
                // store data to QList
                dirFileInfo->clear();

                for(quint16 i = 0; i < fileCount; ++i)
                    dirFileInfo->append(*(data + i));//存储到QList

                workstep = WS_DONE;
            }

            Utils::delete_array(data);
            break;
        }

        case WS_NEWFILE:
        {
            element.data = filenameOrDir;
            element.len = strlen(filenameOrDir);
            element.operation = ModbusQueue::OP_WRITE_DATA;
            element.startAddress = ModbusAddress::FileManage_NewFile;
            isOk = GlobalData::modbusQueue->sendQuery(&element, false);

            if(true == isOk)
                workstep = WS_DONE;

            break;
        }

        case WS_DELETE:
        {
            element.data = filenameOrDir;
            element.len = strlen(filenameOrDir);
            element.operation = ModbusQueue::OP_WRITE_DATA;
            element.startAddress = ModbusAddress::FileManage_Delete;
            isOk = GlobalData::modbusQueue->sendQuery(&element, false);

            if(true == isOk)
                workstep = WS_DONE;

            break;
        }

        case WS_COPY:
        case WS_PASTE_REMOTE_TO_REMOTE:
        {
            element.data = filenameOrDir;
            element.len = strlen(filenameOrDir);
            element.operation = ModbusQueue::OP_WRITE_DATA;
            element.startAddress = (WS_COPY == workstep ?
                                        ModbusAddress::FileManage_SetCopy : ModbusAddress::FileManage_SetPaste);
            isOk = GlobalData::modbusQueue->sendQuery(&element, false);

            qDebug()<<"isOk"<<"WS_PASTE_REMOTE_TO_REMOTE";
            if(true == isOk)
                workstep = WS_DONE;

            break;
        }

        case WS_PASTE_LOCAL_TO_REMOTE:
        case WS_PASTE_REMOTE_TO_LOCAL:
        case WS_UPGRADE_COPY_TO_REMOTE:
        {
            //debug_printf("filenameRemote=%s", filenameRemote);
            //Utils::printQString(QString("filenameLocal=")+*filenameLocal);

            //modbus 读取文件
            element.operation = (WS_PASTE_REMOTE_TO_LOCAL == workstep ?
                                     ModbusQueue::OP_READ_FILE : ModbusQueue::OP_WRITE_FILE);

            //            qDebug()<<"element.operation"<<element.operation;
            element.mixParam.file.local = filenameLocal;
            element.mixParam.file.remote = filenameRemote;
            connect(GlobalData::modbusQueue, SIGNAL(notifyFileProgress(int)),
                    this, SLOT(_onNotifyFileProgress(int)));

            isOk = GlobalData::modbusQueue->sendQuery(&element, false);

            disconnect(GlobalData::modbusQueue, SIGNAL(notifyFileProgress(int)),
                       this, SLOT(_onNotifyFileProgress(int)));

            if(true == isOk)
            {
                if(workstep == WS_UPGRADE_COPY_TO_REMOTE)
                    workstep = WS_UPGRADE_START;
                else
                {
                    workstep = WS_DONE;
                }
            }

            break;
        }

        case WS_PASTE_LOCAL_TO_LOCAL:
        {
            QFile f1(*filenameLocal);
            QFile f2(*filenameLocalToPaste);

            if(false == f1.open(QIODevice::ReadOnly) || false == f2.open(QIODevice::WriteOnly | QIODevice::Truncate))
            {
                isOk = false;
                break;
            }

            const qint64 bufferSize = 1024;
            qint64 bytesRead = 0;
            qint64 bytesWritten = 0;
            char buffer[bufferSize];
            int progress = 0;
            isOk = true;

            for(qint64 offset = 0; isOk && offset < f1.size(); offset += bytesRead)
            {//本地复制
                f1.seek(offset);
                bytesRead = f1.read(buffer, bufferSize);

                f2.seek(offset);
                bytesWritten = f2.write(buffer, bytesRead);

                if(0 > bytesRead || 0 > bytesWritten)
                {
                    isOk = false;
                }

                // 进度
                progress = 100 * static_cast<float>(offset + 1) / static_cast<float>(f1.size());
                emit updateFileProgess(progress);
            }
            f2.close();
#ifdef  Q_OS_LINUX
            sync();
#endif
            if(true == isOk)
                workstep = WS_DONE;

            break;
        }

        case WS_OPEN_WORKFILE:
        {
            element.data = filenameOrDir;
            element.len = strlen(filenameOrDir);
            element.operation = ModbusQueue::OP_WRITE_DATA;
            element.startAddress = ModbusAddress::CurrentWorkFilename;

            isOk = GlobalData::modbusQueue->sendQuery(&element, false);

            if(true == isOk)
            {
                GlobalData::workfileName = Utils::gbkCharToQString(filenameOrDir); // 更新全局文件名
                Workfile::getInstance()->setIsAlreadyReadAll(false);
                workstep = WS_DONE;
            }

            break;
        }

        case WS_UPGRADE_START:
        {
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
        {
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

        default:
            break;
        }

        if(false == isOk)
            break; // while loop

        Utils::sleep(10);
    }
#ifdef  Q_OS_LINUX
    sync();
#endif
    emit operationDone(isOk);
}

void QThreadFileManage::setDir(char* dir)
{
    _setWorkstepAndPath(dir, WS_WRITE_DIR);
}

void QThreadFileManage::setNewFile(char* filename)
{
    _setWorkstepAndPath(filename, WS_NEWFILE);
}

void QThreadFileManage::setCopy(char* filename)
{
    _setWorkstepAndPath(filename, WS_COPY);
}

void QThreadFileManage::setPasteRemoteToRemote(char* filename)
{
    _setWorkstepAndPath(filename, WS_PASTE_REMOTE_TO_REMOTE);
}

void QThreadFileManage::setPasteLocalToRemote(char* remote, QString* local)
{
    _setWorkstepAndPath(remote, local, WS_PASTE_LOCAL_TO_REMOTE);
}

void QThreadFileManage::setPasteRemoteToLocal(char* remote, QString* local)
{
    _setWorkstepAndPath(remote, local, WS_PASTE_REMOTE_TO_LOCAL);
}

void QThreadFileManage::setPasteLocalToLocal(QString* src, QString* dest)
{
    _setWorkstepAndPath(nullptr, src, WS_PASTE_LOCAL_TO_LOCAL);
    filenameLocalToPaste = dest;
}

void QThreadFileManage::setDelete(char* filename)
{
    _setWorkstepAndPath(filename, WS_DELETE);
}

void QThreadFileManage::setWorkfile(char* filename)
{
    _setWorkstepAndPath(filename, WS_OPEN_WORKFILE);
}


void QThreadFileManage::setUpgrade(char* remote, QString* A9RomFile)
{
    _setWorkstepAndPath(remote, A9RomFile, WS_UPGRADE_COPY_TO_REMOTE);
}
void QThreadFileManage::_setWorkstepAndPath(char* remote, QString* local, int workstep)
{

    //    if(true != isRunning())
    //        return;
    //    qDebug()<<"workstep err"<<workstep;
    filenameRemote = remote;
    filenameLocal = local;
    this->workstep = workstep;
}

void QThreadFileManage::_onNotifyFileProgress(int progress)
{
    emit updateFileProgess(progress);
}

void QThreadFileManage::_setWorkstepAndPath(char* path, int workstep)
{
    if(true == isRunning())
        return;

    filenameOrDir = path;
    this->workstep = workstep;
}
