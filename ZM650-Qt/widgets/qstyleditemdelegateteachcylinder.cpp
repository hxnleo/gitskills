/*
 * qstyleditemdelegateteachcylinder.cpp
 *
 * 教导列表的代理类：气缸
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qstyleditemdelegateteachcylinder.h"
#include "qdialogteachcylinder.h"
#include <QAbstractItemModel>
#include <QPainter>
#include <QApplication>
#include <QFocusEvent>
#include <QBitArray>
#include "data/utils.h"

QStyledItemDelegateTeachCylinder::QStyledItemDelegateTeachCylinder(int rowCount, int maxCylinder, QObject* parent)
    : QStyledItemDelegateTeachBase(rowCount, parent)
{
    this->maxCylinder = maxCylinder;

}

void QStyledItemDelegateTeachCylinder::paint(QPainter* painter, const QStyleOptionViewItem& option,
        const QModelIndex& index) const
{
    _drawBorders(painter, option, index);

    auto value = index.model()->data(index, Qt::DisplayRole);

    if(false == value.isValid())
        return;

    auto bitArray = value.toBitArray();
    QString text;

    // bit较小的为第一个气缸
    for(auto bit = 0; bit < maxCylinder; ++bit)
    {
        text += (bitArray.at(bit) ? "1" : "0");
    }

    QTextOption o(Qt::AlignHCenter | Qt::AlignVCenter);//修改对齐 方向
#ifdef Arm_Kaiteli
    QFont font(QFont("/root/app/x_simsun.ttc"));
    font.setBold(true);
    painter->setFont(font);
#endif

    QRect innerRect(option.rect);
    innerRect.setLeft(innerRect.left() + 2);
    innerRect.setRight(innerRect.right() - 2);

    painter->drawText(innerRect, text, o);
}

QWidget* QStyledItemDelegateTeachCylinder::createEditor(QWidget* parent, const QStyleOptionViewItem& /*option*/,
        const QModelIndex& index) const
{
    auto value = index.model()->data(index, Qt::DisplayRole);

    if(false == value.isValid())
        return nullptr;
    auto editor = new QDialogTeachCylinder(maxCylinder, parent, Qt::FramelessWindowHint);

    editor->setModal(true);

    connect(editor, SIGNAL(accepted()),
            this, SLOT(onCommitAndCloseEditor()));
    connect(editor, SIGNAL(rejected()),
            this, SLOT(onCommitAndCloseEditor()));
    connect(editor, SIGNAL(writeCylinder(int, int)),
            this, SLOT(_onWriteCylinder(int, int)));
    return editor;
}

void QStyledItemDelegateTeachCylinder::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    auto bitArray = index.model()->data(index, Qt::DisplayRole).toBitArray();
    qobject_cast<QDialogTeachCylinder*>(editor)->setBitArray(bitArray);
}

void QStyledItemDelegateTeachCylinder::setModelData(QWidget* editor, QAbstractItemModel* model,
        const QModelIndex& index) const
{
    auto dialog = qobject_cast<QDialogTeachCylinder*>(editor);
    model->setData(index, dialog->getBitArray());

}

void QStyledItemDelegateTeachCylinder::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
        const QModelIndex&) const
{
    // TODO: set correct geomerty for Kinco
    QRect r(option.rect);
    r.setLeft(100);
    r.setRight(500);
    //debug_printf("rect=(%d,%d,%d,%d)",option.rect.left(),option.rect.right(),option.rect.top(),option.rect.bottom());
    editor->setGeometry(r);
}

void QStyledItemDelegateTeachCylinder::_onWriteCylinder(int cylinderIndex, int status)
{
    emit notifyTowriteCylinder(cylinderIndex, status);
}
