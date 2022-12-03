/*
 * qpushbuttonstab.h
 *
 * 单列PushButton，用于页面顶部的标签
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QPUSHBUTTONSTAB_H
#define QPUSHBUTTONSTAB_H

#include <QWidget>
#include <QList>
#include "data/definitions.h"

class QPushButton;

class QPushButtonsTab : public QWidget
{
    Q_OBJECT
public:
    explicit QPushButtonsTab(QWidget* parent = nullptr);

    void init(const QStringList& tabNames);
    void setTabActive(int tabIndex);

signals:
    void tabClicked(int id);

protected slots:
    void _buttonClicked(QWidget* w);

protected:
    Definitions::WidgetMapList<QPushButton, QString> maplist;
    bool isInitialized;
};

#endif // QPUSHBUTTONSTAB_H
