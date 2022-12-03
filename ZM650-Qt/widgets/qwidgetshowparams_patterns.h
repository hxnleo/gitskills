/*
 * qwidgetshowparams_patterns.h
 *
 * 显示所有花样
 *
 * Created on: 2019年 01月 26日 星期六 17:05:50 CST
 * Author: lixingcong
 */

#ifndef QWIDGETSHOWPARAMS_PATTERNS_H
#define QWIDGETSHOWPARAMS_PATTERNS_H

#include "qwidgetshowparamsbase.h"
#include "data/patterns.h"
#include "qpushbuttonparams.h"
#include "qdialogparams_patterns.h"
#include <QPaintEvent>
#include <QStyleOptionButton>
#include <QPainter>
#include <QIcon>

class QPushButtonPattern;

class QWidgetShowParams_Patterns : public QWidgetShowParamsBase
{
    Q_OBJECT
public:
    explicit QWidgetShowParams_Patterns(QWidget* parent = nullptr);
    ~QWidgetShowParams_Patterns();

    int getPatternCount()const
    {
        return adapter->getCount();
    }

protected:
    QWidget* createFieldWidget(int index)override;
    QString getParamName(int offset)const override;
    bool getDataAuthority(int offset) const override;
    ModbusQueue::element_t* createModbusElement(int offset) const override;
    void onSendModbusOK(ModbusQueue::element_t* element)override;
    void onSendModbusFail(ModbusQueue::element_t* element)override;

protected:
    Adapters::DataAdapter_Patterns* adapter;
    bool isGenerating;

signals:
    void dismissProgressDialog();

protected slots:
    void onWroteGoodValue();
    void onUpdateRealtimeInfo();
};

class QPushButtonPattern: public QPushButtonParams<QDialogParams_Patterns, Adapters::DataAdapter_Patterns>
{
    Q_OBJECT
public:
    QPushButtonPattern(Adapters::DataAdapter_Patterns* adapter, int adapterOffset, QWidget* parent):
        QPushButtonParams<QDialogParams_Patterns, Adapters::DataAdapter_Patterns>(adapter, adapterOffset, parent)
    {
        //setMinimumHeight(60);
        // 绘制图标
        auto pattern = adapter->getData(adapterOffset).value<Adapters::PatternItem>();
        setIcon(QIcon(pattern.resourceName[0]));
        setIconSize(QSize(120, 110));
    }

protected:
    void paintEvent(QPaintEvent* event)override
    {
        if(!icon().isNull())
        {
            QStyleOptionButton option;
            initStyleOption(&option);
            option.text =
                QString(""); // set the empty string so that only icon is drawn and not the text, when we specify both icon and text.
            QPainter painter(this);
            style()->drawControl(QStyle::CE_PushButton, &option, &painter, this);
        }
        else
        {
            QPushButton::paintEvent(event);
        }
    }

};

#endif // QWIDGETSHOWPARAMS_PATTERNS_H
