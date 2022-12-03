/*
 * qwidgetdiagnoseversion.h
 *
 * 诊断界面的加密狗信息
 *
 * Created on: 2019年 11月 4日 星期一 11:30:40 CST
 * Author: Caojing
 */
#ifndef QWIDGETDIAGNOSEDOG_H
#define QWIDGETDIAGNOSEDOG_H
#include <../ZM650/src/softdog/Dog_public.h>
#include <QDialog>

class QLabel;

class QWidgetDiagnoseDog : public QDialog
{
    Q_OBJECT
public:
    explicit QWidgetDiagnoseDog(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~QWidgetDiagnoseDog();

protected:
    void _modbusQueryCallback(bool isOk, void* flag)
    {
        emit _signal_modbusQueryCallback(isOk, flag);
    }

protected slots:
    void onModbusQueryCallbackEnd(bool isOk, void* flag);
signals:
    void _signal_modbusQueryCallback(bool isOk, void* flag);
};

#endif // QWIDGETDIAGNOSEDOG_H

