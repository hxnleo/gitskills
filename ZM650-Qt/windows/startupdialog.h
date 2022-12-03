/*
 * startupdialog.h
 *
 * 启动加载参数
 *
 * Created on: 2019年 01月 21日 星期一 15:00:16 CST
 * Author: lixingcong
 */
#ifndef STARTUPDIALOG_H
#define STARTUPDIALOG_H

#include <QDialog>
#include <QThread>
#include <QPainter>
#include <QDesktopWidget>
#include "widgets/qwidgetdiagnose_dogdecrypt.h"

namespace Ui
{
class StartUpDialog;
}

class QThreadStartupWindow;
class ModbusQueue;

class StartUpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StartUpDialog(QWidget* parent = nullptr);
    ~StartUpDialog();

signals:
    void continu();

protected slots:
    void onButtonClickedContinue();
    void onButtonClickedIgnore();
    void onWorkStepChanged(int workstep);
    void reject();
    void onFailed();
    void onNoTime();
    void onCreatDecrypt();
    void decryptFail();
    void decryptOk();
    void onQuitFialConnect();



protected:
    void forceStopThread();


private:
    Ui::StartUpDialog* ui;
    QThreadStartupWindow* workerThread;
};

class QThreadStartupWindow: public QThread
{
    Q_OBJECT
public:
    QThreadStartupWindow(ModbusQueue* mbq, QObject* parent = nullptr);
    ~QThreadStartupWindow();

    void setWorkStep(int workstep);
    void run()override;
    bool timeok;
    int dogflag;//加密狗解密标志位

signals:
    void workStepChanged(int workstep);
    void failed();
    void noTime();
    void creatDecrypt();

public slots:
    void setQuitFlag(bool quitFlag = true);

protected:
    bool quitFlag;
    int workstep;
    ModbusQueue* mbq;

protected: // functions
    bool testConnection();
};

#endif // STARTUPDIALOG_H
