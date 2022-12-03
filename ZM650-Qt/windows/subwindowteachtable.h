/*
 * subwindowteachtable.h
 *
 * 教导界面
 *
 * Created on: 2019年 01月 21日 星期一 15:00:16 CST
 * Author: lixingcong
 */
#ifndef SUBWINDOWTEACHTABLE_H
#define SUBWINDOWTEACHTABLE_H

#include <QWidget>
#include <QMouseEvent>
#include <Teach.h>
#include "data/definitions.h"
#include "data/globaldata.h"
#ifndef Arm_662
namespace Ui
{
class SubWindowTeachTable;
}

class QModelIndex;

class SubWindowTeachTable : public QWidget
{
    Q_OBJECT

public:
    explicit SubWindowTeachTable(QWidget* parent = 0);
    ~SubWindowTeachTable();

public: // public definations
    enum MoveButtonDirection
    {
        MovePlus,
        MoveMinus
    };
    enum ActionEnum
    {
        Action_TeachSend,
        Action_EditCell,
        Action_WriteCylinder, // 写编程输出气缸
        Action_TeachStep,//写教导步长  add by yzg 190529
        Action_WriteButtonCommand,
    };

protected: // protected definations
    struct TEACH_DIALOG_VARS_T
    {
        int startIndex;
        int endIndex;
        int divides;
        int color;
        int spindle;
    };

signals:
    void moveAxis(int axisNum, int direction, int pressType, double distance);

signals: // private signals
    void TableChangePageNum(int pageNum);
    void workstausChangedToStop();
    void SetTeachStepText();//add by yzg 190529
    void closedialog();
    void linkage();
    void quickteach(bool);

public slots:
    void onUpdateRealtimeInfo();
    void onPointIndexChanged(int pointIndex);
    void onTotalPointChanged(int totalPoint);
    void onPointDataChangedByUserInput(int pointIndex);
    void onReloadPageTeachData();


protected slots:
    void onDialogAcceptedSplit();
    void onDialogAcceptedCopy();
    void onDialogAcceptedLocate();
    void onDialogAcceptedDeleteMultiple();
    void onDialogAcceptedSpindleMultiple();
    void onDialogAcceptedDeleteRcet();
    void onDialogAcceptedMultiColor();
    void onDialogAcceptedShift();
    void _onPmoveToPoint(int pointIndex);
    void _onMoveButtonPress(int pressType);
    void _onClickedBottomButton(int btn_id);
    void _onNotifyTowriteCylinder(int cylinderIndex, int status);
    void _onsendTeachAction(unsigned int data);//add by yzg 190529
    void _onsendQuickTeach();
    void _onAddNewPoint();

private:
    Ui::SubWindowTeachTable* ui;

protected:
    TEACH_DIALOG_VARS_T teachDialogVars;
    TEACH_SEND delteachSend ;

protected: // functions
    void sendTEACHSEND(TEACH_SEND* teachSend);
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
    void sendModbusQuery(Definitions::ModbusQueryItem*);
    void _signal_modbusQueryCallback(bool isOk, void* flag);
protected:
    int modbusPendingCount;
    QPoint dialogStart;
    QPoint dialogEnd;
    // Modbus Part End
};
#endif
#endif // SUBWINDOWTEACHTABLE_H
