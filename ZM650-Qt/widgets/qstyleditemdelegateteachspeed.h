/*
 * qstyleditemdelegateteachspeed.h
 *
 * 教导代理：速度，限制输入0-100
 *
 * Created on: 2019年 01月 26日 星期六 17:05:50 CST
 * Author: lixingcong
 */

#ifndef QSTYLEDITEMDELEGATESPEED_H
#define QSTYLEDITEMDELEGATESPEED_H

#include "qstyleditemdelegateteachaxis.h"

class QStyledItemDelegateTeachSpeed : public QStyledItemDelegateTeachBase
{
    Q_OBJECT
public:
    QStyledItemDelegateTeachSpeed(int rowCount, QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
};

#endif // QSTYLEDITEMDELEGATESPEED_H
