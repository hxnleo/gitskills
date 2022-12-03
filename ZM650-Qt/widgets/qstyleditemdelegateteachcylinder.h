/*
 * qstyleditemdelegateteachcylinder.h
 *
 * 教导列表的代理类：气缸
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QSTYLEDITEMDELEGATETEACHDIALOG_H
#define QSTYLEDITEMDELEGATETEACHDIALOG_H

#include "qstyleditemdelegateteachbase.h"
#include <QEvent>

class QStyledItemDelegateTeachCylinder : public QStyledItemDelegateTeachBase
{
    Q_OBJECT
public:
    QStyledItemDelegateTeachCylinder(int rowCount, int maxCylinder, QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex&) const;

signals:
    void notifyTowriteCylinder(int cylinderIndex, int status);

protected:
    int maxCylinder;
    static const QStringList NAMES;

protected slots:
    void _onWriteCylinder(int cylinderIndex, int status);
};

#endif // QSTYLEDITEMDELEGATETEACHDIALOG_H
