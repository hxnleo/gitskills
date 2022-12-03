/*
 * qpushbuttonparams.h
 *
 * 参数界面的按钮模板类
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QPUSHBUTTONPARAMS_H
#define QPUSHBUTTONPARAMS_H

#include "widgets/_qpushbuttonparamsbase.h"
/*
 * 模板类TDialog为QDialog的子类，需要实现以下函数：
 * 构造函数
 *    constructor(Adapters::DataAdapterBase* adapter, int offset, QWidget* parent, Qt::WindowFlags f);
 *
 * 静态函数
 *    static QString getStringFromData(const QVariant& val);
 *
 * 对话框“确认”后的写入动作slot，将UI处理后的数字写入到pData内
 *    void accept();
 *
 * 屏蔽按"Enter"回车键的操作，否则输入数字后回车默认关闭窗口
 *    void keyPressEvent(QKeyEvent *event)override;
 *
*/
template<class TDialog, class TAdapter>
class QPushButtonParams: public _QPushButtonParamsBase
{
public:
    QPushButtonParams(TAdapter* adapter, int adapterOffset, QWidget* parent):
        _QPushButtonParamsBase(parent),
        adapter(adapter),
        adapterOffset(adapterOffset)
    {
        connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));
        readValueToUI();
    }

protected:
    TAdapter* adapter;
    int adapterOffset;
    int status;

public slots:
    void readValueToUI() override
    {
        setText(TDialog::getStringFromData(adapter->getData(adapterOffset)));
    }

protected slots:
    void onClicked() override
    {
        auto dialog = new TDialog(adapter,adapterOffset,this); //Qt::FramelessWindowHint);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        connect(dialog, SIGNAL(accepted()), this, SLOT(onDialogAccepted()));
        dialog->open();
    }

    void onDialogAccepted() override
    {
        //qDebug("Accept and wrote!!!");
        emit wroteValueFromUI();
        readValueToUI();
    }
};

#endif // QPUSHBUTTONPARAMS_H
