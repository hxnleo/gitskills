/*
 * qwidgetshowparams_paramtable.h
 *
 * 参数界面的的SysText.c显示项目
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QWIDGETSHOWPARAMS_PARAMTABLE_H
#define QWIDGETSHOWPARAMS_PARAMTABLE_H

#include "qwidgetshowparamsbase.h"
#include "qdialogparams_authority.h"
#include <SysText.h>
#include <QKeyEvent>
#include "data/adapters.h"

class QWidgetShowParams_ParamTable : public QWidgetShowParamsBase
{
    Q_OBJECT
public:

    enum OType
    {//用来去区别上面还是下面
        One,
        Two,
        Three,
    };

    QWidgetShowParams_ParamTable(TParamTable* paramTable,QWidget* parent = nullptr);//mod by yzg
    QWidgetShowParams_ParamTable(TParamTable* paramTable,int startnameindex=0, QWidget* parent = nullptr);//add by yzg
    ~QWidgetShowParams_ParamTable();

    int type;//区分原点设置
    int lastcolor0;
    int lastcolor1;
    int lastcolor2;

protected:
    QWidget* createFieldWidget(int index)override;
    QString getParamName(int offset)const override;
    bool getDataAuthority(int offset) const override;
    void onSendModbusOK(ModbusQueue::element_t*)override{}
    void onSendModbusFail(ModbusQueue::element_t*)override{}
    ModbusQueue::element_t* createModbusElement(int offset) const override;
#ifdef Arm_460Y
    void sendOffest(void* data, float size, bool write = true);
#endif
public slots:
#ifdef Arm_460Y
    void onOpenDialog();
    void onSetOffset();
    void onMoveAxis(int axisNum, int direction, int pressType, double distance);
    void onrestore();
#endif
    void onUpdataTeachAxis();
    void onUpdataSysAxis();

signals:
    void moveAxis(int axisNum, int direction, int pressType, double distance);
    void openOffset(int);
protected:
    Adapters::DataAdapter_ParamTable* adapter;

    void keyPressEvent(QKeyEvent* evt)
    {
        if(evt->key() == Qt::Key_Enter || evt->key() == Qt::Key_Return)
            return;

        QWidget::keyPressEvent(evt);
    }
};

#endif // QWIDGETSHOWPARAMS_PARAMTABLE_H
