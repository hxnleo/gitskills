/*
 * qstyleditemdelegateteachcombobox.cpp
 *
 * 教导列表的代理类：单选框
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qstyleditemdelegateteachcombobox.h"
#include <QComboBox>
#include <QDebug>
#include <QAbstractItemModel>
#include <QPainter>
#include <QApplication>
#include <QFocusEvent>
#include <QListView>
#include "data/uistyles.h"

QStyledItemDelegateTeachComboBox::QStyledItemDelegateTeachComboBox(int rowCount, const QStringList& stringList,
        QObject* parent)
    : QStyledItemDelegateTeachBase(rowCount, parent)
{
    this->stringList = stringList;
}

void QStyledItemDelegateTeachComboBox::paint(QPainter* painter, const QStyleOptionViewItem& option,
        const QModelIndex& index) const
{
    _drawBorders(painter, option, index);

    auto value = index.model()->data(index, Qt::DisplayRole);

    if(false == value.isValid())
        return;

    int stringIndex = value.toInt();
    QTextOption o(Qt::AlignHCenter | Qt::AlignVCenter);//修改对齐 方向
    //字体修改需要设置 QPainter 绘画者
#ifdef Arm_Kaiteli
    QFont font(QFont("/root/app/x_simsun.ttc"));
    font.setBold(true);
    painter->setFont(font);
#endif

    QRect innerRect(option.rect);
    innerRect.setLeft(innerRect.left() + 2);
    innerRect.setRight(innerRect.right() - 2);

    if(stringIndex < 0 || stringIndex >= stringList.size())
        painter->drawText(innerRect, QString("错误索引=%1").arg(stringIndex), o);
    else
        painter->drawText(innerRect, stringList.at(stringIndex), o);
}

QWidget* QStyledItemDelegateTeachComboBox::createEditor(QWidget* parent, const QStyleOptionViewItem& /*option*/,
        const QModelIndex& index) const
{
    const QMargins defaultMargins(2, 0, 2, 0);

    auto value = index.model()->data(index, Qt::DisplayRole);

    if(false == value.isValid())
        return nullptr;

    auto editor = new QComboBox(parent);
    editor->setView(new QListView(editor));//弹出的时候使用设定的item
    editor->setStyleSheet(UIStyles::CSS_TEACH_COMBOBOX);

    editor->addItems(stringList);
    editor->setContentsMargins(defaultMargins);

    connect(editor, SIGNAL(activated(int)),
            this, SLOT(onComboboxActivated(int)));
    return editor;
}

void QStyledItemDelegateTeachComboBox::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    int stringIndex = index.model()->data(index, Qt::DisplayRole).toInt();
    auto cb = qobject_cast<QComboBox*>(editor);
    cb->setCurrentIndex(stringIndex);

}

void QStyledItemDelegateTeachComboBox::setModelData(QWidget* editor, QAbstractItemModel* model,
        const QModelIndex& index) const
{
    int stringIndex = qobject_cast<QComboBox*>(editor)->currentIndex();
    model->setData(index, stringIndex);

}

void QStyledItemDelegateTeachComboBox::onComboboxActivated(int)
{
    auto* editor = qobject_cast<QWidget*>(sender());
    QApplication::sendEvent(editor, new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier));
}
