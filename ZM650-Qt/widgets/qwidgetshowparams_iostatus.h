/*
 * qwidgetshowparams_iostatus.h
 *
 * 诊断界面的IO状态
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QWIDGETSHOWPARAMS_IOSTATUS_H
#define QWIDGETSHOWPARAMS_IOSTATUS_H

#include "qwidgetshowparamsbase.h"
#include "data/adapters.h"
#include "data/definitions.h"

class QTimer;
class QMutex;

class QWidgetShowParams_IOStatus : public QWidgetShowParamsBase
{
    Q_OBJECT
protected:
    enum ModbusQueryAction
    {
        Action_ReadIO,
        Action_WriteIO,
    };

public:
    //status 1输入 0输出
    QWidgetShowParams_IOStatus(const QString& prefixString, IOConfig* ioConfigBaseAddr, IO_FUN_NAME* funcNameAddr,
                               int funcNameCount, unsigned short modbusAddress, int status, bool isAllowClick,QWidget* parent = nullptr);
    ~QWidgetShowParams_IOStatus();

protected slots:
    void onTimeout();

protected:
    QWidget* createFieldWidget(int index)override;
    QString getParamName(int offset)const override;
    bool getDataAuthority(int offset) const override;
    ModbusQueue::element_t* createModbusElement(int offset)const override;
    void onSendModbusOK(ModbusQueue::element_t* element)override;
    void onSendModbusFail(ModbusQueue::element_t* element)override;
    void onRefreshPageDone()override;

protected:
    Adapters::DataAdapter_IOStatus* adapter;
    IOConfig* ioConfigBaseAddr;
    int status;
    bool isAllowClick;
    unsigned short modbusAddress;
    int funcNameCount;
    QTimer* timer;
    QMutex mutexReadIO;
    unsigned short* ioStatusArray;
    unsigned short* ioStatusArrayLast;
    QString prefixString;
    bool isForceRefresh;

protected:
    void refreshIOStatus();
};

#endif // QWIDGETSHOWPARAMS_IOSTATUS_H
