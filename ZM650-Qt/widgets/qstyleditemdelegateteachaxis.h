/*
 * qstyleditemdelegateteachaxis.h
 *
 * 教导列表的代理类：轴数字输入
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QSTYLEDITEMDELEGATETEACHAXIS_H
#define QSTYLEDITEMDELEGATETEACHAXIS_H

#include "qstyleditemdelegateteachbase.h"
#include <QLineEdit>
#include <QAbstractItemModel>
#include <QPainter>
#include "data/utils.h"
#include "widgets/myinputpanelcontext.h"
#include "QApplication"

class QStyledItemDelegateTeachAxis : public QStyledItemDelegateTeachBase
{
    Q_OBJECT
public:
    QStyledItemDelegateTeachAxis(int rowCount, QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
};

#endif // QSTYLEDITEMDELEGATETEACHAXIS_H
