/*
 * _qpushbuttonparamsbase.h
 *
 * 参数界面的按钮抽象类，其派生类支持点击弹出某一类型的对话框
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef _QPUSHBUTTONPARAMSBASE_H
#define _QPUSHBUTTONPARAMSBASE_H

#include <QPushButton>

class _QPushButtonParamsBase: public QPushButton
{
    Q_OBJECT
public:
    _QPushButtonParamsBase(QWidget* parent = nullptr):
        QPushButton(parent) {}
signals:
    void wroteValueFromUI();
public slots:
    virtual void readValueToUI() = 0;
protected slots:
    virtual void onClicked() = 0;
    virtual void onDialogAccepted() = 0;
};

#endif // _QPUSHBUTTONPARAMSBASE_H
