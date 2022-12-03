/*
 * qstyleditemdelegateteachcombobox.h
 *
 * 教导列表的代理类：单选框
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QSTYLEDITEMDELEGATETEACHCOMBOBOX_H
#define QSTYLEDITEMDELEGATETEACHCOMBOBOX_H

#include "qstyleditemdelegateteachbase.h"
#include <QEvent>

class QStyledItemDelegateTeachComboBox : public QStyledItemDelegateTeachBase
{
    Q_OBJECT
public:
    QStyledItemDelegateTeachComboBox(int rowCount, const QStringList& stringList, QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

protected slots:
    void onComboboxActivated(int);

protected:
    QStringList stringList;
};

#endif // QSTYLEDITEMDELEGATETEACHCOMBOBOX_H
