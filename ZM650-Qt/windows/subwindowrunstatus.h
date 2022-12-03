/*
 * subwindowrunstatus.h
 *
 * 运行状态界面
 *
 * Created on: 2019年 01月 21日 星期一 15:00:16 CST
 * Author: lixingcong
 */
#ifndef SUBWINDOWRUNSTATUS_H
#define SUBWINDOWRUNSTATUS_H

#include <QWidget>
#include <Teach.h>
#include "modbus/modbusqueue.h"
#include <SysKey.h>
#include "data/definitions.h"
#ifdef ARM
#define MIAN_SPACE 80
class QLabel;

namespace Ui
{
class SubWindowRunStatus;
}

class SubWindowRunStatus : public QWidget
{
    Q_OBJECT

public:
    enum ModbusQueryAction
    {
        Action_WriteWorkCommand, // 控制器启动 停止等
        Action_WriteWorkMode, // 自动 半自动 试机
        Action_TeachSend,
        Action_ClearProduced,
        Action_SetVFD, // 设置变频器速比
        Action_WriteButtonCommand,
        Action_WriteClamp // 写夹具输出
    };

public:
    explicit SubWindowRunStatus(QWidget* parent = 0);
    ~SubWindowRunStatus();


signals:
    void workstausChangedToStop();
    void updatePostion();
    void setTrailCurrentPoint(int point);
    void setTrailTotalPoint(int point);
    void resetTrailToFit();
    void closedialog();

public slots:
    void onUpdateRealtimeInfo();
    void onTimerUpdate();

protected slots:
    void onButtonClickedStart();
    void onButtonClicked_FindZero();
    void onButtonClicked_GoToStart();
    void onButtonClicked_GoToStart1();
    void onButtonClicked_Stop();
    void onButtonClicked_Produced();
    void onComboIndexActivated(int);
    void _onClickedBottomButton(int btn_id);
    void onDialogAcceptedLocate();
    void onButtonClicked_Speed1();
    void onButtonClicked_Speed2();
    void onButtonClicked_SpeedValue();
    void onButtonClicked_Table1();
    void onButtonClicked_Table2();
    void onButtonClicked_Drill();
    void onButtonClicked_Drill2();
    void onButtonClicked_MainAxisBrake();
    void onButtonClicked_Hair();
    void onButtonClicked_ClampOut();
    void onWriteClamp(int cylinderIndex, int /*status*/);
    void onDialogAcceptedInputVFD();
    void onFindZero_Machine();
    void onFindZero_MainAxis();
    void onDoubleClickCentralWidget();
    void pickUpPositionData();
    void onTimeoutSetTrailToFit();
    void onButtonClicked_Cylinder();
    void onButtonClicked_DrillHole();//当前钻头孔加工孔位数  add 2022.11.30
protected:
    void sendWorkCommand(KEYVALUE keyValue);
    void sendTEACHSEND(TEACH_SEND* teachSend);
    void sendVFD(quint8 value); // 变频器
    void sendButtonCommand(IO_Op_Type type);
    void switchCentralWidget();
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    // Modbus Part Begin
protected:
    void _modbusQueryCallback(bool isOk, void* flag)
    {
        emit _signal_modbusQueryCallback(isOk, flag);
    }
protected slots:
    void onSendModbusQuery(Definitions::ModbusQueryItem*);
    void onModbusQueryCallbackEnd(bool isOk, void* flag);
signals:
    void _signal_modbusQueryCallback(bool isOk, void* flag);
    void sendModbusQuery(Definitions::ModbusQueryItem*);
protected:
    int modbusPendingCount;
    QPoint dialogStart;
    QPoint dialogEnd;
    // Modbus Part End

private:
    int locatePointIndex;
    quint8 vfdSpeed; // 变频器速比，用于对话框输入
    float positions[MAXTEACHNUM][2]; // 加工点坐标
    int pointColors[MAXTEACHNUM]; // 加工点颜色
    quint8 lastWorkStatus;//上一个运动状态
    quint32 lastPointNumber;//上一个点
    QTimer* timer;
    Ui::SubWindowRunStatus* ui;

};
#endif
#endif // SUBWINDOWRUNSTATUS_H
