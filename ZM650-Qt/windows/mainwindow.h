/*
 * mainwindow.h
 *
 * 主界面
 *
 * Created on: 2019年 01月 21日 星期一 15:00:15 CST
 * Author: lixingcong
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <SysKey.h>
#include <QObject>
#include <QLabel>
#include <QTimer>
#include <QDateTime>
#include <QWidget>
#include <QThread>
#include <QMutex>
#include <QElapsedTimer>
#include <public.h>
#include "data/workfile.h"
#include "data/definitions.h"
#include "modbus/modbusqueue.h"
#include <QMouseEvent>


#define MENUWIDTHTEN 55 //十寸屏logo宽度
#define MENUWIDTHSEV 42
#define TENTOSEV 1024/800 //十寸屏和七寸屏的比例
#define PICTUR_PATH "/root/app/logo.bmp"

class QLabel;
class QThreadMainWindow;
class QThreadScreen;
namespace Ui
{
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    enum ModbusQueryAction
    {
        Action_ReadStatus,
        Action_WriteResetAlarm,
        Action_WriteManualMove,
        Action_WriteInterface, // 写当前界面
        Action_ReadWorkfileName, // 当前加工文件名
        Action_WriteHeartBeat, // 心跳包发送
    };

    struct ModbusQueryItemType
    {
        ModbusQueryAction queryAction;
        Definitions::MixDataType mixData;
        int workerQueryItemIndex;
    };

    bool inparam;

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();


signals:
    void _updateRealtimeInfo(); // 私有的
    void pointIndexChanged(int pointIndex);
    void totalPointChanged(int totalPoint);

public slots:
    void onTabIndexChanged(int tabIndex);
    void onMoveAxis(int axisNum, int direction, int pressType, double distance);
    void onNotifyOffline();

private slots:
    void timerUpDate();
    void timerScreen();

private:
    Ui::MainWindow* ui;
    QTimer *screenTimer;
    QLabel* labelCurrentTabHint;
    QWidget* lastSubWindow = nullptr;
    QThreadMainWindow* workerThread;
    QList<QString> TAB_BUTTON_NAMES;
    quint32 heartBeatCounter;

protected:
    void setSubWindow(QWidget* subWindow);
    //void writeInterface(int i);
    void readWorkfileName();
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void InitPattern();

protected slots:
    void onUpdateRealtimeInfo();
    void onButtonClicked_ResetAlarm();
    void _onClickedBottomButton(int btn_id);
    void _onOpenParnm();
public slots:
    void show(); // it was called only in main.cpp
    void writeInterface(int i);

    // Modbus Part Begin
protected:
    void _modbusQueryCallback(bool isOk, void* flag)
    {
        emit _signal_modbusQueryCallback(isOk, flag);
    }
protected slots:
    void onSendModbusQuery(MainWindow::ModbusQueryItemType* queryItem, int workerQueryItemIndex = -1);
    void onModbusQueryCallbackEnd(bool isOk, void* flag);
signals:
    void sendModbusQuery(MainWindow::ModbusQueryItemType* queryItem, int workerQueryItemIndex = -1);
    void _signal_modbusQueryCallback(bool isOk, void* flag);
protected:
    int modbusPendingCount;

signals:
    void _signal_notifyWorkerQueryDone(int workerQueryItemIndex); // 通知worker线程标记pending为false
    // Modbus Part End
};

class QThreadMainWindow: public QThread
{
    Q_OBJECT

public:
    struct ConstantlyQueryItemType
    {
        MainWindow::ModbusQueryAction queryType;
        qint64 interval;
        QElapsedTimer elapsedTimer;
        QMutex* pendingMutex;
    };
public:
    QThreadMainWindow(QObject* parent = nullptr);
    ~QThreadMainWindow();

    void run()override;

signals:
    void sendModbusQuery(MainWindow::ModbusQueryItemType* itemData, int workerQueryItemIndex);

public slots:
    void setQuitFlag(bool quitFlag = true);
    void queryDone(int workerQueryItemIndex);

protected:
    bool quitFlag;
    QList<ConstantlyQueryItemType> allQueries;
};


#endif // MAINWINDOW_H
