/*
 * qwidgetshowparams_daconfig.h
 *
 * DA测试界面
 *
 * Created on: 2019年 09月 2日 星期一 14:34:20 CST
 * Author: caojing
 */
#ifndef QWIDGETSHOWPARAMS_DACONFIG_H
#define QWIDGETSHOWPARAMS_DACONFIG_H

#include "qwidgetshowparamsbase.h"
#include <QMutex>
#include <QDebug>

class QWidgetShowParams_DAConfig : public QWidgetShowParamsBase
{
    enum ModbusQueryAction
    {
        Action_ReadIO,
        Action_WriteIO,
        Action_TeachSend,
    };
    enum DATypeData
    {//用来去区别上面还是下面
        One,
        Two,
    };
    Q_OBJECT
public:
    explicit QWidgetShowParams_DAConfig(int funcNameCount,QWidget *parent = nullptr);
    ~QWidgetShowParams_DAConfig();

    void daReadData();//数据读取
    float plus = 0.50;

    void DAInit();
signals:

public slots:
    void onChangData(float data);//获取改变的数据
    void onSingleData();//获取改变的数据
    void onSendDAModbus();//获取改变的数据
protected:
    QWidget* createFieldWidget(int index)override;
    QString getParamName(int offset)const override;
    bool getDataAuthority(int offset) const override;
    ModbusQueue::element_t* createModbusElement(int offset)const override;
    void onSendModbusOK(ModbusQueue::element_t* element) override;
    void onSendModbusFail(ModbusQueue::element_t* element) override;
    int funcNameCount;
    float* daArray;//存储da数据
    float* daInit;//初始化
    QList<QString> daName;//存储输入的值
    float getData;
    QMutex mutexSendDA;
};

#endif // QWIDGETSHOWPARAMS_DACONFIG_HQWidgetShowParams_IOConfig
