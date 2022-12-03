/*
 * qwidgetshowparams_axisenable.h
 *
 * 参数界面的IO配置
 *
 * Created on: 2020年 01月 5日 星期一 14:57:40 CST
 * Author: caojing
 */
#ifndef QWIDGETSHOWPARAMS_AXISENABLE_H
#define QWIDGETSHOWPARAMS_AXISENABLE_H

#include "widgets/qwidgetshowparamsbase.h"
#include "data/adapters.h"
#include "data/definitions.h"

class QTimer;
class QMutex;

class QWidgetShowParams_AxisEnable : public QWidgetShowParamsBase
{
    Q_OBJECT
protected:
    enum ModbusQueryAction
    {
        Action_RAXISEBALE,
        Action_WAXISEBALE,
    };

public:
    //status 1输入 0输出
    QWidgetShowParams_AxisEnable(QWidget* parent = nullptr);
    ~QWidgetShowParams_AxisEnable();

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
    QTimer* timer;
    QMutex mutexReadIO;
    unsigned short* AixsStatusArray;
    unsigned short* AixsStatusArrayLast;
    bool isForceRefresh;

protected:
    void refreshIOStatus();
};



#endif // QWIDGETSHOWPARAMS_AXISENABLE_H
