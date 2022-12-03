/*
 * QStyledItemDelegateTeachPushButton.cpp
 *
 * 教导列表的代理类：单选框
 *
 * Created on: 2020年 04月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qstyleditemdelegateteachpushbutton.h"
#include <QPushButton>
#include <QDebug>
#include <QAbstractItemModel>
#include <QPainter>
#include <QApplication>
#include <QFocusEvent>
#include <QListView>
#include "data/uistyles.h"
#include "data/utils.h"

QStyledItemDelegateTeachPushButton::QStyledItemDelegateTeachPushButton(int rowCount, const QStringList& stringList,
        QObject* parent)
    : QStyledItemDelegateTeachBase(rowCount, parent)
{
    this->stringList = stringList;
}

void QStyledItemDelegateTeachPushButton::paint(QPainter* painter, const QStyleOptionViewItem& option,
        const QModelIndex& index) const
{
    _drawBorders(painter, option, index);

    auto value = index.model()->data(index, Qt::DisplayRole);

    if(false == value.isValid())
        return;

    int status = value.toInt();
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
    painter->drawText(innerRect, stringList.at(status), o);
}

QWidget* QStyledItemDelegateTeachPushButton::createEditor(QWidget* parent, const QStyleOptionViewItem& /*option*/,
        const QModelIndex& index) const
{
    const QMargins defaultMargins(2, 0, 2, 0);

    auto value = index.model()->data(index, Qt::DisplayRole);

    if(false == value.isValid())
        return nullptr;

    auto editor = new QPushButton(parent);

    editor->setText(stringList.at(0));
    editor->setContentsMargins(defaultMargins);

//    connect(editor, SIGNAL(clicked()),
//            this, SLOT(onClickButton()));
    return editor;
}

void QStyledItemDelegateTeachPushButton::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    int status = index.model()->data(index, Qt::DisplayRole).toInt();
    QString text = stringList.at(status);
    if(text == stringList.at(0))
    {//关开
        qobject_cast<QPushButton*>(editor)->setText(stringList.at(1));
    }
    else
    {
        qobject_cast<QPushButton*>(editor)->setText(stringList.at(0));
    }
    //模仿按钮按下确定
    QApplication::sendEvent(editor, new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier,QString(Qt::Key_Enter)));
    QApplication::sendEvent(editor, new QKeyEvent(QEvent::KeyRelease, Qt::Key_Enter, Qt::NoModifier,QString()));
//    qobject_cast<QPushButton*>(editor)->setText(text);


}

void QStyledItemDelegateTeachPushButton::setModelData(QWidget* editor, QAbstractItemModel* model,
        const QModelIndex& index) const
{
    int status = 0;
    QString text = qobject_cast<QPushButton*>(editor)->text();
    if(text == stringList.at(0))
    {//关开
        status = 0;
    }
    else
    {
        status = 1;
    }
    qDebug()<<"status"<<status;
    model->setData(index, status);

}

void QStyledItemDelegateTeachPushButton::onClickButton()
{
    auto* editor = qobject_cast<QPushButton*>(sender());
    QString text = editor->text();
    if(text == stringList.at(0))
    {//关开
        editor->setText(stringList.at(1));
    }
    else
    {
        editor->setText(stringList.at(0));
    }
    //模仿按钮按下确定
    QApplication::sendEvent(editor, new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier,QString(Qt::Key_Enter)));
    QApplication::sendEvent(editor, new QKeyEvent(QEvent::KeyRelease, Qt::Key_Enter, Qt::NoModifier,QString()));
}

