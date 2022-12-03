/*
 * qstyleditemdelegateteachbase.cpp
 *
 * 教导列表的代理基类，绘制高亮边框等
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qstyleditemdelegateteachbase.h"
#include "data/globaldata.h"
#include <QLineEdit>
#include <QPainter>
#include <QEvent>
#include <QDebug>
#include <QApplication>
#include <QInputContext>

QStyledItemDelegateTeachBase::QStyledItemDelegateTeachBase(int rowCount, QObject* parent): QStyledItemDelegate(parent)
{
    this->selectedColumn = 0;
    this->rowCount = rowCount;
}

void QStyledItemDelegateTeachBase::paint(QPainter* painter, const QStyleOptionViewItem& option,
        const QModelIndex& index) const
{
#if 1
    _drawBorders(painter, option, index);
#endif

    QStyledItemDelegate::paint(painter, option, index);
}

void QStyledItemDelegateTeachBase::onCommitAndCloseEditor()
{
    auto* editor = qobject_cast<QWidget*>(sender());

    emit commitData(editor);
    emit closeEditor(editor);
}

void QStyledItemDelegateTeachBase::_drawBorders(QPainter* painter, const QStyleOptionViewItem& option,
        const QModelIndex& index) const
{
    auto old_pen = painter->pen();
    auto new_pen = QPen(old_pen);
    new_pen.setWidth(3);
    new_pen.setColor(QColor(0xff,0x00,0x00));

//    // background color
    if(index.row() % 2 == 1)
    {//表格背景颜色
        painter->fillRect(option.rect, QColor(0x00, 0x98, 0xF8));
    }
    else
    {
        painter->fillRect(option.rect, QColor(0x48, 0x48, 0xF8));
    }
    {//表格字体颜色
        auto com_pen = painter->pen();
        com_pen.setColor(QColor(0xE0, 0xFF, 0xFF));
        painter->setPen(com_pen);
        //painter->drawLine(option.rect.topLeft(), option.rect.bottomLeft());
        painter->drawLine(option.rect.topRight(), option.rect.bottomRight());
        //painter->drawLine(option.rect.topLeft(), option.rect.topRight());
        painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
    }

    // border
    if(index.column() == selectedColumn)
    {
        GlobalData::swithcolumn = selectedColumn;
        painter->setPen(new_pen);

        painter->drawLine(option.rect.topLeft(), option.rect.bottomLeft());
        painter->drawLine(option.rect.topRight(), option.rect.bottomRight());

        if(index.row() == 0)
            painter->drawLine(option.rect.topLeft(), option.rect.topRight());

        if(index.row() == rowCount - 1)
            painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());

//        painter->setPen(old_pen);
        //设置选中列颜色
        auto com_pen = painter->pen();
        com_pen.setColor(QColor(0x22, 0x22, 0x29));
        painter->setPen(com_pen);
    }
}

bool QStyledItemDelegateTeachBase::eventFilter(QObject* object, QEvent* event)
{//孔的位置编辑
    switch(event->type())
    {
    case QEvent::FocusOut:
        if(true == qApp->inputContext()->isComposing()) // 正在编辑
            return true;

        break;

    default:
        break;
    }

    return QStyledItemDelegate::eventFilter(object, event);
}
