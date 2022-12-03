/*
 * subwindowpatterns.h
 *
 * 花样子窗口
 *
 * Created on: 2019年 01月 26日 星期六 17:02:46 CST
 * Author: lixingcong
 */

#ifndef SUBWINDOWPATTERNS_H
#define SUBWINDOWPATTERNS_H

#include <QWidget>
#include <QVBoxLayout>
#include "data/patterns.h"

class QWidgetShowParams_Patterns;

class SubWindowPatterns : public QWidget
{
    Q_OBJECT
public:
    explicit SubWindowPatterns(QWidget* parent = nullptr);

protected:
    QVBoxLayout* layout_v;
    QWidgetShowParams_Patterns* widget;
    Adapters::DataAdapter_Patterns adapter;
};

#endif // SUBWINDOWPATTERNS_H
