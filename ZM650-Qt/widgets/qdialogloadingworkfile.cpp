/*
 * qdialogloadingworkfile.cpp
 *
 * 加载加工文件进度条弹出对话框
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qdialogloadingworkfile.h"
#include <QLabel>
#include <QIcon>
#include "data/workfile.h"
#include "data/utils.h"
#include "data/globaldata.h"
#include "modbus/modbusqueue.h"

const QStringList QDialogLoadingWorkfile::hintText =
{
    QString::fromUtf8("正在加载文件，请稍候"),
    QString::fromUtf8("加载失败，请重试")
};

QDialogLoadingWorkfile::QDialogLoadingWorkfile(QWidget* parent) : QProgressDialog(parent, Qt::FramelessWindowHint),
    workerThread(new QThreadLoadingWorkfile(this))
{
    // no cancel button
    setWindowIcon(QIcon(":/images/images/NULL.ico"));
    setCancelButton(nullptr);
    setLabel(new QLabel(hintText.at(0), this));
    setRange(0, 100);

    connect(Workfile::getInstance(), SIGNAL(loading(int)),
            this, SLOT(setValue(int)));
    connect(Workfile::getInstance(), SIGNAL(fifoReadWriteDone()),
            this, SLOT(accept()));
    connect(Workfile::getInstance(), SIGNAL(fifoReadWriteFail()),
            this, SLOT(onQueryFail()));
}

QDialogLoadingWorkfile::~QDialogLoadingWorkfile()
{
    //debug_printf("~QDialogLoadingWorkfile()");
    Utils::waitForThreadFinished(workerThread);
}

void QDialogLoadingWorkfile::readWrite(bool isRead, int startIndex, int count, bool isHide)
{
    Utils::waitForThreadFinished(workerThread);
    workerThread->setMode(startIndex, count, isRead);
    // start thread
    workerThread->start();

    if(false == isHide)
        open();
}

void QDialogLoadingWorkfile::onQueryFail()
{
    setLabelText(hintText.at(1));

    if(Utils::dialogIsModbusRetry(this, GlobalData::modbusQueue))
    {
        GlobalData::modbusQueue->queryResume();
        Utils::waitForThreadFinished(workerThread);
        workerThread->start();
        setLabelText(hintText.at(0));
    }
    else
    {
        GlobalData::modbusQueue->queryStopThenOffline();
        reject();
    }
}
QThreadLoadingWorkfile::QThreadLoadingWorkfile(QObject* parent): QThread(parent)
{
    setMode(-1, 0, true);
}

void QThreadLoadingWorkfile::run()
{
    if(isRead)
    {
        if(startIndex < 0)
            Workfile::getInstance()->readAll();
        else
            Workfile::getInstance()->read(startIndex, count);
    }
    else
    {
        Workfile::getInstance()->write(startIndex, count);
    }
}
