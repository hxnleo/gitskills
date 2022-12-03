/*
 * qstyleditemdelegateteachbase.h
 *
 * 教导列表的代理基类，绘制高亮边框等
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QSTYLEDITEMDELEGATETEACHBASE_H
#define QSTYLEDITEMDELEGATETEACHBASE_H

#include <QStyledItemDelegate>
#include <QVariant>

class QPainter;

class QStyledItemDelegateTeachBase : public QStyledItemDelegate
{
    Q_OBJECT

public:
    QStyledItemDelegateTeachBase(int rowCount, QObject* parent = nullptr);
    // override functions
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

signals:
    void editorClicked(int col);

public slots:
    void onSelectedColumnChanged(int selectedColumn)
    {
        this->selectedColumn = selectedColumn;
        //qDebug()<<"selectedColumn"<<selectedColumn;
    }

protected slots:
    void onCommitAndCloseEditor();

protected:
    int selectedColumn;
    int rowCount;

protected:
    void _drawBorders(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

protected:
    bool eventFilter(QObject* object, QEvent* event);
};

#endif // QSTYLEDITEMDELEGATETEACHBASE_H
