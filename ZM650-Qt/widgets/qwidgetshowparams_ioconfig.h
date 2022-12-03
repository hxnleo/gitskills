/*
 * qwidgetshowparams_ioconfig.h
 *
 * 参数界面的IO配置
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QWIDGETSHOWPARAMS_IOCONFIG_H
#define QWIDGETSHOWPARAMS_IOCONFIG_H

#include "qwidgetshowparamsbase.h"
#include "data/adapters.h"

class QWidgetShowParams_IOConfig : public QWidgetShowParamsBase
{
public:
    QWidgetShowParams_IOConfig(IO_FUN_NAME* funCodeName, IOConfig* ioConfig, quint16 modbusAddr,int status,QWidget* parent = nullptr);
    ~QWidgetShowParams_IOConfig();

protected:
    QWidget* createFieldWidget(int index)override;
    QString getParamName(int offset)const override;
    bool getDataAuthority(int offset) const override;
    void onSendModbusOK(ModbusQueue::element_t*)override{}
    void onSendModbusFail(ModbusQueue::element_t*)override{}
    ModbusQueue::element_t* createModbusElement(int offset) const override;

protected:
    Adapters::DataAdapter_IOConfigText* adapter;
};

#endif // QWIDGETSHOWPARAMS_IOCONFIG_H
